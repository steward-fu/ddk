# TRANS31.MAK - makefile for VxD TRANS31

DEVICENAME = TRANS31
DYNAMIC = 1
NDIS=1
FRAMEWORK = C
DEBUG = 1
OBJECTS = TRANS31.OBJ TRANSFCN.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

TRANS31.OBJ:	TRANS31.C TRANS31.H
