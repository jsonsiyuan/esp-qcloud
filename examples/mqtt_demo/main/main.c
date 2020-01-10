

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "device_tree_tuple.h"
#include "tc_iot_wifi_boarding_esp8266.h"
#include "device_flash.h"
#include "device_main.h"
#include "device_fac.h"

#define tag_flag "sun"


void app_main()
{
	char wifi_ssid[32+1]={0};    
	char  wifi_password[64+1]={0}; 
	uint8_t device_status=0;
	ESP_ERROR_CHECK(nvs_flash_init());
#if 0
	dooya_fac_start();

#else
	dooya_qcloud_init();
    /*启动灯和按键*/
	
	ESP_LOGI(tag_flag, " tree tuple is %s,%s,%s",QCLOUD_DEVICE_NAME,QCLOUD_DEVICE_SECRET,QCLOUD_PRODUCT_ID);
	 if(!dooya_get_wifi_status_from_flash(&device_status))
	 {
		switch(device_status)
		{
			case D_WIFI_FAC:
				ESP_LOGI(tag_flag, "D_WIFI_FAC");
				dooya_set_wifi_STA();
				dooya_fac_start();
			break;
			case D_WIFI_SOFTAP:
				ESP_LOGI(tag_flag, "D_WIFI_SOFTAP");
				dooya_set_wifi_STA();
				start_softAP("dooya", "12345678");
				
			break;
			case D_WIFI_STA:
				ESP_LOGI(tag_flag, "D_WIFI_STA");
				if(!dooya_get_wifi_ssid_from_flash(wifi_ssid,sizeof(wifi_ssid)))
				{
					if(!dooya_get_wifi_password_from_flash(wifi_password,sizeof(wifi_password)))
					{
						//有网络相关信息
						ESP_LOGI(tag_flag, "satrt esp32_wifi_initialise");
						esp32_wifi_initialise();
					}
				}
				else
				{
					//没有网络相关信息
					ESP_LOGI(tag_flag, "no wifi info");
					dooya_set_wifi_STA();
					
					start_softAP("dooya", "12345678");
					//esp32_wifi_initialise();
				}
			break;
		}
	 }
	 else
	 {
		ESP_LOGI(tag_flag, "no wifi status");
		dooya_set_wifi_STA();
		start_softAP("dooya", "12345678");
	 }
	uint32_t status=0;
	if(-1==dooya_get_ota_flag_from_flash(&status))
	{
		status=0;
	}
	 while(1)
	 {
		vTaskDelay(30000/portTICK_RATE_MS);
	 	if((esp_get_free_heap_size()>5500)&&(status))
	 	{
	 		esp_restart();
	 	}
		ESP_LOGI(tag_flag, "free_heap is [%d]",esp_get_free_heap_size());
		vTaskDelay(3000/portTICK_RATE_MS);
	 }
 #endif
	 
}

