
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_poco
PRJ_DEPS := poco
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			poco/Foundation/include poco/Net/include \
			openssl/include poco/Crypto/include poco/NetSSL_OpenSSL/include
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage jpeg png tiff zlib \
			poco_a poco_b poco_a poco_b openssl_a openssl_b openssl_a openssl_b zlib
PRJ_DEFS := HAVE_MEMMOVE POCO_NO_AUTOMATIC_LIBS XML_STATIC PCRE_STATIC POCO_STATIC
PRJ_WINL := iphlpapi crypt32
 
PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

PRJ_SUBROOT := _sqmod

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PROC),arm)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)
	ifneq ($(UNICODE),)
		PRJ_DEFS := $(PRJ_DEFS) POCO_WIN32_UTF8
	endif

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

endif
