
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_ssh2
PRJ_DEPS := libssh2
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			libssh2/include openssl/include zlib
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage jpeg png tiff zlib \
			ssh2 openssl_a openssl_b openssl_a openssl_b
PRJ_DEFS := LIBSSH2DEBUG_USERCALLBACK
PRJ_WINL := strmiids winmm
PRJ_VSLB := dshowbase
 
PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_SUBROOT := _sqmod

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := $(PRJ_INCS) libssh2/win32
else
	PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/libssh2/inc/posix $(PRJ_INCS)
endif

#-------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


