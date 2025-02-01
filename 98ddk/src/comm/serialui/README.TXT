To build serialui.dll properly you must pay close attention to your tools and 
your build environment.  This sample is a 16 bit DLL.  Thus the need for MSVC 1.52   

Tools:
1. msvc 1.52
2. masm6.11c or d
3. Win98 DDK

Environment:
1. In the WIN98 DDK build environment, make sure you have the PATH, INCLUDE and LIB
	environment variables set correctly for your machine.
2. make sure that your 16 bit tools are included in the PATH
3. run the following batch file from the ddk with the following command line
   options:

   D:\98ddk\bin\setenv.bat D:\98ddk free

This will make sure your environment variables and tools will be properly configured
to build serialui.dll.

4.  Then, if you cd to the src/comm/serialui directory and execute 
	build -cef
	The object and dll files will magically appear in the obj\i386 subdirectory.

This sample is provided in the ddk as an example of a property page provider, adding
proprietary property pages, and adding context sensitive help to a property page.  This
is almost exactly the source to the property page provider supplied in Windows 98
Microsoft product.  It is, however offered here without guarantee or warranty as to
suitability to the intent or purpose of the user.  