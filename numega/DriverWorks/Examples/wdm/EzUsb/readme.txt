Readme.txt - DriverWorks EzUsb example
Copyright (c) 1999 Compuware Corporation

This sample is designed as a drop in replacement for the
USB generic driver for the Cypress/Anchor Chips EZ_USB
Development Kit.  The sample driver is designed to work
on Windows 98 and Windows 2000 operating systems.

To install the driver for use with the Cypress/Anchor Chips 
EZ_USB Development Kit, use the EzUsb.inf file included
with the sample.  Remove existing drivers installed for
the Development Kit prior to installing the sample.

The EzUsb.inf file includes support for the following EZ_USB 
Development Kits, identified by Vendor and Product Id:
 
 VID=0547 PID=0080: Anchor Chips EZ-USB Development Board
 VID=0547 PID=1002: Anchor Chips EZ-USB Sample Device
 VID=0547 PID=2131: Anchor Chips EZ-USB (2131Q/2131S/2135S)
 VID=0547 PID=2126: Anchor Chips EZ-USB (2126S)
 VID=0547 PID=2125: Anchor Chips EZ-USB (2121S/2125S)
 VID=0547 PID=2136: Anchor Chips EZ-USB (2136S)
 VID=0547 PID=2235: Anchor Chips EZ-USB (2235)
 VID=0547 PID=2236: Anchor Chips EZ-USB (2236) 
 VID=0547 PID=2225: Anchor Chips EZ-USB (2225) 
 VID=0547 PID=2226: Anchor Chips EZ-USB (2226) 

 VID=04B4 PID=8613: Cypress EZ-USB FX2 (68613) 
 VID=04B4 PID=1002: Cypress EZ-USB FX2 Sample Device
 VID=04B4 PID=0081: Cypress EZ-USB FX2 Development Board

To install the DriverWorks UsbFilt sample as a lower filter
driver for this example, use the EzUsbFilt.inf file included
with the sample.  The UsbFilt driver must first be built 
before installing the sample with EzUsbFilt.inf.

To test interrupt transfer code, Walter Oney's device firmware 
was used.  This firmware is not distributed with this sample. 
If you have his book, you can test this code by downloading
his sample firmware.  For an excellent reference of WDM device 
drivers, see his book 
"Programming the Microsoft Windows Driver Model" 
http://www.oneysoft.com/

You can test this program by running Cypress's "EZ_USB Control Panel", 
which is installed from the CDROM that accompanies the
EZ_USB board. The program is accessible as C:\Cypress\USB\Bin\EzMr.exe.  
Once you load the program, you can go to "Help" and then "Contents and
Tutorials" for an introduction to testing the board and the driver. 
A quick test is to select the correct board type in the "Target" dropdown
box and then click on the "GetDev" button. You should get a short report that 
looks like this:

 Device Descriptor:  
 bLength:  18
 bDescriptorType:  1
 bcdUSB:  256
 bDeviceClass:  0xff
 bDeviceSubClass:  0xff
 bDeviceProtocol:  0xff
 bMaxPacketSize0:  0x40
 idVendor:  0x547
 idProduct:  0x2131
 bcdDevice:  0x4
 iManufacturer:  0x0
 iProduct:  0x0
 iSerialNumber:  0x0
 bNumConfigurations:  0x1

Note that the chip should respond with the same Vendor and Product ids that belong
to your board and that are selected from the .inf install file. In this case, for
example, the Vendor Id is 0x547 and the Product Id is 0x2131, which tells us that
the board is an Anchor Chips EZ_USB Model 2131 or 2135. 

Another quick test is to hit the Download button and download file dev_io.hex from
the C:\Cypress\USB\Examples\EZ_USB\dev_io directory (or from the FX2\dev_io directory
depending on your EZ_USB board). If it works, the one-digit LED display in your board 
will lit up with a value (typically 0). You can now press the on-board f1 and f2 buttons
to increment the LED value, or press f1 or f4 to reset it to a value of 0 or F. 

You can find more information in the documentation that accompanies your EZ_USB board, 
or in the "HELP" section of your EZ_USB Control Panel. If you want to do more detailed
testing, you can use the Keil UV2.EXE program, which allows you to cross-compile C-code
and download it to be run on your EZ_USB board. 

