# HWINT.MAK - makefile for VxD HWINT

DEVICENAME = HWINT
FRAMEWORK = C
DEBUG = 1
OBJECTS = HWINT.OBJ CMOSIO.OBJ
DYNAMIC = 1

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

HWINT.OBJ:	HWINT.C HWINT.H
