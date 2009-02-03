
# Total dependencies
BUILDDIRS := lib/oexlib app/test app/srvrund app/srvmod	app/vidcap app/avi_scan app/sqengine

ifdef BUILDDEP
	BUILDDIRS := dep $(BUILDDIRS)
endif

.PHONY all: $(BUILDDIRS)

.PHONY $(BUILDDIRS) :
	$(MAKE) -C $@ $(BUILDTYPE)

