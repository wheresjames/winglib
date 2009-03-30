
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_irrlicht
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
		    irrlicht/include
PRJ_LIBS := sqbind oexlib squirrel sqstdlib sqplus irrlicht

ifneq ($(OS),win32)
	PRJ_OSLB := X11 GLU Xxf86vm
	PRJ_DEFS := $(PRJ_DEFS) _IRR_COMPILE_WITH_OPENGL_ _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
endif
 
PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_LIBROOT := ../../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


