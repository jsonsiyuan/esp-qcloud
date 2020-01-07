#include "device_info.h"
#include "cJSON.h"
#include "device_uart.h"
#include "device_uart_send.h"


uint8_t dooya_post_flag=1;
uint8_t dooya_post_flag_motor_status=0;
uint8_t dooya_CurtainPosition_data=0;

user_dev_status_t user_dev_status=
{
	.CurtainPosition=0,
	.CurtainOperation=MOTOR_STOP,
	.Error_status=0,
	.SetDir=DIR_POSITIVE,
};

user_dev_status_t * _g_pDEVMgr = &user_dev_status;

user_dev_status_t *  dooya_get_dev_info(void)
{
	return _g_pDEVMgr;
}

CurtainOperation_T dooya_get_dev_CurtainOperation(void  )
{
	user_dev_status_t * dev_tmp=dooya_get_dev_info();
	return dev_tmp->CurtainOperation;
}

int dooya_get_dev_SetDir(void  )
{
	user_dev_status_t * dev_tmp=dooya_get_dev_info();
	return dev_tmp->SetDir;
}

int dooya_get_dev_CurtainPosition(void)
{
	user_dev_status_t *dev_tmp=dooya_get_dev_info();
	return dev_tmp->CurtainPosition;
}

int dooya_get_dev_error(void)
{
	user_dev_status_t *dev_tmp=dooya_get_dev_info();
	return dev_tmp->Error_status;
}


void dooya_set_dev_CurtainOperation(CurtainOperation_T  data)
{   
	user_dev_status_t * dev_tmp=dooya_get_dev_info();
	if(dev_tmp->CurtainOperation!=data)
	{
		if(data!=MOTOR_STOP)
		{
			dooya_post_flag=1;
		}
		else
		

		{
			dooya_post_flag_motor_status=1;
		}
		
	}
	dev_tmp->CurtainOperation=data;
}

void dooya_set_dev_SetDir(int data  )
{
	user_dev_status_t * dev_tmp=dooya_get_dev_info();
	if(dev_tmp->SetDir!=data)
	{
		//dooya_post_flag=1;
	}
	dev_tmp->SetDir=data;
}

int CurtainPosition_tmp=0;
int CurtainPosition_number=0;
void dooya_set_dev_CurtainPosition(int data)
{
	user_dev_status_t *dev_tmp=dooya_get_dev_info();
	if(CurtainPosition_tmp!=data)
	{
		CurtainPosition_tmp=data;
		CurtainPosition_number=0;
	}
	else if(CurtainPosition_tmp==data)
	{
		CurtainPosition_number++;
		if(CurtainPosition_number>1)
		{
			CurtainPosition_number=0;
			//if(dev_tmp->CurtainPosition!=data)
			if(my_abs(dev_tmp->CurtainPosition-data)>3)
			{
				dooya_post_flag=1;
				dooya_CurtainPosition_data=0xff;
				dev_tmp->CurtainPosition=data;
			}
			else
			{
				dooya_CurtainPosition_data=0xff;
			}


			
		}
	}
	
}
void dooya_set_dev_CurtainPosition_dec(int data)
{
	user_dev_status_t *dev_tmp=dooya_get_dev_info();
	if(my_abs(dev_tmp->CurtainPosition-data)>3)
	//if(dev_tmp->CurtainPosition!=data)
	{
		dooya_post_flag=1;
		CurtainPosition_tmp=0xff;
		dev_tmp->CurtainPosition=data;
	}
	
}

void dooya_set_dev_error(int data)
{
	user_dev_status_t *dev_tmp=dooya_get_dev_info();
	dev_tmp->Error_status=data;
}
int my_abs(int number)

{

   return (number>= 0 ? number : -number);

}


