
MK_INC := lib_gst-c-gst-plugins-good.i

.PHONY all: 
	$(MAKE) -f $(MK_INC) \
			MK_MODULE=coreelements \
			MK_PATH=gstreamer/plugins/elements
	$(MAKE) -f $(MK_INC) \
			MK_MODULE=video4linux \
			MK_PATH=gst-plugins-good/sys/v4l2 \
			MK_EXCLUDE=gstv4l2xoverlay			
	$(MAKE) -f $(MK_INC) \
			MK_MODULE=xvimage \
			MK_PATH=gst-plugins-base/sys/xvimage \
			MK_OSLB="X11 Xxf86vm Xv"
	$(MAKE) -f $(MK_INC) \
			MK_MODULE=ffmpegcolorspace \
			MK_PATH=gst-plugins-base/gst/ffmpegcolorspace

