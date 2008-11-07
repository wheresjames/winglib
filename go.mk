
# go.mk

GO_LIBS	 := $(foreach lib,$(PRJ_LIBS),-l$(lib)$(CFG_DPOSTFIX))
GO_LIBS	 := $(GO_LIBS) $(foreach lib,$(PRJ_OSLB),-l$(lib))

GO_LIBPATHS := -L$(CFG_BINROOT)
ifneq ($(CFG_BINROOT),$(CFG_OUTROOT))
	GO_LIBPATHS := $(GO_LIBPATHS) -L$(CFG_OUTROOT)
endif

ifeq ($(PRJ_TYPE),lib)
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL)	
	$(CFG_AR) $(CFG_AFLAGS) $(CFG_AR_OUT)$@ $(BLD_OBJECTS_TOTAL)
else
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL)	
	$(CFG_LD) $(CFG_LFLAGS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS)
endif

all: $(BLD_ALL) $(BLD_PATH_EXE) 
rebuild: $(BLD_REBUILD) $(BLD_PATH_EXE)
setup: $(BLD_SETUP)
clean: $(BLD_CLEAN)


