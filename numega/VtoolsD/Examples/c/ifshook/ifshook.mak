# IFSHOOK.mak - makefile for VxD IFSHOOK

DEVICENAME = IFSHOOK
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = IFSHOOK.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

IFSHOOK.OBJ:	IFSHOOK.c IFSHOOK.h