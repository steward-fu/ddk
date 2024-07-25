# MAPDEV.mak - makefile for VxD MAPDEV

DEVICENAME = MAPDEV
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = mapdev.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

mapdev.OBJ:	mapdev.c mapdev.h