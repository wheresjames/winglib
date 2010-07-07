
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_ffmpeg
PRJ_DEPS := ffmpeg
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus ffmpeg
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage jpeg png tiff zlib
PRJ_DEFS := HAVE_AV_CONFIG_H=1 __STDC_CONSTANT_MACROS
PRJ_LIBP :=
PRJ_OSLB :=

PRJ_SUBROOT := _sqmod

PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	ifeq ($(BUILD),vs)
		PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/windows/vs xlibs $(PRJ_INCS)
		PRJ_PLIB := libgcc.a libmingwex.a
		ifdef DBG
			PRJ_PLIB := $(PRJ_PLIB) ffmpeg_d.lib
		else
			PRJ_PLIB := $(PRJ_PLIB) ffmpeg.lib
		endif
		PRJ_LIBP := $(CFG_LIBROOT)/xlibs
		ifneq ($(PROC),arm)
			PRJ_OSLB := $(PRJ_OSLB) vfw32.lib
		endif
	else
		PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/windows/gcc $(PRJ_INCS) zlib
		PRJ_LIBS := $(PRJ_LIBS) ffmpeg zlib
		ifneq ($(PROC),arm)
			PRJ_OSLB := $(PRJ_OSLB) vfw32
		endif
	endif
else
	PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/posix $(PRJ_INCS)
	PRJ_LIBS := $(PRJ_LIBS) ffmpeg
	PRJ_OSLB := $(PRJ_OSLB) z
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


