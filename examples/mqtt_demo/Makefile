#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := esp-qcloud
export PROJECT_PATH := $(PWD)

EXTRA_COMPONENT_DIRS += $(realpath ../..)

CFLAGS += -DMBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED

include $(IDF_PATH)/make/project.mk
