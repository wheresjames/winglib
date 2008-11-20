
# build.mk

ifndef BLD_FILE_EXE
	ifdef PRJ_FILE_EXE		 
		BLD_FILE_EXE := $(PRJ_FILE_EXE)
	else
		ifeq ($(PRJ_TYPE),lib)
			BLD_FILE_EXE := $(CFG_LIB_PRE)$(PRJ_NAME)$(CFG_DPOSTFIX)$(CFG_LIB_POST)
		else
			ifeq ($(PRJ_TYPE),dll)
				BLD_FILE_EXE := $(CFG_DLL_PRE)$(PRJ_NAME)$(CFG_DPOSTFIX)$(CFG_DLL_POST)
			else
				BLD_FILE_EXE := $(CFG_EXE_PRE)$(PRJ_NAME)$(CFG_DPOSTFIX)$(CFG_EXE_POST)
			endif
		endif
	endif
	BLD_PATH_EXE := $(CFG_OUTROOT)/$(BLD_FILE_EXE)	
endif

ifndef LOC_CXX_$(LOC_TAG)
	LOC_CXX_$(LOC_TAG) := cpp
endif

ifndef LOC_H_$(LOC_TAG)
	LOC_H_$(LOC_TAG) := h
endif

ifeq ($(LOC_CXX_$(LOC_TAG)),c)
BLD_COMPILER := $(CFG_CC)
else
BLD_COMPILER := $(CFG_PP)
endif

# Using full paths helps IDE editors to locate the file when there's an error ;)
ifdef LOC_SRC_$(LOC_TAG)
BLD_PATH_SRC_$(LOC_TAG) := $(CFG_CUR_ROOT)/$(LOC_SRC_$(LOC_TAG))
else
BLD_PATH_SRC_$(LOC_TAG) := $(CFG_CUR_ROOT)
endif

ifdef LOC_INC_$(LOC_TAG)
BLD_PATH_INC_$(LOC_TAG) := $(CFG_CUR_ROOT)/$(LOC_INC_$(LOC_TAG))
else
BLD_PATH_INC_$(LOC_TAG) := $(CFG_CUR_ROOT)
endif

ifdef LOC_OUT_$(LOC_TAG)
BLD_PATH_BIN_$(LOC_TAG) := $(CFG_OUTROOT)/_$(PRJ_NAME)/$(LOC_OUT_$(LOC_TAG))
else
BLD_PATH_BIN_$(LOC_TAG) := $(CFG_OUTROOT)/_$(PRJ_NAME)
endif

BLD_PATH_OBJ_$(LOC_TAG) := $(BLD_PATH_BIN_$(LOC_TAG))/obj
BLD_PATH_INS_$(LOC_TAG) := /usr/share/$(PRJ_NAME)
BLD_PATH_LNK_$(LOC_TAG) := /usr/bin


#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------
BLD_SOURCES_$(LOC_TAG) 	:= $(wildcard $(BLD_PATH_SRC_$(LOC_TAG))/*.$(LOC_CXX_$(LOC_TAG)))
BLD_OBJECTS_$(LOC_TAG) 	:= $(subst $(BLD_PATH_SRC_$(LOC_TAG))/,$(BLD_PATH_OBJ_$(LOC_TAG))/, $(BLD_SOURCES_$(LOC_TAG):.$(LOC_CXX_$(LOC_TAG))=.$(CFG_OBJ_EXT)) )
BLD_INCS			    := $(CFG_CC_INC)$(BLD_PATH_INC_$(LOC_TAG)) $(foreach inc,$(PRJ_INCS), $(CFG_CC_INC)$(CFG_LIBROOT)/$(inc))

BLD_OBJECTS_TOTAL := $(BLD_OBJECTS_TOTAL) $(BLD_OBJECTS_$(LOC_TAG))

include $(wildcard $(BLD_PATH_OBJ_$(LOC_TAG))/*.d)

#-------------------------------------------------------------------
# Options
#-------------------------------------------------------------------

all_$(LOC_TAG): setup_$(LOC_TAG)

rebuild_$(LOC_TAG): clean_$(LOC_TAG) all_$(LOC_TAG)

setup_$(LOC_TAG): $(BLD_PATH_OBJ_$(LOC_TAG))

ifeq ($(OS),win32)

$(BLD_PATH_OBJ_$(LOC_TAG)):
	$(CFG_MD) $(subst /,\,$@)
	
clean_$(LOC_TAG):
	- $(CFG_RM) "$(subst /,\,$(BLD_PATH_OBJ_$(LOC_TAG)))"
	
else

$(BLD_PATH_OBJ_$(LOC_TAG)):
	$(CFG_MD) $@
	
clean_$(LOC_TAG):
	- $(CFG_RM) $(BLD_PATH_OBJ_$(LOC_TAG))

endif

BLD_ALL := $(BLD_ALL) all_$(LOC_TAG)
BLD_REBUILD := $(BLD_REBUID) rebuild_$(LOC_TAG)
BLD_SETUP := $(BLD_SETUP) setup_$(LOC_TAG)
BLD_CLEAN := $(BLD_CLEAN) clean_$(LOC_TAG)

#-------------------------------------------------------------------
# Build
#-------------------------------------------------------------------

$(BLD_PATH_OBJ_$(LOC_TAG))/%.$(CFG_OBJ_EXT) : $(BLD_PATH_SRC_$(LOC_TAG))/%.$(LOC_CXX_$(LOC_TAG))
	- $(CFG_DEL) $@
	$(BLD_COMPILER) $(CFG_CFLAGS) $(BLD_INCS) $< $(CFG_CC_OUT)$@


