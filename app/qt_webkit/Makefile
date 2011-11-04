
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := qt_webkit
#PRJ_DEPS := qt-win,qt-linux
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib
PRJ_LIBS := oexlib
PRJ_FWRK := Qt

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(OS),windows)
	PRJ_OSLB := QtWebKit4 QtGui4 QtNetwork4 QtCore4
else
	PRJ_OSLB := QtWebKit QtGui QtNetwork QtCore
endif

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := moc
LOC_BLD_moc := moc
LOC_LST_moc := mainwindow network
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

