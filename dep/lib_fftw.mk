
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := fftw
PRJ_TYPE := lib
PRJ_INCS := fftw/kernel fftw/dft fftw/dft/scalar
PRJ_LIBS := 

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(PLATFORM),windows)
	PRJ_INCS := winglib/dep/etc/fftw/inc/windows $(PRJ_INCS)
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
LOC_CXX_def := c
LOC_SRC_def := $(CFG_LIBROOT)/fftw/api
LOC_EXC_def := version
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := kernel
LOC_CXX_kernel := c
LOC_SRC_kernel := $(CFG_LIBROOT)/fftw/kernel
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := dft
LOC_CXX_dft := c
LOC_SRC_dft := $(CFG_LIBROOT)/fftw/dft
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := dft_scalar
LOC_CXX_dft_scalar := c
LOC_SRC_dft_scalar := $(CFG_LIBROOT)/fftw/dft/scalar
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := dft_scalar_codlets
LOC_CXX_dft_scalar_codlets := c
LOC_SRC_dft_scalar_codlets := $(CFG_LIBROOT)/fftw/dft/scalar/codelets
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk


