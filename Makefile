
# Total dependencies
BUILDDIRS := lib/oexlib app/test app/iptest app/srvrund app/srvmod app/sqengine

ifdef SQMOD
	BUILDDIRS := $(BUILDDIRS) app/sq_modules/sqmod_test app/sq_modules/sqmod_cell \
	                          app/sq_modules/sqmod_http
endif

ifdef GUI
	BUILDDIRS := $(BUILDDIRS) app/vidcap app/avi_scan
endif

ifdef BUILDDEP
	BUILDDIRS := dep $(BUILDDIRS)
endif

.PHONY all: $(BUILDDIRS)

.PHONY $(BUILDDIRS) :
	$(MAKE) -C $@ $(BUILDTYPE)

