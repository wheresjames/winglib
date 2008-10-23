
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

CDEBUG	 := -DDEBUG -D_DEBUG -g
LDEBUG	 := -g

ifeq ($(OS),win32)

	PLATFORM := windows

else

	PLATFORM := posix

	LIBS := -L$(LIBROOT)/lib/oexlib/bin/$(TARGET)/ -loexlib_d

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

			LFLAGS := -lrt -static $(LDEBUG)
			CFLAGS := -c -DOEX_ARM -DOEX_LOWRAM -DOEX_SNAPGEAR -DOEX_VFL1 $(CDEBUG)
			SFLAGS := $(CFLAGS) -S -MMD
			AFLAGS := cq
			
		else

			# Google Android
			CC := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-g++
			LD := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-g++
			AR := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-ar
			DP := $(CODEROOT)/../CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-makedepend

			LFLAGS := -lrt -static $(LDEBUG)
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

		LFLAGS := -lrt -luuid $(LDEBUG)
		CFLAGS := -c $(CDEBUG)
		SFLAGS := $(CFLAGS) -S -MMD
		AFLAGS := cq
		
	endif	

endif

# Flags
#LFLAGS := -g -static -libgcc -Wl,-static -la
#CFLAGS := -g -c
#SFLAGS := $(CFLAGS) -S -MMD
#AFLAGS := rcs
#AFLAGS := cq

#-------------------------------------------------------------------
# Vars
#-------------------------------------------------------------------
NAME_PRJ := oexlib

FILE_EXE := lib$(NAME_PRJ)_d.a
PATH_SRC := ../../src
PATH_BIN := ../../bin
PATH_OUT := $(PATH_BIN)/$(PLATFORM)-$(OS)-$(PROC)-$(TOOLS)
PATH_OBJ := $(PATH_OUT)/obj
PATH_EXE := $(PATH_OUT)/$(FILE_EXE)
PATH_INS := /usr/share/$(NAME_PRJ)
PATH_LNK := /usr/bin

#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------
SOURCES :=	$(PATH_SRC)/alloc.cpp 						\
			$(PATH_SRC)/asso_list.cpp					\
			$(PATH_SRC)/circ_buf.cpp					\
			$(PATH_SRC)/data_packet.cpp					\
			$(PATH_SRC)/fifo_sync.cpp					\
			$(PATH_SRC)/file.cpp						\
			$(PATH_SRC)/find_files.cpp					\
			$(PATH_SRC)/image.cpp						\
			$(PATH_SRC)/log.cpp							\
			$(PATH_SRC)/md5.cpp							\
			$(PATH_SRC)/oex.cpp							\
			$(PATH_SRC)/parser.cpp						\
			$(PATH_SRC)/sys_time.cpp
			
OS_SOURCES:= $(PATH_SRC)/os/$(PLATFORM)/base_file.cpp	\
			 $(PATH_SRC)/os/$(PLATFORM)/capture.cpp		\
 			 $(PATH_SRC)/os/$(PLATFORM)/debug.cpp		\
			 $(PATH_SRC)/os/$(PLATFORM)/event.cpp		\
			 $(PATH_SRC)/os/$(PLATFORM)/fmap.cpp		\
			 $(PATH_SRC)/os/$(PLATFORM)/hq_timer.cpp	\
			 $(PATH_SRC)/os/$(PLATFORM)/ip_address.cpp	\
			 $(PATH_SRC)/os/$(PLATFORM)/ip_socket.cpp	\
			 $(PATH_SRC)/os/$(PLATFORM)/mem.cpp			\
			 $(PATH_SRC)/os/$(PLATFORM)/resource.cpp	\
			 $(PATH_SRC)/os/$(PLATFORM)/sys.cpp			\
			 $(PATH_SRC)/os/$(PLATFORM)/thread.cpp		\
			 $(PATH_SRC)/os/$(PLATFORM)/tls.cpp			\
			 $(PATH_SRC)/os/$(PLATFORM)/trace.cpp

OBJECTS := $(subst $(PATH_SRC),$(PATH_OBJ), $(SOURCES:.cpp=.o) )
OS_OBJECTS := $(subst $(PATH_SRC)/os/$(PLATFORM),$(PATH_OBJ)/os, $(OS_SOURCES:.cpp=.o) )

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
	@-mkdir -p $(PATH_OBJ)/os

clean:
	@rm -rf $(PATH_OUT)
	
install:
	@-mkdir -p "$(PATH_INS)"
	cp "$(PATH_EXE)" "$(PATH_INS)/$(FILE_EXE)"
	ln -s "$(PATH_INS)/$(FILE_EXE)" "$(PATH_LNK)/$(FILE_EXE)"

uninstall:	
	@rm "$(PATH_LNK)/$(FILE_EXE)"
	@rm -rf "$(PATH_INS)"

#-------------------------------------------------------------------
# Auto
#-------------------------------------------------------------------

$(PATH_EXE): $(OBJECTS)	$(OS_OBJECTS)
	$(AR) $(AFLAGS) $@ $(OBJECTS) $(OS_OBJECTS)
#	$(LD) $(LFLAGS) -o $@ $(OBJECTS) 

$(PATH_OBJ)/os/%.o:
	$(CC) $(CFLAGS) $(subst $(PATH_OBJ)/os,$(PATH_SRC)/os/$(PLATFORM), $(subst .o,.cpp, $@)) -o $@
	- $(CC) $(SFLAGS) $(subst $(PATH_OBJ)/os,$(PATH_SRC)/os/$(PLATFORM), $(subst .o,.cpp, $@)) -o $(subst .o,.asm, $@)

$(PATH_OBJ)/%.o:
	$(CC) $(CFLAGS) $(subst $(PATH_OBJ),$(PATH_SRC), $(subst .o,.cpp, $@)) -o $@
	- $(CC) $(SFLAGS) $(subst $(PATH_OBJ),$(PATH_SRC), $(subst .o,.cpp, $@)) -o $(subst .o,.asm, $@)
	

