# PCICFG.mak - makefile for VxD PCICFG

DEVICENAME = PCICFG
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = pcicfg.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

pcicfg.OBJ:	pcicfg.c pcicfg.h