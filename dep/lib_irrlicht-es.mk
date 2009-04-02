
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := irrlicht-es
PRJ_TYPE := lib
PRJ_INCS := irrlicht-es/source/Irrlicht/zlib ogles/include
PRJ_LIBS := 
PRJ_DEFS := _IRR_COMPILE_WITH_OPENGL_

PRJ_LIBROOT := ..

PRJ_DEFS := _IRR_CEGCC_ GL_APIENTRY= glTexEnvi=glTexEnvx \
		    EGL_RENDERABLE_TYPE=0x3040 EGL_OPENGL_ES_BIT=0x0001 EGL_CLIENT_APIS=0x308D

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#ifeq ($(TOOLS),iphone)
#	PRJ_DEFS := _IRR_IPHONE_PLATFORM_ _IRR_USE_IPHONE_DEVICE_
#endif

ifeq ($(PLATFORM),windows)
	PRJ_DEFS := _IRR_STATIC_LIB_
	
	ifeq ($(OS),wince)
		PRJ_INCS := $(PRJ_INCS) ogles/include
	endif	
endif

ifeq ($(PROC),i386)
all:
	@echo =======================================================
	@echo = !!! Processor $(PROC) not supported
	@echo =======================================================
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

ifeq ($(OS),win32)
CFG_CFLAGS := $(CFG_CFLAGS) /DIRRLICHT_EXPORTS
endif

export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/irrlicht-es/include
LOC_SRC_def := $(CFG_LIBROOT)/irrlicht-es/source/Irrlicht
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif


