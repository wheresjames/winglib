
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := curl
PRJ_TYPE := lib
PRJ_INCS := curl/src openldap/include
PRJ_LIBS := 
PRJ_DEFS := HAVE_CONFIG_H CURL_STATICLIB CURL_DISABLE_LDAP
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(BUILD_CURL),)
UNSUPPORTED := Set BUILD_CURL=1 to build this project
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
#LOC_BLD_def := cpp
LOC_INC_def := $(CFG_LIBROOT)/curl/include
LOC_SRC_def := $(CFG_LIBROOT)/curl/lib
LOC_EXC_def := 
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif


