
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := WebKit
PRJ_TYPE := lib
PRJ_INCS := wxWidgets/include freetype/include cairo/src libxml/include curl/include sqlite \
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
			WebKit/WebCore/svg/graphics WebKit/WebCore/svg/graphics/filters WebKit/WebCore/platform/image-decoders/bmp \
			WebKit/WebCore/platform/image-decoders/gif WebKit/WebCore/platform/image-decoders/ico \
			WebKit/WebCore/platform/image-decoders/jpeg WebKit/WebCore/platform/image-decoders/png \
			WebKit/WebCore/platform/image-decoders/xbm WebKit/WebCore/platform/image-decoders \
			\
			WebKit/JavaScriptCore WebKit/JavaScriptCore/API WebKit/JavaScriptCore/ForwardingHeaders WebKit/JavaScriptCore/interpreter \
			WebKit/JavaScriptCore/bytecode WebKit/JavaScriptCore/bytecompiler WebKit/JavaScriptCore/debugger WebKit/JavaScriptCore/jit \
			WebKit/JavaScriptCore/pcre WebKit/JavaScriptCore/profiler WebKit/JavaScriptCore/runtime WebKit/JavaScriptCore/wrec \
			WebKit/JavaScriptCore/assembler WebKit/JavaScriptCore/wtf/unicode WebKit/JavaScriptCore/yarr WebKit/JavaScriptCore/pcre \
			WebKit/JavaScriptCore/parser WebKit/JavaScriptCore/runtime WebKit/JavaScriptCore/wtf WebKit/JavaScriptCore/icu \
			\
			WebKit/WebCore/icu

PRJ_SYSI := 
PRJ_LIBS := 
PRJ_DEFS := USE_FREETYPE PTW32_STATIC_LIB

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
	PRJ_DEFS := $(PRJ_DEFS) NDEBUG=1 wxUSE_GRAPHICS_CONTEXT=1
#NEED_ERRNO 


	PRJ_INCS := winglib/dep/etc/WebKit/inc/windows winglib/dep/etc/cairo/inc/windows \
				winglib/dep/etc/wxWidgets/inc/windows winglib/dep/etc/libxml/inc/windows \
				$(PRJ_INCS) pthreads \
				WebKit/WebKit/wx/WebKitSupport WebKit/WebCore/platform/wx/wxcode \
				WebKit/WebCore/platform/win WebKit/WebCore/page/win \
				WebKit/WebKit/wx WebKit/WebKit/wx/WebCoreSupport WebKit/WebKit/wx/webkit \
				WebKit/WebCore/platform/wx WebKit/WebCore/platform/graphics/wx \
				WebKit/WebCore/accessibility/win WebKit/WebCore/loader/wx WebKit/WebCore/page/wx \
				WebKit/WebCore/platform/graphics/cairo WebKit/WebCore/platform/network/curl
	
#	PRJ_INCS := winglib/dep/etc/WebKit/inc/windows winglib/dep/etc/cairo/inc/windows \
#				$(PRJ_INCS) pthreads \
#				WebKit/WebKit/win WebKit/WebKit/win/WebCoreSupport WebKit/WebKit/win/webkit \
#				WebKit/WebCore/platform/win WebKit/WebCore/platform/graphics/win \
#				WebKit/WebCore/accessibility/win WebKit/WebCore/loader/win WebKit/WebCore/page/win \
#				WebKit/WebCore/platform/graphics/cairo WebKit/WebCore/platform/network/curl
#				WebKit/WebCore/platform/network/soup

else
	PRJ_EXTC := `wx-config --cppflags`
	PRJ_EXTL := `wx-config --libs`
#	PRJ_OSLB := X11 GLU Xxf86vm

	PRJ_DEFS := $(PRJ_DEFS) CAIRO_HAS_FT_FONT CAIRO_HAS_FC_FONT

	PRJ_SYSI := /usr/include/libsoup-2.4
	PRJ_INCS := winglib/dep/etc/WebKit/inc/posix winglib/dep/etc/cairo/inc/posix $(PRJ_INCS) \
				WebKit/WebKit/gtk WebKit/WebKit/gtk/WebCoreSupport WebKit/WebKit/gtk/webkit \
				WebKit/WebCore/platform/gtk WebKit/WebCore/platform/graphics/gtk \
				WebKit/WebCore/accessibility/gtk WebKit/WebCore/loader/gtk WebKit/WebCore/page/gtk \
				WebKit/WebCore/platform/graphics/cairo WebKit/WebCore/platform/network/soup

endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := ds
LOC_SRC_ds := $(CFG_LIBROOT)/WebKit/DerivedSources
LOC_EXC_ds := CSSPropertyNames DocTypeStrings JSElementTimeControl JSSVGAnimatedPathData \
			  JSSVGAnimatedPoints JSSVGElementWrapperFactory JSSVGExternalResourcesRequired \
			  JSSVGFilterPrimitiveStandardAttributes JSSVGFitToViewBox JSSVGLangSpace \
			  JSSVGLocatable JSSVGStylable JSSVGTests JSSVGTransformable JSSVGTransformable \
			  JSSVGURIReference JSSVGViewSpec JSSVGZoomAndPan SVGElementFactory tokenizer \
			  webkitenumtypes webkitmarshal
			  
include $(PRJ_LIBROOT)/build.mk

#----------------------------
# -- WebCore
#----------------------------

export LOC_TAG := wc_ac
LOC_SRC_wc_ac := $(CFG_LIBROOT)/WebKit/WebCore/accessibility
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_bind_js
LOC_SRC_wc_bind_js := $(CFG_LIBROOT)/WebKit/WebCore/bindings/js
LOC_EXC_wc_bind_js := ScriptControllerQt JSCSSStyleDeclarationCustom
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_bridge
LOC_SRC_wc_bridge := $(CFG_LIBROOT)/WebKit/WebCore/bridge
LOC_EXC_wc_bridge := testbindings testqtbindings
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_bridge_c
LOC_SRC_wc_bridge_c := $(CFG_LIBROOT)/WebKit/WebCore/bridge/c
#LOC_EXC_wc_bridge_c := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_css
LOC_SRC_wc_css := $(CFG_LIBROOT)/WebKit/WebCore/css
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_dom
LOC_SRC_wc_dom := $(CFG_LIBROOT)/WebKit/WebCore/dom
LOC_EXC_wc_dom := XMLTokenizerQt
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_ed
LOC_SRC_wc_ed := $(CFG_LIBROOT)/WebKit/WebCore/editing
LOC_EXC_wc_ed := SmartReplaceCF
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_hist
LOC_SRC_wc_hist := $(CFG_LIBROOT)/WebKit/WebCore/history
LOC_EXC_wc_hist := BackForwardListChromium
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_html
LOC_SRC_wc_html := $(CFG_LIBROOT)/WebKit/WebCore/html
LOC_EXC_wc_html := HTMLElementsAllInOne
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_ins
LOC_SRC_wc_ins := $(CFG_LIBROOT)/WebKit/WebCore/inspector
LOC_EXC_wc_ins := JavaScriptProfileNode
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_ld
LOC_SRC_wc_ld := $(CFG_LIBROOT)/WebKit/WebCore/loader
LOC_EXC_wc_ld := UserStyleSheetLoader
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_ld_ac
LOC_SRC_wc_ld_ac := $(CFG_LIBROOT)/WebKit/WebCore/loader/appcache
#LOC_EXC_wc_ld_ac :=
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_ld_ico
LOC_SRC_wc_ld_ico := $(CFG_LIBROOT)/WebKit/WebCore/loader/icon
LOC_EXC_wc_ld_ico := IconDatabaseNone
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pg
LOC_SRC_wc_pg := $(CFG_LIBROOT)/WebKit/WebCore/page
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pg_a
LOC_SRC_wc_pg_a := $(CFG_LIBROOT)/WebKit/WebCore/page/animation
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pg_wx
LOC_SRC_wc_pg_wx := $(CFG_LIBROOT)/WebKit/WebCore/page/wx
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf
LOC_SRC_wc_pf := $(CFG_LIBROOT)/WebKit/WebCore/platform
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_ani
LOC_SRC_wc_pf_ani := $(CFG_LIBROOT)/WebKit/WebCore/platform/animation
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_g
LOC_SRC_wc_pf_g := $(CFG_LIBROOT)/WebKit/WebCore/platform/graphics
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_g_c
LOC_SRC_wc_pf_g_c := $(CFG_LIBROOT)/WebKit/WebCore/platform/graphics/cairo
LOC_EXC_wc_pf_g_c := FontCairo GradientCairo ImageBufferCairo PathCairo PatternCairo \
					 TransformationMatrixCairo
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_g_t
LOC_SRC_wc_pf_g_t := $(CFG_LIBROOT)/WebKit/WebCore/platform/graphics/transforms
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_net
LOC_SRC_wc_pf_net := $(CFG_LIBROOT)/WebKit/WebCore/platform/network
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_net_curl
LOC_SRC_wc_pf_net_curl := $(CFG_LIBROOT)/WebKit/WebCore/platform/network/curl
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_sql
LOC_SRC_wc_pf_sql := $(CFG_LIBROOT)/WebKit/WebCore/platform/sql
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_pf_txt
LOC_SRC_wc_pf_txt := $(CFG_LIBROOT)/WebKit/WebCore/platform/text
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_plugins
LOC_SRC_wc_plugins := $(CFG_LIBROOT)/WebKit/WebCore/plugins
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_st
LOC_SRC_wc_st := $(CFG_LIBROOT)/WebKit/WebCore/storage
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_svg
LOC_SRC_wc_svg := $(CFG_LIBROOT)/WebKit/WebCore/svg
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_svg_g
LOC_SRC_wc_svg_g := $(CFG_LIBROOT)/WebKit/WebCore/svg/graphics
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_rd
LOC_SRC_wc_rd := $(CFG_LIBROOT)/WebKit/WebCore/rendering
LOC_EXC_wc_rd := RenderMediaControls RenderThemeChromiumLinux RenderThemeChromiumWin \
				 RenderThemeWin
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_rd_s
LOC_SRC_wc_rd_s := $(CFG_LIBROOT)/WebKit/WebCore/rendering/style
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wc_xml
LOC_SRC_wc_xml := $(CFG_LIBROOT)/WebKit/WebCore/xml
include $(PRJ_LIBROOT)/build.mk

#----------------------------
# -- JavaScriptCore
#----------------------------

export LOC_TAG := jsc_bc
LOC_SRC_jsc_bc := $(CFG_LIBROOT)/WebKit/JavaScriptCore/bytecode
#LOC_EXC_jsc_bc := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_bcomp
LOC_SRC_jsc_bcomp := $(CFG_LIBROOT)/WebKit/JavaScriptCore/bytecompiler
#LOC_EXC_jsc_bcomp := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_int
LOC_SRC_jsc_int := $(CFG_LIBROOT)/WebKit/JavaScriptCore/interpreter
#LOC_EXC_jsc_int := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_jit
LOC_SRC_jsc_jit := $(CFG_LIBROOT)/WebKit/JavaScriptCore/jit
LOC_EXC_jsc_jit := ExecutableAllocatorPosix
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_parser
LOC_SRC_jsc_parser := $(CFG_LIBROOT)/WebKit/JavaScriptCore/parser
LOC_EXC_jsc_parser :=
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_pcre
LOC_SRC_jsc_pcre := $(CFG_LIBROOT)/WebKit/JavaScriptCore/pcre
LOC_EXC_jsc_pcre := ucptable
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_profiler
LOC_SRC_jsc_profiler := $(CFG_LIBROOT)/WebKit/JavaScriptCore/profiler
#LOC_EXC_jsc_profiler := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_run
LOC_SRC_jsc_run := $(CFG_LIBROOT)/WebKit/JavaScriptCore/runtime
LOC_EXC_jsc_run := JSONObject
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_wtf
LOC_SRC_jsc_wtf := $(CFG_LIBROOT)/WebKit/JavaScriptCore/wtf
LOC_EXC_jsc_wtf := GOwnPtr ThreadingWin ThreadSpecificWin
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := jsc_yar
LOC_SRC_jsc_yar := $(CFG_LIBROOT)/WebKit/JavaScriptCore/yar
include $(PRJ_LIBROOT)/build.mk


#----------------------------
# -- wx
#----------------------------

export LOC_TAG := wx
LOC_SRC_wx := $(CFG_LIBROOT)/WebKit/WebKit/wx
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wx_p_ed
LOC_SRC_wx_p_ed := $(CFG_LIBROOT)/WebKit/WebCore/editing/wx
LOC_EXC_wx_p_ed := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wx_p_g
LOC_SRC_wx_p_g := $(CFG_LIBROOT)/WebKit/WebCore/platform/graphics/wx
LOC_EXC_wx_p_g := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wx_p_txt
LOC_SRC_wx_p_txt := $(CFG_LIBROOT)/WebKit/WebCore/platform/text/wx
LOC_EXC_wx_p_txt := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wx_p
LOC_SRC_wx_p := $(CFG_LIBROOT)/WebKit/WebCore/platform/wx
LOC_EXC_wx_p := KeyEventWin 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wx_wk_sup
LOC_SRC_wx_wk_sup := $(CFG_LIBROOT)/WebKit/WebKit/wx/WebKitSupport
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

