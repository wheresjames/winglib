
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := squirrel
PRJ_TYPE := lib
PRJ_INCS := SqPlus/include
PRJ_LIBS :=

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#ifeq ($(PROC),arm)
	PRJ_DEFS := $(PRJ_DEFS) SQNOSETJMP
#endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/SqPlus/squirrel
LOC_SRC_def := $(CFG_LIBROOT)/SqPlus/squirrel
LOC_EXC_def := sqmem
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


