
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irr_android
PRJ_TYPE := dll
PRJ_PACK := apk
PRJ_RESD :=
PRJ_INCS :=  
PRJ_LIBS := 
PRJ_OSLB := 

PRJ_RESOURCE := res
PRJ_MANIFEST := AndroidManifest.xml
PRJ_PKG_NAME := winglib.app.irrandroid

PRJ_COMPANY 	:= Company
PRJ_URL 		:= http://www.example.com
PRJ_EMAIL 		:= email@exemple.com
PRJ_COPYRIGHT 	:= Copyright 2011
PRJ_SIGN 		:= ss/ss
PRJ_SIGNALIAS 	:= ss

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifneq ($(OS),android)
UNSUPPORTED := OS=$(OS) not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

# -Wl,-rpath=/system/lib/egl GLESv1_CM
PRJ_DEFS := ANDROID_NDK DISABLE_IMPORTGL
PRJ_INCS := $(PRJ_INCS) irrlicht-android/project/include irrlicht-android/project/jni
PRJ_LIBS := $(PRJ_LIBS) irrlicht-android cximage jpeg png tiff zlib bzip2
PRJ_OSLB := log
ifneq ($(LIBLINK),static)
	PRJ_OSLB := $(PRJ_OSLB) GLESv2
endif
CFG_LFLAGS := $(CFG_LFLAGS) -Wl,-allow-shlib-undefined

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := java
LOC_EXT_def := class
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := c
#LOC_CXX_c := c
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif
