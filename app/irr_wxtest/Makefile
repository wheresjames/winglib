
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irr_wxtest
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib irrlicht/include
PRJ_LIBS := oexlib irrlicht cximage jpeg png tiff zlib

PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(OS),win32)
	ifeq ($(BUILD),gcc)
		PRJ_OSLB := comctl32 comdlg32 uuid oleaut32 opengl32
	endif
	PRJ_LIBS := $(PRJ_LIBS) wxWidgets jpeg
	PRJ_INCS := $(PRJ_INCS) winglib/dep/etc/wxWidgets/inc/windows wxWidgets/include
	PRJ_DEFS := _IRR_STATIC_LIB_ _IRR_COMPILE_WITH_OPENGL_
else
	PRJ_EXTC := `wx-config --cppflags`
	PRJ_EXTL := `wx-config --libs`
	PRJ_OSLB := X11 GLU Xxf86vm
	PRJ_DEFS := $(PRJ_DEFS) _IRR_COMPILE_WITH_OPENGL_ _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


