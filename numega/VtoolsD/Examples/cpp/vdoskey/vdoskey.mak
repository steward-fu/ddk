# Copyright (c) 1994 Compuware Corporation
# Makefile for VDOSKEY example

DEVICENAME = VDOSKEY
FRAMEWORK = CPP
OBJECTS = dkkey.obj dkbuff.obj dkhist.obj dkcmdlin.obj dkcmdq.obj vdoskey.obj
DEBUG = 1

#DYNAMIC = 1

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

dkkey.obj:	dkkey.cpp	
dkbuff.obj:	dkbuff.cpp	
dkhist.obj:	dkhist.cpp	
dkcmdlin.obj:	dkcmdlin.cpp	
dkcmdq.obj:	dkcmdq.cpp	
vdoskey.obj:	vdoskey.cpp
