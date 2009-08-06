
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gst-plugins-base
PRJ_TYPE := lib
PRJ_INCS := gstreamer/gstreamer gstreamer/gstreamer/libs \
			winglib/dep/etc/gstreamer/inc/posix \
			winglib/dep/etc/gstreamer/inc/posix/gst \
			winglib/dep/etc/gstreamer/inc/posix/gst/interfaces \
			gstreamer/gst-plugins-base/gst-libs \
			gstreamer/gst-plugins-base/gst-libs/gst/interfaces \
			glib glib/glib glib/gmodule glib/gobject \
			libxml/include winglib/dep/etc/libxml/inc/posix
PRJ_LIBS := 
PRJ_DEFS := HAVE_CONFIG_H=1 
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifndef BUILD_GSTREAMER
UNSUPPORTED := Set make option BUILD_GSTREAMER=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := gpb_custom
LOC_CXX_gpb_custom := c
#LOC_INC_gpb_custom := $(CFG_LIBROOT)/gstreamer/gst-plugins-base/gst-libs/gst/interfaces
LOC_SRC_gpb_custom := $(CFG_LIBROOT)/winglib/dep/etc/gstreamer/src/posix/plugins
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gpb_interfaces
LOC_CXX_gpb_interfaces := c
LOC_SRC_gpb_interfaces := $(CFG_LIBROOT)/gstreamer/gst-plugins-base/gst-libs/gst/interfaces
LOC_EXC_gpb_interfaces := 
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gpb_video
LOC_CXX_gpb_video := c
LOC_SRC_gpb_video := $(CFG_LIBROOT)/gstreamer/gst-plugins-base/gst-libs/gst/video
LOC_EXC_gpb_video := 
include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := gst_libs_base
#LOC_CXX_gst_libs_base := c
#LOC_INC_gst_libs_base := $(CFG_LIBROOT)/gstreamer/gstreamer/libs
#LOC_SRC_gst_libs_base := $(CFG_LIBROOT)/gstreamer/gstreamer/libs/gst/base
#LOC_LST_gst_libs_base := gstbasetransform gstbasesink gstbasesrc gstpushsrc \
#						 gstdataqueue gsttypefindhelper
#include $(PRJ_LIBROOT)/build.mk


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

