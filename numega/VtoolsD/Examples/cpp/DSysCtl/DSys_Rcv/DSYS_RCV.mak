# DSYS_RCV.mak - makefile for VxD DSYS_RCV

DEVICENAME = DSYS_RCV
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = dsys_rcv.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

dsys_rcv.OBJ:	dsys_rcv.cpp dsys_rcv.h