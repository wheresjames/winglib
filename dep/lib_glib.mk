
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := glib
PRJ_TYPE := lib
PRJ_INCS := glib glib/glib \
			winglib/dep/etc/glib/inc/posix
PRJ_LIBS := 
PRJ_DEFS := GOBJECT_COMPILATION LINK_SIZE=2 MAX_NAME_COUNT=10000 MAX_NAME_SIZE=32 NEWLINE=-1 \
			POSIX_MALLOC_THRESHOLD=10 MATCH_LIMIT=10000000 MATCH_LIMIT_RECURSION=10000000

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifndef BUILD_GSTREAMER
UNSUPPORTED := Set make option BUILD_GSTREAMER=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

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

export LOC_TAG := libcharset
LOC_CXX_libcharset := c
LOC_SRC_libcharset := $(CFG_LIBROOT)/glib/glib/libcharset
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := pcre
LOC_CXX_pcre := c
LOC_SRC_pcre := $(CFG_LIBROOT)/glib/glib/pcre
include $(PRJ_LIBROOT)/build.mk


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

