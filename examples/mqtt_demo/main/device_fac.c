
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include <esp_wifi.h>
#include <tcpip_adapter.h>




#include "device_fac.h"
#include "device_flash.h"
#include "device_led.h"
#include "device_uart_send.h"
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#define dooya_fac_ssid "chanxiancheck"
#define dooya_fac_key "12345678"


#define FAC_TIME_OUT 60
static uint8_t dooya_fac_model=0;
static uint8_t dooya_fac_wifi_model=0;
static uint8_t dooya_fac_led_model=0;


static void dooya_fac_handle(void *pvParameters)
{
	vTaskDelay(100/portTICK_RATE_MS);
	uint8_t count_tmp=0;
	int8_t wifi_rssi_int=0;
	while(1)
	{
		wifi_ap_record_t ap_info;
        Log_i("esp_wifi_sta_get_ap_info start");
		if((dooya_fac_wifi_model_check()==1)&&(dooya_fac_led_model==0))
		{
			count_tmp= 0;
			if(!esp_wifi_sta_get_ap_info(&ap_info))
			{
				wifi_rssi_int=ap_info.rssi;
				Log_i("wifi_rssi_int is %d",wifi_rssi_int);
				dooya_response_fac(1,wifi_rssi_int);	
				Log_i("esp_wifi_sta_get_ap_info over");
			}
			else
			{
				Log_i("esp_wifi_sta_get_ap_info fail");
				continue;
			}


			
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

static uint8_t dooya_create_fac_thread(void)
{
	xTaskCreate(dooya_fac_handle, "fac", 1024*2, NULL, 3, NULL);
	return 0;
}

static void wifi_ota_connection(void)

{
	wifi_config_t wifi_config = {
		.sta = {
			.ssid = dooya_fac_ssid,
			.password = dooya_fac_key,
		},
	};
	Log_i( "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	esp_wifi_connect();
}


static esp_err_t event_ota_handler(void* ctx, system_event_t* event)
{
    Log_i("event = %d", event->event_id);

    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            Log_i("SYSTEM_EVENT_STA_START");
	
            wifi_ota_connection();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            Log_i("Got IPv4[%s]", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			
			dooya_fac_wifi_model_ok();
		
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            Log_i("SYSTEM_EVENT_STA_DISCONNECTED");

            wifi_ota_connection();

            break;

        default:
            break;
    }

    return ESP_OK;
}

static void wifi_ota_initialise(void)
{
    tcpip_adapter_init();


    ESP_ERROR_CHECK(esp_event_loop_init(event_ota_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void dooya_fac_start(void)
{
	dooya_fac_model=1;
	dooya_set_wifi_STA();
	dooya_set_led_r_status(LED_CLOSE ,1 );
	dooya_set_led_g_status(LED_TAGGLE ,1);
	wifi_ota_initialise();
	dooya_create_fac_thread();
}

void dooya_fac_stop(void)
{
	dooya_set_wifi_STA();
}


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


