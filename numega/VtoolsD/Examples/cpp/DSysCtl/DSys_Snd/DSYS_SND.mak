# DSYS_SND.mak - makefile for VxD DSYS_SND

DEVICENAME = DSYS_SND
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = dsys_snd.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

dsys_snd.OBJ:	dsys_snd.cpp dsys_snd.h