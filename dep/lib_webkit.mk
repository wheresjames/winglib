
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := WebKit
PRJ_TYPE := lib
PRJ_INCS := WebKit wxWidgets/include freetype/include \
			WebKit/WebCore WebKit/WebCore/editing WebKit/WebCore/dom \
			WebKit/WebCore/platform WebKit/WebCore/platform/text WebKit/WebCore/icu \
			WebKit/WebCore/platform/graphics WebKit/WebCore/page WebKit/WebCore/rendering \
			WebKit/WebCore/loader WebKit/WebCore/rendering/style WebKit/WebCore/css \
			WebKit/WebCore/platform/network WebKit/WebCore/platform/network/soup \
			WebKit/WebCore/html WebKit/WebCore/platform/graphics/transforms \
			WebKit/WebCore/platform/animation WebKit/WebCore/platform/graphics/wx \
			WebKit/JavaScriptCore WebKit/JavaScriptCore/jit WebKit/JavaScriptCore/interpreter \
			WebKit/JavaScriptCore/runtime
PRJ_SYSI := /usr/include/cairo
PRJ_LIBS := 
PRJ_DEFS := USE_FREETYPE

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(BUILD_WEBKIT),)
UNSUPPORTED := Set make option BUILD_WEBKIT=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(OS),win32)
	PRJ_LIBS := $(PRJ_LIBS) wxWidgets
	PRJ_DEFS := 
else
	PRJ_EXTC := `wx-config --cppflags`
	PRJ_EXTL := `wx-config --libs`
#	PRJ_OSLB := X11 GLU Xxf86vm
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

#----------------------------
# -- JavaScriptCore
#----------------------------

export LOC_TAG := wc_ac
#LOC_INC_wc_ac := $(CFG_LIBROOT)/WebKit/WebCore/accessibility
LOC_SRC_wc_ac := $(CFG_LIBROOT)/WebKit/WebCore/accessibility
#LOC_LST_wc_ac := AllInOneFile
include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := jsc_api
#LOC_INC_jsc_api := $(CFG_LIBROOT)/WebKit/JavaScriptCore
#LOC_SRC_jsc_api := $(CFG_LIBROOT)/WebKit/JavaScriptCore
#LOC_LST_jsc_api := AllInOneFile
#include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

