# POSTMSG.MAK - makefile for VxD POSTMSG

DEVICENAME = POSTMSG
FRAMEWORK = C
DEBUG = 1
OBJECTS = POSTMSG.OBJ CMOSIO.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

POSTMSG.OBJ:	POSTMSG.C POSTMSG.H
