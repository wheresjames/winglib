
# Total dependencies

makefiles   = $(wildcard *.mk)

.PHONY all: $(makefiles)
.PHONY rebuild: $(makefiles)
.PHONY clean: $(makefiles)

.PHONY $(makefiles) :
	$(MAKE) -f $@ $(BUILDTYPE)

