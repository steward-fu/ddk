# Make file for Win32/VxD interface APPLICATION
# Copyright (c) 1997 Compuware Corporation

# Assumes path and environment variables for compiler

!include $(VTOOLSD)\include\user.mak

w32ifapp.exe:	w32ifapp.c
	$(C32) w32ifapp.c