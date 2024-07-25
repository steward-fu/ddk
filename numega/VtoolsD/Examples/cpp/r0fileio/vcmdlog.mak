# VCMDLOG.mak - makefile for VxD VCMDLOG

DEVICENAME = VCMDLOG
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = vcmdlog.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

vcmdlog.OBJ:	vcmdlog.cpp vcmdlog.h