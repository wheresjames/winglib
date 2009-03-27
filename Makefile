
# Total dependencies
BUILDDIRS := app/hello lib/oexlib app/test app/iptest app/srvrund app/srvmod app/modrun

ifdef SQMOD
	BUILDDIRS := $(BUILDDIRS) lib/sqbind app/sqengine \
							  app/sq_modules/sqmod_test app/sq_modules/sqmod_cell \
	                          app/sq_modules/sqmod_http app/sq_modules/sqmod_gdchart                         
endif

ifdef GUI
	BUILDDIRS := $(BUILDDIRS) app/vidcap app/avi_scan
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

