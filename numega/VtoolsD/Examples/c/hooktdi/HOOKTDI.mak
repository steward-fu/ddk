# HOOKTDI.mak - makefile for VxD HOOKTDI

DEVICENAME = HOOKTDI
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = hooktdi.OBJ
XFLAGS = -I. -Di386=1 -Duint=UINT -Dushort=USHORT -Dulong=ULONG -Duchar=UCHAR

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

hooktdi.OBJ:	hooktdi.c hooktdi.h