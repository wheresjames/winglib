
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := ftgl
PRJ_TYPE := lib
PRJ_INCS := freetype/include ftgl/src
PRJ_LIBS := 

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

PRJ_DEFS := M_PI=3.14159265358979323846 

ifeq ($(PLATFORM),windows)
	PRJ_DEFS := $(PRJ_DEFS) FTGL_LIBRARY_STATIC
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := def
LOC_SRC_def := $(CFG_LIBROOT)/ftgl/src
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := font
LOC_SRC_font := $(CFG_LIBROOT)/ftgl/src/FTFont
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gl
LOC_SRC_gl := $(CFG_LIBROOT)/ftgl/src/FTGl
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := glyph
LOC_SRC_glyph := $(CFG_LIBROOT)/ftgl/src/FTGlyph
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := layout
LOC_SRC_layout := $(CFG_LIBROOT)/ftgl/src/FTLayout
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


