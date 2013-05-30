
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqplugin
PRJ_DEPS := firebreath
PRJ_TYPE := dll
PRJ_INCS := boost winglib/app/sqplugin winglib/lib/oexlib \
			firebreath/src/config firebreath/src/ActiveXCore \
			firebreath/src/PluginCore firebreath/src/ScriptingCore
PRJ_DEFS := FBSTRING_PLUGIN_VERSION="\"1.0.0.0\""
PRJ_LIBS := oexlib firebreath
PRJ_OSLB := 
PRJ_RESD :=
PRJ_FWRK := mfc

PRJ_EXPORTS := DllMain DllGetClassObject DllCanUnloadNow \
			   DllRegisterServer DllUnregisterServer

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifneq ($(PLATFORM),windows)
UNSUPPORTED := BUILD=$(BUILD) is invalid, $(PRJ_NAME) can only be built on windows
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(BUILD),vs)
	PRJ_DEFS := $(PRJ_DEFS) FB_WIN
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

ifeq ($(PLATFORM),windows)

	export LOC_TAG := idl
	LOC_CXX_idl := idl
	LOC_SRC_idl := res
	include $(PRJ_LIBROOT)/build.mk

	export LOC_TAG := paw
	LOC_EXC_paw := np_winmain NpapiPluginWin
	LOC_SRC_paw := $(CFG_LIBROOT)/firebreath/src/PluginAuto/Win
	include $(PRJ_LIBROOT)/build.mk

endif

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif
