/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS chips only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "mqtt_client.h"

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "device_OTA.h"
#include "device_main.h"
#include "device_tree_tuple.h"
#include "device_flash.h"
#include "cJSON.h"

#define MAX_SIZE_OF_TOPIC_CONTENT 100

static const int CONNECTED_BIT = BIT0;
static const char* TAG = "qcloud-mqtt";
static int sg_count = 0;
static EventGroupHandle_t wifi_event_group;

static int sub_packet_id = 0;
static bool sub_ack = false;

static int pub_packet_id = 0;
static bool pub_ack = false;


#define mqtt_ota_flag ("$ota/update")
#define down_flag ("$thing/down/property")

#define up_info ("{\"method\":\"report_info\", \"clientToken\":\"%s-%d\", \"params\":{\"module_hardinfo\":\"ESP8266\",\"fw_ver\":\"1.0.0\"}}")
#define up_curtainConrtol ("{\"method\":\"report\", \"clientToken\":\"%s-%d\", \"params\":{\"curtainConrtol\":%d}}")
#define up_curtainPosition ("{\"method\":\"report\", \"clientToken\":\"%s-%d\", \"params\":{\"curtainPosition\":%d}}")



void mqtt_demo_event_handler(void* pclient, void* handle_context, MQTTEventMsg* msg)
{
    MQTTMessage* mqtt_messge = (MQTTMessage*)msg->msg;
    uintptr_t packet_id = (uintptr_t)msg->msg;

    switch (msg->event_type) {
        case MQTT_EVENT_UNDEF:
            ESP_LOGI(TAG, "undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "MQTT disconnect.");
            break;

        case MQTT_EVENT_RECONNECT:
            ESP_LOGI(TAG, "MQTT reconnect.");
            break;

        case MQTT_EVENT_PUBLISH_RECVEIVED:
            ESP_LOGI(TAG, "topic message arrived but without any related handle: topic=%d-%s, topic_msg=%d-%s",
                     mqtt_messge->topic_len,
                     (char*)mqtt_messge->ptopic,
                     mqtt_messge->payload_len,
                     (char*)mqtt_messge->payload);
            break;

        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            ESP_LOGI(TAG, "subscribe success, packet-id=%u", (unsigned int)packet_id);
            if (sub_packet_id == packet_id)
			{
				sub_ack = true;
			}
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            ESP_LOGI(TAG, "subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            ESP_LOGI(TAG, "subscribe nack, packet-id=%u", (unsigned int)packet_id);
            
            break;

        case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            ESP_LOGI(TAG, "unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            ESP_LOGI(TAG, "unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_UNSUBCRIBE_NACK:
            ESP_LOGI(TAG, "unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            ESP_LOGI(TAG, "publish success, packet-id=%u", (unsigned int)packet_id);
		
			if (pub_packet_id == packet_id)
			{
				pub_ack = true;
			}
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            ESP_LOGI(TAG, "publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            ESP_LOGI(TAG, "publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        default:
            ESP_LOGI(TAG, "Should NOT arrive here.");
            break;
    }
}

/**
 * MQTT消息接收处理函数
 *
 * @param topicName         topic主题
 * @param topicNameLen      topic长度
 * @param message           已订阅消息的结构
 * @param userData         消息负载
 */
static void on_message_callback(void* pClient, MQTTMessage* message, void* userData)
{
	cJSON *root = NULL;
	cJSON *item_CurtainPosition = NULL;
	cJSON * item_CurtainOperation= NULL;
//	cJSON * item_SetDir= NULL;
	cJSON * params= NULL;
	cJSON * url= NULL;

	
    if (message == NULL) 
	{
        return;
    }

    Log_i("Receive Message With topicName:%.*s, payload:%.*s",
             (int) message->topic_len, message->ptopic, (int) message->payload_len, (char*) message->payload);
	if(NULL!=strstr(message->ptopic,mqtt_ota_flag))
	{
		//ota
		Log_i("is ota_flag flag");
		root = cJSON_Parse(message->payload);
		if (root!= NULL)
		{
			url=cJSON_GetObjectItem(root, "url");
			if (url!= NULL)
			{	
				dooya_set_ota_flag_to_flash(1);
				esp_restart();
			}
		}
		
		
             
	}
	else if(NULL!=strstr(message->ptopic,down_flag))
	{
		Log_i("is down_flag flag");
		if (message->payload == NULL)
	    {
	        return;
	    }
		root = cJSON_Parse(message->payload);
		if (root!= NULL)
		{
			params=cJSON_GetObjectItem(root, "params");
			if (params!= NULL)
			{
				item_CurtainOperation=cJSON_GetObjectItem(params, "curtainConrtol");
				if(item_CurtainOperation!= NULL)
				{
					if(cJSON_IsNumber(item_CurtainOperation))
					{
						Log_i("int is [%d]",item_CurtainOperation->valueint);
					}
					else
					{
						Log_i("string is [%d]",atoi(item_CurtainOperation->valuestring));
					}
				}
				item_CurtainPosition=cJSON_GetObjectItem(params, "curtainPosition");
				if(item_CurtainPosition!= NULL)
				{
					
					if(cJSON_IsNumber(item_CurtainPosition))
					{
						Log_i("int is [%d]\r\n",item_CurtainPosition->valueint);
					}
					else
					{
						Log_i("string is [%d]\r\n",atoi(item_CurtainPosition->valuestring));
					}
				}
			}
			cJSON_Delete(root);
		}


		
	}
		
}

/**
 * 设置MQTT connet初始化参数
 *
 * @param initParams MQTT connet初始化参数
 *
 * @return 0: 参数初始化成功  非0: 失败
 */

static int setup_connect_init_params(MQTTInitParams* initParams)
{
	initParams->device_name = QCLOUD_DEVICE_NAME;
	initParams->product_id = QCLOUD_PRODUCT_ID;


	initParams->device_secret = QCLOUD_DEVICE_SECRET;


	initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

	initParams->auto_connect_enable = 1;
	initParams->event_handle.h_fp = mqtt_demo_event_handler;
	initParams->event_handle.context = NULL;

	return QCLOUD_RET_SUCCESS;
}


/**
 * 发送topic消息
 *
 */

static int publish_to_upstream_topic_info(void* client,  char *pJsonDoc)
{


	char topic[MAX_SIZE_OF_CLOUD_TOPIC] = {0};
	int size;


	size = HAL_Snprintf(topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/up/property/%s/%s",QCLOUD_PRODUCT_ID, QCLOUD_DEVICE_NAME);	

	if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
	{
	Log_e("buf size < topic length!");
	IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

	PublishParams pubParams = DEFAULT_PUB_PARAMS;
	pubParams.qos = QOS0;
	pubParams.payload_len = strlen(pJsonDoc);
	pubParams.payload = (char *) pJsonDoc;
	IOT_FUNC_EXIT_RC(IOT_MQTT_Publish(client, topic, &pubParams));
	/*pub_packet_id = IOT_MQTT_Publish(client, topic, &pubParams);
	Log_i("pub_packet_id is %d",pub_packet_id);
	if(pub_packet_id<0)
	{	
		return -1;
	}
	while (!pub_ack) 
	{
		Log_i("sg_pub_ack is 1");
		HAL_SleepMs(1000);
		IOT_MQTT_Yield(client, 200);
	}
	pub_ack = false;

	IOT_FUNC_EXIT_RC(pub_packet_id);*/
}

static int publish_to_ota_topic_info(void* client)
{


	char topic[MAX_SIZE_OF_CLOUD_TOPIC] = {0};
	char JsonDoc[100] = {0};
	int size;


	size=HAL_Snprintf(JsonDoc,100,"{\"type\":\"report_version\",\"report\":{\"version\":\"%s\"}}",fwVer);
	if (size < 0 || size > 100 - 1)
	{
		Log_e("buf size < topic length!");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}
	
	Log_i("#JsonDoc is %s",JsonDoc);
	
	size = HAL_Snprintf(topic, MAX_SIZE_OF_CLOUD_TOPIC, "$ota/report/%s/%s",QCLOUD_PRODUCT_ID, QCLOUD_DEVICE_NAME);	

	if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
	{
		Log_e("buf size < topic length!");
		IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}

	PublishParams pubParams = DEFAULT_PUB_PARAMS;
	pubParams.qos = QOS1;
	pubParams.payload_len = strlen(JsonDoc);
	pubParams.payload = (char *) JsonDoc;

	pub_packet_id = IOT_MQTT_Publish(client, topic, &pubParams);
	while (!pub_ack) 
	{
		Log_i("publish_to_ota_topic_info is 1");
		HAL_SleepMs(1000);
		IOT_MQTT_Yield(client, 200);
	}
	pub_ack = false;

	IOT_FUNC_EXIT_RC(pub_packet_id);
}




/**
 * 订阅关注topic和注册相应回调处理
 *
 */

static int subscribe_downstream_topic(void* client)
{

	int size;
	 
	
    char *downstream_topic = (char *)HAL_Malloc(MAX_SIZE_OF_CLOUD_TOPIC * sizeof(char));
    if (downstream_topic == NULL) IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);

    memset(downstream_topic, 0x0, MAX_SIZE_OF_CLOUD_TOPIC);
	size = HAL_Snprintf(downstream_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/down/property/%s/%s", QCLOUD_PRODUCT_ID, QCLOUD_DEVICE_NAME);	
	if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
    {
        Log_e("buf size < topic length!");
        HAL_Free(downstream_topic);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }
    SubscribeParams subscribe_params = DEFAULT_SUB_PARAMS;
    subscribe_params.on_message_handler = on_message_callback;
    subscribe_params.qos = QOS0;

    sub_packet_id = IOT_MQTT_Subscribe(client, downstream_topic, &subscribe_params);
    if (sub_packet_id < 0) 
	{
        Log_e("subscribe topic: %s failed: %d.",downstream_topic, sub_packet_id);
    }
	else
	{
		while (!sub_ack) 
		{
			Log_i("subscribe_downstream_topic is 1");
		 	HAL_SleepMs(1000);
			//订阅不上，可能需要重启
		 	IOT_MQTT_Yield(client, 200);
		}
		sub_ack = false;
	}
	HAL_Free(downstream_topic);

    IOT_FUNC_EXIT_RC(sub_packet_id);
}

static int subscribe_ota_topic(void* client)
{
	int size;
	char *ota_topic = (char *)HAL_Malloc(MAX_SIZE_OF_CLOUD_TOPIC * sizeof(char));
	if (ota_topic == NULL) IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	memset(ota_topic, 0x0, MAX_SIZE_OF_CLOUD_TOPIC);
	
	/* subscribe the OTA topic: "$ota/update/$(product_id)/$(device_name)" */
	size = HAL_Snprintf(ota_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$ota/update/%s/%s", QCLOUD_PRODUCT_ID, QCLOUD_DEVICE_NAME);	 
    if (size < 0 || size > MAX_SIZE_OF_CLOUD_TOPIC - 1)
    {
        Log_e("buf size < topic length!");
        HAL_Free(ota_topic);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

	SubscribeParams subscribe_params = DEFAULT_SUB_PARAMS;
    subscribe_params.on_message_handler = on_message_callback;
    subscribe_params.qos = QOS1;


    sub_packet_id = IOT_MQTT_Subscribe(client, ota_topic, &subscribe_params);
    if (sub_packet_id < 0) 
	{
        Log_e("subscribe topic: %s failed: %d.",ota_topic, sub_packet_id);
    }
	else
	{
		while (!sub_ack) 
		{
			Log_i("subscribe_ota_topic is 1");
		 	HAL_SleepMs(1000);
			//订阅不上，可能需要重启
		 	IOT_MQTT_Yield(client, 200);
		}
		sub_ack = false;
	}
	HAL_Free(ota_topic);
	IOT_FUNC_EXIT_RC(sub_packet_id);
}


void qcloud_mqtt_demo(void)
{
    IOT_Log_Set_Level(eLOG_DEBUG);
	char JsonDoc[254] = {0};
    int rc;

    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;

    rc = setup_connect_init_params(&init_params);

    if (rc != QCLOUD_RET_SUCCESS) 
	{
        Log_i("setup_connect_init_params Failed");
        
    }

    void* client = IOT_MQTT_Construct(&init_params);

    if (client != NULL) 
	{
        Log_i("Cloud Device Construct Success");
    } 
	else 
	{
        Log_i("Cloud Device Construct Failed");
		esp_restart();
        
    }
	/****************************************************************/
	rc = subscribe_downstream_topic(client);

    if (rc < 0) 
	{
        Log_i( "Client Subscribe Topic Failed: %d", rc);
    }
	rc = subscribe_ota_topic(client);
    if (rc < 0) 
	{
        Log_i( "Client Subscribe Topic Failed: %d", rc);
    }

	rc =publish_to_ota_topic_info(client);
	if (rc < 0) 
	{
		Log_i( "publish_to_ota_topic_info: %d", rc);
	}
	
	memset (JsonDoc,0,sizeof(JsonDoc));
	HAL_Snprintf(JsonDoc,sizeof(JsonDoc),up_info,QCLOUD_PRODUCT_ID,sg_count++);
	Log_i( "publish_to_ota_topic_info: %s", JsonDoc);
	publish_to_upstream_topic_info(client,  JsonDoc);
	HAL_SleepMs(1000);

	memset (JsonDoc,0,sizeof(JsonDoc));
	HAL_Snprintf(JsonDoc,sizeof(JsonDoc),up_curtainConrtol,QCLOUD_PRODUCT_ID,sg_count++,10);
	Log_i( "publish_to_ota_topic_info: %s", JsonDoc);
	publish_to_upstream_topic_info(client,  JsonDoc);
	HAL_SleepMs(1000);

	
	memset (JsonDoc,0,sizeof(JsonDoc));
	HAL_Snprintf(JsonDoc,sizeof(JsonDoc),up_curtainPosition,QCLOUD_PRODUCT_ID,sg_count++,10);
	Log_i( "publish_to_ota_topic_info: %s", JsonDoc);
	publish_to_upstream_topic_info(client,  JsonDoc);
	HAL_SleepMs(1000);
	
    do {

        rc = IOT_MQTT_Yield(client, 200);
		
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) 
		{
            HAL_SleepMs(1000);
            continue;
        } 
		else if (rc != QCLOUD_RET_SUCCESS) 
		{
			Log_e("Exit loop caused of errCode: %d", rc);
			HAL_SleepMs(1000);
			continue;
		}


		
		/*report msg to server*/
		/*report the lastest properties's status*/
		{
			Log_i("report msg to server");
			//rc = IOT_Template_JSON_ConstructReportArray(client, sg_data_report_buffer, sg_data_report_buffersize, ReportCont, pReportDataList);
			if (rc == QCLOUD_RET_SUCCESS) 
			{
				//ESP_LOGI(TAG, "IOT_Template_JSON_ConstructReportArray");
				//rc = IOT_Template_Report(client, sg_data_report_buffer, sg_data_report_buffersize, 
				//							OnReportReplyCallback, NULL, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
				if (rc == QCLOUD_RET_SUCCESS) 
				{
				
					Log_i("data template reporte success");
				} 
				else 
				{
					Log_e("data template reporte failed, err: %d", rc);
				}
			} 
			else 
			{
				Log_e("construct reporte data failed, err: %d", rc);
			}
	
		}
		memset (JsonDoc,0,sizeof(JsonDoc));
		HAL_Snprintf(JsonDoc,sizeof(JsonDoc),up_curtainPosition,QCLOUD_PRODUCT_ID,sg_count++,10);
		Log_i( "publish_to_ota_topic_info: %s", JsonDoc);
		publish_to_upstream_topic_info(client,  JsonDoc);
		HAL_SleepMs(2000);
    } while (1);

    IOT_MQTT_Destroy(&client);
}

void qcloud_example_task(void* parm)
{
    EventBits_t uxBits;

    while (1) {
        uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, true, false, portMAX_DELAY);

        if (uxBits & CONNECTED_BIT) {
            Log_i("WiFi Connected to ap");
            qcloud_mqtt_demo();
            vTaskDelete(NULL);
        }

        vTaskDelay(1);
    }

    vTaskDelete(NULL);
}

static void wifi_connection(void)
{
	wifi_config_t wifi_config;
	size_t size = 0;
	memset(&wifi_config,0x0,sizeof(wifi_config));
	size=sizeof(wifi_config.sta.ssid);
	dooya_get_wifi_ssid_from_flash((char *)wifi_config.sta.ssid,size);
	size=sizeof(wifi_config.sta.password);
	dooya_get_wifi_password_from_flash((char *)wifi_config.sta.password,size);
	
	Log_i("Setting WiFi configuration SSID %s ...", wifi_config.sta.ssid);
	Log_i("Setting WiFi configuration password %s...", wifi_config.sta.password);
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	esp_wifi_connect();
}


static esp_err_t event_handler(void* ctx, system_event_t* event)
{
    Log_i("event = %d", event->event_id);
	uint32_t status=0;
	if(-1==dooya_get_ota_flag_from_flash(&status))
	{
		status=0;
	}
	Log_i("event_handler status = %d", status);
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            Log_i("SYSTEM_EVENT_STA_START");
			if(status==0)
			{
				xTaskCreate(qcloud_example_task, "qcloud_example_task", 1024*5, NULL, 3, NULL);
			}
            wifi_connection();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            Log_i("Got IPv4[%s]", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			if(status==0)
			{
				xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
			}
            else
            {
            	 dooya_create_ota_thread();
            }
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            Log_i("SYSTEM_EVENT_STA_DISCONNECTED");
			if(status==0)
			{
				 xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
			}
            esp_wifi_connect();
            break;

        default:
            break;
    }

    return ESP_OK;
}

void esp32_wifi_initialise(void)
{
    tcpip_adapter_init();

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}


