
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_SQRL := yes
PRJ_NAME := sqa_irrtest

SQMOD_STATIC := irrlicht

PRJ_LIBROOT := ../../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(TOOLS),iphone)
UNSUPPORTED := TOOLS=$(TOOLS) not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifdef SQMOD_STATIC
PRJ_INCS := $(PRJ_INCS) bullet/src freetype/include ftgl/src
PRJ_LIBS := $(PRJ_LIBS) bullet freetype ftgl
endif

ifeq ($(PROC),arm)
	PRJ_INCS := $(PRJ_INCS) irrlicht-es/include
	PRJ_LIBS := $(PRJ_LIBS) irrlicht-es jpeg png tiff zlib
else
	PRJ_INCS := $(PRJ_INCS) irrlicht/include
	PRJ_LIBS := $(PRJ_LIBS) irrlicht jpeg png tiff zlib
endif

#_IRR_USE_WINDOWS_CE_DEVICE_
ifeq ($(PLATFORM),posix)

	ifneq ($(TOOLS),iphone)
		PRJ_OSLB := X11 GLU Xxf86vm
		PRJ_DEFS := $(PRJ_DEFS) _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
	endif
else
	PRJ_DEFS := _IRR_STATIC_LIB_
	ifeq ($(OS),wince)
		PRJ_OSLB := libGLES_CM
		#PRJ_DEFS := $(PRJ_DEFS) _IRR_COMPILE_WITH_OGLES1_
		#PRJ_INCS := $(PRJ_INCS) ogles/include
	else
		ifeq ($(BUILD),gcc)
			PRJ_OSLB := opengl32
		endif
#		PRJ_DEFS := $(PRJ_DEFS) _IRR_COMPILE_WITH_OPENGL_
	endif	
endif

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

