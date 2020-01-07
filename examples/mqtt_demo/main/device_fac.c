
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "device_fac.h"
#include "device_flash.h"
#include "device_led.h"
#include "device_uart_send.h"


#define FAC_TIME_OUT 60
static uint8_t dooya_fac_model=0;
static uint8_t dooya_fac_wifi_model=0;
static uint8_t dooya_fac_led_model=0;

void dooya_fac_set(void)
{
	dooya_set_wifi_FAC();
	vTaskDelay(100/portTICK_RATE_MS);
	esp_restart();
}

uint8_t dooya_fac_check(void)
{
	return dooya_fac_model;
}

void dooya_fac_start(void)
{
	dooya_fac_model=1;
	dooya_set_wifi_STA();
	dooya_set_led_r_status(LED_CLOSE ,1 );
	dooya_set_led_g_status(LED_TAGGLE ,1);
}

void dooya_fac_stop(void)
{
	dooya_set_wifi_STA();
}

uint8_t dooya_fac_wifi_model_check(void)
{
	return dooya_fac_wifi_model;
}

void dooya_fac_wifi_model_ok(void)
{
	dooya_fac_wifi_model=1;
	
	 
}

void dooya_fac_key_led_check(void)
{
	if(dooya_fac_wifi_model_check()==1)
	{
		dooya_fac_led_model=1;
	}
	
}

static void dooya_fac_handle(void *pvParameters)
{
	vTaskDelay(100/portTICK_RATE_MS);
	uint8_t count_tmp=0;
	int8_t wifi_rssi_int=0;
	int8_t wifi_rssi[4]={0};

	while(1)
	{

		if((dooya_fac_wifi_model_check()==1)&&(dooya_fac_led_model==0))
		{
			count_tmp= 0;		

			dooya_response_fac(1,wifi_rssi_int);	

			
		}
		else if((dooya_fac_wifi_model_check()==1)&&(dooya_fac_led_model==1))
		{
			count_tmp= 0;
			dooya_set_led_g_status(LED_CLOSE ,1);
			dooya_set_led_r_status( LED_TAGGLE,1);
			dooya_fac_stop();
			while(1)
			{
				vTaskDelay(5000/portTICK_RATE_MS);
				
				
			}
		}
		else 
		{
			count_tmp++;
		}		
		if(count_tmp>FAC_TIME_OUT)
		{
			dooya_response_fac(0,0);
			dooya_fac_stop();
			while(1)
			{
				vTaskDelay(5000/portTICK_RATE_MS);
				
				
			}
		}
		vTaskDelay(1000/portTICK_RATE_MS);
		
	}	

}

uint8_t dooya_create_fac_thread(void)
{
	xTaskCreate(dooya_fac_handle, "fac", 1024, NULL, 3, NULL);

	return 0;
}


