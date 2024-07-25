# SCHOOL.mak - makefile for VxD SCHOOL

DEVICENAME = SCHOOL
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = school.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

school.OBJ:	school.c school.h