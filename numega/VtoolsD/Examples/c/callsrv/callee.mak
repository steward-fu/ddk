# CALLEE.mak - makefile for VxD CALLEE
# Copyright (c) 1994 Compuware Corporation

DEVICENAME = CALLEE
FRAMEWORK = C
DEBUG = 1
DYNAMIC = 1
OBJECTS = CALLEE.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

CALLEE.OBJ:	CALLEE.c CALLEE.h
