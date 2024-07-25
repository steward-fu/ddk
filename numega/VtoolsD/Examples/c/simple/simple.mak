# SIMPLE.MAK - makefile for VxD SIMPLE

DEVICENAME	=	SIMPLE
FRAMEWORK	=	C
DEBUG		=	1
DYNAMIC		=	1
TARGET		=	WIN95

OBJECTS = SIMPLE.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

simple.obj:	simple.c simple.h

