# VSDXMPL.MAK - makefile for VxD VSDXMPL

DEVICENAME = VSDXMPL
DYNAMIC = 1
NDIS=0
FRAMEWORK = C
DEBUG = 1
OBJECTS = VSDXMPL.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

VSDXMPL.OBJ:	VSDXMPL.C VSDXMPL.H
