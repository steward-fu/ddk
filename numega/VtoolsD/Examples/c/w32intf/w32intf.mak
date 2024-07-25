# w32intf.mak - makefile for VxD W32INTF
# Copyright (c) 1995 Compuware Corporation

DEVICENAME = W32INTF
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = W32INTF.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

w32intf.obj:	w32intf.c w32intf.h

