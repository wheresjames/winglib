
#-------------------------------------------------------------------
# Apps
#-------------------------------------------------------------------
BUILDDIRS := app/hello lib/oexlib \
			 tools/resbld tools/join \
			 app/test app/iptest app/srvrund app/srvmod app/modrun \
			 app/gstest app/irr_test \
			 app/axtest

#-------------------------------------------------------------------
# Squirrel engine
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) lib/sqbind app/sqrl app/sqengine


#-------------------------------------------------------------------
# Squirrel modules
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) sqmod/sqmod_cell \
                          sqmod/sqmod_curl \
						  sqmod/sqmod_fftw \
						  sqmod/sqmod_ffmpeg \
                          sqmod/sqmod_gdchart \
						  sqmod/sqmod_gstreamer \
						  sqmod/sqmod_irrlicht \
                          sqmod/sqmod_live555 \
                          sqmod/sqmod_mimetic \
                          sqmod/sqmod_mysql \
                          sqmod/sqmod_openssl \
                          sqmod/sqmod_poco \
                          sqmod/sqmod_portaudio \
						  sqmod/sqmod_test \
						  sqmod/sqmod_tinyxml \
                          sqmod/sqmod_vmime \
                          sqmod/sqmod_webkit

#-------------------------------------------------------------------
# WxWidgets
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) app/vidcap app/avi_scan \
						  app/irr_wxtest \
						  app/wx_webkit

.PHONY all: $(BUILDDIRS)

.PHONY $(BUILDDIRS) :
	$(MAKE) -C $@ $(BUILDTYPE)

