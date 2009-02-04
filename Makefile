
# Total dependencies
BUILDDIRS := lib/oexlib app/test app/srvrund app/srvmod	app/sqengine

ifdef GUI
	BUILDDIRS := $(BUILDDIRS) app/vidcap app/avi_scan
endif

ifdef BUILDDEP
	BUILDDIRS := dep $(BUILDDIRS)
endif

.PHONY all: $(BUILDDIRS)

.PHONY $(BUILDDIRS) :
	$(MAKE) -C $@ $(BUILDTYPE)

