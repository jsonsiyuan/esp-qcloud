#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "esp_log.h"

#include "device_led.h"

#include "device_uart.h"
#include "device_uart_recv_handle.h"


static uint16_t retry_num=0;
static uint8_t retry_over_flag=0;

static const char *TAG = "uart_events";

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (512)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

#define UUID 0x1923


static SemaphoreHandle_t xSemaphore;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *) malloc(RD_BUF_SIZE);
    uint16_t crc_tmp;
    for (;;) {
        // Waiting for UART event.
        if (xQueueReceive(uart0_queue, (void *)&event, (portTickType)portMAX_DELAY)) 
	 {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);

            switch (event.type) {
                // Event of UART receving data
                // We'd better handler data event fast, there would be much more data events than
                // other types of events. If we take too much time on data event, the queue might be full.
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
				if(event.size<5)
				{
					break;
				}
				if(dtmp[2]+3>event.size)
				{
					break;
				}
		      if((dtmp[0]==0x55)&&(dtmp[1]==0xAA))
		      	{
		      		crc_tmp=CRC16_MODBUS(dtmp, dtmp[2]+1);
				if((dtmp[dtmp[2]+1]==(crc_tmp/256))
					&&(dtmp[dtmp[2]+2]==(crc_tmp%256)))
				{
					switch(dtmp[3])
					{
						case CONTROL_CODE:
							dooya_control_handle(dtmp+4,dtmp[2]-2);
						break;
						case CHECK_CODE:
							dooya_check_handle(dtmp+4,dtmp[2]-2);

						break;
						case NOTICE_CODE:
							dooya_notice_handle(dtmp+4,dtmp[2]-2);
						break;
						case OTA:
						break;
					}
				}
		      	}
                    //ESP_LOGI(TAG, "[DATA EVT]:");
                    //uart_write_bytes(EX_UART_NUM, (const char *) dtmp, event.size);
                    break;

                // Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart0_queue);
                    break;

                // Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart0_queue);
                    break;

                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;

                // Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;

                // Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }

    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

static void uart_timer_handler(void *pvParameters)
{
	while(1)
	{
		retry_num++;
		if(retry_num>5)
		{
			dooya_set_led_g_status(LED_CLOSE,1);
			dooya_set_led_r_status(LED_TAGGLE,10);
			retry_over_flag=1;
		}
		else if(retry_over_flag)
		{
			retry_over_flag=0;
			dooya_set_led_r_status(LED_CLOSE,1);
		}
		
		/*if(dooya_fac_check()==1)
		{
			retry_num=0;
		}
		else
		{
			dooya_start_motor_check();
		}*/
		vTaskDelay(10000 / portTICK_RATE_MS);
	}
}


void dooya_create_uart_thread(void)
{
    // Configure parameters of an UART driver,
    // communication pins and install the driver
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(EX_UART_NUM, &uart_config);

    // Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 100, &uart0_queue);

    // Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 1024*2, NULL, 3, NULL);

	xSemaphore = xSemaphoreCreateMutex();
	xTaskCreate(uart_timer_handler,"uart_time",512, NULL, 3,NULL);
}

void dooya_uart_send( uint8_t *src, uint32_t size)
{
	xSemaphoreTake( xSemaphore, portMAX_DELAY );
	uart_write_bytes(EX_UART_NUM,(const char *) src, size);
	xSemaphoreGive( xSemaphore );
}

uint16_t CRC16_MODBUS(uint8_t *puchMsg, uint16_t usDataLen)
{
	uint16_t wCRCin = UUID;

	int16_t wCPoly = 0x8005;
	uint8_t wChar = 0;
	uint8_t i = 0;
	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for(i = 0; i < 8; i++)
		{
			if(wCRCin & 0x8000)
			{
				wCRCin = (wCRCin << 1) ^ wCPoly;
			}
			else
			{
				wCRCin = wCRCin << 1;
			}
		}
	}
	return (wCRCin);
}


