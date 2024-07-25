# Make file for Overlapped I/O application
# Copyright (c) 1997 Compuware Corporation

!include $(VTOOLSD)\include\user.mak

# assumes paths and environment variables for compiler

ovlpapp.exe: ovlpapp.c
	$(C32) ovlpapp.c