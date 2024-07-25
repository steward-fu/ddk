# SERIAL.MAK - makefile for VxD SERIAL
# Copyright (c) 1994, Compuware Corporation

DEVICENAME = SERIAL
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
#DEBUG = 0
OBJECTS = SERLDEV.OBJ SERLPORT.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

