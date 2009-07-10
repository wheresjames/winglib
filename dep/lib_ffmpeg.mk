
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := ffmpeg
PRJ_TYPE := lib
PRJ_INCS := ffmpeg
PRJ_LIBS := 
PRJ_DEFS := attribute_align_arg= HAVE_AV_CONFIG_H=1
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifneq ($(BUILD),gcc)
UNSUPPORTED := BUILD=$(BUILD) is invalid, ffmpeg can only be built with 'gcc'
include $(PRJ_LIBROOT)/unsupported.mk
else

ifndef BUILD_FFMPEG
UNSUPPORTED := Set make option BUILD_FFMPEG=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

ifeq ($(PLATFORM),windows)
	PRJ_INCS := winglib/dep/etc/ffmpeg/inc/windows $(PRJ_INCS) zlib
else
	PRJ_INCS := winglib/dep/etc/ffmpeg/inc/posix $(PRJ_INCS)
endif

CFG_CFLAGS := $(CFG_CFLAGS) -ffast-math -fomit-frame-pointer

ifdef DBG
	CFG_CFLAGS := $(CFG_CFLAGS) -fno-stack-check -O3
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := libavutil
LOC_CXX_libavutil := c
LOC_SRC_libavutil := $(CFG_LIBROOT)/ffmpeg/libavutil
#LOC_LST_libavutil := log rational
LOC_EXC_libavutil := integer softfloat
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := libavformat
LOC_CXX_libavformat := c
LOC_SRC_libavformat := $(CFG_LIBROOT)/ffmpeg/libavformat
LOC_EXC_libavformat := avisynth libnut
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := libavcodec
LOC_CXX_libavcodec := c
LOC_SRC_libavcodec := $(CFG_LIBROOT)/ffmpeg/libavcodec

LOC_EXC_libavcodec := libamr libdiracdec libdiracenc \
					  libfaac libfaad libgsm libmp3lame libopenjpeg libschroedinger \
					  libschroedingerdec libschroedingerenc libspeexdec libtheoraenc \
					  libvorbis libx264 libxvidff libxvid_rc \
					  \
					  aacenc aacpsy beosthread g729dec imgconvert_template motion_est_template \
					  mpegvideo_xvmc os2thread svq3 vdpau 

ifeq ($(PLATFORM),windows)
	LOC_EXC_libavcodec := $(LOC_EXC_libavcodec) pthread
else
	LOC_EXC_libavcodec := $(LOC_EXC_libavcodec) w32thread
endif

include $(PRJ_LIBROOT)/build.mk
					  				  
ifeq ($(PROC),i386)
	export LOC_TAG := x86
	LOC_CXX_x86 := c
	LOC_SRC_x86 := $(CFG_LIBROOT)/ffmpeg/libavcodec/x86
	LOC_EXC_x86 := dsputil_h264_template_mmx dsputil_h264_template_ssse3 dsputil_mmx_avg_template \
				   dsputil_mmx_qns_template dsputil_mmx_rnd_template h264dsp_mmx \
				   mpegvideo_mmx_template
	include $(PRJ_LIBROOT)/build.mk
endif


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

endif

