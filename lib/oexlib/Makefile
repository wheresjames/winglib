
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := oexlib
PRJ_TYPE := lib
PRJ_INCS := winglib/lib/oexlib

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(OS),android)
	PRJ_DEFS := $(PRJ_DEFS) SQLITE_NO_SYNC
endif

# +++ src/os/posix/sys_util.cpp:495:4: error: i_cpuid() inconsistent operand constraints in an ‘asm’
ifneq ($(PLATFORM),windows)
	PRJ_DEFS := $(PRJ_DEFS) OEX_NO_CPUID
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_INC_def := inc
LOC_SRC_def := src
include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := vid
#LOC_SRC_vid := src/vid
#include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := os
LOC_INC_os := inc/os
LOC_SRC_os := src/os/$(PLATFORM)
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := sqlite
LOC_CXX_sqlite := c
LOC_SRC_sqlite := oss/sqlite
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk
