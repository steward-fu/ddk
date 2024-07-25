# TSTNTKRN.mak - makefile for VxD TSTNTKRN

DEVICENAME = TSTNTKRN
DYNAMIC = 1
FRAMEWORK = CPP
DEBUG = 1
OBJECTS = tstntkrn.OBJ
TARGET = WIN98

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

tstntkrn.OBJ:	tstntkrn.cpp tstntkrn.h