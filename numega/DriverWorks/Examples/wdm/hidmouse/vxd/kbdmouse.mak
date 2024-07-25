# KBDMOUSE.mak - makefile for VxD KBDMOUSE

DEVICENAME = KBDMOUSE
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = kbdmouse.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

kbdmouse.OBJ:	kbdmouse.cpp kbdmouse.h