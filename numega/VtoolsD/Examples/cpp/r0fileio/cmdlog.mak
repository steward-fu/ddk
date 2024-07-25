# Make file for Ring 0 file I/O application
# Copyright (c) 1997 Compuware Corporation

!include $(VTOOLSD)\include\user.mak

# assumes paths and environment variables for compiler

cmdlog.exe: cmdlog.c
	$(C32) cmdlog.c
