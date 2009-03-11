
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := R
PRJ_TYPE := lib
PRJ_INCS := 
PRJ_LIBS := 

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := cxx
LOC_BLD_def := cpp
LOC_INC_def := $(CFG_LIBROOT)/R/src/main
LOC_SRC_def := $(CFG_LIBROOT)/R/src/main
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


