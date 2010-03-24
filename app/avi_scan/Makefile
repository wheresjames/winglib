
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := avi_scan
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib
PRJ_LIBS := oexlib

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(OS),win32)
	ifeq ($(BUILD),gcc)
		PRJ_OSLB := comctl32 comdlg32 uuid oleaut32
	endif
	PRJ_LIBS := $(PRJ_LIBS) wxWidgets jpeg
	PRJ_INCS := $(PRJ_INCS) winglib/dep/etc/wxWidgets/inc/windows wxWidgets/include
else
	PRJ_LIBS := $(PRJ_LIBS) jpeg
	PRJ_EXTC := `wx-config --cppflags`
	PRJ_EXTL := `wx-config --libs`
	PRJ_OSLB := X11 GLU Xxf86vm
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


