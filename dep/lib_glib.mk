
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := glib
PRJ_TYPE := lib
PRJ_INCS := glib glib/glib
PRJ_LIBS := 
PRJ_DEFS := GOBJECT_COMPILATION
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#ifndef BUILD_GSTREAMER
#UNSUPPORTED := Set make option BUILD_GSTREAMER=1 to build
#include $(PRJ_LIBROOT)/unsupported.mk
#else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := glib
LOC_CXX_glib := c
LOC_SRC_glib := $(CFG_LIBROOT)/glib/glib
LOC_EXC_glib := giowin32 gspawn-win32 gspawn-win32-helper gwin32 \
				win_iconv
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gobj
LOC_CXX_gobj := c
LOC_SRC_gobj := $(CFG_LIBROOT)/glib/gobject
LOC_EXC_gobj := gmarshal
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gthread
LOC_CXX_gthread := c
LOC_SRC_gthread := $(CFG_LIBROOT)/glib/gthread
LOC_EXC_gthread := gthread-none gthread-posix gthread-win32
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gmodule
LOC_CXX_gmodule := c
LOC_SRC_gmodule := $(CFG_LIBROOT)/glib/gmodule
LOC_EXC_gmodule := gmodule-ar gmodule-beos gmodule-dl gmodule-dld \
				   gmodule-dyld	gmodule-os2 gmodule-win32
include $(PRJ_LIBROOT)/build.mk


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

#endif

