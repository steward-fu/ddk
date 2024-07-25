# NDISMAC.MAK - makefile for VxD NDISMAC
# Copyright (c) 1995-1998 Compuware Corporation

DEVICENAME = NDISMAC
DYNAMIC = 1
NDIS=1
FRAMEWORK = C
DEBUG = 1
OBJECTS = NDISMAC.OBJ MACFUNCS.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

NDISMAC.OBJ:	NDISMAC.C NDISMAC.H
