# CALLMINI.mak - makefile for VxD CALLMINI

DEVICENAME = CALLMINI
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = callmini.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

callmini.OBJ:	callmini.c callmini.h