# MINIMOUS.mak - makefile for VxD MINIMOUS

DEVICENAME = MINIMOUS
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = MINIMOUS.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

MINIMOUS.OBJ:	MINIMOUS.c MINIMOUS.h