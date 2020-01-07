#ifndef __DEVICE_LED_H__
#define __DEVICE_LED_H__

#include <stdio.h>
#include <stdint.h>
typedef enum	
{
	LED_ALWAYS_OPEN=0,
	LED_ALWAYS_CLOSE,
	LED_TAGGLE,
}LED_MODE_T;

typedef enum	
{
	LED_CLOSE=0,
	LED_OPEN,
}LED_ACTION_T;


typedef  struct 
{
	LED_MODE_T   led_mode;
	LED_ACTION_T led_active;
	uint8_t led_HZ;
}LED_STATUS_T;



extern LED_STATUS_T led_g_status;

extern LED_STATUS_T led_r_status;


uint8_t dooya_create_led_thread(void);
void dooya_set_led_r_status(LED_MODE_T mode,uint8_t hz);
void dooya_set_led_g_status(LED_MODE_T mode,uint8_t hz);


#endif


