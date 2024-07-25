# CALLERP.MAK - makefile for VxD CALLERP
# Copyright (c) 1994 Compuware Corporation

DEVICENAME = CALLERP
FRAMEWORK = CPP
DYNAMIC = 1
DEBUG = 1
OBJECTS = CALLERP.OBJ CALLWRPP.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

# tasm32 is required to build this example if you
# are using the Borland compiler. See definition of
# VxDCall, VxDJmp macros in vmm.h
XFLAGS=$(XFLAGS) -DTASM_AVAILABLE

CALLERP.OBJ:	CALLERP.CPP CALLERP.H
CALLWRPP.OBJ:	CALLWRPP.CPP CALLEEP.H
