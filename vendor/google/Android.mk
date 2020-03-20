ifdef BUILD_GMS_nik-kst
ifeq ($(strip $(BUILD_GMS_nik-kst)), yes)

include $(call all-subdir-makefiles)

GMS_PRODUCT_MODULES := $(strip $(foreach p,$(GMS_PRODUCT_PACKAGES),$(PACKAGES.$(p).OVERRIDES)))
$(info [BUILD_GMS_nik-kst] override $(GMS_PRODUCT_MODULES))

GMS_OVERRIDE_DIRS := \
  $(TARGET_OUT_APPS) \
  $(TARGET_OUT_APPS_PRIVILEGED) \
  $(TARGET_OUT_DATA_APPS) \
  $(TARGET_OUT_VENDOR_APPS) \
  $(TARGET_OUT_VENDOR_APPS_PRIVILEGED) \
  $(TARGET_OUT_VENDOR)/operator/app \
  $(TARGET_OUT_PRODUCT_APPS) \
  $(TARGET_OUT_PRODUCT_APPS_PRIVILEGED)

GMS_PRODUCT_FILES := \
  $(foreach d,$(GMS_OVERRIDE_DIRS),$(addprefix $(d)/,$(GMS_PRODUCT_MODULES))) \
  $(dir $(call module-installed-files,$(GMS_PRODUCT_MODULES)))

endif
endif


ifdef BUILD_GMS
ifeq ($(strip $(BUILD_GMS)), yes)

include $(call all-subdir-makefiles)

GMS_PRODUCT_MODULES := $(strip $(foreach p,$(GMS_PRODUCT_PACKAGES),$(PACKAGES.$(p).OVERRIDES)))
$(info [BUILD_GMS] override $(GMS_PRODUCT_MODULES))

GMS_OVERRIDE_DIRS := \
  $(TARGET_OUT_APPS) \
  $(TARGET_OUT_APPS_PRIVILEGED) \
  $(TARGET_OUT_DATA_APPS) \
  $(TARGET_OUT_VENDOR_APPS) \
  $(TARGET_OUT_VENDOR_APPS_PRIVILEGED) \
  $(TARGET_OUT_VENDOR)/operator/app \
  $(TARGET_OUT_PRODUCT_APPS) \
  $(TARGET_OUT_PRODUCT_APPS_PRIVILEGED)

GMS_PRODUCT_FILES := \
  $(foreach d,$(GMS_OVERRIDE_DIRS),$(addprefix $(d)/,$(GMS_PRODUCT_MODULES))) \
  $(dir $(call module-installed-files,$(GMS_PRODUCT_MODULES)))

GMS_PRODUCT_CLEAN := $(GMS_PRODUCT_FILES)

ifeq ($(strip $(WITH_DEXPREOPT_BOOT_IMG_ONLY)),true)
  GMS_ODEX_CLEAN := yes
else ifeq ($(strip $(WITH_DEXPREOPT)),false)
  GMS_ODEX_CLEAN := yes
endif

ifeq ($(GMS_ODEX_CLEAN),yes)
GMS_ODEX_FILES := $(shell find $(TARGET_OUT) -name "*.odex")
ifneq ($(strip $(GMS_ODEX_FILES)),)
$(info GMS_ODEX_FILES = $(GMS_ODEX_FILES))
GMS_PRODUCT_CLEAN += $(dir $(patsubst %/,%,$(dir $(GMS_ODEX_FILES))))
endif
endif

GMS_PRODUCT_CLEAN := $(strip $(wildcard $(sort $(GMS_PRODUCT_CLEAN))))
ifneq ($(GMS_PRODUCT_CLEAN),)
$(info [BUILD_GMS] rm -rf $(GMS_PRODUCT_CLEAN))
$(shell rm -rf $(GMS_PRODUCT_CLEAN))
endif
endif
endif

