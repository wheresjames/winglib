
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_irrlicht
PRJ_DEPS := irrlicht
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/sqbind winglib/lib/oexlib \
			SqPlus/include SqPlus/sqplus \
			freetype2/include ftgl/src
#			bullet/src 
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel \
			freetype2 
#			ftgl

#			bullet
PRJ_DEFS := OEX_NOBULLET _STDCALL_SUPPORTED
PRJ_WINL := strmiids winmm opengl32 glu32
PRJ_VSLB := dshowbase

PRJ_EXPORTS := DllMain SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_SUBROOT := _sqmod

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#ifeq ($(PROC),arm)
#UNSUPPORTED := PROC=$(PROC) is not supported
#include $(PRJ_LIBROOT)/unsupported.mk
#else

ifeq ($(PLATFORM),windows)
	PRJ_DEFS := $(PRJ_DEFS) FTGL_LIBRARY_STATIC
endif

ifeq ($(PROC),arm)
	ifeq ($(OS),android)
		PRJ_INCS := $(PRJ_INCS) irrlicht-android/project/include
		PRJ_LIBS := $(PRJ_LIBS) irrlicht-android cximage jpeg png tiff zlib bzip2
		ifneq ($(LIBLINK),static)
			PRJ_OSLB := $(PRJ_OSLB) GLESv2
		endif
		CFG_LFLAGS := $(CFG_LFLAGS) -Wl,-allow-shlib-undefined
	else
		PRJ_INCS := $(PRJ_INCS) irrlicht-es/include
		PRJ_LIBS := $(PRJ_LIBS) irrlicht-es cximage jpeg png tiff zlib bzip2
	endif
else
	PRJ_INCS := $(PRJ_INCS) irrlicht/include
	PRJ_LIBS := $(PRJ_LIBS) irrlicht cximage jpeg png tiff zlib bzip2
endif

#_IRR_USE_WINDOWS_CE_DEVICE_
ifeq ($(PLATFORM),posix)
	ifneq ($(PROC),arm)
		PRJ_OSLB := X11 GL GLU Xxf86vm
	endif
#	PRJ_DEFS := $(PRJ_DEFS) _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
else
	PRJ_DEFS := $(PRJ_DEFS) _IRR_STATIC_LIB_ _IRR_MULTI_ZLIB_
	ifeq ($(OS),wince)
		#_IRR_COMPILE_WITH_OGLES1_
#		PRJ_OSLB := libGLES_CM
#		PRJ_INCS := $(PRJ_INCS) ogles/include
		PRJ_DEFS := $(PRJ_DEFS) OEX_NOBULLET
	else
		ifeq ($(BUILD),gcc)
			PRJ_OSLB := opengl32
		endif
#		PRJ_DEFS := $(PRJ_DEFS) _IRR_COMPILE_WITH_OPENGL_
	endif	
endif
 
#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

#endif
