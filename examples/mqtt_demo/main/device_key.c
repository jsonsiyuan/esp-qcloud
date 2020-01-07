#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_system.h"

#include "device_key.h"


typedef enum	
{
	KEY_LOW=0,
	KEY_HIGH,
}KEY_MODE_T;


#define GPIO_INPUT_KEY_1     4
#define GPIO_INPUT_KEY_2     5
#define GPIO_INPUT_KEY_3     16
#define GPIO_INPUT_KEY_4     2

#define GPIO_INPUT_KEY_SEL  ((1ULL<<GPIO_INPUT_KEY_1) |(1ULL<<GPIO_INPUT_KEY_2) |(1ULL<<GPIO_INPUT_KEY_3) | (1ULL<<GPIO_INPUT_KEY_4))






static void dooya_key_init(void)
{
	gpio_config_t io_conf;

	//interrupt of rising edge
	io_conf.intr_type = GPIO_INTR_DISABLE;
	//bit mask of the pins, use GPIO4/5 here
	io_conf.pin_bit_mask = GPIO_INPUT_KEY_SEL;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//enable pull-up mode
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);


}

static int dooya_get_key1_status(void)
{
	return gpio_get_level(GPIO_INPUT_KEY_1);

}

static int dooya_get_key2_status(void)
{
	return gpio_get_level(GPIO_INPUT_KEY_2);

}
static int dooya_get_key3_status(void)
{
	return gpio_get_level(GPIO_INPUT_KEY_3);

}
static int dooya_get_key4_status(void)
{
	return gpio_get_level(GPIO_INPUT_KEY_4);

}


static int dooya_read_key_value(void)
{
	unsigned char key_flag=0;
	if(KEY_LOW==dooya_get_key1_status())
	{
		key_flag=key_flag|0x01;
	}
	if(KEY_LOW==dooya_get_key2_status())
	{
		key_flag=key_flag|0x02;
	}
	if(KEY_LOW==dooya_get_key3_status())
	{
		key_flag=key_flag|0x04;
	}
	if(KEY_LOW==dooya_get_key4_status())
	{
		key_flag=key_flag|0x08;
	}
	return key_flag;
}










static void dooya_key_handle(void *pvParameters)
{
	unsigned char key_flag_tmp;
	unsigned char key_flag_tmp1;
	while(1)
	{
		key_flag_tmp=0;
		key_flag_tmp=dooya_read_key_value();
		if(0!=key_flag_tmp)
		{
			vTaskDelay(50/portTICK_RATE_MS);
			
			key_flag_tmp1=0;
			key_flag_tmp1=dooya_read_key_value();
			
			if(key_flag_tmp1==key_flag_tmp)
			{
				

				
			}
			
		}

		vTaskDelay(30/portTICK_RATE_MS);
	}
}



uint8_t dooya_create_key_thread(void)
{
	printf("dooya_create_key_thread\r\n");
	dooya_key_init();
	xTaskCreate(dooya_key_handle, "key", 512, NULL, 3, NULL);

	
	return 0;
}

