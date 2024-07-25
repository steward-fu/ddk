# HOOKVIRQ.MAK - makefile for VxD HOOKVIRQ

DEVICENAME = HOOKVIRQ
FRAMEWORK = C
DEBUG = 1
OBJECTS = HOOKVIRQ.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

HOOKVIRQ.OBJ:	HOOKVIRQ.C HOOKVIRQ.H