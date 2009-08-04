
.PHONY all: 
	$(MAKE) -f lib_gst-plugins-good.i \
			MK_MODULE=v4l2src \
			MK_PATH=gst-plugins-good/sys/v4l2 \
			MK_EXCLUDE=gstv4l2xoverlay			
	$(MAKE) -f lib_gst-plugins-good.i \
			MK_MODULE=xvimage \
			MK_PATH=gst-plugins-base/sys/xvimage \
			MK_EXCLUDE= \
			MK_OSLB="X11 Xxf86vm Xv"

