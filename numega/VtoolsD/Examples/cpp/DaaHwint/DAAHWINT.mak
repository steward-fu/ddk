# DAAHWINT.mak - makefile for VxD DAAHWINT

DEVICENAME = DAAHWINT
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = \
	daahwint.OBJ \
	realclk.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

daahwint.OBJ:	daahwint.cpp daahwint.h
realclk.OBJ:	realclk.cpp realclk.h