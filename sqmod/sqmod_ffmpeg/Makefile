
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := sqmod_ffmpeg
PRJ_DEPS := ffmpeg x264 vpx lame
PRJ_TYPE := dll
PRJ_INCS := winglib/lib/oexlib winglib/lib/sqbind SqPlus/include SqPlus/sqplus \
			ffmpeg openssl/include
PRJ_LIBS := sqbind oexlib sqplus sqstdlib squirrel cximage jpeg png tiff zlib
PRJ_DEFS := HAVE_AV_CONFIG_H=1 __STDC_CONSTANT_MACROS
PRJ_LIBP :=
PRJ_OSLB :=
PRJ_WINL := winmm

PRJ_SUBROOT := _sqmod

PRJ_EXPORTS := SRV_GetModuleInfo SRV_Start SRV_Stop SQBIND_Export_Symbols

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PROC),arm)
UNSUPPORTED := PROC=$(PROC) is not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(BUILD)-$(PROC),vs-x64)
	UNSUPPORTED := PROC=$(PROC) + BUILD=$(BUILD) is not supported
	include $(PRJ_LIBROOT)/unsupported.mk
else

#ifeq ($(PLATFORM)-$(PROC),posix-x64)
#	UNSUPPORTED :=  $(PLATFORM)-$(PROC), for whatever stupid reason, this isn't working atm
#	include $(PRJ_LIBROOT)/unsupported.mk
#else

ifeq ($(PLATFORM),windows)
	ifeq ($(BUILD),vs)
		CFG_LFLAGS := $(CFG_LFLAGS) /FORCE
		PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/windows/vs xlibs $(PRJ_INCS)
		PRJ_PLIB := ffa ffb ffc ffd ffe vpx rtmpd x264 lame openssl_a openssl_b openssl_c
		# PRJ_DEFS := $(PRJ_DEFS)
		ifeq ($(OS),win64)
			PRJ_LIBP := $(PRJ_LIBP) $(CFG_LIBROOT)/xlibs/x64
		else
			PRJ_LIBP := $(PRJ_LIBP) $(CFG_LIBROOT)/xlibs/x86
			PRJ_WINX := libgcc.a libmingwex.a
#			PRJ_WINX := libgcc.a libmingw32.a libmingwex.a libpthread.a libiconv.a libmsvcrt.a
		endif
	else
		PRJ_WINL := $(PRJ_WINL) iconv
#		PRJ_DEFS := $(PRJ_DEFS) _mkdir=mkdir _rmdir=rmdir
		PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/windows/gcc $(PRJ_INCS) zlib
		PRJ_LIBS := $(PRJ_LIBS) ffa ffb ffc ffd ffe ffa ffb ffc ffd ffe rtmpd x264 lame vpx \
								openssl_a openssl_b openssl_c openssl_a openssl_b openssl_c zlib
	endif
else
	PRJ_INCS := $(CFG_LIB2BLD)/dep/etc/ffmpeg/inc/posix $(PRJ_INCS)
	PRJ_LIBS := $(PRJ_LIBS) ffa ffb ffc ffd ffa ffb ffc ffd rtmpd vpx x264 lame \
							openssl_a openssl_b openssl_c openssl_a openssl_b openssl_c zlib
	PRJ_OSLB := $(PRJ_OSLB)
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

#endif

endif

endif