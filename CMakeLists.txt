set(COMPONENT_ADD_INCLUDEDIRS
    qcloud_iot_c_sdk/include 
    qcloud_iot_c_sdk/include/exports 
    qcloud_iot_c_sdk/sdk_src/internal_inc)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES "mbedtls")
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS
    qcloud_iot_c_sdk/sdk_src 
    qcloud_iot_c_sdk/platform)

register_component()
component_compile_options(-DAUTH_MODE_CERT)
