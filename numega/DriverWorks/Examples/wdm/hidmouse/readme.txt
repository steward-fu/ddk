Virtual HID Mouse Example
Copyright (c) 1997-1999 Compuware Corporation

The virtual HID mouse example shows how to build a WDM mouse driver
for Windows 98. The mouse driver is 'virtual' in the sense that it
does not require a physical mouse device. Instead, it gets mouse
input from a VxD (kbdmouse), which traps hot keys that correspond
to mouse moves and clicks.

The top level directory of the zip contains the source code 
for the VHIDMOU.SYS component of the example. This is a 
HID minidriver which can be installed under Windows 98. It has
been tested on builds 1525 and 1546. The example is built with
Compuware Numega's Driver::Works device driver development kit.
This tool kit is used to build drivers for Windows NT and WDM.

Subdirectory VXD contains the source code for the KBDMOUSE.VXD
driver. This is the VxD that converts hot keys to mouse input
by calling the exported functions of VHIDMOU.SYS. The driver
is based on Compuware Numega's VtoolsD device driver kit, which
is used to build VxDs for Windows 9x and Windows 3.x.
