
.PHONY : default_target
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := axtest_stub

PRJ_LIBROOT := ../../../../deftbuild/v1
PRJ_SPAWNROOT := ../../../deftbuild/v1

#-------------------------------------------------------------------
# Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

ifeq ($(CFG_MSCAB),)
UNSUPPORTED := cabarc is not installed
include $(PRJ_LIBROOT)/unsupported.mk
else

ifneq ($(PLATFORM),windows)
UNSUPPORTED := PLATFORM=$(PLATFORM) not supported
include $(PRJ_LIBROOT)/unsupported.mk
else

ifneq ($(DBG)$(NOINSTALL),)
UNSUPPORTED := Not making cab file for debug version
include $(PRJ_LIBROOT)/unsupported.mk
else

#-------------------------------------------------------------------
# Build
#-------------------------------------------------------------------

MK_INF_ROOT := $(CFG_OUTROOT)/$(PRJ_NAME)

all :
	- $(CFG_MD) $(MK_INF_ROOT)
	- $(CFG_DEL) "$(MK_INF_ROOT)/*"
	$(CFG_CPY) * "$(MK_INF_ROOT)"
	$(CFG_SAR) "s/@LIBROOT/$(subst /,\/,$(PRJ_SPAWNROOT))/g" "$(MK_INF_ROOT)/Makefile.mk"
	$(MAKE) -C $(MK_INF_ROOT) -f Makefile.mk

endif
endif
endif
