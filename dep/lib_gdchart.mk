
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gdchart
PRJ_TYPE := lib
PRJ_INCS := 
PRJ_LIBS := 
PRJ_DEFS :=
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
LOC_BLD_def := cpp
LOC_INC_def := $(CFG_LIBROOT)/gdchart
LOC_SRC_def := $(CFG_LIBROOT)/gdchart
LOC_EXC_def := gdc_samp1 gdc_samp2
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gd
LOC_CXX_gd := c
LOC_BLD_gd := cpp
LOC_INC_gd := $(CFG_LIBROOT)/gdchart/gd
LOC_SRC_gd := $(CFG_LIBROOT)/gdchart/gd
LOC_EXC_gd := gddemo giftogd webgif
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

