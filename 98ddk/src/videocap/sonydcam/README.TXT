Readme.txt for SONYDCAM


Description:
------------

   Sonydcam is a sample WDM stream class video capture driver which supports
two IEEE 1394 digital conferencing cameras:

   - Sony 1394 CCM-DS250 Desktop Camera, and
   - TI 1394 MC680-DCC Desktop Camera

   It manifests itself in a DShow graph as a "WDM Streaming Capture Device."  
The capture portion produces one output capture stream supporting image sizes
of 320x240 with a UVUY color space. 
      
               
Installation:
-------------

   Sonydcam is a PnP driver, the image.inf shipped with Windows OS is 
used to install detected devices.   It cannot be manually installed.
                                                       

Testing:
--------               

   1.  Build the simplest capture graph in GraphEdt.exe
    
   - Make sure you're using a version of DShow 5.2 or better 
      GraphEdt.exe. This is needed to support new PnP features.
   - Graph.InsertFilters
      "WDM Streaming Capture Devices"
         "Sony 1394 CCM-D250 Desktop Camera"
   - Render its output pins by right clicking on the pin
   - Graph.Play
      At this point, you should see capture image
                                                     
   2.  Use AMCap.exe.  This capture application is provided in the DShow 
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

   - Sonydcam does not persist property settings between reboots. It should.

   - When calculating timestamps, always use the KSCONVERT_PERFORMANCE_TIME 
macro provided in ks.h.  Many of the beta drivers used an incorrect implementation 
of this calculation which ignored the remainder in the HighPart of the LARGE_INTEGER.

   - Sonydcam does not keep track of its dropped frame; however, it does keep
track of the frames actually capture.  Dropped frame can be calculated using
frame rate, capture start timestamp, current timestamp, and capture frame count.

   - Sonydcam processes 1st PnP bus reset and ignore other reset requests for 
a fix amount of time.  These other requests might be caused due to bus noise.  It does 
keep track of the correct generation count.  After the bus reset is settled, it queries 
its id to determine if a device is being plugged or unplugged.  If the device is plugged
in, it will restore to its previous state.

