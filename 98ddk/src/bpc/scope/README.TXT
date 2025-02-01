Application that displays waveforms of data from specific scan lines 

SUMMARY
=======

This sample application, Scope.exe, is a tool with which users can 
see whether a tuned channel is receiving any data from 
specific scan lines of the vertical blanking interval (VBI) of 
a television signal. The tool is designed to run with the current version 
of Broadcast Architecture running under the Microsoft(R) Windows(R) 98 
operating system.

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

To Build Scope.exe from the Command Line 
-----------------------------
1. You must install the Visual C++ development system before you 
install the Windows 98 DDK. Otherwise, the Setenv.bat file in the 
Windows 98 DDK will not set up the correct environment when you run 
it. 

2. Open a Command Prompt window (for example, in the free build 
environment that the Windows 98 DDK provides); switch to the 
directory on your computer that contains the Sources file, the 
Makefile, and the source files for Scope.exe; and then type the 
following command:

build

To Install the VBI Scope Filter and the Sample Streaming VBI Codec
---------------
Running Scope.exe requires the filter VBI Scope (Vbiscope.ax) 
and the Sample Streaming VBI Codec (Codec.sys), a generic VBI minidriver. 
VBI Scope was developed from the Scope sample obtained from 
the Microsoft(R) DirectShow(TM) Software Development 
Kit (SDK). To view the source for the Scope sample, see the 
DirectShow SDK. The Sample Streaming VBI Codec minidriver is 
a sample that is part of the Windows 98 DDK. To view the source for 
this minidriver, see the Windows 98 DDK. Before you can 
run Scope.exe, you must install and register the VBI Scope filter 
and install the Sample Streaming VBI Codec minidriver. 

To install and register the VBI Scope filter, copy Vbiscope.ax to 
the \System directory of your computer's \Windows directory, switch 
to the \System directory, and type the following command:

regsvr32.exe vbiscope.ax 

To install the Sample Streaming VBI Codec minidriver, 
perform the following steps:

1. Run the Add New Hardware application, a Control Panel application. 

2. In the application window, click "Sound, video and game controllers" as 
the type of hardware to install.

3. In the Add New Hardware Wizard, select Microsoft as the hardware 
manufacturer and Sample Streaming VBI Codec as the hardware model, 
and then click Have Disk.

4. In the Install From Disk dialog box, click Browse to locate the Codec.inf 
setup information file. This file provides information that Windows 
requires to install the minidriver. Codec.inf by default installs Codec.sys 
in the \System directory of your computer's \Windows directory. 

5. Move Codec.sys to the \System32\Drivers directory of your 
computer's \Windows directory, as required by Scope.exe. 

6. Restart your computer. 

To Run Scope.exe from the Command Line
---------------
1. Switch to the \Debug directory created within the directory containing 
the application's source files, where Scope.exe resides. Scope.exe is also 
copied to the subdirectory \Lib\I386\Free\ of the base directory within 
the Windows 98 DDK, for example C:\98ddk\Lib\I386\Free.

2. Type: 

Scope 

Scope.exe first starts the VBI Scope filter and then opens its 
own dialog box. The Scope.exe dialog box displays video streaming from 
the channel to which the broadcast receiver card is currently tuned. The 
dual trace functionality of the VBI Scope filter displays waveforms 
of data from specific scan lines. 

3. To stop video streaming, click Play. 

4. To resume video streaming, click Play again. 

Scope Files
------------
Scope.h and Scope.cpp
Main header and source files that define and implement the class for 
the Scope application. Because the Scope application might behave 
unpredictably if Microsoft(R) WebTV(TM) for Windows is running, this 
class first either verifies that WebTV for Windows is not running or presents 
a dialog box using which users can close WebTV for Windows if it is running. 
This class next uses components of the DirectShow SDK to build a filter graph 
by adding filters and connecting them. This class then creates and initializes 
a dialog box and sets the dialog box to be the application's main window.

Scopedlg.h and Scopedlg.cpp
Header and source files that define and implement the CScopeDlg class. 
This class contains member variables that store data entered in the 
dialog box's controls by a user and that store data displayed in the 
dialog box's controls for the user. This class also contains methods 
that initialize the dialog box's controls with the proper values and 
retrieve and display data. 

This class overrides the OnInitDialog method of the CDialog class to: 
1. Set the video-playback window of the previously built filter 
graph to a frame-window control of the dialog box class.
2. Retrieve the channel number to which the broadcast receiver card is 
currently tuned and set the text in an edit-box control to 
that channel number. 

Testgrf.h and Testgrf.cpp
Header and source files that define and implement the CTestGraph class. 
This class contains a member variable that is a pointer to an 
IGraphBuilder interface. The IGraphBuilder interface enables Scope.exe 
to call the filter graph manager so as to build the filter graph by 
adding filters and creating filter connections. 

Guids.cpp
This file defines the globally unique identifier (GUID) for the 
included VBI Scope filter, Vbiscope.ax.

StdAfx.h and StdAfx.cpp
These files are used to build a precompiled header (PCH) file named 
Scope.pch and a precompiled types file named StdAfx.obj.

Resource.h
Header file that defines resource identifiers that identify the 
dialog box's controls. 

\DirectShow
Directory containing header and library files obtained from the 
DirectShow SDK. 

These files were obtained from the core build environment 
and Microsoft(R) DirectX(R) media components of the Microsoft(R) 
Platform SDK. The header files that the Scope project requires from the 
DirectShow SDK are Amextra.h, Amfilter.h, Amvideo.h, Cache.h, 
Combase.h, Comlite.h, Control.h, Cprop.h, Ctlutil.h, Dllsetup.h, 
Edevdefs.h, Errors.h, Evcode.h, Fourcc.h, Ksuuids.h, Measure.h, 
Msgthrd.h, Mtype.h, Outputq.h, Pstream.h, Refclock.h, Reftime.h, 
Renbase.h, Schedule.h, Source.h, Streams.h, Strmctl.h, Strmif.h, 
Sysclock.h, Transfrm.h, Transip.h, Uuids.h, Vfwmsgs.h, Videoctl.h, 
Vtrans.h, Winctrl.h, Winutil.h, Wxdebug.h, Wxlist.h, and Wxutil.h. 
The libraries that the Scope project requires are Quartz.lib, 
Strmbasd.lib, and Strmbase.lib. 

Scope.rc
Resource file that lists and describes all the Windows resources that the Scope 
application uses, including the icon stored in the \Res subdirectory. 

Res\Scope.ico
Icon file that is used as the Scope application's icon.

Res\Scope.rc2
File that contains resources that are not edited by Visual C++. 

Makefile 
Makefile for the project. This file provides the Nmake utility with information 
to build Scope.exe. This information includes, for example, the source 
files to compile, the header files to include, the libraries to link 
to, and the location to store the built Scope.exe application.

Sources
Sources file for the project. This file causes the build utility to run Nmake 
on the specified makefile. 

Scope.dsp 
Project file.

Vbiscope.ax
VBI Scope filter.

Codec.sys
Sample Streaming VBI Codec minidriver.

Codec.inf
Setup information file to install Codec.sys.

ABOUT SCOPE
================

Scope.exe builds a filter graph by adding and connecting together 
specific filters, starts the filter graph's VBI Scope filter, and 
presents a dialog box that displays the channel to which 
the filter graph's broadcast receiver card is currently tuned. 
Using Scope.exe, a user can view waveforms from the VBI Scope filter 
of data from specific VBI scan lines of the current channel. 
Thus, the user can see whether a channel is receiving any data from 
specific VBI scan lines.

Scope.exe adds the following filters to its filter graph:

	TVTuner
	Crossbar
	Capture
	CC Decoder
	NABTS
	Generic VBI Codec
	Tee
	VBI Surface Allocator
	Line 21 Decoder
	Overlay Mixer
	Video Renderer
	VBI Scope 

Scope.exe connects TVTuner to Crossbar to Capture to VBI Surface Allocator 
to Tee. Scope.exe then connects the Tee splitter to Generic VBI Codec, 
NABTS, and CC Decoder. Scope.exe then connects CC Decoder to 
Line 21 Decoder to Overlay Mixer. For information on DirectShow, 
including information on building a filter graph and creating new 
DirectShow filters, see the Microsoft DirectShow SDK section in the 
Platform SDK.

