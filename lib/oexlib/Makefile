
# oexlib

include ../../make.def

default_target: all

NAME_PRJ := oexlib
FILE_EXE := lib$(NAME_PRJ)_d.a

export LOC_TAG := def
LOC_INC_def := inc
LOC_SRC_def := src
LOC_OUT_def := bin/$(BUILD_TYPE)
include ../../setup.mk

export LOC_TAG := os
LOC_INC_os := inc/os
LOC_SRC_os := src/os/$(PLATFORM)
LOC_OUT_os := bin/$(BUILD_TYPE)/os
include ../../setup.mk

all: all_def all_os $(PATH_EXE_def) 
rebuild: rebuild_def rebuild_os $(PATH_EXE_def)
clean: clean_def clean_os

$(PATH_EXE_def): $(OBJECTS_def) $(OBJECTS_os)
	$(AR) $(AFLAGS) $@ $(OBJECTS_def) $(OBJECTS_os)



