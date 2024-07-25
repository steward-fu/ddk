# Copyright (c) 1994 Compuware Corporation
# Makefile for XHOTKEY example

DEVICENAME = HOTKEY
FRAMEWORK = CPP
OBJECTS = hotkey.obj
DEBUG=1
DYNAMIC=1

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

hotkey.obj:	hotkey.cpp

