
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := axtest
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib
PRJ_LIBS := oexlib 
PRJ_OSLB := 
PRJ_RESD :=
PRJ_HACK := win_fwd_slashes

PRJ_EXPORTS := DllMain DllGetClassObject DllCanUnloadNow \
			   DllRegisterServer DllUnregisterServer

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifneq ($(PLATFORM),windows)
UNSUPPORTED := BUILD=$(BUILD) is invalid, axtest can only be built on windows
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif
