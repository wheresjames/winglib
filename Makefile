
# Directories to build
LIBDIRS := lib/oexlib 
APPDIRS := app/test

# Total dependencies
BUILDDIRS := $(LIBDIRS) $(APPDIRS)

action_all   = $(patsubst %,all-%,$(BUILDDIRS))
action_rebuild = $(patsubst %,rebuild-%,$(BUILDDIRS))
action_clean = $(patsubst %,clean-%,$(BUILDDIRS))

all: $(action_all)

rebuild: $(action_rebuild)

clean: $(action_clean)

.PHONY: dummy

$(action_all): dummy
	$(MAKE) -C $(patsubst all-%,%,$@) all

$(action_rebuild): dummy
	$(MAKE) -C $(patsubst rebuild-%,%,$@) rebuild

$(action_clean): dummy
	$(MAKE) -C $(patsubst clean-%,%,$@) clean

