
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := poco
PRJ_TYPE := lib
PRJ_INCS := poco/Poco/Foundation/include poco/Poco/Net/include \
			poco/Poco/Crypto/include poco/Poco/WebWidgets/include \
			poco/Poco/Xml/include poco/Poco/Zip/include
PRJ_LIBS := 
PRJ_DEFS := HAVE_MEMMOVE POCO_NO_AUTOMATIC_LIBS

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#ifeq ($(PLATFORM),windows)
#	PRJ_INCS := winglib/dep/etc/mimetic/inc/windows $(PRJ_INCS)
#else
#	PRJ_INCS := winglib/dep/etc/mimetic/inc/posix $(PRJ_INCS)
#endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_SRC_def := $(CFG_LIBROOT)/poco/Poco/Foundation/src
LOC_EXC_def := DirectoryIterator_UNIX DirectoryIterator_VMS DirectoryIterator_WIN32 DirectoryIterator_WIN32U \
			   Environment_UNIX Environment_VMS Environment_WIN32 Environment_WIN32U \
			   Event_POSIX Event_WIN32 File_UNIX File_VMS File_WIN32 File_WIN32U \
			   FileStream_POSIX FileStream_WIN32 \
			   FPEnvironment_C99 FPEnvironment_DEC FPEnvironment_DUMMY FPEnvironment_SUN FPEnvironment_WIN32 \
			   LogFile_STD LogFile_VMS LogFile_WIN32 LogFile_WIN32U Mutex_POSIX Mutex_WIN32 \
			   NamedEvent_UNIX NamedEvent_VMS NamedEvent_WIN32 NamedEvent_WIN32U \
			   NamedMutex_UNIX NamedMutex_VMS NamedMutex_WIN32 NamedMutex_WIN32U \
			   Path_UNIX Path_VMS Path_WIN32 Path_WIN32U \
			   PipeImpl_DUMMY PipeImpl_POSIX PipeImpl_WIN32 \
			   Process_UNIX Process_VMS Process_WIN32 Process_WIN32U \
			   RWLock_POSIX RWLock_WIN32 Semaphore_POSIX Semaphore_WIN32 \
			   SharedLibrary_HPUX SharedLibrary_UNIX SharedLibrary_VMS \
			   SharedLibrary_WIN32 SharedLibrary_WIN32U \
			   SharedMemory_DUMMY SharedMemory_POSIX SharedMemory_WIN32 SharedMemory_WIN32U \
			   Thread_POSIX Thread_WIN32 Thread_WIN32U Timezone_UNIX Timezone_WIN32 \
			   OpcomChannel	SyslogChannel
ifneq ($(PLATFORM),windows)
	LOC_EXC_def := $(LOC_EXC_def) EventLogChannel WindowsConsoleChannel
endif
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := net
LOC_SRC_net := $(CFG_LIBROOT)/poco/Poco/Net/src
include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := ssl
#LOC_INC_ssl := $(CFG_LIBROOT)/poco/Poco/NetSSL_OpenSSL/include
#LOC_SRC_ssl := $(CFG_LIBROOT)/poco/Poco/NetSSL_OpenSSL/src
#include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := web
#LOC_SRC_web := $(CFG_LIBROOT)/poco/Poco/WebWidgets/src
#include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := xml
#LOC_SRC_xml := $(CFG_LIBROOT)/poco/Poco/Xml/src
#include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := zip
#LOC_SRC_zip := $(CFG_LIBROOT)/poco/Poco/Zip/src
#include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

