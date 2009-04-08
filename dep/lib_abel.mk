
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := abel
PRJ_TYPE := lib
PRJ_INCS := 
PRJ_LIBS := 
PRJ_DEFS :=
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifneq ($(PROC),i386)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_BLD_def := cpp
LOC_INC_def := $(CFG_LIBROOT)/abel/lib
LOC_SRC_def := $(CFG_LIBROOT)/abel/lib
LOC_EXC_def := serial
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

