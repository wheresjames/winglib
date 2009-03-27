
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irrlicht-es
PRJ_TYPE := lib
PRJ_INCS := irrlicht-es/source/Irrlicht/zlib
PRJ_LIBS := 

PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

ifeq ($(OS),win32)
CFG_CFLAGS := $(CFG_CFLAGS) /DIRRLICHT_EXPORTS
endif

export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/irrlicht-es/include
LOC_SRC_def := $(CFG_LIBROOT)/irrlicht-es/source/Irrlicht
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


