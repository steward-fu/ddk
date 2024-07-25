# TCPECHO.mak - makefile for VxD TCPECHO

DEVICENAME = TCPECHO
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

OBJECTS = $(OBJPATH)\tcpecho.OBJ  $(TDILIBRARY)
INCVXD2 = $(DRIVERNETWORKS)\include\tdiclient;$(DRIVERNETWORKS)\include;$(TARGET_INC_PATH);$(DDK_INC_PATH);$(BASEDIR)\src\network\inc;$(BASEDIR)\src\network\wshsmple

# NOTE: defines for undefines used in tdi 
XFLAGS = -DTDI_WINDOWS_95 -Di386=1

# XLFLAGS = /MAP

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

$(OBJPATH)\tcpecho.OBJ:	tcpecho.cpp tcpecho.h tcpecho.mak

