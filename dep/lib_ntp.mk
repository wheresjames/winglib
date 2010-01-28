
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := ntp
PRJ_TYPE := lib
PRJ_INCS := ntp/include ntp/lib ntp/lib/isc/include
PRJ_LIBS := 
PRJ_DEFS := HAVE_CONFIG_H=1 SYS_WINNT=1 _WIN32_WINNT=0x400 _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1 _CRT_SECURE_NO_DEPRECATE

# _DEBUG;_LIB;WIN32;__STDC__;SYS_WINNT;HAVE_CONFIG_H;_WIN32_WINNT=0x400;_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1;_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1;_CRT_SECURE_NO_DEPRECATE

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := ntp/ports/winnt/include ntp/lib/isc/win32/include $(PRJ_INCS)
else
	PRJ_INCS := winglib/dep/etc/ntp/inc/posix \
				ntp/lib/isc/unix/include $(PRJ_INCS) 
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_SRC_def := $(CFG_LIBROOT)/ntp/libntp
LOC_EXC_def := adjtimex
include $(PRJ_LIBROOT)/build.mk


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


