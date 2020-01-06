#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "device_tree_tuple.h"


#define MFG_PARTITION_NAME "fctry"

#define tree_tuple "qcloud_tuple"

#define DeviceName   "DeviceName"
#define DeviceSecret "DeviceSecret"
#define ProductKey   "ProductKey"

#define tag_flag "sun"

char QCLOUD_DEVICE_NAME[30];      
char QCLOUD_DEVICE_SECRET[30]; 
char QCLOUD_PRODUCT_ID[30];   

static int32_t dooya_get_tree_tuple_from_flash(void)
{
	nvs_handle handle_1;
	size_t read_len = 0;
	size_t len_tmp;
	esp_err_t ret = nvs_open_from_partition(MFG_PARTITION_NAME, tree_tuple, NVS_READONLY, &handle_1);
	if (ret != ESP_OK)
	{	
		ESP_LOGI(tag_flag, " nvs_open fail");
		return -1;
	}

	len_tmp = sizeof(QCLOUD_DEVICE_NAME);
	ret = nvs_get_str(handle_1, DeviceName, QCLOUD_DEVICE_NAME, &len_tmp);

	if (ret != ESP_OK)
	{
		ESP_LOGI(tag_flag, " nvs_get_str 1 fail");
		nvs_close(handle_1);
		return -1;
	}	
	
	len_tmp = sizeof(QCLOUD_DEVICE_SECRET);
	ret = nvs_get_str(handle_1, DeviceSecret, QCLOUD_DEVICE_SECRET, &len_tmp);

	if (ret != ESP_OK)
	{
		ESP_LOGI(tag_flag, " nvs_get_str 2 fail");
		nvs_close(handle_1);
		return -1;
	}	
	

	len_tmp = sizeof(QCLOUD_PRODUCT_ID);
	ret = nvs_get_str(handle_1, "ProductKey", QCLOUD_PRODUCT_ID, &len_tmp);

	if (ret != ESP_OK)
	{
		ESP_LOGI(tag_flag, " nvs_get_str 3 fail");
		nvs_close(handle_1);
		return -1;
	}	


	nvs_close(handle_1);
	return 0;



}
void dooya_qcloud_init(void)
{
	memset(QCLOUD_DEVICE_NAME,0,sizeof(QCLOUD_DEVICE_NAME));
	memset(QCLOUD_DEVICE_SECRET,0,sizeof(QCLOUD_DEVICE_SECRET));
	memset(QCLOUD_PRODUCT_ID,0,sizeof(QCLOUD_PRODUCT_ID));
	ESP_ERROR_CHECK(nvs_flash_init_partition(MFG_PARTITION_NAME));
	
	dooya_get_tree_tuple_from_flash();
}


