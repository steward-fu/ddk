# TELEPATH.mak - makefile for VxD TELEPATH

DEVICENAME = TELEPATH
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = telepath.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

telepath.OBJ:	telepath.c telepath.h