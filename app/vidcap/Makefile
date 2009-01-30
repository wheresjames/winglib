
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := vidcap
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib

ifeq ($(OS),win32)
	PRJ_LIBS := oexlib wxWidgets jpeg
	PRJ_OSLB := gdi32.lib comdlg32.lib comctl32.lib rpcrt4.lib
else
	PRJ_LIBS := oexlib
	PRJ_EXTC := -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread	
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


