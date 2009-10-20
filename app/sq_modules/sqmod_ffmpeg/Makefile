
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_ffmpeg
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus ffmpeg
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage ffmpeg jpeg png tiff zlib			
PRJ_DEFS := HAVE_AV_CONFIG_H=1
PRJ_LIBP :=
PRJ_OSLB :=
 
PRJ_SUBROOT := sqmod

PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_LIBROOT := ../../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifndef BUILD_FFMPEG
UNSUPPORTED := Set make option BUILD_FFMPEG=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)
	PRJ_INCS := winglib/dep/etc/ffmpeg/inc/windows $(PRJ_INCS)
	ifeq ($(BUILD),vs)
		PRJ_INCS := winglib/dep/etc/ffmpeg/inc/windows/vs $(PRJ_INCS)
		PRJ_OSLB := $(PRJ_OSLB) libgcc.a libmingwex.a
	endif
else
	PRJ_INCS := winglib/dep/etc/ffmpeg/inc/posix $(PRJ_INCS)
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
