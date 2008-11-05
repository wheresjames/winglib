
# Cross compiler config

OS		 := linux
#OS		 := win32

PROC	 := i386
#PROC	 := arm

TOOLS	 := local
#TOOLS	 := debian
#TOOLS	 := android
#TOOLS	 := snapgear

CUR_ROOT := $(shell pwd)
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

ifeq ($(OS),win32)

	PLATFORM := windows

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

endif

# Tools
PP := $(TOOLPREFIX)g++
LD := $(TOOLPREFIX)g++
CC := $(TOOLPREFIX)gcc
AR := $(TOOLPREFIX)ar
DP := $(TOOLPREFIX)makedepend

BUILD_TYPE := $(PLATFORM)-$(OS)-$(PROC)-$(TOOLS)

ifdef UNICODE
BUILD_TYPE := $(BUILD_TYPE)-unicode
endif

ifdef DBG
BUILD_TYPE := $(BUILD_TYPE)-debug
endif

BINROOT  := $(LIBROOT)/bin/$(BUILD_TYPE)



