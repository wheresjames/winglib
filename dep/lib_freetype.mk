
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := freetype
PRJ_TYPE := lib
PRJ_INCS := freetype/include
PRJ_LIBS := 
PRJ_DEFS := FT2_BUILD_LIBRARY NO_ERRNO_H
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(3D),)
UNSUPPORTED := Set make option 3D=1 to build
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := autofit
LOC_CXX_autofit := c
LOC_BLD_autofit := cpp
LOC_SRC_autofit := $(CFG_LIBROOT)/freetype/src/autofit
LOC_LST_autofit := autofit
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := base
LOC_CXX_base := c
LOC_BLD_base := cpp
LOC_SRC_base := $(CFG_LIBROOT)/freetype/src/base
LOC_LST_base := ftbase ftinit ftsystem ftdebug
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bdf
LOC_CXX_bdf := c
LOC_BLD_bdf := cpp
LOC_SRC_bdf := $(CFG_LIBROOT)/freetype/src/bdf
LOC_LST_bdf := bdf
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := cache
LOC_CXX_cache := c
LOC_BLD_cache := cpp
LOC_SRC_cache := $(CFG_LIBROOT)/freetype/src/cache
LOC_LST_cache := ftcache
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := cff
LOC_CXX_cff := c
LOC_BLD_cff := cpp
LOC_SRC_cff := $(CFG_LIBROOT)/freetype/src/cff
LOC_LST_cff := cff
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := cid
LOC_CXX_cid := c
LOC_BLD_cid := cpp
LOC_SRC_cid := $(CFG_LIBROOT)/freetype/src/cid
LOC_LST_cid := type1cid
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gxvalid
LOC_CXX_gxvalid := c
LOC_BLD_gxvalid := cpp
LOC_SRC_gxvalid := $(CFG_LIBROOT)/freetype/src/gxvalid
LOC_LST_gxvalid := gxvalid
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := gzip
LOC_CXX_gzip := c
LOC_BLD_gzip := cpp
LOC_SRC_gzip := $(CFG_LIBROOT)/freetype/src/gzip
LOC_LST_gzip := ftgzip
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := lzw
LOC_CXX_lzw := c
LOC_BLD_lzw := cpp
LOC_SRC_lzw := $(CFG_LIBROOT)/freetype/src/lzw
LOC_LST_lzw := ftlzw
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := otvalid
LOC_CXX_otvalid := c
LOC_BLD_otvalid := cpp
LOC_SRC_otvalid := $(CFG_LIBROOT)/freetype/src/otvalid
LOC_LST_otvalid := otvalid
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := pcf
LOC_CXX_pcf := c
LOC_BLD_pcf := cpp
LOC_SRC_pcf := $(CFG_LIBROOT)/freetype/src/pcf
LOC_LST_pcf := pcf
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := pfr
LOC_CXX_pfr := c
LOC_BLD_pfr := cpp
LOC_SRC_pfr := $(CFG_LIBROOT)/freetype/src/pfr
LOC_LST_pfr := pfr
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := psaux
LOC_CXX_psaux := c
LOC_BLD_psaux := cpp
LOC_SRC_psaux := $(CFG_LIBROOT)/freetype/src/psaux
LOC_LST_psaux := psaux
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := pshinter
LOC_CXX_pshinter := c
LOC_BLD_pshinter := cpp
LOC_SRC_pshinter := $(CFG_LIBROOT)/freetype/src/pshinter
LOC_LST_pshinter := pshinter
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := psnames
LOC_CXX_psnames := c
LOC_BLD_psnames := cpp
LOC_SRC_psnames := $(CFG_LIBROOT)/freetype/src/psnames
LOC_LST_psnames := psnames
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := raster
LOC_CXX_raster := c
LOC_BLD_raster := cpp
LOC_SRC_raster := $(CFG_LIBROOT)/freetype/src/raster
LOC_LST_raster := raster
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := sfnt
LOC_CXX_sfnt := c
LOC_BLD_sfnt := cpp
LOC_SRC_sfnt := $(CFG_LIBROOT)/freetype/src/sfnt
LOC_LST_sfnt := sfnt
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := smooth
LOC_CXX_smooth := c
LOC_BLD_smooth := cpp
LOC_SRC_smooth := $(CFG_LIBROOT)/freetype/src/smooth
LOC_LST_smooth := smooth
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := truetype
LOC_CXX_truetype := c
LOC_BLD_truetype := cpp
LOC_SRC_truetype := $(CFG_LIBROOT)/freetype/src/truetype
LOC_LST_truetype := truetype
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := type1
LOC_CXX_type1 := c
LOC_BLD_type1 := cpp
LOC_SRC_type1 := $(CFG_LIBROOT)/freetype/src/type1
LOC_LST_type1 := type1
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := type42
LOC_CXX_type42 := c
LOC_BLD_type42 := cpp
LOC_SRC_type42 := $(CFG_LIBROOT)/freetype/src/type42
LOC_LST_type42 := type42
include $(PRJ_LIBROOT)/build.mk

ifeq ($(PLATFORM),windows)
	export LOC_TAG := winfonts
	LOC_CXX_winfonts := c
	LOC_BLD_winfonts := cpp
	LOC_SRC_winfonts := $(CFG_LIBROOT)/freetype/src/winfonts
	include $(PRJ_LIBROOT)/build.mk
endif

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

