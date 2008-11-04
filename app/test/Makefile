
# test

include ../../make.def

default_target: all

NAME_PRJ := test
FILE_EXE := test
LIBS 	 := -L../../lib/oexlib/bin/$(BUILD_TYPE)/ -loexlib_d
DEPENDS  := ../../lib/oexlib/Makefile

export LOC_TAG := def
LOC_INC_def := 
LOC_SRC_def := 
LOC_OUT_def := bin/$(BUILD_TYPE)
include ../../setup.mk

all: all_def $(PATH_EXE_def) 
rebuild: rebuild_def $(PATH_EXE_def)
rebuild: clean_def

$(PATH_EXE_def): $(OBJECTS_def) $(DEPENDS)
	$(LD) $(LFLAGS) $(OBJECTS_def) -o $@ $(LIBS) 


