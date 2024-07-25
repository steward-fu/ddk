# OVRLAPIO.MAK - makefile for VxD OVRLAPIO
# Copyright (c) 1996, Compuware Corporation

DEVICENAME = OVRLAPIO
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = ovrlapio.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

ovrlapio.OBJ:	ovrlapio.cpp ovrlapio.h