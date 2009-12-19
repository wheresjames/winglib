
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_vmime
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			libvmime

PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cell cximage jpeg png tiff zlib \
		    vmime
PRJ_DEFS := VMIME_HAVE_MESSAGING_FEATURES VMIME_HAVE_FILESYSTEM_FEATURES
 
PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_SUBROOT := _sqmod

PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := winglib/dep/etc/vmime/inc/windows $(PRJ_INCS)
else
	PRJ_INCS := winglib/dep/etc/vmime/inc/posix $(PRJ_INCS)
endif

#-------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------

export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk
