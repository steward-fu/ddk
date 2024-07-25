# INITFIO.MAK - makefile for VxD INITFIO

DEVICENAME = INITFIO
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = INITFIO.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

INITFIO.OBJ:	INITFIO.CPP INITFIO.H
