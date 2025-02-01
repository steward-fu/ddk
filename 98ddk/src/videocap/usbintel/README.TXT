Readme.txt for USBINTEL


Description:
------------

   USBINTEL is a WDM stream class video capture camera minidriver which requires helper driver USBCAMD
   for operation. When used, it plugs-in as a socket in USBCAMD. 

   This is a sample minidriver. It doesn't contain all the functionalities of the production level USBINTEL
   shipped with Win98 to support Intel USB Create & Share camera. However, it contains all the code required
   to build a functional and complete camera minidriver. An IHV who has no desire to use USBCAMD should study the 
   source code of this module along with USBCAMD, since the comination of the two modules will
   constitute a complete WDM USB video capture minidriver.

   USBINTEL creates capture streams in IYUV, I420 color spaces formats .
   
               
Installation:
-------------

   Plug-in Intel USB Create & Share camera, and both USBINTEL & USBCAMD will be installed by device mgr.
                                                       

Testing:
--------               

   1.  Build the simplest capture graph in GraphEdt.exe
    
   - Make sure you're using a version of DShow 5.2 or better 
      GraphEdt.exe. This is needed to support new PnP features.
   - Graph.InsertFilters
      "WDM Streaming Capture Devices"
         "USB Video Camera for Intel Proshare technology"
	
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

   - USBINTEL does persist property settings between reboots. 
   
   - When calculating timestamps, always use the 
KSCONVERT_PERFORMANCE_TIME macro provided in ks.h.  Many of the 
beta drivers used an incorrect implementation of this calculation 
which ignored the remainder in the HighPart of the LARGE_INTEGER.