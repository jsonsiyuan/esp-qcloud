#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "device_flash.h"


#define space1 "space1"
#define wifi_info "wifiinfo"

#define wifi_ssid "ssid"
#define wifi_password "password"

#define wifi_status "wifistatus"

#define ota_number "otanumber"
#define ota_flag "otaflag"



int32_t esp_qcloud_wifi_clear_info(void)
{
    nvs_handle out_handle;
    if (nvs_open(wifi_info, NVS_READWRITE, &out_handle) == ESP_OK) 
	{
        nvs_erase_all(out_handle);
        nvs_close(out_handle);
		return 0;
    }
	return -1;
}



int32_t esp_qcloud_wifi_save_info(uint8_t*ssid,uint8_t*password)
{
    nvs_handle out_handle;
    char data[65];
    if (nvs_open(wifi_info, NVS_READWRITE, &out_handle) != ESP_OK) 
	{
        return -1;
    }

    memset(data,0x0,sizeof(data));
    strncpy(data,(char*)ssid,strlen((char*)ssid));
    if (nvs_set_str(out_handle,wifi_ssid,data) != ESP_OK) 
	{
        nvs_close(out_handle);
		return -1;
    }

    memset(data,0x0,sizeof(data));
    strncpy(data,(char*)password,strlen((char*)password));
    if (nvs_set_str(out_handle,wifi_password,data) != ESP_OK) 
	{
        nvs_close(out_handle);
		return -1;
    }
    nvs_close(out_handle);
	return 0;
}


/********************wifi_ssid***************************/
int32_t dooya_get_wifi_ssid_from_flash(char *ssid,size_t len)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(wifi_info, NVS_READWRITE, &handle_1);
	
	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}
	//读取 字符串
	size_t len_tmp = len;
	err = nvs_get_str(handle_1, wifi_ssid, ssid, &len_tmp);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	
	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	if (len_tmp > 0) 
	{
		return 0;
	}
	else
	{
		return -1;
	}



}

/********************wifi_key***************************/

int32_t dooya_get_wifi_password_from_flash(char *password,size_t len)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(wifi_info, NVS_READWRITE, &handle_1);
	
	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}
	//读取 字符串
	size_t len_tmp = len;
	err = nvs_get_str(handle_1, wifi_password, password, &len_tmp);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	

	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	if (len_tmp > 0) 
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


/********************wifi_status***************************/
int32_t dooya_get_wifi_status_from_flash(uint8_t *status)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(space1, NVS_READWRITE, &handle_1);

	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}

	err = nvs_get_u8(handle_1, wifi_status, status);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	
	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	return 0;

}

int32_t dooya_set_wifi_status_to_flash(uint8_t status)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(space1, NVS_READWRITE, &handle_1);

	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}

	err = nvs_set_u8(handle_1, wifi_status, status);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	

	nvs_commit(handle_1);
	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	return 0;

}

int32_t dooya_set_wifi_STA(void)
{
	int8_t d_wifi_status=(int8_t)D_WIFI_STA;
	return dooya_set_wifi_status_to_flash(d_wifi_status);

}
int32_t dooya_set_wifi_softAP(void)
{
	D_WIFI_STATUS_T d_wifi_status=D_WIFI_SOFTAP;
	return dooya_set_wifi_status_to_flash(d_wifi_status);
}
int32_t dooya_set_wifi_FAC(void)
{
	D_WIFI_STATUS_T d_wifi_status=D_WIFI_FAC;
	return dooya_set_wifi_status_to_flash(d_wifi_status);
}

/***************************************************************************************/
int32_t dooya_set_ota_number_to_flash(uint32_t status)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(space1, NVS_READWRITE, &handle_1);

	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}

	err = nvs_set_u32(handle_1, ota_number, status);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	

	nvs_commit(handle_1);
	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	return 0;

}

int32_t dooya_get_ota_number_from_flash(uint32_t *status)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(space1, NVS_READWRITE, &handle_1);

	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}

	err = nvs_get_u32(handle_1, ota_number, status);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	
	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	return 0;

}
int32_t dooya_set_ota_flag_to_flash(uint32_t status)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(space1, NVS_READWRITE, &handle_1);

	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}

	err = nvs_set_u32(handle_1, ota_flag, status);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	

	nvs_commit(handle_1);
	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	return 0;

}

int32_t dooya_get_ota_flag_from_flash(uint32_t *status)
{
	nvs_handle handle_1;
	esp_err_t err = nvs_open(space1, NVS_READWRITE, &handle_1);

	//打开数据库，打开一个数据库就相当于会返回一个句柄
	if (err != ESP_OK)
	{
		return -1;
	}

	err = nvs_get_u32(handle_1, ota_flag, status);

	if (err != ESP_OK)
	{
		nvs_close(handle_1);
		return -1;
	}	
	//关闭数据库，关闭面板！
	nvs_close(handle_1);
	return 0;

}



