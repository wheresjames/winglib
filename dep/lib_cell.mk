
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := cell
PRJ_TYPE := lib
PRJ_INCS := 
PRJ_LIBS := 
PRJ_DEFS :=

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLC),)
UNSUPPORTED := Set make option PLC=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(TOOLS),iphone)
UNSUPPORTED := TOOLS=$(TOOLS) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_BLD_def := cpp
LOC_INC_def := $(CFG_LIBROOT)/cell/lib
LOC_SRC_def := $(CFG_LIBROOT)/cell/lib
LOC_EXC_def := serial
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif
