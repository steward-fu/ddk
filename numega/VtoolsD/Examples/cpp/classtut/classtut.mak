# CLASSTUT.MAK - makefile for VxD classtut

DEVICENAME = CLASSTUT
DEBUG = 1
FRAMEWORK = CPP
OBJECTS = CLASSTUT.OBJ DBWINSTR.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

CLASSTUT.OBJ:	CLASSTUT.CPP CLASSTUT.H
DBWINSTR.OBJ:	DBWINSTR.CPP DBWINSTR.H