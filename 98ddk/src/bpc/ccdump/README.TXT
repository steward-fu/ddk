Application that copies closed captioning (CC) data to a file and 
displays the number of bytes copied to the file 

SUMMARY
=======

This sample application, CCDump.exe, is a tool with which users can 
connect to the vertical blanking interval (VBI) minidriver named 
Closed Caption Decoder and read data asynchronously from the minidriver. 
While a tuned channel receives CC data, users can use CCDump.exe to 
copy that data to a file, either by specifying a file name or by using the default file. 
CCDump.exe then counts and displays the number of bytes copied to the file. As a 
CCDump.exe option, users can specify a communications (COM) port 
from which to retrieve serial data that CCDump.exe copies to the specified file. 
The tool is designed to run with the current version of Broadcast Architecture 
running under the Microsoft(R) Windows(R) 98 operating system.

MORE INFORMATION
================

The following information describes this sample application.

Requirements
-----------------------------
Note: To build this application, you need the following:
      
The Microsoft(R) Visual C++(R) development system, version 5.0. For 
more information on installing and using Visual C++ 5.0, including 
Microsoft Foundation Classes (MFC), see the Visual C++ 5.0 
documentation. 

The Windows 98 Device-Driver Kit (DDK). 

To Build and Run CCDump.exe from the Command Line 
-----------------------------
1. You must install the Visual C++ development system before you 
install the Windows 98 DDK. Otherwise, the Setenv.bat file in the 
Windows 98 DDK will not set up the correct environment when you run 
it. 

The header files that the CCDump project must obtain from the 
Windows 98 DDK are Ccdecode.h, Iks.h, Icodec.h, Ks.h, and Ksmedia.h. 
The libraries that the CCDump project must link to are Icodec.lib 
and Ksuser.lib. 

2. Open a Command Prompt window (for example, in the free build 
environment that the Windows 98 DDK provides); switch to the 
directory on your computer that contains the Sources file, the 
Makefile, and the source files for CCDump.exe; and then type the 
following command:

build

3. To run the built CCDump.exe application, switch to the \Debug 
directory created within the directory containing the source files. 
CCDump.exe is also copied to subdirectory \Lib\I386\Free\ within the base 
directory of the Windows 98 DDK, for example C:\98ddk\Lib\I386\Free.

4. Type 

   CCDump 

5. Unless you want to use the default file that CCDump.exe provides, 
Hubdump.cc, type a file name in the edit box. Click 
"Clear parity bit," "ignore extra nulls," and "Microsoft CC Codec". 

6. Start Microsoft(R) WebTV(TM) for Windows, and tune to a 
channel that receives CC data. 

7. Click Start to start reading and copying CC data.

8. Click Stop to stop reading and copying CC data.

CCDump Files
------------
Hdmp.h and Hdmp.cpp
Main header and source files that define and implement the class for 
the CCDump application. This class creates and initializes a dialog 
box and sets the dialog box to be the application's main window.

Hdmpdlg.h and Hdmpdlg.cpp
Header and source files that define and implement the CHDmpDlg class. 
This class contains member variables that store data entered in the 
dialog box's controls by a user and that store data displayed in the 
dialog box's controls for the user. This class also contains methods 
that initialize the dialog box's controls with the proper values and 
that retrieve and display data.

StdAfx.h and StdAfx.cpp
These files are used to build a precompiled header (PCH) file named 
HDmp.pch and a precompiled types file named StdAfx.obj.

Resource.h
Header file that defines resource identifiers that identify the 
dialog box's controls. 

Hdmp.rc
Resource file that lists and describes all the Windows resources that the CCDump 
application uses, including the icon stored in the \Res subdirectory. 

Res\Hdmp.ico
File containing the icon used as the CCDump application's icon.

Res\Hdmp.rc2
File that contains resources that are not edited by Visual C++. 

Makefile 
Makefile for the project. This makefile provides the Nmake utility with information 
to build CCDump.exe. This information includes, for example, the source 
files to compile, the header files to include, the libraries to link 
to, and the location to store the built CCDump.exe application.

Sources
Sources file for the project. This file causes the build utility to run Nmake 
on the specified makefile. 

Hdmp.dsp 
Project file.

ABOUT CCDUMP
================

CCDump.exe presents a dialog box in which a user can type the name 
of a file to which to copy CC data. When the user clicks Start, 
CCDump.exe first creates a new thread of execution and calls 
the starting-point function of the thread. From within the new thread, 
CCDump.exe opens a connection to the VBI minidriver named Closed Caption Decoder 
and requests scan line number 21 and data from the odd scan of the 
interleaved television signal, that is the odd substream. CCDump.exe reads 
data asynchronously from the CC data stream and copies the CC data to 
the specified file. CCDump.exe continues to read and copy CC data until the user 
clicks Stop. 

In addition to reading and copying CC data, CCDump.exe counts and 
displays in an edit box the number of bytes copied to the file. 
When the user clicks Stop, CCDump.exe disconnects from the 
Closed Caption Decoder and frees resources. To open a 
connection to a VBI minidriver and read data from the minidriver, 
CCDump.exe calls functions of the coder/decoder (codec) library for 
the VBI client interface, Icodec.lib. For information on working with 
the VBI client interface, see Using the VBI Client Interface in the 
Broadcast Architecture DDK documentation. For 
reference information on the VBI client interface, see the VBI Client 
Interface Reference in the Broadcast Architecture DDK documentation.
