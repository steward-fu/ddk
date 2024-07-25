# BEEPER.MAK - makefile for VxD BEEPER
# Copyright (c) 1995 Compuware Corporation

DEVICENAME = BEEPER
FRAMEWORK = CPP
DYNAMIC = 1

DEBUG = 1

OBJECTS = BEEPER.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

BEEPER.OBJ:	BEEPER.CPP BEEPER.H
