
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := j2k
PRJ_TYPE := lib
PRJ_INCS := 
PRJ_LIBS := 

ifeq ($(OS),win32)
CFLAGS := $(CFLAGS) /DLIBJ2K_EXPORTS
endif

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_INC_def := $(CFG_LIBROOT)/j2k
LOC_SRC_def := $(CFG_LIBROOT)/j2k
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


