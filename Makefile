
#-------------------------------------------------------------------
# Apps
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) \
			 app/hello \
			 app/hello_android \
			 app/webdroid \
			 lib/oexlib \
			 tools/resbld \
			 tools/join \
			 app/test \

ifdef PETS
BUILDDIRS := $(BUILDDIRS) \
			 app/hello \
			 app/plock \
			 lib/oexlib \
			 tools/resbld \
			 tools/join \
			 app/test \
			 app/iptest \
			 app/srvrund \
			 app/srvmod \
			 app/modrun \
			 app/gstest \
			 app/irr_android \
			 app/alink \
			 app/axtest \
			 app/sqplugin
endif

#			 app/fftest
#			 tools/cii \
#			 app/irr_test \
#			 app/hello_android \

#-------------------------------------------------------------------
# Qt WebKit apps
#-------------------------------------------------------------------
ifdef QTWK
BUILDDIRS := $(BUILDDIRS) \
			 qtwk/gtw
endif

#-------------------------------------------------------------------
# Squirrel engine
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) \
			 lib/sqbind \
			 tools/sqrbld \
			 app/sqrl \
			 app/sqrl-cgi

#			 app/sqengine

#-------------------------------------------------------------------
# Squirrel Apps
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) \
			 sq/sntpsync

ifdef PETS
BUILDDIRS := $(BUILDDIRS) \
			 sq/topsize \
			 sq/wlink
endif

#-------------------------------------------------------------------
# Squirrel modules
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) \
			 sqmod/sqmod_asio \
			 sqmod/sqmod_cell \
			 sqmod/sqmod_crtmp \
			 sqmod/sqmod_curl \
			 sqmod/sqmod_fftw \
			 sqmod/sqmod_ffmpeg \
			 sqmod/sqmod_freetype2 \
			 sqmod/sqmod_gdchart \
			 sqmod/sqmod_gsoap \
			 sqmod/sqmod_live555 \
			 sqmod/sqmod_mysql \
			 sqmod/sqmod_openssl \
			 sqmod/sqmod_poco \
			 sqmod/sqmod_portaudio \
			 sqmod/sqmod_rtmpd \
			 sqmod/sqmod_ssh2 \
			 sqmod/sqmod_tinyxml

ifdef PETS
BUILDDIRS := $(BUILDDIRS) \
			 sqmod/sqmod_freenect \
			 sqmod/sqmod_gstreamer \
			 sqmod/sqmod_haru \
			 sqmod/sqmod_irrlicht \
			 sqmod/sqmod_mimetic \
			 sqmod/sqmod_quickfix \
			 sqmod/sqmod_test \
			 sqmod/sqmod_usb \
			 sqmod/sqmod_vmime \
			 sqmod/sqmod_webkit \
			 sqmod/sqmod_webrtc
endif
#-------------------------------------------------------------------
# WxWidgets
#-------------------------------------------------------------------
#BUILDDIRS := $(BUILDDIRS) app/vidcap app/avi_scan \
#						  app/irr_wxtest \
#						  app/wx_webkit

#-------------------------------------------------------------------
# Installers
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) install/sntpsync

#-------------------------------------------------------------------
# APK
#-------------------------------------------------------------------
#BUILDDIRS := $(BUILDDIRS) app/atest

#-------------------------------------------------------------------
# doxygen
#-------------------------------------------------------------------
ifdef DOX
BUILDDIRS := $(BUILDDIRS) dox
endif

#-------------------------------------------------------------------
# multithreading
#-------------------------------------------------------------------
ifeq ($(NOMULTI),)
ifeq ($(NUMJOBS),)
ifdef NUMBER_OF_PROCESSORS
NUMJOBS := -j$(NUMBER_OF_PROCESSORS)
else
NUMJOBS := -j2
endif
endif
endif


.PHONY all: $(BUILDDIRS)

.PHONY $(BUILDDIRS) :
	$(MAKE) $(NUMJOBS) -C $@ $(BUILDTYPE)

