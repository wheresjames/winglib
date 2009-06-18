
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := soup
PRJ_TYPE := lib
PRJ_INCS := libsoup libgio
PRJ_LIBS := 
PRJ_DEFS :=
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
UNSUPPORTED := PLATFORM=$(PLATFORM) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_SRC_def := $(CFG_LIBROOT)/libsoup/libsoup
LOC_EXC_def := 			   
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

