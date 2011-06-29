
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_mimetic
PRJ_DEPS := mimetic
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			mimetic

PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage jpeg png tiff zlib \
		    mimetic
			
PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_SUBROOT := _sqmod

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PROC),arm)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(BUILD),vs)
	PRJ_DEFS := uint32_t=unsigned
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

endif
