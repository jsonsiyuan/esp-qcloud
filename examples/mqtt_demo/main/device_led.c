#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_system.h"

#include "device_led.h"

#define GPIO_OUTPUT_IO_R    16
#define GPIO_OUTPUT_IO_G    15
#define GPIO_OUTPUT_PIN_SEL_R  (1ULL<<GPIO_OUTPUT_IO_R) 
#define GPIO_OUTPUT_PIN_SEL_G  (1ULL<<GPIO_OUTPUT_IO_G) 

LED_STATUS_T led_g_status={
	.led_mode=LED_ALWAYS_CLOSE,
	.led_HZ=1,
};

LED_STATUS_T led_r_status={
	.led_mode=LED_ALWAYS_CLOSE,
	.led_HZ=1,
};

	

void dooya_led_g_init(void)
{
	gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_G;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void dooya_led_r_init(void)
{
	gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL_R;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void dooya_led_init(void)
{
	dooya_led_r_init();
	dooya_led_g_init();
}

void dooya_open_led_g(void)
{
	gpio_set_level(GPIO_OUTPUT_IO_G, LED_OPEN);
	led_g_status.led_active=LED_OPEN;
}

void dooya_close_led_g(void)
{
	gpio_set_level(GPIO_OUTPUT_IO_G, LED_CLOSE);
	led_g_status.led_active=LED_CLOSE;
}

void dooya_toggle_led_g(void)
{
	if(led_g_status.led_active==LED_OPEN)
	{
		dooya_close_led_g();
	}
	else
	{
		dooya_open_led_g();
	}
}


void dooya_open_led_r(void)
{
	gpio_set_level(GPIO_OUTPUT_IO_R, LED_OPEN);
	led_r_status.led_active=LED_OPEN;
}

void dooya_close_led_r(void)
{
	gpio_set_level(GPIO_OUTPUT_IO_R, LED_CLOSE);
	led_r_status.led_active=LED_CLOSE;
}

void dooya_toggle_led_r(void)
{
	if(led_r_status.led_active==LED_OPEN)
	{
		dooya_close_led_r();
	}
	else
	{
		dooya_open_led_r();
	}
}

void dooya_set_led_r_status(LED_MODE_T mode,uint8_t hz)
{
	led_r_status.led_mode=mode;
	led_r_status.led_HZ=hz;
	switch(mode)
	{
		case LED_ALWAYS_OPEN:
			dooya_open_led_r();
		break;
		case LED_ALWAYS_CLOSE:
			dooya_close_led_r();
			
		break;
		case LED_TAGGLE:
			dooya_toggle_led_r();
			
		break;
	}
}

void dooya_set_led_g_status(LED_MODE_T mode,uint8_t hz)
{
	led_g_status.led_mode=mode;
	led_g_status.led_HZ=hz;
	
	switch(mode)
	{
		case LED_ALWAYS_OPEN:
			dooya_open_led_g();
		
		break;
		case LED_ALWAYS_CLOSE:
			dooya_close_led_g();
		
		break;
		case LED_TAGGLE:
			dooya_toggle_led_g();
		
		break;
	}
}

static void dooya_led_g_handle(void *pvParameters)
{
	dooya_close_led_g();
	vTaskDelay(300/led_g_status.led_HZ);
	printf("####sun# dooya_led_g_handle start\r\n");
	while(1)
	{
		switch(led_g_status.led_mode)
		{
			case LED_ALWAYS_OPEN:
				dooya_open_led_g();
				vTaskDelay(300/portTICK_RATE_MS);
			break;
			case LED_ALWAYS_CLOSE:
				dooya_close_led_g();
				vTaskDelay(300/portTICK_RATE_MS);
			break;
			case LED_TAGGLE:
				dooya_toggle_led_g();
				vTaskDelay((1000/led_g_status.led_HZ) / portTICK_RATE_MS);
			break;
		}
		
	}
}

static void dooya_led_r_handle(void *pvParameters)
{
	dooya_close_led_r();
	vTaskDelay(300/led_g_status.led_HZ);
	printf("####sun# dooya_led_r_handle start\r\n");
	while(1)
	{
		switch(led_r_status.led_mode)
		{
			case LED_ALWAYS_OPEN:
				dooya_open_led_r();
				vTaskDelay(300/portTICK_RATE_MS);
			break;
			case LED_ALWAYS_CLOSE:
				dooya_close_led_r();
				vTaskDelay(300/portTICK_RATE_MS);
			break;
			case LED_TAGGLE:
				dooya_toggle_led_r();
				vTaskDelay((1000/led_g_status.led_HZ) / portTICK_RATE_MS);
			break;
		}
		
	}	
}
uint8_t dooya_create_led_thread(void)
{
	dooya_led_init();	
	xTaskCreate(dooya_led_g_handle, "led_g", 512, NULL, 3, NULL);
	xTaskCreate(dooya_led_r_handle, "led_r", 512, NULL, 3, NULL);
	
	return 0;
}

