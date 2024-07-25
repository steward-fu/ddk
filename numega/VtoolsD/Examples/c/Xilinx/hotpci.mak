# HOTPCI.MAK - makefile for VxD HOTPCI
# Copyright (c) 1998 Compuware Corporation
# Copyright (c) 1998 Virtual Computer Corporation
# Copyright (c) 1998 Xilinx

DEVICENAME = HOTPCI
DYNAMIC = 1
FRAMEWORK = C
DEBUG = 1
OBJECTS = HOTPCI.OBJ DEVICE.OBJ

!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak
