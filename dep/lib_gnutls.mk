
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gnutls
PRJ_TYPE := lib
PRJ_INCS := gnutls/lib gnutls/lib/includes gnutls/libextra/includes \
			gnutls/lib/minitasn1 gnutls/lib/gl gnutls/lib/x509 libgcrypt/src
PRJ_LIBS := 
PRJ_DEFS := ASN1_API= VERSION=\"1\" ENABLE_ANON strcasecmp="_stricmp" strncasecmp="_strnicmp"\
			SIZEOF_UNSIGNED_LONG_INT=4 SIZEOF_UNSIGNED_INT=4 ASN1_BUILDING

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := winglib/dep/etc/gnutls/inc/windows libgsasl/win32/include $(PRJ_INCS)
	PRJ_DEFS := $(PRJ_DEFS) SINLINE=
else
	PRJ_INCS := winglib/dep/etc/gnutls/inc/posix $(PRJ_INCS)
	PRJ_DEFS := $(PRJ_DEFS) SINLINE=inline
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_SRC_def := $(CFG_LIBROOT)/gnutls/lib
LOC_EXC_def := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := x
LOC_CXX_x := c
LOC_SRC_x := $(CFG_LIBROOT)/gnutls/lib/x509
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := minitasn1
LOC_CXX_minitasn1 := c
LOC_SRC_minitasn1 := $(CFG_LIBROOT)/gnutls/lib/minitasn1
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gl
LOC_CXX_gl := c
LOC_SRC_gl := $(CFG_LIBROOT)/gnutls/lib/gl
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


