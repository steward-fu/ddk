This information in this article applies to 
--------------------------

Microsoft Windows 98 DDK

--------------------------

SUMMARY

In this sample we demonstrate a custom class installer dll.

MORE INFORMATION

All entry points of a class installer are included in the sample.
This was to illustrate the entry points in a kernel level debugger.

BUILDING NETWORK INSTALL DLL

Tools that you need to build CUSTOM.DLL
  Microsoft Visual C 1.52c
  Platform SDK
  Windows 98 DDK

Installation steps
  A sources. and makefile. are supplied for build.exe.
     One must edit the ..\98ddk\inc\master.mk file before attempting
     to build a 16-bit protected mode target.  The C16_ROOT environment
     variable must be set to the fully qualified path of the MSVC
     root directory.

  custom.mak is supplied to build the sample in the MSVC IDE.
     custom.mak assumes that MSVC is installed to d:\msvc

INSTALLATION

The class install section of an Inf file is discussed in the Win98 DDK and
in the Win98 Resource Kit.  One needs to have a device with its own class 
to properly use this sample.  Examples of a class install section of a
dll can be found in the Inf files that ship with Windows 98.  Please look
in the Inf subdirectory of the windows root, %WINDIR%.

TESTING AND DEBUGGING

The installer includes debug statements in every entry point to study the
flow of entry points that are called.  A kernel level debugger is required
because one can not obtain the executable that calls a class installer
in most instances.  Methods of debugging 16-bit dll's are discussed in the
platform SDK.

