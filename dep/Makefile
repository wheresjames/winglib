
# Total dependencies

action_all   = $(patsubst %,all-%,$(wildcard *.mk))
action_rebuild = $(patsubst %,rebuild-%,$(wildcard *.mk))
action_clean = $(patsubst %,clean-%,$(wildcard *.mk))

all: $(action_all)

rebuild: $(action_rebuild)

clean: $(action_clean)

.PHONY: dummy

$(action_all): dummy
	$(MAKE) -f $(patsubst all-%,%,$@) all

$(action_rebuild): dummy
	$(MAKE) -f $(patsubst rebuild-%,%,$@) rebuild

$(action_clean): dummy
	$(MAKE) -f $(patsubst clean-%,%,$@) clean

