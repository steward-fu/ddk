# EXECAPP.MAK - makefile for "execute application" sample
# copyright (c) 1997 Compuware Corporation

DEVICENAME = EXECAPP
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = execapp.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

execapp.OBJ:	execapp.c execapp.h
