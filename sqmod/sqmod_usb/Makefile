
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_usb
PRJ_DEPS := libusb-win32
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel jpeg png zlib tiff 
PRJ_DEFS := 
 
PRJ_SUBROOT := _sqmod

PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := $(PRJ_INCS) libusb-win32/libusb libusb-win32/msvc
	PRJ_LIBS := $(PRJ_LIBS) usb-win32
	ifeq ($(BUILD),vs)
		PRJ_OSLB := Setupapi.lib
	else
		PRJ_OSLB := setupapi
	endif
else
	PRJ_INCS := $(PRJ_INCS) libusb/libusb
	PRJ_LIBS := $(PRJ_LIBS) usb
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


