
# Total dependencies
BUILDDIRS := lib/oexlib app/test

ifdef BUILDDEP
	BUILDDIRS := dep $(BUILDDIRS)
endif

.PHONY all: $(BUILDDIRS)

.PHONY $(BUILDDIRS) :
	$(MAKE) -C $@ $(BUILDTYPE)

