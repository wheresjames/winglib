
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gst-plugins-base
PRJ_TYPE := lib
PRJ_INCS := gstreamer gstreamer/libs \
			winglib/dep/etc/gstreamer/inc/posix \
			gst-plugins-base/gst-libs \
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

export LOC_TAG := gpb_interfaces
LOC_CXX_gpb_interfaces := c
LOC_SRC_gpb_interfaces := $(CFG_LIBROOT)/gst-plugins-base/gst-libs/gst/interfaces
LOC_EXC_gpb_interfaces := navigation
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

