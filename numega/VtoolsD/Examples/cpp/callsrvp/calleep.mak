# CALLEEP.MAK - makefile for VxD CALLEEP
# Copyright (c) 1994 Compuware Corporation

DEVICENAME = CALLEEP
FRAMEWORK = CPP
DYNAMIC = 1
DEBUG = 1
OBJECTS = CALLEEP.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

CALLEEP.OBJ:	CALLEEP.CPP CALLEEP.H
