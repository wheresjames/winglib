
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_SQRL := service
PRJ_NAME := sntpsync
PRJ_DESC := SNTP Sync
PRJ_DEFS := 
PRJ_WINL := winmm strmiids
PRJ_VSLB := dshowbase
PRJ_RESD := res

SQMOD_STATIC :=

PRJ_LIBROOT := ../../../../deftbuild/v1

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# Add resources
#-------------------------------------------------------------------
ifeq ($(OS),windows)
export LOC_TAG := rc
LOC_CXX_rc := rc
LOC_BLD_rc := rc
LOC_SRC_rc := res
include $(PRJ_LIBROOT)/build.mk
endif

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

