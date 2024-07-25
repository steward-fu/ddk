# REGFSD.mak - makefile for VxD REGFSD
# Copyright (c) 1996, Compuware Corporation

DEVICETYPE = PORTDRIVER
DEVICENAME = REGFSD
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = regfsd.obj rfsdifs.obj rfsdios.obj rfsdutil.obj rfsddbg.obj

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

