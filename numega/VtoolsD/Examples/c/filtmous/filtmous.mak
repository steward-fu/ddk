# FILTMOUS.mak - makefile for VxD FILTMOUS

DEVICENAME = FILTMOUS
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = filtmous.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

filtmous.OBJ:	filtmous.c filtmous.h