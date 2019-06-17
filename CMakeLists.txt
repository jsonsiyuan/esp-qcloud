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

set(COMPONENT_SRCDIRS
    platform/sdk-impl
    platform/os/espressif
    platform/ssl/mbedtls
    qcloud-iot-sdk-embedded-c/src/mqtt/src
    qcloud-iot-sdk-embedded-c/src/utils/digest
    qcloud-iot-sdk-embedded-c/src/utils/farra
    qcloud-iot-sdk-embedded-c/src/device/src)

register_component()
component_compile_options(-DAUTH_MODE_CERT)
