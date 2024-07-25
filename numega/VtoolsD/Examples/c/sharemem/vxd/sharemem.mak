# SHAREMEM.mak - makefile for VxD SHAREMEM

DEVICENAME = SHAREMEM
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = sharemem.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

sharemem.OBJ:	sharemem.c sharemem.h