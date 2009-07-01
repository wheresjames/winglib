
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := ffmpeg
PRJ_TYPE := lib
PRJ_INCS := winglib/dep/etc/ffmpeg/inc/posix ffmpeg ffmpeg/libavutil
PRJ_LIBS := 
PRJ_DEFS := attribute_align_arg= NDEBUG=1
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifndef BUILD_FFMPEG
UNSUPPORTED := Set make option BUILD_FFMPEG=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := libavutil
LOC_CXX_libavutil := c
LOC_SRC_libavutil := $(CFG_LIBROOT)/ffmpeg/libavutil
#LOC_LST_libavutil := log rational
LOC_EXC_libavutil := integer softfloat
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := libavcodec
LOC_CXX_libavcodec := c
LOC_SRC_libavcodec := $(CFG_LIBROOT)/ffmpeg/libavcodec
#LOC_LST_libavcodec := dsputil utils 
LOC_EXC_libavcodec := aacenc aacpsy acelp_filters beosthread \
					  g729dec libdiracdec libdiracenc \
					  libfaac libfaad libgsm libmp3lame libopenjpeg libschroedinger \
					  libschroedingerdec libschroedingerenc libspeexdec libtheoraenc \
					  libvorbis libx264 libxvidff libxvid_rc lsp motion_est_template \
					  mpegvideo_xvmc os2thread svq3 \
					  vaapi vaapi_mpeg2 vaapi_mpeg4 vaapi_vc1 vdpau w32thread
					  				  
include $(PRJ_LIBROOT)/build.mk

ifeq ($(PROC),i386)
	export LOC_TAG := x86
	LOC_CXX_x86 := c
	LOC_SRC_x86 := $(CFG_LIBROOT)/ffmpeg/libavcodec/x86
	LOC_EXC_x86 := dsputil_h264_template_mmx dsputil_h264_template_ssse3 dsputil_mmx_avg_template \
				   dsputil_mmx_qns_template dsputil_mmx_rnd_template h264dsp_mmx idct_mmx_xvid \
				   mpegvideo_mmx_template 
#	LOC_LST_x86 := dsputil_mmx 
	include $(PRJ_LIBROOT)/build.mk
endif


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif



