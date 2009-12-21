
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gsasl
PRJ_TYPE := lib
PRJ_INCS := libgsasl libgsasl/gl
PRJ_LIBS := 
PRJ_DEFS := GSASL_API= HAVE_CONFIG_H LOCALEDIR=\"\"

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := winglib/dep/etc/gsasl/inc/windows libgsasl/win32/include $(PRJ_INCS)
else
	PRJ_INCS := winglib/dep/etc/gsasl/inc/posix $(PRJ_INCS)
endif

ifeq ($(BUILD),vs)
	PRJ_DEFS := $(PRJ_DEFS) RESTRICT=__restrict 
else
	PRJ_DEFS := $(PRJ_DEFS) RESTRICT=restrict 
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_SRC_def := $(CFG_LIBROOT)/libgsasl/src
LOC_EXC_def := 
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


