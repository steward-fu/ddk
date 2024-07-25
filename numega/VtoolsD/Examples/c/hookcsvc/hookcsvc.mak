# HOOKCSVC.mak - makefile for VxD HOOKCSVC

DEVICENAME = HOOKCSVC
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = HOOKCSVC.OBJ 

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

HOOKCSVC.OBJ:	HOOKCSVC.c HOOKCSVC.h

