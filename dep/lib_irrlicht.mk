
# irrlicht

default_target: all
OFFSET_ROOT := ..
include $(OFFSET_ROOT)/config.mk

CFLAGS := $(CFLAGS) -I$(LIBROOT)/irrlicht/include
CFLAGS := $(CFLAGS) -I$(LIBROOT)/irrlicht/source/Irrlicht/zlib

NAME_PRJ := irrlicht
TYPE_PRJ := lib

export LOC_TAG := def
LOC_INC_def := $(LIBROOT)/irrlicht/source/Irrlicht
LOC_SRC_def := $(LIBROOT)/irrlicht/source/Irrlicht
include $(OFFSET_ROOT)/build.mk

all: all_def $(PATH_EXE_def) 
rebuild: rebuild_def $(PATH_EXE_def)
rebuild: clean_def

$(PATH_EXE_def): $(OBJECTS_def) $(DEPENDS)
	$(AR) $(AFLAGS) $@ $(OBJECTS_def)


