
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_ffmpeg
PRJ_DEPS := ffmpeg x264
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

ifeq ($(PROC),arm)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(BUILD)$(PROC),vsx64)
	UNSUPPORTED := PROC=$(PROC) + BUILD=$(BUILD) is not supported
	include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)
	ifeq ($(BUILD),vs)
		PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/windows/vs xlibs $(PRJ_INCS)
		PRJ_PLIB := ffmpeg_a ffmpeg_b ffmpeg_c x264
		ifeq ($(OS),win64)
			PRJ_LIBP := $(PRJ_LIBP) $(CFG_LIBROOT)/xlibs/x64
		else
			PRJ_LIBP := $(PRJ_LIBP) $(CFG_LIBROOT)/xlibs/x86
			PRJ_WINX := libgcc.a libmingwex.a
		endif
	else
		PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/windows/gcc $(PRJ_INCS) zlib
		PRJ_LIBS := $(PRJ_LIBS) ffmpeg_a ffmpeg_b ffmpeg_c ffmpeg_a ffmpeg_b ffmpeg_c x264 zlib
	endif
else
	PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/posix $(PRJ_INCS)
	PRJ_LIBS := $(PRJ_LIBS) ffmpeg_a ffmpeg_b ffmpeg_c ffmpeg_a ffmpeg_b ffmpeg_c x264 zlib
	PRJ_OSLB := $(PRJ_OSLB)
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

endif
