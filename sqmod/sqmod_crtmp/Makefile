
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_crtmp
PRJ_DEPS := crtmp
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			openssl/include lua/src tinyxml\
			crtmp/sources/thelib/include crtmp/sources/common/include
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage jpeg png tiff \
			crtmp openssl_a openssl_b openssl_a openssl_b lua tinyxml zlib 
PRJ_DEFS := NET_SELECT HAS_LUA LITTLE_ENDIAN_BYTE_ALIGNED \
			HAS_PROTOCOL_TS HAS_PROTOCOL_RTMP
PRJ_LIBP :=
PRJ_OSLB := 
PRJ_WINL := shlwapi
 
PRJ_SUBROOT := _sqmod

PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	ifeq ($(BUILD),vs)
		PRJ_INCS := $(PRJ_INCS) $(CFG_LIB2BLD)/dep/etc/vs/inc/c99
	endif
	PRJ_OSLB := $(PRJ_OSLB) winmm 
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

