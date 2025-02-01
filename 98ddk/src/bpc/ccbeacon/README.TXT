Head-end application that inserts Microsoft(R) TV Crossover Links(TM)
in a stream, and client application that reads closed captioning 
(CC) data, displays the number of bytes of CC data read, and 
detects and displays TV Crossover Links  

SUMMARY
=======

This CC Beacon tool consists of a head-end application, Ccbeacon.exe, 
and a client application, Ccbc.exe, that you can use to verify 
the accuracy of your system for broadcasting and receiving CC 
information. 

The head-end application inserts both TV Crossover Links 
and a formatted string pattern of characters A through Z 
in the standard output (stdout) stream of the operating 
system. The client application connects to the VBI minidriver 
named Closed Caption Decoder in the Microsoft(R) Windows(R) 98 
operating system and reads data asynchronously from the 
minidriver. While a channel to which a broadcast client is tuned 
receives CC data with inserted TV Crossover Links, the client 
application counts and displays the number of bytes read 
from the CC data stream and detects if the CC data came 
from the Captioning or the Text-2 operating mode of a CC data stream. 
Caption data, such as the A through Z pattern, is transmitted in 
Captioning mode, and TV Crossover Links are transmitted in Text-2 mode. 

When the client application receives strings containing TV Crossover 
Links, it displays them. When the client application receives CC data, it 
displays the 2-byte hexadecimal values of the CC data it read and the 
2-byte hexadecimal values of the CC data that was expected. The 
client application runs under the assumption that it reads a CC data 
stream containing the string pattern of characters A through Z inserted 
by the head-end application.

The CC beacon tool is designed to run with the current version of 
Broadcast Architecture. To transmit TV Crossover Links over the 
vertical blanking interval (VBI) of a television signal, run the head-end 
application under the Microsoft(R) Windows NT(R) operating system, 
version 4.0 and later, with Microsoft(R) Broadcast Server components installed. 
Required Broadcast Server components include, for example, the 
Microsoft(R) Broadcast Router and a virtual interface (VIF) for VBI data. 
For more information about the Broadcast Router and virtual interface, see the 
sections Microsoft Broadcast Router and Virtual Interfaces in the Broadcast 
Architecture Programmer's Reference, part of the Microsoft(R) Platform SDK. 

The client application runs under Windows 98. 

MORE INFORMATION
================

The following information describes the CC Beacon tool.

What Are TV Crossover Links? 
-----------------------------
TV Crossover Links are a way to link the Internet to 
television shows. A TV Crossover Link contains an address 
of an Internet site and is inserted in the CC data stream of 
a show. This Internet site directly links viewers of the show 
to Internet content specifically designed to complement the show. 
TV Crossover Links consist of attributes in the following 
general format: 
 	URL			Type		Name		   Checksum
 <http://webtv.microsoft.com>[type:SPONSOR][name:WebTV for Windows][3592] 

The universal resource locator (URL) attribute is an address of 
an Internet site to which the interactive television content being 
broadcast points.

The type attribute indicates the portion of a show in which the 
TV Crossover Link is displayed, such as during the actual 
show, during a commercial, or during station identification.

The name attribute is a message that appears to viewers after 
they click the "i" icon on their screens. This message describes 
Internet content that viewers can receive. 

The checksum verifies the accuracy of the string that contains 
the TV Crossover Link and detects data corruption. 

To locate more information on TV Crossover Links, 
visit the Internet site http://webtv.net.

Redirecting the Operating System's Standard Output Stream 
-----------------------------
The operating system by default prints stdout to the screen, 
but you can redirect stdout to a different device. You 
can modify the head-end application or create an additional 
head-end application that redirects the stdout stream at a 
scheduled time to a device called a VBI inserter. Doing so 
creates a content server application. For more information 
about how to write content server applications that gather, schedule, 
and send data to the Broadcast Router and a virtual interface for VBI data, see 
Writing Content Server Applications in the Broadcast Architecture 
Programmer's Reference.

Requirements
-----------------------------
Note: To build and run these applications, you need the following:
      
The Microsoft(R) Visual C++(R) development system, version 5.0. For 
more information on installing and using Visual C++ 5.0, including 
Microsoft Foundation Classes (MFC), see the Visual C++ 5.0 
documentation. 

The Windows 98 Device-Driver Kit (DDK). 

A Norpak TES-3 encoder, the output device used to encode 
and insert data into the VBI of a standard analog television signal. 
The Broadcast Router forwards IP multicast packets from the 
Broadcast Router to the virtual interface for VBI data. The 
virtual interface then sends the packets to the Norpak TES-3 
encoder to transmit the packets over the VBI. 

To Build and Run Ccbeacon.exe from the Command Line 
-----------------------------
1. You must install the Visual C++ development system before you 
install the Windows 98 DDK. Otherwise, the Setenv.bat file in the 
Windows 98 DDK will not set up the correct environment when you run 
it. 

2. Open a Command Prompt window (for example, in the free build 
environment that the Windows 98 DDK provides); switch to the 
directory on your computer that contains the Sources file, the 
Makefile, and the source files for Ccbeacon.exe; and then type the 
following command:

build

3. To run the built Ccbeacon.exe application, switch to the \Debug 
directory created within the directory containing the source files.
Ccbeacon.exe is also copied to the subdirectory \Lib\I386\Free\ of the 
base directory within the Windows 98 DDK, for example 
C:\98ddk\Lib\I386\Free.

4. Type 

ccbeacon

5. In addition, you can use the following command-line syntax to 
direct the behavior of Ccbeacon.exe: 

-d (Delay, in microseconds, after sending each line of the 
    A through Z pattern or after sending each TV Crossover Link; 
    for example, 5000 equals 5 seconds) 
-i (Input file containing TV Crossover Links to insert in the stream) 
-f (Frequency to insert TV Crossover Links in the stream; for example, 10 
    specifies to insert TV Crossover Links after every 10 lines of the 
    A through Z pattern)

To run Ccbeacon.exe so that it has a 5-second delay per line, 
inserts TV Crossover Links from a file stored in the 
C:\links.txt file, and inserts the TV Crossover Links after every 
10 lines of the A through Z pattern, type: 

ccbeacon.exe -d 5000 -i c:\links.txt -f 10 

If no options are specified, Ccbeacon.exe provides the following 
default values:  

Delay: None, that is as fast as possible 
TV Crossover Links: One link as follows: 
 <http://webtv.microsoft.com>[type:SPONSOR][name:WebTV for Windows][3592] 
Frequency: Inserts the preceding link after every 100 instances of the 
A through Z pattern

To Build and Run Ccbc.exe from the Command Line 
-----------------------------
1. You must install the Visual C++ development system before you 
install the Windows 98 DDK. Otherwise, the Setenv.bat file in the 
Windows 98 DDK will not set up the correct environment when you run 
it. 

The header files that the Ccbc project must obtain from the 
Windows 98 DDK are Ccdecode.h, Iks.h, Icodec.h, Ks.h, and Ksmedia.h. 
The libraries that the Ccbc project must link to are Icodec.lib 
and Ksuser.lib. 

2. Open a Command Prompt window (for example, in the free build 
environment that the Windows 98 DDK provides); switch to the 
directory on your computer that contains the Sources file, the 
Makefile, and the source files for Ccbc.exe; and then type the 
following command:

build

3. To run the built Ccbc.exe application, switch to the \Debug 
directory created within the directory containing the source files.
Ccbc.exe is also copied to the subdirectory \Lib\I386\Free\ of the 
base directory within the Windows 98 DDK, for example 
C:\98ddk\Lib\I386\Free.

4. Type 

   ccbc 

5. Start Microsoft(R) WebTV(TM) for Windows and tune to the channel 
that receives the test CC data stream containing TV 
Crossover Links and the A through Z pattern. 

6. Click Run to start reading CC data. 

7. Click Run again to stop reading CC data.

Ccbeacon Head-End Application Files
------------
Getopt.h and Getopt.c
Header and source files that define and implement the getopt 
function used by main.cpp. The getopt function retrieves and 
processes options from the head-end application's command line. 

Ccbeacon.h 
Header file that defines CC control codes. CC control codes 
identify the operating mode of the CC data stream, that is 
whether CC data is in Captioning or Text-2 mode, and 
identify the format, attributes, and display of characters. 
For more information about control codes, see 
Closed Caption Decoder Requirements
from FCC Report and Order FCC 91-119.

Ccbeacon.cpp 
Source file that implements functions that create the A 
through Z pattern string and the default TV 
Crossover Link string. 

Main.cpp
Main source file that inserts alternating TV Crossover 
Links with a formatted A through Z pattern in the operating 
system's stdout stream.

Makefile 
Makefile for the project. This file provides the Nmake utility with information 
to build Ccbeacon.exe. This information includes, for example, the 
source files to compile, the header files to include, the libraries to 
link to, and the location to store the built Ccbeacon.exe application.

Sources
Sources file for the project. This file causes the build utility to run Nmake 
on the specified makefile. 

Ccbeacon.dsp 
Project file.

Ccbc Client Application Files
------------
Ccbc.h and Ccbc.cpp
Main header and source files that define and implement the class for 
the Ccbc application. This class creates and initializes a dialog 
box and sets the dialog box to be the application's main window.

Ccbcdlg.h and Ccbcdlg.cpp
Header and source files that define and implement the CCcbcDlg class. 
This class contains member variables that store data entered in the 
dialog box's controls by a user and that store data displayed in the 
dialog box's controls for the user. This class also contains methods 
that initialize the dialog box's controls with the proper values and 
retrieve and display data.

Ccbeacon.h and Ccbeacon.cpp
These files are identical to those described under Ccbeacon Head-End 
Application Files, preceding.

StdAfx.h and StdAfx.cpp
These files are used to build a precompiled header (PCH) file named 
Ccbc.pch and a precompiled types file named StdAfx.obj.

Resource.h
Header file that defines resource identifiers that identify the 
dialog box's controls. 

Ccbc.rc
File that lists and describes all the Windows resources that the Ccbc 
application uses, including the icon stored in the \Res subdirectory. 

Res\Ccbc.ico
Icon file that is used as the Ccbc application's icon.

Res\Ccbc.rc2
File that contains resources that are not edited by Visual C++. 

Makefile 
Makefile for the project. This file provides the Nmake utility with information 
to build Ccbc.exe. This information includes, for example, the source 
files to compile, the header files to include, the libraries to link 
to, and the location to store the built Ccbc.exe application.

Sources
Sources file for the project. This file causes the build utility to run Nmake 
on the specified makefile. 

Ccbc.dsp 
Project file.

ABOUT THE CC BEACON TOOL
================
Ccbeacon.exe inserts an alternating A through Z test pattern 
with at least one TV Crossover Link in the operating 
system's stdout stream. 

Ccbc.exe connects to the VBI minidriver named Closed Caption Decoder 
and presents a dialog box to users. When the user clicks Run, 
Ccbc.exe first requests scan line 21 and data from 
the odd scan of the interleaved television signal, that is the odd substream. 
Ccbc.exe next creates a new thread of execution and calls the 
starting-point function of the thread. From within the new thread, 
Ccbc.exe reads data asynchronously from the CC data stream. Ccbc.exe 
continues to read CC data until the user clicks Run again. 

In addition to reading CC data, Ccbc.exe does the following: 

1. Counts and displays in an edit box the number of bytes read. 
2. Detects if the CC data stream's operating mode is Captioning or 
   Text-2. Caption data, such as the A through Z pattern, is 
   transmitted in Captioning mode, and TV Crossover Links 
   are transmitted in Text-2 mode.
3. When Ccbc.exe receives strings containing TV Crossover 
   Links, it displays them in a text box. When Ccbc.exe receives CC data, 
   it displays the 2-byte hexadecimal values of the CC data it read and the 
   2-byte hexadecimal values of the CC data that was expected in a text box. 
4. Displays the number of errors indicating differences between read 
    and expected values. 
5. Increments twice the last 2-byte value received and assigns the results to 
   the expected values in preparation for the next received group of two 
   2-byte values. Because Ccbc.exe is designed to receive a sequential pattern 
   from Ccbeacon.exe, this step is only valid if Ccbc.exe reads the A through Z 
   test pattern from Ccbeacon.exe.

When the user clicks Run again, Ccbc.exe stops reading 
CC data, exits the thread, and frees resources. To open a connection 
to a VBI minidriver and read data from the minidriver, Ccbc.exe 
calls functions of the coder/decoder (codec) library for the VBI 
client interface, Icodec.lib. For information on working with the 
VBI client interface, see Using the VBI Client Interface in the 
Broadcast Architecture DDK documentation. For 
reference information on the VBI client interface, see the VBI Client 
Interface Reference in the Broadcast Architecture DDK documentation.
