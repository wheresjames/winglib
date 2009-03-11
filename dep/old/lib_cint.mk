
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := cint
PRJ_TYPE := lib
PRJ_INCS := cint/include cint/lib/gcc3strm
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
LOC_INC_def := $(CFG_LIBROOT)/cint/inc
LOC_SRC_def := $(CFG_LIBROOT)/cint/src
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


