
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irrlicht
PRJ_TYPE := lib
PRJ_INCS := jpeg png tiff zlib
PRJ_LIBS := 
PRJ_DEFS := _IRR_COMPILE_WITH_OPENGL_

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(3D),)
UNSUPPORTED := Set make option 3D=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

ifneq ($(PROC),i386)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)
	PRJ_DEFS := $(PRJ_DEFS) _IRR_STATIC_LIB_ _IRR_MULTI_ZLIB_
else
	PRJ_DEFS := $(PRJ_DEFS) _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
endif

ifeq ($(OS),win32)
	PRJ_DEFS := $(PRJ_DEFS) IRRLICHT_EXPORTS
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/irrlicht/include
LOC_SRC_def := $(CFG_LIBROOT)/irrlicht/source/Irrlicht
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

endif

