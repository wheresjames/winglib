
# CxImage

default_target: all
OFFSET_ROOT := ..
include $(OFFSET_ROOT)/config.mk

CFLAGS := $(CFLAGS) -I$(LIBROOT)/zlib

NAME_PRJ := cximage
TYPE_PRJ := lib

export LOC_TAG := def
LOC_INC_def := $(LIBROOT)/CxImage
LOC_SRC_def := $(LIBROOT)/CxImage
include $(OFFSET_ROOT)/build.mk

all: all_def $(PATH_EXE_def) 
rebuild: rebuild_def $(PATH_EXE_def)
rebuild: clean_def

$(PATH_EXE_def): $(OBJECTS_def) $(DEPENDS)
	$(AR) $(AFLAGS) $@ $(OBJECTS_def)


