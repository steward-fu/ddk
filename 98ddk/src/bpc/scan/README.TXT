Application that detects and displays the scan lines containing 
closed captioning (CC) and raw North American Basic Teletext 
Specification (NABTS) data, and that retrieves specific types of 
enhancement data

SUMMARY
=======

This sample application, Scan.exe, is a tool with which users can 
connect to and retrieve information from the vertical blanking interval 
(VBI) minidrivers named Closed Caption Decoder and 
NABTS/FEC VBI Codec in the Microsoft(R) Windows(R) 98 operating 
system. 

While a tuned channel receives data services, users can use Scan.exe 
to detect and display the VBI scan lines that contain CC and raw 
NABTS data. In addition, with Scan.exe, users can detect the specific 
types of enhancement data that the NABTS/FEC VBI Codec minidriver 
sends. The NABTS/FEC VBI Codec minidriver specifies each different 
type of enhancement data with a group identifier. Scan.exe is designed to 
run with the current version of Broadcast Architecture running under the 
Microsoft(R) Windows(R) 98 operating system.

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

To Build and Run Scan.exe from the Command Line 
-----------------------------
1. You must install the Visual C++ development system before you 
install the Windows 98 DDK. Otherwise, the Setenv.bat file in the 
Windows 98 DDK will not set up the correct environment when you run 
it. 

The header files that the Scan project must obtain from the 
Windows 98 DDK are Ccdecode.h, Nabts.h, Nabtsfec.h, Iks.h, 
Icodec.h, Ks.h, and Ksmedia.h. The libraries that the Scan 
project must link to are Icodec.lib and Ksuser.lib. 

2. Open a Command Prompt window (for example, in the free build 
environment that the Windows 98 DDK provides); switch to the 
directory on your computer that contains the Sources file, the 
Makefile, and the source files for Scan.exe; and then type the 
following command:

build

3. To run the built Scan.exe application, switch to the \Debug 
directory created within the directory containing the source files.
Scan.exe is also copied to the subdirectory \Lib\I386\Free\ of the base 
directory within the Windows 98 DDK, for example C:\98ddk\Lib\I386\Free.

4. Type 

   Scan 

5. Start Microsoft(R) WebTV(TM) for Windows, and tune to a channel that 
receives CC and enhancement data. 

6. Click Refresh for both CC and raw NABTS data detection to display 
information that indicates the scan lines that contain those types of data. 
Click Refresh for NABTS/FEC data detection to display the group 
identifiers that indicate the specific types of enhancement data that the 
NABTS/FEC VBI Codec minidriver sends. Click "Refresh All" to display 
information for CC, raw NABTS, and NABTS/FEC data detection.

Scan Files
------------
Scan.h and Scan.cpp
Main header and source files that define and implement the class for 
the Scan.exe application. This class creates and initializes a dialog box 
and sets the dialog box to be the application's main window.

Scandlg.h and Scandlg.cpp
Header and source files that define and implement the CScanDlg class. 
This class contains member variables that store data entered in the 
dialog box's controls by a user and that store data displayed in the 
dialog box's controls for the user. This class also contains methods 
that initialize the dialog box's controls with the proper values and 
retrieve and display data.

StdAfx.h and StdAfx.cpp
These files are used to build a precompiled header (PCH) file named 
Scan.pch and a precompiled types file named StdAfx.obj.

Resource.h
Header file that defines resource identifiers that identify the 
dialog box's controls. 

Scan.rc
Resource file that lists and describes all the Windows resources that the scan 
application uses, including the icon stored in the \Res subdirectory. 

Res\Scan.ico
Icon file that is used as the scan application's icon.

Res\Scan.rc2
File that contains resources that are not edited by Visual C++. 

Makefile 
Makefile for the project. This file provides the Nmake utility with information 
to build Scan.exe. This information includes, for example, the source 
files to compile, the header files to include, the libraries to link 
to, and the location to store the built Scan.exe application.

Sources
Sources file for the project. This file causes the build utility to run Nmake 
on the specified makefile. 

Scan.dsp 
Project file

ABOUT SCAN
================

Scan.exe presents a dialog box with which a user can select the VBI 
scan lines and enhancement data content of interest. When the user 
clicks Refresh for each data type or "Refresh All" for all data types, 
Scan.exe opens connections to the VBI minidrivers and requests scan 
lines and substreams (also known as groups) of interest. Scan.exe then 
detects if the scan lines contain CC and raw NABTS data and if the 
stream sent by the NABTS/FEC VBI Codec minidriver contains the 
types of enhancement data specified. Scan.exe displays in list boxes 
whether or not the scan lines contain CC and raw NABTS data. In 
addition, Scan.exe displays in a list box the detected types of 
enhancement data. 

Scan.exe also displays in a list box the status of calls made to the 
coder/decoder (codec) library for the VBI client interface, Icodec.lib. 
The VBI client interface is a set of interfaces that control VBI minidrivers 
and retrieve data from the streams of such minidrivers. For information 
on working with the VBI client interface, see Using the VBI Client Interface 
in the Broadcast Architecture DDK documentation. For reference information 
on the VBI client interface, see the VBI Client Interface Reference in the 
Broadcast Architecture DDK documentation. 

When Scan.exe completes detecting and displaying information, it 
disconnects from the VBI minidrivers and frees resources. 

VBI minidrivers obtain VBI data streams from just a portion of the scan 
lines of a television signal. The television scan lines that contain
transmitted VBI data are scan lines 10 through 21. 

Data services send their enhancement data in separate substreams of a 
VBI data stream. The NABTS/FEC VBI Codec minidriver specifies a 
particular type of enhancement data with a group identifier. The 
following table lists and describes valid group identifiers.
	Group identifier	Description
	0x800		Data content created by individuals and placed 
			within the footage of original shows. 
	0x810		Data content created by individuals and placed 
			in the advertisement portion of the footage of 
			original shows.
	0x820		Data content created by production companies 
			and placed in previously finished shows. 
	0x830		Data content created by production companies and 
			placed in the advertisement portion of previously 
			finished shows. 
	0x840		Data content created by broadcast syndicates and 
			placed in syndicated shows. 
	0x850		Data content created by broadcast syndicates and 
			placed in the advertisement portion of syndicated 
			shows. 
	0x860		Data content created by television networks and 
			placed in shows that the networks broadcast. 
	0x870		Data content created by television networks and 
			placed in the advertisement portion of shows that 
			the networks broadcast. 
	0x880		Data content created by individual television 
			stations and placed in shows that those stations 
			broadcast. 
	0x890		Data content created by individual television 
			stations and placed in the advertisement portion 
			of shows that those stations broadcast. 
	0x8A0		Data content created by local cable companies and 
			placed in shows broadcast from those companies' 
			cable head ends. 
	0x8B0		Data content created by local cable companies and 
			placed in the advertisement portion of shows 
			broadcast from those companies' head ends. 
	0x8C0-0x8EF 	Reserved for future expansion.
	0x8F0-0x8FF	Used for testing purposes.

