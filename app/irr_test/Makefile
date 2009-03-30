
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irr_test
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib irrlicht/include
PRJ_LIBS := oexlib irrlicht cximage jpeg png tiff zlib
PRJ_DEFS := _IRR_STATIC_LIB_ _IRR_COMPILE_WITH_OPENGL_

ifeq ($(OS),win32)
	PRJ_LIBS := $(PRJ_LIBS) wxWidgets
else
	PRJ_LIBS := oexlib
	PRJ_EXTC := `wx-config --cppflags`
	PRJ_EXTL := `wx-config --libs`
endif

PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


