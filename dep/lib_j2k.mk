
# j2k

default_target: all
OFFSET_ROOT := ..
include $(OFFSET_ROOT)/config.mk

ifeq ($(OS),win32)
CFLAGS := $(CFLAGS) /DLIBJ2K_EXPORTS
endif

#CFLAGS := $(CFLAGS) -I$(LIBROOT)/

NAME_PRJ := j2k
TYPE_PRJ := lib

export LOC_TAG := def
LOC_CXX_def := c
LOC_INC_def := $(LIBROOT)/j2k
LOC_SRC_def := $(LIBROOT)/j2k
include $(OFFSET_ROOT)/build.mk

all: all_def $(PATH_EXE_def) 
rebuild: rebuild_def $(PATH_EXE_def)
rebuild: clean_def

$(PATH_EXE_def): $(OBJECTS_def) $(DEPENDS)
	$(AR) $(AFLAGS) $(AR_OUT)$@ $(OBJECTS_def)


