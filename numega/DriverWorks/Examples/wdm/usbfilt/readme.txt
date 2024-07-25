Readme.txt - DriverWorks UsbFilt example
Copyright (c) 1999 Compuware Corporation

This sample can be used as an upper or lower filter for a USB function driver.  As a 
lower filter, the driver uses KDebugOnlyTrace operator << to display URBs
submitted by the function driver to the USB bus driver.

In order to build and load the sample for a particular USB function driver, please
be aware of the following requirements:

1. A sample .inf file is provided (UsbFilt.inf.txt).  It is not a complete .inf file.  
   The intention is for the driver writer to modify a .inf file for a USB function
   driver by adding relevant sections and entries as indicated by UsbFilt.inf.txt.
   It lists sections of a .inf file that need to be added to the USB function driver's 
   .inf file to have the UsbFilt sample driver loaded for a USB device for Windows 98 
   and Windows 2000.  Please refer to the comments in the sample .inf file for details.

2. An upper filter driver must match the behavior of the function driver.  The filter
   driver must handle all of the IRPs that the function driver does.  Depending on a 
   particular USB function driver that is being filtered, the driver writer may have 
   to implement additional IRP handlers to pass a particular IRP through to the 
   function driver.  For an upper filter, it is also important to match the DeviceType, 
   Characteristics, and DeviceFlags of the function driver DEVICE_OBJECT.  This can be
   modified in the AddDevice routine by editing the parameters passed to placement new
   for KPnpDevice.


