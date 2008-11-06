
# Cross compiler config

OS		 := linux
#OS		 := win32

PROC	 := i386
#PROC	 := arm

TOOLS	 := local
#TOOLS	 := debian
#TOOLS	 := android
#TOOLS	 := snapgear

TOOLROOT := $(OFFSET_ROOT)/../..
LIBROOT  := $(OFFSET_ROOT)/..

ifdef DBG
CEXTRA	 := -g -DDEBUG -D_DEBUG
LEXTRA	 := -g -rdynamic
DPOSTFIX := _d
else
CEXTRA	 := -s
LEXTRA	 := -s
endif

ifdef UNICODE
CEXTRA := $(CEXTRA) -DUNICODE -D_UNICODE
endif

ONESPACE := $(nullstring) # end of the line

ifeq ($(OS),win32)

	PLATFORM := windows

	# Tools
	PP := cl /nologo /DWIN32 /wd4996
	LD := link /NOLOGO
	CC := cl /nologo /DWIN32 /wd4996
	AR := lib /nologo
	DP := cl /nologo
	MD := md
	RM := rmdir /s /q

	CC_OUT := /Fo
	LD_OUT := /OUT:
	AR_OUT := /OUT:

	CFLAGS := /EHsc /c

	OBJ_EXT := obj
	CUR_ROOT := $(shell cd)

else

	PLATFORM := posix

	# Arm compiler
	ifeq ($(PROC),arm)

		ifeq ($(TOOLS),snapgear)
		
			# Snapgear
			TOOLPREFIX := $(TOOLROOT)/tools/snapgear/usr/local/bin/arm-linux-

			LFLAGS := $(LEXTRA) -static -lrt -pthread
			CFLAGS := -c -MMD -DOEX_ARM -DOEX_LOWRAM -DOEX_NOSTRUCTINIT -DOEX_PACKBROKEN -DOEX_NOUUID -DOEX_NOSHM $(CEXTRA)
			SFLAGS := $(CFLAGS) -S -MMD
			AFLAGS := cq
			
		else
		
			# Google Android
			TOOLS := android
			TOOLPREFIX := $(TOOLROOT)/tools/android/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-

			LFLAGS := $(LEXTRA) -static -lrt -pthread 
			CFLAGS := -c -MMD -DOEX_ARM -DOEX_LOWRAM -DOEX_NOSHM -DOEX_NOUUID $(CEXTRA)
			SFLAGS := $(CFLAGS) -S -MMD
			AFLAGS := cq
			
		endif

	else

		# Local platform
		TOOLPREFIX := 

		LFLAGS := $(LEXTRA) -lrt -pthread -luuid
		CFLAGS := -c -MMD $(CEXTRA) -DOEX_VFL1
		SFLAGS := $(CFLAGS) -S -MMD
		AFLAGS := cq
		
	endif

	# Tools
	PP := $(TOOLPREFIX)g++
	LD := $(TOOLPREFIX)g++
	CC := $(TOOLPREFIX)gcc
	AR := $(TOOLPREFIX)ar
	DP := $(TOOLPREFIX)makedepend
	MD := mkdir -p
	RM := rm -rf
	
	CC_OUT := -o$(ONE_SPACE)

	OBJ_EXT := o
	CUR_ROOT := $(shell pwd)
	
endif

BUILD_TYPE := $(PLATFORM)-$(OS)-$(PROC)-$(TOOLS)

ifdef UNICODE
BUILD_TYPE := $(BUILD_TYPE)-unicode
endif

ifdef DBG
BUILD_TYPE := $(BUILD_TYPE)-debug
endif

BINROOT  := $(LIBROOT)/bin/$(BUILD_TYPE)


