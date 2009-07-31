
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gst-plugins
PRJ_TYPE := lib
PRJ_INCS := gstreamer gst-plugins/gst-libs \
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

export LOC_TAG := gp_v4l2
LOC_CXX_gp_v4l2 := c
LOC_SRC_gp_v4l2 := $(CFG_LIBROOT)/gst-plugins/sys/v4l2
LOC_EXC_gp_v4l2 := 
include $(PRJ_LIBROOT)/build.mk


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

