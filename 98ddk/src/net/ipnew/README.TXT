========================================================================

 Module:                ipdetect.exe
 File:                  readme.txt
 Descriptions:          developer information
 Contains:              

 Copyright (c) 1996-1998, Microsoft Corporation, all rights reserved

 This document is provided for informational purposes only and Microsoft 
 Corporation makes no warranties, either expressed or implied, in this document.
 Information in this document may be substantially changed without notice in
 subsequent versions of windows and does not represent a commitment on the 
 part of Microsoft Corporation. 

========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : IPDETECT
========================================================================

IPDETECT.MAK
    This project file is compatible with the Visual C++ Workbench.

    It is also compatible with the NMAKE program provided with the
    Professional Edition of Visual C++.

    To build a debug version of the program from the MS-DOS prompt, type
	nmake DEBUG=1 /f IPDETECT.MAK
    or to build a release version of the program, type
	nmake DEBUG=0 /f IPDETECT.MAK

    It is very important to have your development environment set up
    correctly.  The following path information is based on installing 
    msvc 1.5x, Win98 ddk, Win32 sdk on drive 'd:' :

    INCLUDE=D:\DDK\INC16;D:\MSVC\INCLUDE;D:\MSVC\MFC\INCLUDE;D:\DDK\INC32;...
    LIB=D:\DDK\LIB;D:\MSVC\LIB;D:\MSVC\MFC\LIB;...
    PATH=D:\MSVC\BIN;...

    This is a 16bit MFC application.

MAKEFILE.
    This project file is compatible with the Win98 DDK build tool, build.exe

    You must edit your master.mk file in the inc directory of the Win98
    DDK so that C16_ROOT is set to the fully qualified path of your
    MSVC root directory, e.g. C16_ROOT=D:\MSVC
    
IPDETECT.H
    This is the main include file for the application.  It includes other
    project specific includes (including RESOURCE.H) and declares the
    CIpdetectApp application class.

IPDETECT.CPP
    This is the main application source file that contains the application
    class CIpdetectApp.
	 
	 ipdetect <-d> <-v> <-m<detect|PnP String>> <-p[COM1|COM2]>

	 Command Line Options:
		-d  Install Direct Cable Connect if necessary
		-v  VERBOSE mode
		-m  Install Modem if necessary using Modem Wizard
		
		-mdetect
		    Search registry for existing modem and 
		    use hardware detection to find a modem if
		    none exists

		-m<PnP or Unimodem String>
		    Install modem identified by PnP or Unimodem String

		-p[COM1|COM2]
		    Attach modem to specified com port


	This contains the detection, validation, installation and removal code.
	This is the important source file for developers.

IPDETECT.RC
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited with App Studio.

RES\IPDETECT.ICO
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file IPDETECT.RC.

RES\IPDETECT.RC2
    This file contains resources that are not edited by App Studio.  Initially
    this contains a VERSIONINFO resource that you can customize for your
    application.  You should place other non-App Studio editable resources
    in this file.

IPDETECT.DEF
    This file contains information about the application that must be
    provided to run with Microsoft Windows.  It defines parameters
    such as the name and description of the application, and the size
    of the initial local heap.  The numbers in this file are typical
    for applications developed with the Microsoft Foundation Class Library.
    The default stack size can be adjusted by editing the project file.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MAINFRM.H, MAINFRM.CPP
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

RES\TOOLBAR.BMP
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the
    CMainFrame class.  Edit this toolbar bitmap along with the
    array in MAINFRM.CPP to add more toolbar buttons.

/////////////////////////////////////////////////////////////////////////////

AppWizard creates one document type and one view:

IPDETDOC.H, IPDETDOC.CPP - the document
    These files contain your CIpdetectDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CIpdetectDoc::Serialize).

IPDETVW.H, IPDETVW.CPP - the view of the document
    These files contain your CIpdetectView class.
    CIpdetectView objects are used to view CIpdetectDoc objects.


/////////////////////////////////////////////////////////////////////////////
Other standard files:

STDAFX.H, STDAFX.CPP
    These files are used to build a precompiled header (PCH) file
    named STDAFX.PCH and a precompiled types (PCT) file named STDAFX.OBJ.

RESOURCE.H
    This is the standard header file, which defines new resource IDs.
    App Studio reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
