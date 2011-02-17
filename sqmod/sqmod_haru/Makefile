
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_haru
PRJ_DEPS := haru
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			haru/include

PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel haru cximage jpeg png tiff zlib
PRJ_DEFS := 
 
PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_SUBROOT := _sqmod

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------

export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


