-----------------------------------------------------------------------------

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

    Copyright (C) 1998 Microsoft Corporation.  All Rights Reserved.

    File:       readme.txt
    Content:    Overview and installation instructions for Win98 DDK 
                DigiJoy sample

-----------------------------------------------------------------------------

Overview

DigiJoy is a sample joystick mini-driver which uses a standard analog gameport 
to drive a joystick which is not catered for by the standard analog driver.  
Since all truely digital joysticks use proprietary protocols and for the sake 
of simplicity, this driver polls a standard analog joystick but it interprets 
the button information in such a way as to generate POV and/or extra button 
information depending on values in the registry.

Installation

You must have an analog gameport installed on your system before installing 
this driver.  Since this driver uses a device plugged into the gameport, it 
is not plug and play compatible.  First build the driver and copy it and the 
INF to a floppy or other test location.  To install the driver, use "Add New 
Hardware" from the control panel ignore any PnP hardware that may be listed 
and choose to select the hardware from a list.  Select the "Sound, video and 
game controllers" as the type of hardware and then use the "Have Disk..." 
button and direct it to the location of the sample INF and built VxD.  Now 
run "Game Controllers" from the control panel and you should be able to add 
one of the new device types.  Since many joystick drivers do not share IO 
resources correctly it is probably best to start by removing any devices 
that are already set up.  Once set up, you can modify joystick mini-drivers 
and then reload them by deselecting the driver from the control panel, then 
reselecting it.
