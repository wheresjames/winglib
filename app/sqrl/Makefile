
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqrl
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus
PRJ_LIBS := sqbind oexlib dshowbase sqplus sqstdlib squirrel cximage jpeg png tiff zlib
PRJ_WINL := winmm strmiids

ifdef SQMOD_STATIC
	PRJ_INCS := $(PRJ_INCS) gdchart cell/lib
	PRJ_LIBS := $(PRJ_LIBS) gdchart cell
endif

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


