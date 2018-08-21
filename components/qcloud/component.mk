#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := \
	qcloud-iot-sdk-embedded-c/src/sdk-impl \
	qcloud-iot-sdk-embedded-c/src/sdk-impl/exports \
	qcloud-iot-sdk-embedded-c/src/mqtt/include \
	qcloud-iot-sdk-embedded-c/src/utils/include \
	qcloud-iot-sdk-embedded-c/src/device/include \
	port/include 

COMPONENT_SRCDIRS := \
	qcloud-iot-sdk-embedded-c/src/sdk-impl \
	qcloud-iot-sdk-embedded-c/src/platform/os/linux \
	qcloud-iot-sdk-embedded-c/src/platform/ssl/mbedtls \
	qcloud-iot-sdk-embedded-c/src/mqtt/src \
	qcloud-iot-sdk-embedded-c/src/utils/src \
	qcloud-iot-sdk-embedded-c/src/device/src 
	