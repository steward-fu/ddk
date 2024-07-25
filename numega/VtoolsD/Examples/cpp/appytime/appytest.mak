# APPYTEST.mak - makefile for VxD APPYTEST
# Copyright (c) 1994, Compuware Corporation

DEVICENAME = APPYTEST
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = APPYTEST.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

APPYTEST.OBJ:	APPYTEST.cpp APPYTEST.h
