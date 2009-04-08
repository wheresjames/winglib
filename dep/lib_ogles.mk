
ifeq ($(BUILD_OGLES),)

default_target:

else

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := ogles-bld
PRJ_TYPE := lib
PRJ_INCS := ogles/src ogles/src/WinCE ogles/include ogles/include/EVC3Inc/ARM \
			ogles/src/codegen
PRJ_LIBS := 

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(OS),wince)
	PRJ_DEFS := NO_GETENV
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/ogles/src
LOC_SRC_def := $(CFG_LIBROOT)/ogles/src
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := codegen
LOC_CXX_codegen := c
LOC_INC_codegen := $(CFG_LIBROOT)/ogles/src/codegen
LOC_SRC_codegen := $(CFG_LIBROOT)/ogles/src/codegen
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := arm
LOC_INC_arm := $(CFG_LIBROOT)/ogles/src/arm
LOC_SRC_arm := $(CFG_LIBROOT)/ogles/src/arm
LOC_EXC_arm := GenScanline
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := wince
LOC_INC_wince := $(CFG_LIBROOT)/ogles/src/WinCE
LOC_SRC_wince := $(CFG_LIBROOT)/ogles/src/WinCE
LOC_EXC_wince := 
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif


