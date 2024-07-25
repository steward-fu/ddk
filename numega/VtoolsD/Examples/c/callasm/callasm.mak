# CALLASM.mak - makefile for VxD CALLASM

DEVICENAME = CALLASM
FRAMEWORK = C
DEBUG = 1
OBJECTS = CALLASM.OBJ FUNCS.OBJ
DYNAMIC = 1

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

CALLASM.OBJ:	CALLASM.c CALLASM.h
FUNCS.OBJ:	FUNCS.asm
