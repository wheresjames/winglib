
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := live555
PRJ_TYPE := lib
PRJ_INCS := live555/groupsock/include live555/UsageEnvironment/include
PRJ_LIBS := 
PRJ_DEFS := 

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(BUILD_LIVE555),)
UNSUPPORTED := Set make option BUILD_LIVE555=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

ifneq ($(PROC),i386)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else


#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/live555/liveMedia/include
LOC_SRC_def := $(CFG_LIBROOT)/live555/liveMedia
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

endif

