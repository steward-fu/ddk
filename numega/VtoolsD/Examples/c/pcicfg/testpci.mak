# Make file for PCI test application
# Copyright (c) 1997 Compuware Corporation

# Paths and environment variables for the compiler assumed

!include $(VTOOLSD)\include\user.mak

testpci.exe: testpci.c
	$(C32) testpci.c