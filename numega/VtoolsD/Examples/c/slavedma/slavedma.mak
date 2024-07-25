# SLAVEDMA.mak - makefile for VxD SLAVEDMA

DEVICENAME = SLAVEDMA
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = slavedma.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

slavedma.OBJ:	slavedma.c slavedma.h