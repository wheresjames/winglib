
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := ffmpeg
PRJ_TYPE := lib
PRJ_INCS := winglib/dep/etc/ffmpeg/inc/posix ffmpeg ffmpeg/libavutil
PRJ_LIBS := 
PRJ_DEFS := 
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := libavcodec
LOC_CXX_libavcodec := c
#LOC_BLD_libavcodec := cpp
LOC_SRC_libavcodec := $(CFG_LIBROOT)/ffmpeg/libavcodec
LOC_LST_libavcodec := h264 h264dspenc h264enc h264idct ht264_mp4toannexb_bsf \
					  h264_parser h264pred
include $(PRJ_LIBROOT)/build.mk


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


