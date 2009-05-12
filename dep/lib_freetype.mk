
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := freetype
PRJ_TYPE := lib
PRJ_INCS := freetype/include
PRJ_LIBS := 
PRJ_DEFS := FT2_BUILD_LIBRARY
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := autofit
LOC_CXX_autofit := c
LOC_BLD_autofit := cpp
LOC_SRC_autofit := $(CFG_LIBROOT)/freetype/src/autofit
LOC_EXC_autofit := aflatin2
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := base
LOC_CXX_base := c
LOC_BLD_base := cpp
LOC_SRC_base := $(CFG_LIBROOT)/freetype/src/base
ifneq ($OS,mac)
	LOC_EXC_base := $(LOC_EXC_base) ftmac
endif
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bdf
LOC_CXX_bdf := c
LOC_BLD_bdf := cpp
LOC_SRC_bdf := $(CFG_LIBROOT)/freetype/src/bdf
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := cache
LOC_CXX_cache := c
LOC_BLD_cache := cpp
LOC_SRC_cache := $(CFG_LIBROOT)/freetype/src/cache
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := cff
LOC_CXX_cff := c
LOC_BLD_cff := cpp
LOC_SRC_cff := $(CFG_LIBROOT)/freetype/src/cff
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := cid
LOC_CXX_cid := c
LOC_BLD_cid := cpp
LOC_SRC_cid := $(CFG_LIBROOT)/freetype/src/cid
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gxvalid
LOC_CXX_gxvalid := c
LOC_BLD_gxvalid := cpp
LOC_SRC_gxvalid := $(CFG_LIBROOT)/freetype/src/gxvalid
LOC_EXC_gxvalid := gxvfgen
include $(PRJ_LIBROOT)/build.mk

#export LOC_TAG := gzip
#LOC_CXX_gzip := c
#LOC_BLD_gzip := cpp
#LOC_SRC_gzip := $(CFG_LIBROOT)/freetype/src/gzip
#LOC_EXC_gzip := 
#include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := lzw
LOC_CXX_lzw := c
LOC_BLD_lzw := cpp
LOC_SRC_lzw := $(CFG_LIBROOT)/freetype/src/lzw
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := otvalid
LOC_CXX_otvalid := c
LOC_BLD_otvalid := cpp
LOC_SRC_otvalid := $(CFG_LIBROOT)/freetype/src/otvalid
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := pcf
LOC_CXX_pcf := c
LOC_BLD_pcf := cpp
LOC_SRC_pcf := $(CFG_LIBROOT)/freetype/src/pcf
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := pfr
LOC_CXX_pfr := c
LOC_BLD_pfr := cpp
LOC_SRC_pfr := $(CFG_LIBROOT)/freetype/src/pfr
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := psaux
LOC_CXX_psaux := c
LOC_BLD_psaux := cpp
LOC_SRC_psaux := $(CFG_LIBROOT)/freetype/src/psaux
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := pshinter
LOC_CXX_pshinter := c
LOC_BLD_pshinter := cpp
LOC_SRC_pshinter := $(CFG_LIBROOT)/freetype/src/pshinter
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := psnames
LOC_CXX_psnames := c
LOC_BLD_psnames := cpp
LOC_SRC_psnames := $(CFG_LIBROOT)/freetype/src/psnames
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := raster
LOC_CXX_raster := c
LOC_BLD_raster := cpp
LOC_SRC_raster := $(CFG_LIBROOT)/freetype/src/raster
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := sfnt
LOC_CXX_sfnt := c
LOC_BLD_sfnt := cpp
LOC_SRC_sfnt := $(CFG_LIBROOT)/freetype/src/sfnt
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := smooth
LOC_CXX_smooth := c
LOC_BLD_smooth := cpp
LOC_SRC_smooth := $(CFG_LIBROOT)/freetype/src/smooth
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := truetype
LOC_CXX_truetype := c
LOC_BLD_truetype := cpp
LOC_SRC_truetype := $(CFG_LIBROOT)/freetype/src/truetype
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

