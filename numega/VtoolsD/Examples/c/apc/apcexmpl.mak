# APCEXMPL.MAK - makefile for VxD APCEXMPL
# Copyright (c) 1995 Compuware Corporation

DEVICENAME = APCEXMPL
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = APCEXMPL.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

apcexmpl.obj:	apcexmpl.c apcexmpl.h

