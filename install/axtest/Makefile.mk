
.PHONY : default_target
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := axtest
PRJ_DESC := axtest cab

PRJ_LIBROOT := @LIBROOT

#-------------------------------------------------------------------
# Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/config.mk

#-------------------------------------------------------------------
# Build
#-------------------------------------------------------------------

MK_INF_DST := $(PRJ_NAME).inf
MK_CAB := ../$(PRJ_NAME)_$(CFG_FVER)_$(PROC).cab
MK_INF_FILES := $(PRJ_NAME).dll
MK_COPY := $(CFG_OUTROOT)/$(PRJ_NAME).dll

include $(PRJ_LIBROOT)/sign.mk

.PHONY : mk_inf $(MK_CAB)
.PRECIOUS : $(MK_INF_DST)

mk_inf :
	@echo $(foreach f,$(MK_COPY),$(shell $(CFG_CPY) "$(f)" "./"))
	$(CFG_SAR) "s/@NAME/$(PRJ_NAME)/g" "$(MK_INF_DST)"
	$(CFG_SAR) "s/@VER/$(subst .,\.,$(CFG_VER))/g" "$(MK_INF_DST)"
	$(CFG_SAR) "s/@CVER/$(subst .,\$(CFG_COMMA),$(CFG_VER))/g" "$(MK_INF_DST)"

ifneq ($(GO_SIGN_ENABLED),)
$(MK_CAB) : mk_inf
	- $(CFG_DEL) $(subst \,/,$@)
	$(CFG_MSCAB) -s 6144 n $@ $(MK_INF_FILES) $(MK_INF_DST)
	$(call GO_SIGN_EXE,$@,$(PRJ_DESC))
	$(call GO_SIGN_VERIFY,$@)
else
$(MK_CAB) : mk_inf
	- $(CFG_DEL) $(subst \,/,$(MK_CAB))
	$(CFG_MSCAB) n $(MK_CAB) $(MK_INF_FILES) $(MK_INF_DST)
endif

.PHONY : all
all : $(MK_CAB)

