#ifndef DEVICE_FLASH_H
#define DEVICE_FLASH_H
typedef enum	
{
	D_WIFI_SMART_CONFIG=0,
	D_WIFI_STA,
	D_WIFI_SOFTAP,
	D_WIFI_FAC,
}D_WIFI_STATUS_T;


int32_t esp_qcloud_wifi_clear_info(void);
int32_t esp_qcloud_wifi_save_info(uint8_t*ssid,uint8_t*password);
int32_t dooya_get_wifi_ssid_from_flash(char *ssid,size_t len);
int32_t dooya_get_wifi_password_from_flash(char *password,size_t len);

int32_t dooya_get_wifi_status_from_flash(uint8_t *status);
int32_t dooya_set_wifi_status_to_flash(uint8_t status);
int32_t dooya_set_wifi_STA(void);
int32_t dooya_set_wifi_softAP(void);
int32_t dooya_set_wifi_FAC(void);

int32_t dooya_set_ota_number_to_flash(uint32_t status);
int32_t dooya_get_ota_number_from_flash(uint32_t *status);
int32_t dooya_set_ota_flag_to_flash(uint32_t status);
int32_t dooya_get_ota_flag_from_flash(uint32_t *status);





#endif


