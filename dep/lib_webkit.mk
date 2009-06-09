
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := WebKit
PRJ_TYPE := lib
PRJ_INCS := wxWidgets/include freetype/include pthreads cairo/src \
			WebKit WebKit/DerivedSources \
			WebKit/WebCore WebKit/WebCore/accessibility WebKit/WebCore/bindings/js \
			WebKit/WebCore/bridge WebKit/WebCore/bridge/c WebKit/WebCore/css WebKit/WebCore/dom \
			WebKit/WebCore/editing WebKit/WebCore/history WebKit/WebCore/html WebKit/WebCore/inspector \
			WebKit/WebCore/loader WebKit/WebCore/loader/appcache WebKit/WebCore/loader/archive WebKit/WebCore/loader/icon \
			WebKit/WebCore/page WebKit/WebCore/page/animation WebKit/WebCore/platform WebKit/WebCore/platform/animation \
			WebKit/WebCore/platform/graphics WebKit/WebCore/platform/graphics/filters WebKit/WebCore/platform/graphics/transforms \
			WebKit/WebCore/platform/network WebKit/WebCore/platform/text WebKit/WebCore/plugins WebKit/WebCore/rendering \
			WebKit/WebCore/rendering/style WebKit/WebCore/workers WebKit/WebCore/xml WebKit/WebCore/bindings/js \
			WebKit/WebCore/platform/sql WebKit/WebCore/storage WebKit/WebCore/svg WebKit/WebCore/svg/animation \
			WebKit/WebCore/svg/graphics WebKit/WebCore/svg/graphics/filters \
			\
			WebKit/JavaScriptCore WebKit/JavaScriptCore/API WebKit/JavaScriptCore/ForwardingHeaders WebKit/JavaScriptCore/interpreter \
			WebKit/JavaScriptCore/bytecode WebKit/JavaScriptCore/bytecompiler WebKit/JavaScriptCore/debugger WebKit/JavaScriptCore/jit \
			WebKit/JavaScriptCore/pcre WebKit/JavaScriptCore/profiler WebKit/JavaScriptCore/runtime WebKit/JavaScriptCore/wrec \
			WebKit/JavaScriptCore/assembler WebKit/JavaScriptCore/wtf/unicode WebKit/JavaScriptCore/yarr WebKit/JavaScriptCore/pcre \
			WebKit/JavaScriptCore/parser WebKit/JavaScriptCore/runtime WebKit/JavaScriptCore/wtf WebKit/JavaScriptCore/icu \
			\
			WebKit/WebCore/icu \

PRJ_SYSI := 
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

ifeq ($(PLATFORM),windows)
	PRJ_LIBS := $(PRJ_LIBS) wxWidgets
	PRJ_DEFS := 
	
	PRJ_INCS := winglib/dep/etc/WebKit/inc/windows winglib/dep/etc/cairo/inc/windows $(PRJ_INCS) \
				WebKit/WebKit/win WebKit/WebKit/win/WebCoreSupport WebKit/WebKit/win/webkit \
				WebKit/WebCore/platform/win WebKit/WebCore/platform/graphics/win \
				WebKit/WebCore/accessibility/win WebKit/WebCore/loader/win WebKit/WebCore/page/win \
				WebKit/WebCore/platform/graphics/cairo WebKit/WebCore/platform/network/curl
#				WebKit/WebCore/platform/network/soup

else
	PRJ_EXTC := `wx-config --cppflags`
	PRJ_EXTL := `wx-config --libs`
#	PRJ_OSLB := X11 GLU Xxf86vm

	PRJ_INCS := winglib/dep/etc/WebKit/inc/posix winglib/dep/etc/cairo/inc/posix $(PRJ_INCS) \
				WebKit/WebKit/gtk WebKit/WebKit/gtk/WebCoreSupport WebKit/WebKit/gtk/webkit \
				WebKit/WebCore/platform/gtk WebKit/WebCore/platform/graphics/gtk \
				WebKit/WebCore/accessibility/gtk WebKit/WebCore/loader/gtk WebKit/WebCore/page/gtk \
				WebKit/WebCore/platform/graphics/cairo WebKit/WebCore/platform/network/soup

endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

#----------------------------
# -- JavaScriptCore
#----------------------------

export LOC_TAG := wc_ac
LOC_SRC_wc_ac := $(CFG_LIBROOT)/WebKit/WebCore/accessibility
include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := wc_dom
#LOC_SRC_wc_dom := $(CFG_LIBROOT)/WebKit/WebCore/dom
#include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf
LOC_SRC_wc_pf := $(CFG_LIBROOT)/WebKit/WebCore/platform
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_api
LOC_SRC_jsc_api := $(CFG_LIBROOT)/WebKit/JavaScriptCore
LOC_LST_jsc_api := AllInOneFile
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

