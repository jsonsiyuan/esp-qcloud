#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_ota_ops.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_attr.h"
#include "esp_flash_data_types.h"
#include "esp_spi_flash.h"
#include "esp_partition.h"



#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "lite-utils.h"


#include "device_flash.h"
#include "device_OTA.h"






#define fwVer "1.0.0"
#define OTA_BUF_LEN (1024+1)
char buf_ota[OTA_BUF_LEN];


#define QCLOUD_IOT_MY_PRODUCT_ID       CONFIG_QCLOUD_PROUDCT_ID
#define QCLOUD_IOT_MY_DEVICE_NAME      CONFIG_QCLOUD_DEVICE_NAME
#define QCLOUD_IOT_DEVICE_SECRET       CONFIG_QCLOUD_DEVICE_SECRET



static bool sg_pub_ack = false;
static int sg_packet_id = 0;
void *h_ota=NULL;
static int32_t dooya_qcloud_ota_deal(void *pclient);

static void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) 
{	
	uintptr_t packet_id = (uintptr_t)msg->msg;

	switch(msg->event_type) {
		case MQTT_EVENT_UNDEF:
			Log_i("undefined event occur.");
			break;

		case MQTT_EVENT_DISCONNECT:
			Log_i("MQTT disconnect.");
			break;

		case MQTT_EVENT_RECONNECT:
			Log_i("MQTT reconnect.");
			break;

		case MQTT_EVENT_SUBCRIBE_SUCCESS:
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_SUCCESS:
			Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
            if (sg_packet_id == packet_id)
                sg_pub_ack = true;
			break;

		case MQTT_EVENT_PUBLISH_TIMEOUT:
			Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_NACK:
			Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
			break;
		default:
			Log_i("Should NOT arrive here.");
			break;
	}
}

static int _setup_connect_init_params(MQTTInitParams* initParams)
{
	
	
	initParams->device_name = QCLOUD_IOT_MY_DEVICE_NAME;
	initParams->product_id = QCLOUD_IOT_MY_PRODUCT_ID;

#ifdef AUTH_MODE_CERT
#else
	initParams->device_secret = QCLOUD_IOT_DEVICE_SECRET;
#endif

    
	initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;
	initParams->auto_connect_enable = 1;
    initParams->event_handle.h_fp = event_handler;

    return QCLOUD_RET_SUCCESS;
}

static void dooya_qcloud_ota_all(  void )
{
	int rc;
	void *client=NULL;

	MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
	rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_RET_SUCCESS) 
	{
		Log_e("init params err,rc=%d", rc);
	}

	


	Log_e("IOT_MQTT_Construct start ");
    client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) 
	{
        Log_i("Cloud Device Construct Success");
    } 
	else 
    {
        Log_e("Cloud Device Construct Failed");
        
    }
	h_ota = IOT_OTA_Init(QCLOUD_IOT_MY_PRODUCT_ID, QCLOUD_IOT_MY_DEVICE_NAME, client);
    if (NULL == h_ota) 
	{
        Log_e("initialize OTA failed");
       
    }

	IOT_MQTT_Yield(client, 1000);  //make sure subscribe success
	/* Must report version first */
    if (0 > IOT_OTA_ReportVersion(h_ota,  fwVer)) 
	{
        Log_e("report OTA version failed");
        
    }
    do {
        

        IOT_MQTT_Yield(client, 200);
	
        if(-1==dooya_qcloud_ota_deal(client))
        {
        	esp_restart();
        }
		
		
    } while(1);

	

}

static int32_t dooya_qcloud_ota_deal(void *pclient)
{
	bool upgrade_fetch_success = true;
	int len;
	static int ota_over = 0;
	uint32_t size_file=0;
	uint32_t offset = 0;
	uint32_t offset_tmp=0;
	uint32_t offset_all=0;

	char buf_ota_tmp[2];
	int rc;
	esp_err_t err;
	const esp_partition_t *update_partition = NULL;
	
	/*
	Log_i( "Starting OTA example...  flash %s",  CONFIG_ESPTOOLPY_FLASHSIZE);
	const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();
	

	if (configured != running) 
	{
        Log_i( "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
                 configured->address, running->address);
        Log_i( "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }
    Log_i( "Running partition type %d subtype %d (offset 0x%08x)",running->type, running->subtype, running->address);
	*/

	update_partition = esp_ota_get_next_update_partition(NULL);
	Log_i( "Writing to partition subtype %d at offset 0x%x",update_partition->subtype, update_partition->address);

	if(-1==dooya_get_ota_number_from_flash(&offset))
	{
		offset=0;
	}
	offset_tmp=offset/SPI_FLASH_SEC_SIZE;
	offset_tmp=offset_tmp*SPI_FLASH_SEC_SIZE;
	
	if (IOT_OTA_IsFetching(h_ota)) 
	{
		Log_i("IOT_OTA_IsFetching enter");
	
		//getFwOffset
		//cal file md5

		//set offset and start http connect	
		IOT_OTA_GetSize(h_ota,&size_file);
		
		//HTTP get
		Log_i("ota all is [%d]",size_file);
		rc = IOT_OTA_StartDownload(h_ota, offset_tmp, size_file);
		if(QCLOUD_RET_SUCCESS != rc)
		{
			Log_e("OTA download start err,rc:%d",rc);
			upgrade_fetch_success = false;
			Log_i("offset data len is [%d]\r\n",offset);
			return -1;
		}	
		
		
		offset_all=((size_file-offset_tmp) / SPI_FLASH_SEC_SIZE + 1) * SPI_FLASH_SEC_SIZE;
		
		Log_i( "offset %d  offset_tmp %d,all is %d",offset, offset_tmp,offset_all);
		
		err=spi_flash_erase_range(update_partition->address + offset_tmp,offset_all );
		if(err!=0)
		{
			Log_i("spi_flash_erase_range fail");
		}

		do 
		{	
			Log_i("###############################################");
			len = IOT_OTA_FetchYield(h_ota, buf_ota, OTA_BUF_LEN, 1000);
			if (len > 0) 
			{
					
					
					/*Log_i("recv data len is [%d][%x][%x]\r\n",len,buf_ota[0],buf_ota[1]);	
					for (i=0;i<len;i++)
					{
						Log_i("[%x]\r\n",buf_ota[i]);						
					}
					printf("\r\n");*/
					err=spi_flash_write(update_partition->address + offset_tmp, buf_ota, len);
					if(err!=0)
					{
						Log_i("spi_flash_write fail");
					}
					
					/*读一下
					memset(buf_ota_tmp,0,2);
					err=spi_flash_read(update_partition->address + offset_tmp, buf_ota_tmp, 2);
					if(err!=0)
					{
						Log_i("spi_flash_write fail");
					}
					Log_i("flash data len is [%x][%x]\r\n",buf_ota_tmp[0],buf_ota_tmp[1]);
					*/

					offset_tmp+=len;
					
					Log_i("offset data len is [%d]\r\n",offset_tmp);
					dooya_set_ota_number_to_flash(offset_tmp);
					
					
			}
			else if (len < 0) 
			{
				
				upgrade_fetch_success = false;
				Log_i("offset data len is [%d]\r\n",offset_tmp);	
				return -1;
			}				
			
			
		} while (!IOT_OTA_IsFetchFinish(h_ota));


		
		
		Log_i("while over\r\n",len);	

	    // Must check MD5 match or not

		ota_over = 1;
		dooya_set_ota_number_to_flash(0);
	}
	//err = esp_ota_set_boot_partition(update_partition);
	//esp_restart();

	
	if(ota_over == 1) 
	{
		Log_i("ota_over is 1");
		if (upgrade_fetch_success)
		{
	    	sg_packet_id = IOT_OTA_ReportUpgradeBegin(h_ota);
			if (0 > sg_packet_id)
			{
				Log_e("report OTA begin failed error:%d", sg_packet_id);
				return QCLOUD_ERR_FAILURE;
			}
			while (!sg_pub_ack) 
			{
				Log_i("sg_pub_ack is 1");
             	HAL_SleepMs(1000);
             	IOT_MQTT_Yield(pclient, 200);
         	}
         	sg_pub_ack = false;

			
			esp_ota_set_boot_partition(update_partition);
			
			sg_packet_id = IOT_OTA_ReportUpgradeSuccess(h_ota, NULL);//sg_packet_id = IOT_OTA_ReportUpgradeFail(h_ota, NULL); 
		    if (0 > sg_packet_id) 
			{
	             Log_e("report OTA result failed error:%d", sg_packet_id);
	             return QCLOUD_ERR_FAILURE;
	        }
			while (!sg_pub_ack) 
			{
				Log_i("sg_pub_ack is 2");
				HAL_SleepMs(1000);
				IOT_MQTT_Yield(pclient, 200);
			}
			sg_pub_ack = false;	
			esp_restart();

      
    	}
		
	}
	

	return 0;
}
static void qcloud_ota_task(void* parm)
{
    while (1) 
	{
		dooya_qcloud_ota_all();
		vTaskDelete(NULL);
		vTaskDelay(1);
    }

    vTaskDelete(NULL);
}

void dooya_create_ota_thread(void)
{
	Log_i("sun start OTA\r\n");	
	xTaskCreate(qcloud_ota_task, "OTA_task", 1024*8, NULL, 3, NULL);
}



