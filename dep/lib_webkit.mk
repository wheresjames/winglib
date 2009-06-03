
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := WebKit
PRJ_TYPE := lib
PRJ_INCS := WebKit WebKit/JavaScriptCore/API
PRJ_LIBS := 

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(BUILD_WEBKIT),)
UNSUPPORTED := Set make option BUILD_WEBKIT=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

#----------------------------
# -- JavaScriptCore
#----------------------------

export LOC_TAG := jsc_api
LOC_INC_jsc_api := $(CFG_LIBROOT)/WebKit/JavaScriptCore
LOC_SRC_jsc_api := $(CFG_LIBROOT)/WebKit/JavaScriptCore
LOC_LST_jsc_api := AllInOneFile
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

