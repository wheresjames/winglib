
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gst$(MK_MODULE)
PRJ_TYPE := dll
PRJ_INCS := gstreamer/gstreamer gstreamer/gstreamer/libs \
			winglib/dep/etc/gstreamer/inc/posix \
			gstreamer/gst-plugins-base/gst-libs \
			gstreamer/gst-plugins-good/gst-libs \
			glib glib/glib glib/gmodule glib/gobject \
			libxml/include winglib/dep/etc/libxml/inc/posix
PRJ_LIBS := gstreamer
PRJ_DEFS := HAVE_CONFIG_H=1
PRJ_OSLB := $(MK_OSLB)
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

export LOC_TAG := gstm_$(MK_MODULE)
LOC_CXX_gstm_$(MK_MODULE) := c
LOC_SRC_gstm_$(MK_MODULE) := $(CFG_LIBROOT)/gstreamer/$(MK_PATH)
LOC_EXC_gstm_$(MK_MODULE) := $(MK_EXCLUDE)
include $(PRJ_LIBROOT)/build.mk


#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

