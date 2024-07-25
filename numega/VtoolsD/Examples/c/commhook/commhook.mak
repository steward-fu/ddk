# COMMHOOK.mak - makefile for VxD COMMHOOK

DEVICENAME = COMMHOOK
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = commhook.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

commhook.OBJ:	commhook.c commhook.h