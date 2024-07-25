# CALLER.mak - makefile for VxD CALLER
# Copyright (c) 1994 Compuware Corporation

DEVICENAME = CALLER
FRAMEWORK = C
DYNAMIC = 1
DEBUG = 1
OBJECTS = CALLER.OBJ CALLWRAP.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

# tasm32 is required to build this example if you
# are using the Borland compiler. See definition of
# VxDCall, VxDJmp macros in vmm.h
XFLAGS=$(XFLAGS) -DTASM_AVAILABLE

CALLER.OBJ:	CALLER.C CALLER.H
CALLWRAP.OBJ:	CALLWRAP.C CALLEE.H
