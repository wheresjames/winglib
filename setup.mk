
#-------------------------------------------------------------------
# Vars
#-------------------------------------------------------------------
PATH_SRC_$(LOC_TAG) := $(CUR_ROOT)/$(LOC_SRC_$(LOC_TAG))
PATH_INC_$(LOC_TAG) := $(CUR_ROOT)/$(LOC_INC_$(LOC_TAG))
PATH_BIN_$(LOC_TAG) := $(CUR_ROOT)/$(LOC_OUT_$(LOC_TAG))
PATH_OBJ_$(LOC_TAG) := $(PATH_BIN_$(LOC_TAG))/obj
PATH_EXE_$(LOC_TAG) := $(PATH_BIN_$(LOC_TAG))/$(FILE_EXE)
PATH_INS_$(LOC_TAG) := /usr/share/$(NAME_PRJ)
PATH_LNK_$(LOC_TAG) := /usr/bin

#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------
HEADERS_$(LOC_TAG) 	:= $(wildcard $(PATH_INC_$(LOC_TAG))/*.h)
SOURCES_$(LOC_TAG) 	:= $(wildcard $(PATH_SRC_$(LOC_TAG))/*.cpp)
OBJECTS_$(LOC_TAG) 	:= $(subst $(PATH_SRC_$(LOC_TAG))/,$(PATH_OBJ_$(LOC_TAG))/, $(SOURCES_$(LOC_TAG):.cpp=.o) )

#-------------------------------------------------------------------
# Options
#-------------------------------------------------------------------

all_$(LOC_TAG): setup_$(LOC_TAG)

rebuild_$(LOC_TAG): clean_$(LOC_TAG) all_$(LOC_TAG)

setup_$(LOC_TAG): $(PATH_BIN_$(LOC_TAG)) $(PATH_OBJ_$(LOC_TAG))

$(PATH_BIN_$(LOC_TAG)):
	-mkdir -p $@

$(PATH_OBJ_$(LOC_TAG)):
	-mkdir -p $@

clean_$(LOC_TAG):
	@rm -rf $(PATH_OUT_$(LOC_TAG))
	
#-------------------------------------------------------------------
# Build
#-------------------------------------------------------------------

$(PATH_OBJ_$(LOC_TAG))/%.o : $(PATH_SRC_$(LOC_TAG))/%.cpp $(HEADERS_$(LOC_TAG))
	$(CC) $(CFLAGS) $< -o $@
#	- $(CC) $(SFLAGS) $< -o $(subst .o,.asm, $@)


