#
# Component Makefile
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS := \
	platform/sdk-impl \
	platform/sdk-impl/exports \
	qcloud-iot-sdk-embedded-c/src/mqtt/include \
	qcloud-iot-sdk-embedded-c/src/utils/digest \
	qcloud-iot-sdk-embedded-c/src/utils/farra \
	qcloud-iot-sdk-embedded-c/src/device/include

COMPONENT_SRCDIRS := \
	platform/sdk-impl \
	platform/os/espressif \
	platform/ssl/mbedtls \
	qcloud-iot-sdk-embedded-c/src/mqtt/src \
	qcloud-iot-sdk-embedded-c/src/utils/digest \
	qcloud-iot-sdk-embedded-c/src/utils/farra \
	qcloud-iot-sdk-embedded-c/src/device/src 
