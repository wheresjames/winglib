
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_freenect
PRJ_DEPS := libfreenect
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			libfreenect/include libfreenect/src
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel jpeg png zlib tiff \
			freenect
PRJ_DEFS := 
PRJ_WINL := setupapi
 
PRJ_SUBROOT := _sqmod

PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	ifeq ($(BUILD),vs)
		PRJ_INCS := $(PRJ_INCS) $(CFG_LIB2BLD)/dep/etc/vs/inc/c99 libusb-win32/msvc
		PRJ_DEFS := ssize_t=unsigned
	endif
	PRJ_INCS := $(PRJ_INCS) libusb-win32/libusb
	PRJ_LIBS := $(PRJ_LIBS) usb-win32
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


