
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := gstest
PRJ_TYPE := exe
PRJ_INCS := gstreamer/gstreamer winglib/dep/etc/gstreamer/inc/posix \
			glib glib/glib glib/gmodule glib/gobject \
			libxml/include winglib/dep/etc/libxml/inc/posix
PRJ_LIBS := gst-plugins-base gstreamer glib xml zlib
PRJ_DEFS := 
PRJ_LIBP :=
PRJ_OSLB :=
 
PRJ_LIBROOT := ../..

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
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif
