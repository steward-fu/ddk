# COMMCLNT.mak - makefile for VxD COMMCLNT

DEVICENAME = COMMCLNT
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = commclnt.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

commclnt.OBJ:	commclnt.c commclnt.h