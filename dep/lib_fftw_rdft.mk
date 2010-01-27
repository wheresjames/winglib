
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := fftw_rdft
PRJ_TYPE := lib
PRJ_INCS := fftw/kernel fftw/dft fftw/rdft fftw/rdft/scalar fftw/reodft
PRJ_LIBS := 

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := winglib/dep/etc/fftw/inc/windows $(PRJ_INCS)
else
	PRJ_INCS := winglib/dep/etc/fftw/inc/posix $(PRJ_INCS)
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------

export LOC_TAG := rdft
LOC_CXX_rdft := c
LOC_SRC_rdft := $(CFG_LIBROOT)/fftw/rdft
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := rdft_scalar
LOC_CXX_rdft_scalar := c
LOC_SRC_rdft_scalar := $(CFG_LIBROOT)/fftw/rdft/scalar
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := rdft_scalar_r2r
LOC_CXX_rdft_scalar_r2r := c
LOC_SRC_rdft_scalar_r2r := $(CFG_LIBROOT)/fftw/rdft/scalar/r2r
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := rdft_scalar_r2cb
LOC_CXX_rdft_scalar_r2cb := c
LOC_SRC_rdft_scalar_r2cb := $(CFG_LIBROOT)/fftw/rdft/scalar/r2cb
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := rdft_scalar_r2cf
LOC_CXX_rdft_scalar_r2cf := c
LOC_SRC_rdft_scalar_r2cf := $(CFG_LIBROOT)/fftw/rdft/scalar/r2cf
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := reodft
LOC_CXX_reodft := c
LOC_SRC_reodft := $(CFG_LIBROOT)/fftw/reodft
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


