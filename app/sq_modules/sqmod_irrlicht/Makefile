
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_irrlicht
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind \
			SqPlus/include SqPlus/sqplus bullet/src \
			freetype/include ftgl/src
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel \
			bullet freetype ftgl
PRJ_DEFS := 

PRJ_EXPORTS := DllMain SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_LIBROOT := ../../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_DEFS := $(PRJ_DEFS) FTGL_LIBRARY_STATIC
endif

ifeq ($(PROC),arm)
	PRJ_INCS := $(PRJ_INCS) irrlicht-es/include
	PRJ_LIBS := $(PRJ_LIBS) irrlicht-es cximage jpeg png tiff zlib
else
	PRJ_INCS := $(PRJ_INCS) irrlicht/include
	PRJ_LIBS := $(PRJ_LIBS) irrlicht cximage jpeg png tiff zlib
endif

#_IRR_USE_WINDOWS_CE_DEVICE_
ifeq ($(PLATFORM),posix)
	PRJ_OSLB := X11 GLU Xxf86vm
	PRJ_DEFS := $(PRJ_DEFS) _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
else
	PRJ_DEFS := $(PRJ_DEFS) _IRR_STATIC_LIB_
	ifeq ($(OS),wince)
		#_IRR_COMPILE_WITH_OGLES1_
#		PRJ_OSLB := libGLES_CM
#		PRJ_INCS := $(PRJ_INCS) ogles/include
		PRJ_DEFS := $(PRJ_DEFS) OEX_NOBULLET
	else
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


