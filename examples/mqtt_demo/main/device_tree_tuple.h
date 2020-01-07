#ifndef DEVICE_TREE_TUPLE_H
#define DEVICE_TREE_TUPLE_H


#include "qcloud_iot_export.h"


extern char QCLOUD_PRODUCT_ID[MAX_SIZE_OF_PRODUCT_ID+1];      
extern char QCLOUD_DEVICE_NAME[MAX_SIZE_OF_PRODUCT_SECRET+1];      
extern char QCLOUD_DEVICE_SECRET[MAX_SIZE_OF_DEVICE_SECRET+1];  
void dooya_qcloud_init(void);

#endif



