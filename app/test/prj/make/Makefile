
# Use Bourne shell
SHELL := /bin/sh

default_target: all

OS		 := linux
#OS		 := win32

#PROC	 := i386
PROC	 := arm

#TOOLS	 := debian
#TOOLS	 := android
TOOLS	 := snapgear

CODEROOT := ../../../../../..
LIBROOT := ../../../..

CDEBUG	 := -ggdb -DDEBUG -D_DEBUG
LDEBUG	 := -ggdb -rdynamic

ifeq ($(OS),win32)

	PLATFORM := windows

else

	PLATFORM := posix

	#LIBS := 
	LIBS := -L../../../../lib/oexlib/bin/$(PLATFORM)-$(OS)-$(PROC)-$(TOOLS)/ -loexlib_d

	# Flags
	#LFLAGS := -g -lstdc++ -lrt -luuid

	# Arm compiler
	ifeq ($(PROC),arm)

		ifeq ($(TOOLS),snapgear)

			# Snapgear
			CC := $(CODEROOT)/arm/usr/local/bin/arm-linux-g++
			LD := $(CODEROOT)/arm/usr/local/bin/arm-linux-g++
			AR := $(CODEROOT)/arm/usr/local/bin/arm-linux-ar
			DP := $(CODEROOT)/arm/usr/local/bin/arm-linux-makedepend

			LFLAGS := $(LDEBUG) -static -lrt -pthread
			CFLAGS := -c -DOEX_ARM -DOEX_LOWRAM -DOEX_SNAPGEAR -DOEX_VFL1 $(CDEBUG)
			SFLAGS := $(CFLAGS) -S -MMD
			AFLAGS := cq
			
		else

			# Google Android
			CC := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-g++
			LD := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-g++
			AR := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-ar
			DP := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-makedepend

			LFLAGS := $(LDEBUG) -static -lrt -pthread 
			CFLAGS := -c -DOEX_ARM -DOEX_LOWRAM $(CDEBUG)
			SFLAGS := $(CFLAGS) -S -MMD
			AFLAGS := cq
			
		endif

	else

		# Local platform
		CC := g++
		LD := g++
		AR := ar
		DP := makedepend

		LFLAGS := $(LDEBUG) -lrt -pthread -luuid
		CFLAGS := -c $(CDEBUG) -DOEX_VFL1 -DUNICODE -D_UNICODE
		SFLAGS := $(CFLAGS) -S -MMD
		AFLAGS := cq

	endif

endif


#-------------------------------------------------------------------
# Vars
#-------------------------------------------------------------------
NAME_PRJ := test

FILE_EXE := $(NAME_PRJ)
PATH_SRC := ../..
PATH_BIN := ../../bin
PATH_OUT := $(PATH_BIN)/$(PLATFORM)-$(OS)-$(PROC)-$(TOOLS)
PATH_OBJ := $(PATH_OUT)/obj
PATH_EXE := $(PATH_OUT)/$(FILE_EXE)
PATH_INS := /usr/share/$(NAME_PRJ)
PATH_LNK := /usr/bin

#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------
SOURCES :=	$(PATH_SRC)/stdafx.cpp 					\
			$(PATH_SRC)/test.cpp

OBJECTS := $(subst $(PATH_SRC),$(PATH_OBJ), $(SOURCES:.cpp=.o) )

TARGETS := $(PATH_EXE)

#-------------------------------------------------------------------
# Options
#-------------------------------------------------------------------

all: setup $(TARGETS)
	cp $(PATH_EXE) ~/Public

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
#	- $(CC) $(SFLAGS) $(subst $(PATH_OBJ),$(PATH_SRC), $(subst .o,.cpp, $@)) -o $(subst .o,.asm, $@)


	

