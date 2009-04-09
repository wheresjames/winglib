
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irrlicht
PRJ_TYPE := lib
PRJ_INCS := irrlicht/source/Irrlicht/zlib
PRJ_LIBS := 
PRJ_DEFS := _IRR_COMPILE_WITH_OPENGL_

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_DEFS := $(PRJ_DEFS) _IRR_STATIC_LIB_
else
	PRJ_DEFS := $(PRJ_DEFS) _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
endif

ifneq ($(PROC),i386)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

ifeq ($(OS),win32)
CFG_CFLAGS := $(CFG_CFLAGS) /DIRRLICHT_EXPORTS
endif

export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/irrlicht/include
LOC_SRC_def := $(CFG_LIBROOT)/irrlicht/source/Irrlicht
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

