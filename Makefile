
# regular apps
BUILDDIRS := app/hello lib/oexlib \
			 tools/resbld tools/join \
			 app/test app/iptest app/srvrund app/srvmod app/modrun \
			 app/gstest

#-------------------------------------------------------------------
# BUILDDEP	= Build dependent libraries
# SQMOD 	= Squirrel
# 3D 		= Irrlicht
# GUI 		= OpenGL / wxWidgets
# GUI_ES	= Embedded OpenGL / wxWidgets
#-------------------------------------------------------------------

ifdef 3D
	BUILDDIRS := $(BUILDDIRS) app/irr_test
endif

ifdef SQMOD
	BUILDDIRS := $(BUILDDIRS) lib/sqbind app/sqrl app/sqengine \
							  \
							  sqmod/sqmod_cell \
	                          sqmod/sqmod_curl \
							  sqmod/sqmod_fftw \
							  sqmod/sqmod_ffmpeg \
	                          sqmod/sqmod_gdchart \
							  sqmod/sqmod_gstreamer \
	                          sqmod/sqmod_http \
	                          sqmod/sqmod_live555 \
	                          sqmod/sqmod_mimetic \
	                          sqmod/sqmod_mysql \
	                          sqmod/sqmod_poco \
							  sqmod/sqmod_test \
							  sqmod/sqmod_tinyxml \
	                          sqmod/sqmod_vmime \
	                          sqmod/sqmod_webkit

	ifdef 3D
		BUILDDIRS := $(BUILDDIRS) sqmod/sqmod_irrlicht
	endif

endif

ifdef GUI
	BUILDDIRS := $(BUILDDIRS) app/vidcap app/avi_scan
	ifdef 3D
		BUILDDIRS := $(BUILDDIRS) app/irr_wxtest
	endif
	ifdef BUILD_WEBKIT
		BUILDDIRS := $(BUILDDIRS) app/wx_webkit
	endif
endif

ifdef BUILDDEP
	ifdef GUI
		BUILDDIRS := dep/gui $(BUILDDIRS)
	endif
	ifdef GUI_ES
		BUILDDIRS := dep/gui_es $(BUILDDIRS)
	endif
	BUILDDIRS := dep $(BUILDDIRS)
endif

.PHONY all: $(BUILDDIRS)

.PHONY $(BUILDDIRS) :
	$(MAKE) -C $@ $(BUILDTYPE)

