
# oexlib

default_target: all
OFFSET_ROOT := ../..
include $(OFFSET_ROOT)/config.mk

NAME_PRJ := oexlib
TYPE_PRJ := lib

export LOC_TAG := def
LOC_INC_def := inc
LOC_SRC_def := src
include $(OFFSET_ROOT)/build.mk

export LOC_TAG := os
LOC_INC_os := inc/os
LOC_SRC_os := src/os/$(PLATFORM)
LOC_OUT_os := os
include $(OFFSET_ROOT)/build.mk

all: all_def all_os $(PATH_EXE_def) 
rebuild: rebuild_def rebuild_os $(PATH_EXE_def)
clean: clean_def clean_os

$(PATH_EXE_def): $(OBJECTS_def) $(OBJECTS_os)
	$(AR) $(AFLAGS) $@ $(OBJECTS_def) $(OBJECTS_os)



