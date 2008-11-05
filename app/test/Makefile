
# test

default_target: all
OFFSET_ROOT := ../..
include $(OFFSET_ROOT)/stdinc.mk

NAME_PRJ := test
TYPE_PRJ := exe

export LOC_TAG := def
include $(OFFSET_ROOT)/build.mk

all: all_def $(PATH_EXE_def) 
rebuild: rebuild_def $(PATH_EXE_def)
rebuild: clean_def

LIBS 	 := -L$(BINROOT) -loexlib$(DPOSTFIX)
DEPENDS  := $(BINROOT)/liboexlib$(DPOSTFIX).a
.PHONY $(BINROOT)/liboexlib$(DPOSTFIX).a:

$(PATH_EXE_def): $(OBJECTS_def) $(DEPENDS)
	$(LD) $(LFLAGS) $(OBJECTS_def) -o $@ $(LIBS) 


