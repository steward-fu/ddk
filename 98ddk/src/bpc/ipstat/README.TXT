A C application that prints the Internet Protocol (IP) addresses and ports 
that a broadcast client is currently using to receive North American Basic 
Teletext Specification (NABTS) data

SUMMARY
=======
Ipstat is a tool that runs on broadcast clients. It connects to the 
NABTS/IP driver to determine the IP addresses and ports a client 
is using to receive data. The tool then prints a report of this information.

Note:
Ipstat may occasionally report invalid IP address and port combinations. 
This occurs when the client in question receives bad packets. To account 
for this, look at the time stamp of each address; addresses on which the 
client has recently received data are more likely to be valid than 
addresses on which the last data was received some time ago.

MORE INFORMATION
================
The following information describes the Ipstat tool.

Requirements
-----------------------------
Ipstat must be compiled and run on a computer running 
the Microsoft(R) Windows(R) 98 operating system with 
the Microsoft(R) WebTV(TM) for Windows components 
installed. 

To build this application, you need the following:

The Microsoft(R) Visual C++(R) development system, version 5.0. 
For more information on installing and using Visual C++ 5.0, 
see the Visual C++ 5.0 documentation. 

The Windows 98 Device-Driver Kit (DDK). 

Note that you must install the Visual C++ development system 
before you install the Windows 98 DDK. Otherwise, the Setenv.bat 
file in the Windows 98 DDK will not set up the correct environment 
when you run it. 

The header files that the Ipstat project must obtain 
from the Windows 98 DDK are Iks.h, Ks.h, and Ksmedia.h. 
The library that the Ipstat project must link to is Icodecd.lib. 

To Compile Ipstat
------------------
Open a Command Prompt window (for example, in the free build 
environment that the Windows 98 DDK provides); switch to the 
directory on your computer that contains the Sources file, the 
Makefile, and the source files for Ipstat.exe; and then type the 
following command:

build

To Run Ipstat
---------------
If you compiled Ipstat.exe in the free build environment, 
Ipstat.exe is stored to the subdirectory \Lib\I386\Free\ of 
the base directory within the Windows 98 DDK, for example 
C:\98ddk\Lib\I386\Free.

If you compiled Ipstat.exe in the checked build environment, 
Ipstat.exe is stored to the subdirectory \Lib\I386\Checked\ of 
the base directory within the Windows 98 DDK, for example 
C:\98ddk\Lib\I386\Checked.

At the command prompt, type the following command:

Ipstat 

The Ipstat tool prints a report of the currently used
IP addresses and ports to the screen. 

To make Ipstat print the report to a file, type the following 
command at the command prompt:

Ipstat > <filename>

where <filename> is replaced by the name of the output text file 
to receive the report. If this file does not already exist, Ipstat 
creates it.

Ipstat Files
--------------
Ipstat.cpp -- Ipstat source file that implements the main function.

Iks.h -- Header file that contains the kernel mode streaming driver, 
pin, and property interfaces.

Ks.h -- Header file that contains the Windows Driver Model/Connection 
and Streaming Architecture (WDM-CSA) core definitions.

Ksmedia.h -- Header file that contains the WDM-CSA multimedia definitions.

Stats.h -- Header file that defines structures that store address and port 
information.

Wdmioctl.h -- Header file that contains internal input/output control codes 
(IOCTLs) for communication between Ipstat and the NABTS/IP driver.

Makefile -- Makefile for the project. This file provides the Nmake utility 
with information common to all DDK drivers and programs. 

Sources -- File that provides the build utility with information to build 
Ipstat.exe. This information includes, for example, 
the source files to compile, the header files to include, 
the libraries to link to, and the location to store the 
built Ipstat.exe application.
