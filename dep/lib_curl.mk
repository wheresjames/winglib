
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := curl
PRJ_TYPE := lib
PRJ_INCS := curl/src openldap/include
PRJ_LIBS := 
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(BUILD_CURL),)
UNSUPPORTED := Set BUILD_CURL=1 to build this project
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)
# HAVE_CONFIG_H 
	PRJ_DEFS := $(PRJ_DEFS) CURL_STATICLIB CURL_DISABLE_LDAP HAVE_SELECT
	ifeq ($(BUILD),vs)
		PRJ_INCS := $(PRJ_INCS) winglib/dep/etc/vs/inc/c99
	endif
else
	PRJ_DEFS := $(PRJ_DEFS) OS=\"posix\" HAVE_STRUCT_TIMEVAL HTTP_ONLY HAVE_SYS_TYPES_H \
				HAVE_SOCKET HAVE_SELECT HAVE_SYS_STAT_H HAVE_SYS_TIME_H HAVE_FCNTL_H \
				HAVE_RECV RECV_TYPE_ARG1=int RECV_TYPE_ARG2=void* \
				RECV_TYPE_ARG3=size_t RECV_TYPE_ARG4=int RECV_TYPE_RETV=size_t \
				HAVE_SEND SEND_TYPE_ARG1=int SEND_QUAL_ARG2=const SEND_TYPE_ARG2=void* \
				SEND_TYPE_ARG3=size_t SEND_TYPE_ARG4=int SEND_TYPE_RETV=size_t \
				HAVE_FCNTL_O_NONBLOCK HAVE_SYS_IOCTL_H HAVE_UNISTD_H HAVE_NETDB_H \
				HAVE_STDLIB_H HAVE_SYS_UN_H HAVE_SYS_SOCKET_H HAVE_NETINET_IN_H 	
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
#LOC_BLD_def := cpp
LOC_INC_def := $(CFG_LIBROOT)/curl/include
LOC_SRC_def := $(CFG_LIBROOT)/curl/lib
LOC_EXC_def := 
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif


