/*****************************************************************************\
Skeleton source code for a VBI minidriver that contains generic code to control
and decode a VBI data stream.  

This Readme.txt file contains the following sections:
* Summary 		Briefly describes the sample.
* More Information	Details how to build the sample.

For more information about how to write vertical blanking interval (VBI) 
minidrivers that communicate with the Microsoft(r) Windows(r) Driver 
Model (WDM) stream class driver, see VBI Minidriver Development 
Tasks in the Broadcast Architecture Device-Driver Kit (DDK) documentation.

\*****************************************************************************/

SUMMARY
=========
This sample is a VBI minidriver that contains stubbed-out service functions 
used with any VBI data stream. Hardware vendors must include their own 
specific coder/decoders (codecs) to produce VBI minidrivers that interpret 
their specific VBI data streams. 

This sample was developed using the Microsoft(r) Visual C++ development 
system, version 5.0.

MORE INFORMATION
================
Hardware vendors should implement their VBI minidriver code by including the 
Strmini.h and Ksmedia.h header files and by calling the WDM stream class driver 
functions described in the WDM device-driver documentation in the Microsoft(r) 
Windows(r) 98 DDK. 

NOTE
The generic VBI minidriver runs on Windows 98. 

Generic Files
--------------
Codmain.h provides an enumeration of all streams supported by the minidriver; 
structure, constant, and macro definitions; and function declarations for 
entry-point functions and functions private to the minidriver.

Codmain.c provides implementations for all the VBI minidriver's entry-point 
functions and functions that initialize, open, and close VBI data streams 
and that get information about VBI data streams.             

Codstrm.h defines the properties and format of the VBI data streams that the 
VBI minidriver controls and decodes.

Codvideo.c provides implementations for functions that control and decode 
open VBI data streams.             

Codprop.h defines properties of the tuner for the VBI minidriver.

Codprop.c provides implementations for functions that handle property sets. 

Coddebug.h provides definitions for debugging.

Coddebug.c provides implementations for functions that handle debugging. 

Makefile is the Visual C++ makefile for the generic VBI minidriver.

Codec.inf provides setup information. A hardware vendor should include 
information for a specific codec in this file, such as a class identifier 
(CLSID), a device description (provided using the VBIcodec.DeviceDesc 
property), and a name to display to the user (provided using the 
VBIcodec.FriendlyName property). After the vendor does so, installing 
this file sets entries in the Windows registry for decoding VBI data 
streams specific to that vendor.
