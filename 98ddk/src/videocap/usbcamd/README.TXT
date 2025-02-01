Readme.txt for USBCAMD


Description:
------------

   USBCAMD is a Helper WDM stream class video capture driver which requires a Camera minidriver
   for operation. When used, it wraps around the camera minidriver (eg. USBINTEL) from above
   (Stream class interfaces) and below ( USBD interfcaes). Hence, it reduces the complexity of
   writing WDM minidriver for USB video devices. IHVs relying on USBCAMD will only have to 
   write a simplified minidriver similar to USBINTEL.

   This is not a sample, it is actually the source code for the production version of USBCAMD
   shipped in Win98 along with USBINTEL. It is posted here in order to show what it takes to 
   write a stream class minidriver. An IHV who has no desire to use USBCAMD should study the 
   source code of this module along with USBINTEL, since the comination of the two modules will
   constitute a complete WDM USB video capture minidriver.

   USBCAMD creates capture streams in whatever color spaces format declared by camera minidriver.
   
               
Installation:
-------------

   You can't install USBCAMD by itself. However, if you plug in an Intel USB create & share camera
   both USBCAMD & USBINTEL will be innstalled by PnP .
   See USBINTEL readme about more specific installion instruction for that camera minidriver.
                                                       

Testing:
--------               

   1.  Build the simplest capture graph in GraphEdt.exe
    
   - Make sure you're using a version of DShow 5.2 or better 
      GraphEdt.exe. This is needed to support new PnP features.
   - Graph.InsertFilters
      "WDM Streaming Capture Devices"
         "USB Video Camera for Intel Proshare technology"
	
     Note :This string will vary depending on the Camera minidriver that goes along with 
     this driver.

   - Render the capture pin by right clicking on the pin
   - Graph.Play
      At this point, you should see youself on the screen if you are looking at your camera 
      device.
                                                
   
   3.  Use AMCap.exe.  This capture application is provided in the DShow 
       SDK, and does all of the graph building automatically.


Other Notes:
------------

   - There are two ways to change the capture format on the output pins of 
the Capture filter.  Before connecting them to a renderer, you can right 
click on the pin and bring up the Properties page.  This displays a dialog 
showing the color space, dimensions, framerate, and compression 
properties.  Alternatively, when a graph is fully connected, but stopped, 
you can right click on the pin to show the same property page.  
Unfortunately, if you point to the line connecting the pin to the 
downstream filter, Graphedt assumes you're asking for properties about 
the connection (rather than the pin).  So you need to right click on the 
pin but a couple of pixels directly above or below the line to see the 
pin properties.

   - USBCAMD does not persist property settings between reboots. Camera minidriver
     should as USBINTEL does.
   
   - When calculating timestamps, always use the 
KSCONVERT_PERFORMANCE_TIME macro provided in ks.h.  Many of the 
beta drivers used an incorrect implementation of this calculation 
which ignored the remainder in the HighPart of the LARGE_INTEGER.