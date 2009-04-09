
# exit 0 only works on Windows

unsupported:
	@echo =======================================================
	@echo = !!! $(UNSUPPORTED)
	@echo =======================================================
	$(warning $(UNSUPPORTED) )
	exit 0

all: unsupported
rebuild: unsupported
setup: unsupported
clean: unsupported

