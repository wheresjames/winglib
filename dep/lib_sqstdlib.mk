
# sqstdlib

default_target: all
OFFSET_ROOT := ..
include $(OFFSET_ROOT)/config.mk

CFLAGS := $(CFLAGS) -I$(LIBROOT)/SqPlus/include

NAME_PRJ := sqstdlib
TYPE_PRJ := lib

export LOC_TAG := def
LOC_INC_def := $(LIBROOT)/SqPlus/sqstdlib
LOC_SRC_def := $(LIBROOT)/SqPlus/sqstdlib
include $(OFFSET_ROOT)/build.mk

all: all_def $(PATH_EXE_def) 
rebuild: rebuild_def $(PATH_EXE_def)
rebuild: clean_def

$(PATH_EXE_def): $(OBJECTS_def) $(DEPENDS)
	$(AR) $(AFLAGS) $(AR_OUT)$@ $(OBJECTS_def)


