
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqplus
PRJ_TYPE := lib
PRJ_INCS := SqPlus/include
PRJ_LIBS :=

PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_INC_def := $(CFG_LIBROOT)/SqPlus/sqplus
LOC_SRC_def := $(CFG_LIBROOT)/SqPlus/sqplus
LOC_EXC_def := SqPlus_Backup SquirrelBindingsUtilsWin32_Backup \
			   SquirrelObject_Backup SquirrelVM_Backup
ifneq ($(PLATFORM),windows)	  
	LOC_EXC_def := $(LOC_EXC_def) SquirrelBindingsUtilsWin32
endif
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk



