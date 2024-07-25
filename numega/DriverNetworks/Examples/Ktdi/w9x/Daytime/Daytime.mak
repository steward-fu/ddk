# DAYTIME.mak - makefile for VxD DAYTIME

DEVICENAME = DAYTIME
DYNAMIC = 1
NDIS = 1
FRAMEWORK = CPP

! if $(DEBUG) != 0
TDILIBRARY = $(DRIVERNETWORKS)\lib\i386\checked\tdiw9vxd.lib
OBJPATH = Debug
! else
TDILIBRARY = $(DRIVERNETWORKS)\lib\i386\free\tdiw9vxd.lib
OBJPATH = Release
! endif

OBJECTS = $(OBJPATH)\daytime.OBJ   $(TDILIBRARY)
INCVXD2 = $(DRIVERNETWORKS)\include\tdiclient;$(DRIVERNETWORKS)\include;$(TARGET_INC_PATH);$(DDK_INC_PATH);$(BASEDIR)\src\network\inc;$(BASEDIR)\src\network\wshsmple

# NOTE: defines for undefines used in tdi 
XFLAGS = -DTDI_WINDOWS_95 -Di386=1 

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

$(OBJPATH)\daytime.OBJ:	daytime.cpp daytime.h DAYTIME.mak