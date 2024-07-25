Readme.txt - DriverWorks MouseFilter WMI example
Copyright (c) 1999 Compuware Corporation

1. To build the application samples, you will need the 
   WBEM SDK or platform SDK. You can download the WBEM SDK from:

	http://msdn.microsoft.com/developer/sdk/wmisdk/tutorial.asp

   ** The environment variable MSSDK must be set to your SDK directory.

		example:   set MSSDK=c:\mssdk

2. To install the driver, open the device manager, right click on the mouse, 
   and select "Update Driver".  Choose "Have disk", and browse to the mouse 
   filter directory. 

3. The DriverWorks implementation is based on Microsoft's WMILIB. 
   Because WMILIB is not available for Windows 98, DriverWorks provides
   its own implementation of the required WMILIB functions. This
   means that DriverWorks drivers will work on both Windows 
   2000 and Windows 98. 

