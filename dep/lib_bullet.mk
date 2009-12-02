
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := bullet
PRJ_TYPE := lib
PRJ_INCS := bullet/src
PRJ_LIBS := 

PRJ_LIBROOT := ..
PRJ_OBJROOT := _0_dep

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

#----------------------------
# -- BulletCollision
#----------------------------

export LOC_TAG := bc_bc
LOC_INC_bc_bc := $(CFG_LIBROOT)/bullet/src/BulletCollision/BroadphaseCollision
LOC_SRC_bc_bc := $(CFG_LIBROOT)/bullet/src/BulletCollision/BroadphaseCollision
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bc_cd
LOC_INC_bc_cd := $(CFG_LIBROOT)/bullet/src/BulletCollision/CollisionDispatch
LOC_SRC_bc_cd := $(CFG_LIBROOT)/bullet/src/BulletCollision/CollisionDispatch
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bc_cs
LOC_INC_bc_cs := $(CFG_LIBROOT)/bullet/src/BulletCollision/CollisionShapes
LOC_SRC_bc_cs := $(CFG_LIBROOT)/bullet/src/BulletCollision/CollisionShapes
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bc_gi
LOC_INC_bc_gi := $(CFG_LIBROOT)/bullet/src/BulletCollision/Gimpact
LOC_SRC_bc_gi := $(CFG_LIBROOT)/bullet/src/BulletCollision/Gimpact
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bc_np
LOC_INC_bc_np := $(CFG_LIBROOT)/bullet/src/BulletCollision/NarrowPhaseCollision
LOC_SRC_bc_np := $(CFG_LIBROOT)/bullet/src/BulletCollision/NarrowPhaseCollision
include $(PRJ_LIBROOT)/build.mk

#----------------------------
# -- BulletDynamics
#----------------------------

export LOC_TAG := bd_ch
LOC_INC_bd_ch := $(CFG_LIBROOT)/bullet/src/BulletDynamics/Character
LOC_SRC_bd_ch := $(CFG_LIBROOT)/bullet/src/BulletDynamics/Character
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bd_cs
LOC_INC_bd_cs := $(CFG_LIBROOT)/bullet/src/BulletDynamics/ConstraintSolver
LOC_SRC_bd_cs := $(CFG_LIBROOT)/bullet/src/BulletDynamics/ConstraintSolver
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bd_dy
LOC_INC_bd_dy := $(CFG_LIBROOT)/bullet/src/BulletDynamics/Dynamics
LOC_SRC_bd_dy := $(CFG_LIBROOT)/bullet/src/BulletDynamics/Dynamics
include $(PRJ_LIBROOT)/build.mk

export LOC_TAG := bd_vh
LOC_INC_bd_vh := $(CFG_LIBROOT)/bullet/src/BulletDynamics/Vehicle
LOC_SRC_bd_vh := $(CFG_LIBROOT)/bullet/src/BulletDynamics/Vehicle
include $(PRJ_LIBROOT)/build.mk

#----------------------------
# -- BulletSoftBody
#----------------------------

export LOC_TAG := sb
LOC_INC_sb := $(CFG_LIBROOT)/bullet/src/BulletSoftBody
LOC_SRC_sb := $(CFG_LIBROOT)/bullet/src/BulletSoftBody
include $(PRJ_LIBROOT)/build.mk

#----------------------------
# -- LinearMath
#----------------------------

export LOC_TAG := lm
LOC_INC_lm := $(CFG_LIBROOT)/bullet/src/LinearMath
LOC_SRC_lm := $(CFG_LIBROOT)/bullet/src/LinearMath
include $(PRJ_LIBROOT)/build.mk

#-------------------------------------------------------------------
# Execute the build
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/go.mk

endif

