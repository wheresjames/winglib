
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := scintilla
PRJ_TYPE := lib
PRJ_INCS := 
PRJ_LIBS := 

PRJ_LIBROOT := ../..
PRJ_OBJROOT := _0_dep_gui

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := cxx
LOC_BLD_def := cpp
LOC_INC_def := $(CFG_LIBROOT)/scintilla/include
LOC_SRC_def := $(CFG_LIBROOT)/scintilla/src
LOC_EXC_def := ExternalLexer
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


