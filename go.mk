
# go.mk

ifndef PRJ_OSLB
PRJ_OSLB :=	$(CFG_STDLIBS)
endif

ifeq ($(OS),win32)
GO_LIBS	 	:= $(foreach lib,$(PRJ_LIBS), $(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))
GO_LIBS	 	:= $(GO_LIBS) $(PRJ_OSLB)
GO_LIBPATHS := /LIBPATH:$(CFG_BINROOT)
else
GO_LIBS	 	:= $(foreach lib,$(PRJ_LIBS),-l$(lib)$(CFG_DPOSTFIX))
GO_LIBS	 	:= $(GO_LIBS) $(foreach lib,$(PRJ_OSLB), -l$(lib))
GO_LIBPATHS := -L$(CFG_BINROOT)
endif

ifneq ($(CFG_BINROOT),$(CFG_OUTROOT))
	GO_LIBPATHS := $(GO_LIBPATHS) -L$(CFG_OUTROOT)
endif

ifeq ($(PRJ_TYPE),lib)
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL)
	- $(CFG_DEL) $@
	$(CFG_AR) $(CFG_AFLAGS) $(CFG_AR_OUT)$@ $(BLD_OBJECTS_TOTAL)
else
ifeq ($(PRJ_TYPE),dll)

GO_DEPENDS 	:= $(foreach lib,$(PRJ_LIBS),$(CFG_BINROOT)/$(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))

$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)	
	- $(CFG_DEL) $@
	$(CFG_LD) $(CFG_LFLAGS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS)

else

GO_DEPENDS 	:= $(foreach lib,$(PRJ_LIBS),$(CFG_BINROOT)/$(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))

#$(GO_DEPENDS):
#	$(MAKE) -C $(PRJ_LIBROOT)
	
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)	
	- $(CFG_DEL) $@
	$(CFG_LD) $(CFG_LFLAGS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS)
	
endif
endif

all: $(BLD_ALL) $(BLD_PATH_EXE) 
rebuild: $(BLD_REBUILD) $(BLD_PATH_EXE)
setup: $(BLD_SETUP)
clean: $(BLD_CLEAN)


