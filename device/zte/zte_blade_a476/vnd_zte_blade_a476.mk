MTK_TARGET_PROJECT := zte_blade_a476
MTK_BASE_PROJECT := zte_blade_a476
ifndef MTK_TARGET_PROJECT_FOLDER
MTK_TARGET_PROJECT_FOLDER := $(LOCAL_PATH)
endif
MTK_PROJECT_FOLDER := $(MTK_TARGET_PROJECT_FOLDER)

include $(MTK_TARGET_PROJECT_FOLDER)/ProjectConfig.mk
include $(wildcard $(MTK_TARGET_PROJECT_FOLDER)/RuntimeSwitchConfig.mk)
$(call inherit-product, $(MTK_TARGET_PROJECT_FOLDER)/device.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/runtime_libart.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/handheld_vendor.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony_vendor.mk)
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote32

ifndef SYS_TARGET_PROJECT
PRODUCT_BUILD_SYSTEM_IMAGE := false
PRODUCT_BUILD_PRODUCT_IMAGE := false
endif
PRODUCT_LOCALES := ru_RU en_US
PRODUCT_MANUFACTURER := ZTE
PRODUCT_NAME := vnd_zte_blade_a476
PRODUCT_DEVICE := $(strip $(MTK_BASE_PROJECT))
PRODUCT_MODEL := ZTE Blade A476
PRODUCT_POLICY := android.policy_phone
PRODUCT_BRAND := ZTE
BUILD_FINGERPRINT := ZTE/ZTE_Blade_A476/ZTE_Blade_A476:5.1/LMY47D/20151127.162449:user/release-keys

ifeq ($(TARGET_BUILD_VARIANT), eng)
KERNEL_DEFCONFIG ?= zte_blade_a476_defconfig
else
KERNEL_DEFCONFIG ?= zte_blade_a476_defconfig
endif
PRELOADER_TARGET_PRODUCT ?= zte_blade_a476
LK_PROJECT ?= zte_blade_a476
TRUSTY_PROJECT ?= zte_blade_a476
