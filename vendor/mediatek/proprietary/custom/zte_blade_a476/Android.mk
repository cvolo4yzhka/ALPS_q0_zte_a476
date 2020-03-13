LOCAL_PATH:= $(call my-dir)
ifeq ($(MTK_PROJECT), zte_blade_a476)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
