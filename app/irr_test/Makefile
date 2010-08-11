
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irr_test
PRJ_DEPS := irrlicht
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib
PRJ_LIBS := oexlib
PRJ_DEFS := 

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(TOOLS),iphone)
UNSUPPORTED := TOOLS=$(TOOLS) not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PROC),arm)
	PRJ_INCS := $(PRJ_INCS) irrlicht-es/include
	PRJ_LIBS := $(PRJ_LIBS) irrlicht-es jpeg png tiff zlib bzip2
else
	PRJ_INCS := $(PRJ_INCS) irrlicht/include
	PRJ_LIBS := $(PRJ_LIBS) irrlicht jpeg png tiff zlib bzip2
endif

#_IRR_USE_WINDOWS_CE_DEVICE_
ifeq ($(PLATFORM),posix)

	ifneq ($(TOOLS),iphone)
		PRJ_OSLB := X11 GLU Xxf86vm
		PRJ_DEFS := $(PRJ_DEFS) _IRR_USE_LINUX_DEVICE_ _IRR_COMPILE_WITH_X11_
	endif
else
	PRJ_DEFS := _IRR_STATIC_LIB_  _IRR_MULTI_ZLIB_
	ifeq ($(OS),wince)
		#PRJ_OSLB := libGLES_CM
		#PRJ_DEFS := $(PRJ_DEFS) _IRR_COMPILE_WITH_OGLES1_
		#PRJ_INCS := $(PRJ_INCS) ogles/include
	else
		ifeq ($(BUILD),gcc)
			PRJ_OSLB := opengl32 winmm
		else
			PRJ_OSLB := winmm.lib
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

endif



