# Make file for dma test application
# Copyright (c) 1997 Compuware Corporation

# Assumes correct paths and environment variables for compiler

!include $(VTOOLSD)\include\user.mak

testdma.exe: testdma.c
	$(C32) testdma.c