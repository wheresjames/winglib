# build.mk

ifneq ($(PRJ_DEFS),)
	ifeq ($(BUILD),vs)
		CFG_ADD_DEFS := $(foreach def,$(PRJ_DEFS),/D$(def) )
	else
		CFG_ADD_DEFS := $(foreach def,$(PRJ_DEFS),-D$(def) )
	endif	
	CFG_DEFS := $(CFG_DEFS) $(CFG_ADD_DEFS)
	PRJ_DEFS := 
endif

ifeq ($(LOC_CXX_$(LOC_TAG)),)
	LOC_CXX_$(LOC_TAG) := cpp
endif

ifeq ($(LOC_H_$(LOC_TAG)),)
	LOC_H_$(LOC_TAG) := h
endif

ifeq ($(LOC_BLD_$(LOC_TAG)),)
	LOC_BLD_$(LOC_TAG) := $(LOC_CXX_$(LOC_TAG))
else
	ifeq ($(LOC_BLD_$(LOC_TAG)),c)
		BLD_MSFLAGS := /Tc
	else
		ifeq ($(LOC_BLD_$(LOC_TAG)),cpp)
			BLD_MSFLAGS := /Tp
		endif
	endif
endif

ifeq ($(LOC_BLD_$(LOC_TAG)),c)
	BLD_COMPILER := $(CFG_CC)
else
	BLD_COMPILER := $(CFG_PP)
endif

# Using full paths helps IDE editors to locate the file when there's an error ;)
ifneq ($(LOC_SRC_$(LOC_TAG)),)
BLD_PATH_SRC_$(LOC_TAG) := $(CFG_CUR_ROOT)/$(LOC_SRC_$(LOC_TAG))
ifneq ($(LOC_SRC_$(LOC_TAG)),$(LOC_INC_$(LOC_TAG)))
	BLD_PATH_INC_$(LOC_TAG) := $(CFG_CC_INC)$(CFG_CUR_ROOT)/$(LOC_SRC_$(LOC_TAG))
endif
else
	BLD_PATH_SRC_$(LOC_TAG) := $(CFG_CUR_ROOT)
endif

ifneq ($(LOC_INC_$(LOC_TAG)),)
	BLD_PATH_INC_$(LOC_TAG) := $(BLD_PATH_INC_$(LOC_TAG)) $(CFG_CC_INC)$(CFG_CUR_ROOT)/$(LOC_INC_$(LOC_TAG))
else
ifeq ($(LOC_SRC_$(LOC_TAG)),)
	BLD_PATH_INC_$(LOC_TAG) := $(BLD_PATH_INC_$(LOC_TAG)) $(CFG_CC_INC)$(CFG_CUR_ROOT)
endif
endif

#ifneq ($(LOC_OUT_$(LOC_TAG)),)
#BLD_PATH_BIN_$(LOC_TAG) := $(CFG_OUTROOT)/_0_obj/$(PRJ_NAME)/$(LOC_OUT_$(LOC_TAG))
#else
BLD_PATH_BIN_$(LOC_TAG) := $(CFG_OUTROOT)/_0_obj/$(PRJ_NAME)/$(LOC_TAG)
#endif

BLD_PATH_OBJ_$(LOC_TAG) := $(BLD_PATH_BIN_$(LOC_TAG))
BLD_PATH_INS_$(LOC_TAG) := /usr/share/$(PRJ_NAME)
BLD_PATH_LNK_$(LOC_TAG) := /usr/bin

#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------
ifneq ($(LOC_LST_$(LOC_TAG)),)
	BLD_SOURCES_$(LOC_TAG) 	:= $(foreach file,$(LOC_LST_$(LOC_TAG)),$(BLD_PATH_SRC_$(LOC_TAG))/$(file).$(LOC_CXX_$(LOC_TAG)))
else
	BLD_SOURCES_$(LOC_TAG) 	:= $(wildcard $(BLD_PATH_SRC_$(LOC_TAG))/*.$(LOC_CXX_$(LOC_TAG)))
endif

ifneq ($(LOC_EXC_$(LOC_TAG)),)
	BLD_EXCLUDE_$(LOC_TAG) 	:= $(foreach file,$(LOC_EXC_$(LOC_TAG)),$(BLD_PATH_SRC_$(LOC_TAG))/$(file).$(LOC_CXX_$(LOC_TAG)))
	BLD_SOURCES_$(LOC_TAG) 	:= $(filter-out $(BLD_EXCLUDE_$(LOC_TAG)),$(BLD_SOURCES_$(LOC_TAG)))
endif

BLD_OBJECTS_$(LOC_TAG) 	:= $(subst $(BLD_PATH_SRC_$(LOC_TAG))/,$(BLD_PATH_OBJ_$(LOC_TAG))/, $(BLD_SOURCES_$(LOC_TAG):.$(LOC_CXX_$(LOC_TAG))=.$(CFG_OBJ_EXT)) )
BLD_INCS			    := $(BLD_PATH_INC_$(LOC_TAG)) $(foreach inc,$(PRJ_INCS), $(CFG_CC_INC)$(CFG_LIBROOT)/$(inc))
ifneq ($(PRJ_SYSI),)
	BLD_INCS			:= $(BLD_INCS) $(foreach inc,$(PRJ_SYSI), $(CFG_CC_INC)$(inc))
endif
ifdef CFG_RES_OUT
	BLD_INCS			:= $(CFG_CC_INC)$(CFG_RES_OUT) $(BLD_INCS)
endif

BLD_OBJECTS_TOTAL 		:= $(BLD_OBJECTS_TOTAL) $(BLD_OBJECTS_$(LOC_TAG))
#BLD_DEPENDS_$(LOC_TAG) := $(subst $(BLD_PATH_SRC_$(LOC_TAG))/,$(BLD_PATH_OBJ_$(LOC_TAG))/, $(BLD_SOURCES_$(LOC_TAG):.$(LOC_CXX_$(LOC_TAG))=.$(CFG_DEP_EXT)) )
#BLD_DEPENDS_INCS		:= -I$(BLD_PATH_INC_$(LOC_TAG)) $(foreach inc,$(PRJ_INCS), -I$(CFG_LIBROOT)/$(inc))
#BLD_DEPENDS_TOTAL 		:= $(BLD_DEPENDS_TOTAL) $(BLD_DEPENDS_$(LOC_TAG))


ifneq ($(BUILD),vs)
	include $(wildcard $(BLD_PATH_OBJ_$(LOC_TAG))/*.$(CFG_DEP_EXT))
endif

#-------------------------------------------------------------------
# Options
#-------------------------------------------------------------------

all_$(LOC_TAG): setup_$(LOC_TAG)

rebuild_$(LOC_TAG): clean_$(LOC_TAG) all_$(LOC_TAG)

setup_$(LOC_TAG): $(BLD_PATH_OBJ_$(LOC_TAG))

ifeq ($(BUILD),vs)

$(BLD_PATH_OBJ_$(LOC_TAG)):
	$(CFG_MD) "$(subst /,\,$@)"
	
clean_$(LOC_TAG):
	- $(CFG_RM) "$(subst /,\,$(BLD_PATH_OBJ_$(LOC_TAG)))"
	
else

$(BLD_PATH_OBJ_$(LOC_TAG)):
	$(CFG_MD) $@
	
clean_$(LOC_TAG):
	- $(CFG_RM) $(BLD_PATH_OBJ_$(LOC_TAG))

endif

BLD_ALL 	:= $(BLD_ALL) all_$(LOC_TAG)
BLD_REBUILD := $(BLD_REBUID) rebuild_$(LOC_TAG)
BLD_SETUP 	:= $(BLD_SETUP) setup_$(LOC_TAG)
BLD_CLEAN 	:= $(BLD_CLEAN) clean_$(LOC_TAG)

#-------------------------------------------------------------------
# Build
#-------------------------------------------------------------------

$(print BLD_OBJECTS_$(LOC_TAG) )
$(print $(BLD_PATH_OBJ_$(LOC_TAG))/%.$(CFG_OBJ_EXT) )

ifeq ($(BUILD),vs)

#$(subst $(BLD_PATH_SRC_$(LOC_TAG))/,,$<)
#$(BLD_PATH_OBJ_$(LOC_TAG))/%.$(CFG_DEP_EXT) : $(BLD_PATH_SRC_$(LOC_TAG))/%.$(LOC_CXX_$(LOC_TAG))
#	echo # makedepend -o.obj $(subst /,\,$(BLD_DEPENDS_INCS)) -p$(BLD_PATH_OBJ_$(LOC_TAG))/ -f$@ $< >> $@
#	$(CFG_DP) -o.obj $(subst /,\,$(BLD_DEPENDS_INCS)) -f$@ $<

$(BLD_PATH_OBJ_$(LOC_TAG))/%.$(CFG_OBJ_EXT) : $(BLD_PATH_SRC_$(LOC_TAG))/%.$(LOC_CXX_$(LOC_TAG))
	- $(CFG_DEL) $(subst /,\,$@)
	$(BLD_COMPILER) $(CFG_CFLAGS) $(CFG_DEFS) $(BLD_INCS) $(BLD_MSFLAGS) $< $(CFG_CC_OUT)$@
	
else
$(BLD_PATH_OBJ_$(LOC_TAG))/%.$(CFG_OBJ_EXT) : $(BLD_PATH_SRC_$(LOC_TAG))/%.$(LOC_CXX_$(LOC_TAG))
	- $(CFG_DEL) $@
	$(BLD_COMPILER) $(CFG_CFLAGS) $(CFG_DEFS) $(PRJ_EXTC) $(BLD_INCS) $< $(CFG_CC_OUT)$@
endif


