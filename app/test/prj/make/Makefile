
# Use Bourne shell
SHELL := /bin/sh

default_target: all

TARGET	 := linux
PLATFORM := posix

LIBS :=
#LIBS := -L../../../../lib/oexlib/bin/$(TARGET)/ -loexlib_d

# Flags
#LFLAGS := -g -lstdc++ -lrt -luuid
LFLAGS := -lrt -static 
CFLAGS := -c
SFLAGS := $(CFLAGS) -S -MMD
AFLAGS := rcs

# Compiler / Linker
#CC := g++
#CC := /home/landshark/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-g++
CC := /home/landshark/code/arm/usr/local/bin/arm-linux-g++
#LD := g++
#LD := /home/landshark/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-g++
LD := /home/landshark/code/arm/usr/local/bin/arm-linux-g++
#AR := /home/landshark/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-ar
AR := /home/landshark/code/arm/usr/local/bin/arm-linux-ar
DP := makedepend

#-------------------------------------------------------------------
# Vars
#-------------------------------------------------------------------
NAME_PRJ := test

FILE_EXE := $(NAME_PRJ)
PATH_SRC := ../..
PATH_BIN := ../../bin
PATH_OUT := $(PATH_BIN)/$(TARGET)
PATH_OBJ := $(PATH_OUT)/obj
PATH_EXE := $(PATH_OUT)/$(FILE_EXE)
PATH_INS := /usr/share/$(NAME_PRJ)
PATH_LNK := /usr/bin

#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------
SOURCES :=	$(PATH_SRC)/stdafx.cpp 					\
			$(PATH_SRC)/test_arm.cpp

OBJECTS := $(subst $(PATH_SRC),$(PATH_OBJ), $(SOURCES:.cpp=.o) )

TARGETS := $(PATH_EXE)

#-------------------------------------------------------------------
# Options
#-------------------------------------------------------------------

all: setup $(TARGETS)

rebuild: clean all

setup:
	@-mkdir -p $(PATH_BIN)
	@-mkdir -p $(PATH_OUT)
	@-mkdir -p $(PATH_OBJ)

clean:
	@rm -rf $(PATH_OUT)
	
install:
	@-mkdir -p "$(PATH_INS)"
	cp "$(PAT  H_EXE)" "$(PATH_INS)/$(FILE_EXE)"
	ln -s "$(PATH_INS)/$(FILE_EXE)" "$(PATH_LNK)/$(FILE_EXE)"

uninstall:	
	@rm "$(PATH_LNK)/$(FILE_EXE)"
	@rm -rf "$(PATH_INS)"

#-------------------------------------------------------------------
# Auto
#-------------------------------------------------------------------

$(PATH_EXE): $(OBJECTS)	$(SOURCES)
	$(LD) $(LFLAGS) -o $@ $(OBJECTS) $(LIBS) 

$(PATH_OBJ)/%.o: 
	$(CC) $(CFLAGS) $(subst $(PATH_OBJ),$(PATH_SRC), $(subst .o,.cpp, $@)) -o $@
	- $(CC) $(SFLAGS) $(subst $(PATH_OBJ),$(PATH_SRC), $(subst .o,.cpp, $@)) -o $(subst .o,.asm, $@)


	

