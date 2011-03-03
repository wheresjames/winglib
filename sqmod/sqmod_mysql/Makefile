
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_mysql
PRJ_DEPS := mysql
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage jpeg png tiff zlib

PRJ_SUBROOT := _sqmod

PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	ifeq ($(BUILD),gcc)
		ifeq ($(PROC),x86)
			UNSUPPORTED := x86-gcc-windows does not link for whatever reason
		endif
	endif
endif

ifneq ($(UNSUPPORTED),)
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)

	ifeq ($(BUILD),vs)
		PRJ_WINL := libmysql
	else
		PRJ_WINL := mysql
	endif

	PRJ_INCS := $(PRJ_INCS) mysql/include
	ifeq ($(OS),win64)
		PRJ_LIBP := $(PRJ_LIBP) $(CFG_LIBROOT)/mysql/x64/lib/
	else
		PRJ_LIBP := $(PRJ_LIBP) $(CFG_LIBROOT)/mysql/x86/lib
	endif
else
	PRJ_LIBP := $(PRJ_LIBP) /usr/lib/mysql
	PRJ_OSLB := mysqlclient
endif

ifeq ($(TOOLS),mingw32)
	PRJ_EXSY := LIBMYSQL_NULL_THUNK_DATA
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

endif

