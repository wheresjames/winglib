
PRJ_RELROOT := ../..
include $(PRJ_RELROOT)/buildpath.mk

default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := qt_wetbkit
PRJ_DEPS := qt-win
PRJ_TYPE := exe
PRJ_INCS := winglib/lib/oexlib
PRJ_LIBS := oexlib
PRJ_OSLB := QtWebKit4 QtGui4 QtNetwork4 QtCore4
PRJ_FWRK := Qt

#-------------------------------------------------------------------
# Configure build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# File locations
#-------------------------------------------------------------------
export LOC_TAG := def
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := moc
LOC_BLD_moc := moc
LOC_LST_moc := mainwindow
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

