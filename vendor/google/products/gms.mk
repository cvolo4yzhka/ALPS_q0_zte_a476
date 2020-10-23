###############################################################################
# GMS home folder location
# Note: we cannot use $(my-dir) in this makefile
ANDROID_PARTNER_GMS_HOME := vendor/google

$(call inherit-product, build/target/product/product_launched_with_p.mk)
# GMS mandatory core packages
GMS_PRODUCT_PACKAGES += \
    AndroidAutoStub \
    AndroidPlatformServices \
    ConfigUpdater \
    GoogleExtShared \
    GoogleFeedback \
    GoogleLocationHistory \
    GoogleOneTimeInitializer \
    GooglePackageInstaller \
    GooglePartnerSetup \
    GooglePrintRecommendationService \
    GoogleRestore \
    GoogleServicesFramework \
    GoogleCalendarSyncAdapter \
    GoogleContactsSyncAdapter \
    GoogleTTS \
    GmsCore \
    Phonesky \
    SetupWizard \
    WebViewGoogle \
    Wellbeing

# GMS mandatory libraries
PRODUCT_PACKAGES += com.google.android.maps.jar

# GMS common RRO packages
GMS_PRODUCT_PACKAGES += GmsConfigOverlay GmsGoogleDocumentsUIOverlay

# GMS common configuration files
GMS_PRODUCT_PACKAGES += \
    default_permissions_whitelist_google \
    privapp_permissions_google_system \
    privapp_permissions_google \
    split_permissions_google \
    preferred_apps_google \
    sysconfig_google \
    sysconfig_wellbeing \
    google_hiddenapi_package_whitelist \
    sysconfig_gmsexpress

#pre-load turbo
#$(call inherit-product-if-exists, vendor/google/products/turbo.mk)

# Overlay for GMS devices: default backup transport in SettingsProvider
PRODUCT_PACKAGE_OVERLAYS += $(ANDROID_PARTNER_GMS_HOME)/products/gms_overlay
#overlay def wallpaper
#PRODUCT_PACKAGE_OVERLAYS += $(ANDROID_PARTNER_GMS_HOME)/products/qt_beta_overlay

# Overlay for GoogleDialer
ifneq ($(strip $(MTK_TB_WIFI_3G_MODE)),WIFI_ONLY)
#PRODUCT_PACKAGE_OVERLAYS += $(ANDROID_PARTNER_GMS_HOME)/apps/GoogleDialer/overlay
endif

# GMS mandatory application packages
GMS_PRODUCT_PACKAGES += \
    Chrome \
    CalculatorGoogle
#    AssistantShell
#    Drive \
#    Gmail2 \
#    Duo \
#    Maps \
#    YTMusic \
#    Photos \
#    Velvet \
#    Videos \
#    YouTube \


# GMS comms suite
GMS_PRODUCT_PACKAGES += \
    CarrierServices
#    GoogleContacts

ifneq ($(strip $(MTK_TB_WIFI_3G_MODE)),WIFI_ONLY)
#GMS_PRODUCT_PACKAGES += \
#    GoogleDialer \
#    Messages

endif

# GMS optional application packages
#GMS_PRODUCT_PACKAGES += \
#    CalendarGoogle \
#    LatinImeGoogle

GMS_PRODUCT_PACKAGES += \
    Launcher3NoQsb
#    FilesGoogle \
#    SearchLauncherQRef

# Add GMS package into system product packages
PRODUCT_PACKAGES += $(GMS_PRODUCT_PACKAGES)

PRODUCT_SYSTEM_DEFAULT_PROPERTIES += \
    ro.boot.vendor.overlay.theme=com.android.internal.systemui.navbar.threebutton;com.android.theme.icon.roundedrect
#    ro.boot.vendor.overlay.theme=com.android.internal.systemui.navbar.threebutton;com.android.theme.icon.squircle
#com.android.theme.icon.square
#com.android.theme.icon.teardrop
#com.android.theme.icon.squircle
#com.android.theme.icon.roundedrect

# Add acsa property for CarrierServices
PRODUCT_PRODUCT_PROPERTIES += \
    ro.com.google.acsa=true

PRODUCT_PRODUCT_PROPERTIES += \
    ro.setupwizard.rotation_locked=true \
    setupwizard.theme=glif_v3_light \
    ro.opa.eligible_device=true \
    ro.com.google.gmsversion=10_202009
