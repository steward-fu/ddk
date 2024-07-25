# APPYTIME.MAK - makefile for VxD APPYTIME
# Copyright (c) 1994, Compuware Corporation

DEVICENAME = APPYTIME
FRAMEWORK = C
DYNAMIC = 1
DEBUG = 1
OBJECTS = APPYTIME.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

APPYTIME.OBJ:	APPYTIME.C APPYTIME.H
