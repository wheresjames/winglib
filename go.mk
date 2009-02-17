
# go.mk

ifndef PRJ_OSLB
PRJ_OSLB :=	$(CFG_STDLIBS)
endif

ifeq ($(OS),win32)
GO_LIBS	 	:= $(foreach lib,$(PRJ_LIBS), $(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))
GO_LIBS	 	:= $(GO_LIBS) $(PRJ_OSLB)
GO_LIBPATHS := /LIBPATH:$(CFG_BINROOT)
ifneq ($(CFG_BINROOT),$(CFG_OUTROOT))
	GO_LIBPATHS := $(GO_LIBPATHS) /LIBPATH:$(CFG_OUTROOT)
endif
GO_EXPORTS 	:= $(foreach exp,$(PRJ_EXPORTS), $(CFG_FLAG_EXPORT)$(exp))
else
GO_LIBS	 	:= $(foreach lib,$(PRJ_LIBS),-l$(lib)$(CFG_DPOSTFIX))
GO_LIBS	 	:= $(GO_LIBS) $(foreach lib,$(PRJ_OSLB), -l$(lib))
GO_LIBPATHS := -L$(CFG_BINROOT)
ifneq ($(CFG_BINROOT),$(CFG_OUTROOT))
	GO_LIBPATHS := $(GO_LIBPATHS) -L$(CFG_OUTROOT)
endif
endif

ifeq ($(PRJ_TYPE),lib)
ifeq ($(OS),win32)	  
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(BLD_DEPENDS_TOTAL)
	- $(CFG_DEL) $(subst /,\,$@)
	$(CFG_AR) $(CFG_AFLAGS) $(CFG_AR_OUT)$@ $(BLD_OBJECTS_TOTAL)
else
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL)
	- $(CFG_DEL) $@
	$(CFG_AR) $(CFG_AFLAGS) $(CFG_AR_OUT)$@ $(BLD_OBJECTS_TOTAL)
endif
else
ifeq ($(PRJ_TYPE),dll)

GO_DEPENDS 	:= $(foreach lib,$(PRJ_LIBS),$(CFG_BINROOT)/$(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))

ifeq ($(OS),win32)	  
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS) $(BLD_DEPENDS_TOTAL)	
	- $(CFG_DEL) $(subst /,\,$@)
	$(CFG_LD) $(CFG_LFLAGS) $(GO_EXPORTS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS)
else
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)	
	- $(CFG_DEL) $@
	$(CFG_LD) $(CFG_LFLAGS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS) $(CFG_STDLIB)
endif

else

GO_DEPENDS 	:= $(foreach lib,$(PRJ_LIBS),$(CFG_BINROOT)/$(CFG_LIB_PRE)$(lib)$(CFG_DPOSTFIX)$(CFG_LIB_POST))

#$(GO_DEPENDS):
#	$(MAKE) -C $(PRJ_LIBROOT)
	
ifeq ($(OS),win32)	  
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)	$(BLD_DEPENDS_TOTAL)
	- $(CFG_DEL) $(subst /,\,$@)
	$(CFG_LD) $(CFG_LFLAGS) $(GO_EXPORTS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS)
else
$(BLD_PATH_EXE): $(BLD_OBJECTS_TOTAL) $(GO_DEPENDS)	
	- $(CFG_DEL) $@
	$(CFG_LD) $(CFG_LFLAGS) $(BLD_OBJECTS_TOTAL) $(CFG_LD_OUT)$@ $(GO_LIBPATHS) $(GO_LIBS) $(CFG_STDLIB)
endif	
endif
endif

all: $(BLD_ALL) $(BLD_PATH_EXE) 
rebuild: $(BLD_REBUILD) $(BLD_PATH_EXE)
setup: $(BLD_SETUP)
clean: $(BLD_CLEAN)


