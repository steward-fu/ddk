# Copyright (c) 1994-1998 Compuware Corporation
# Makefile for HELLO example

# This is the makefile for simple VxD HELLO.386.

# This is a dynamic VxD
DYNAMIC = 1

# Define the name of the VxD
DEVICENAME = HELLO

# Specify that the VxD uses the C++ framework
FRAMEWORK = CPP

# Specify the list of object modules that comprise the VxD
OBJECTS = hello.obj

# Build for debugging so that the debug message is printed
DEBUG = 1

# Include the necessary files from VToolsD
!include $(VTOOLSD)\include\vtoolsd.mak
!include $(VTOOLSD)\include\vxdtarg.mak

# Define the dependencies for the object modules
hello.obj:	hello.cpp	
