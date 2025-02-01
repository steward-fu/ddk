This information in this article applies to 
--------------------------

Microsoft Windows 98 DDK

--------------------------

SUMMARY

In this sample we demonstrate a NetDi installer dll.

MORE INFORMATION

All entry points of a Network installer are included in the sample.
This was to illustrate the entry points in a kernel level debugger.

BUILDING NETWORK INSTALL DLL

Tools that you need to build MYNDI.DLL
  Microsoft Visual C 1.52c
  Platform SDK
  Windows 98 DDK

Installation steps
  A sources. and makefile. are supplied for build.exe.
     One must edit the ..\98ddk\inc\master.mk file before attempting
     to build a 16-bit protected mode target.  The C16_ROOT environment
     variable must be set to the fully qualified path of the MSVC
     root directory.

  MyNdi.mak is supplied to build the sample in the MSVC IDE.
     MyNdi.mak assumes that MSVC is installed to d:\msvc

INSTALLATION

A checked version of MyNdi.dll is all ready included in the packet driver
sample.  VPacket.Inf demonstrates how to install the dll as the device
installer for the sample protocol vxd.

TESTING AND DEBUGGING

The installer includes debug statements in every entry point to study the
flow of entry points that are called.  You can use any 16-bit Windows
debugger you want.  Methods of debugging 16-bit dll's are discussed in the
platform SDK.

