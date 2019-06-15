set(COMPONENT_ADD_INCLUDEDIRS
    platform/sdk-impl
    platform/sdk-impl/exports
    qcloud-iot-sdk-embedded-c/src/mqtt/include
    qcloud-iot-sdk-embedded-c/src/utils/digest
    qcloud-iot-sdk-embedded-c/src/utils/farra
    qcloud-iot-sdk-embedded-c/src/device/include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES "mbedtls")
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS
    platform/sdk-impl/qcloud_iot_sdk_impl.c
    platform/os/espressif/HAL_OS_linux.c
    platform/os/espressif/HAL_TCP_linux.c
    platform/os/espressif/HAL_Timer_linux.c
    platform/os/espressif/HAL_UDP_linux.c
    platform/ssl/mbedtls/HAL_DTLS_mbedtls.c
    platform/ssl/mbedtls/HAL_TLS_mbedtls.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client_common.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client_connect.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client_net.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client_publish.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client_subscribe.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client_unsubscribe.c
    qcloud-iot-sdk-embedded-c/src/mqtt/src/mqtt_client_yield.c
    qcloud-iot-sdk-embedded-c/src/utils/digest/utils_base64.c
    qcloud-iot-sdk-embedded-c/src/utils/digest/utils_hmac.c
    qcloud-iot-sdk-embedded-c/src/utils/digest/utils_md5.c
    qcloud-iot-sdk-embedded-c/src/utils/digest/utils_sha1.c
    qcloud-iot-sdk-embedded-c/src/utils/farra/utils_httpc.c
    qcloud-iot-sdk-embedded-c/src/utils/farra/utils_list.c
    qcloud-iot-sdk-embedded-c/src/utils/farra/utils_net.c
    qcloud-iot-sdk-embedded-c/src/utils/farra/utils_timer.c
    qcloud-iot-sdk-embedded-c/src/device/src/ca.c
    qcloud-iot-sdk-embedded-c/src/device/src/device.c)

register_component()
component_compile_options(-DAUTH_MODE_CERT)
