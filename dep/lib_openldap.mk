
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := openldap
PRJ_TYPE := lib
PRJ_INCS := openldap/include
PRJ_LIBS := 
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(BUILD_CURL),)
UNSUPPORTED := Set BUILD_CURL=1 to build this project
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)
	PRJ_DEFS := $(PRJ_DEFS) HAVE_WINSOCK2
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
#LOC_BLD_def := cpp
LOC_SRC_def := $(CFG_LIBROOT)/openldap/libraries/libldap
LOC_EXC_def := 
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif


