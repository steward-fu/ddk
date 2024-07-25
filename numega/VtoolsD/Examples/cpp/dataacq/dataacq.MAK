# DATAACQ.MAK - makefile for VxD DATAACQ sample
# Copyright (c) 1997 Compuware Corporation

DEVICENAME = DATAACQ
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = dataacq.obj

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

dataacq.OBJ:	dataacq.cpp dataacq.h 
