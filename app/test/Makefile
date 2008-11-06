
# test

default_target: all
OFFSET_ROOT := ../..
include $(OFFSET_ROOT)/config.mk

NAME_PRJ := test
TYPE_PRJ := exe

export LOC_TAG := def
include $(OFFSET_ROOT)/build.mk

all: all_def $(PATH_EXE_def) 
rebuild: rebuild_def $(PATH_EXE_def)
rebuild: clean_def

ifeq ($(OS),win32)
LIBS	 := /LIBPATH:$(BINROOT) oexlib$(DPOSTFIX).lib 
LIBS	 := $(LIBS) ole32.lib user32.lib
else
LIBS 	 := -L$(BINROOT) -loexlib$(DPOSTFIX)
DEPENDS  := $(BINROOT)/liboexlib$(DPOSTFIX).a
.PHONY $(BINROOT)/liboexlib$(DPOSTFIX).a:
endif

$(PATH_EXE_def): $(OBJECTS_def) $(DEPENDS)
	$(LD) $(LFLAGS) $(OBJECTS_def) $(LD_OUT)$@ $(LIBS)


