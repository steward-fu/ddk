# HWINT.MAK - makefile for VxD HWINT - Microsoft Compiler

DEVICENAME = HWINT
FRAMEWORK = CPP
OBJECTS = BELLEVT.OBJ HWINT.OBJ REALCLK.OBJ INIT.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

HWINT.OBJ:	HWINT.CPP HWINT.H
REALCLK.OBJ:	REALCLK.CPP REALCLK.H
BELLEVT.OBJ:	BELLEVT.CPP BELLEVT.H
INIT.OBJ:	INIT.CPP