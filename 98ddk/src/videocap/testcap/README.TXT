Readme.txt for TESTCAP


Description:
------------

   Testcap is a sample WDM stream class video capture driver which does 
not require any hardware for operation.  It manifests itself in a DShow 
graph as four separate filters:

   - Testcap Capture
   - Testcap TvTuner
   - Testcap TVAudio
   - Testcap Crossbar

   The capture portion produces two output streams, Capture and Preview 
which produce a variety of colorbars and grayscale patterns depending on 
the state of the crossbar.

   This driver contains sample implementations for almost all of the 
property sets involved in WDM video capture.  Since Testcap doesn't 
control actual hardware, most of the property set implementations are 
useful only as prototypes which you can borrow for your own 
implementations.

   Testcap creates capture streams in either RGB24 or YUV422 color spaces 
and a large number of different image dimensions.
      
               
Installation:
-------------

   Testcap.sys is not a PnP driver, hence it requires manual installation.
                                                       
   - Start.Settings.ControlPanel.AddNewHardware
   - No, Don't search for new hardware
   - No, The device isn't in the list (of detected devices)
   - No, I want to select the hardware from a list
   - "Select the type of hardware you want to install"
       Sound, Video, and Game Controllers
   - "Have Disk"
       Browse to find "Testcap.inf"  
   - Browse to find "Testcap.sys" when requested  

Testing:
--------               

   1.  Build the simplest capture graph in GraphEdt.exe
    
   - Make sure you're using a version of DShow 5.2 or better 
      GraphEdt.exe. This is needed to support new PnP features.
   - Graph.InsertFilters
      "WDM Streaming Capture Devices"
         "Testcap Capture"
   - Render one of the output pins by right clicking on the pin
   - Graph.Play
      At this point, you should see color bars
      
                                                
   2.  Build a full capture graph in GraphEdt.exe
   
   - Stop the graph
   - Graph.InsertFilters
      "WDM Streaming Crossbar Devices"
         "Testcap Crossbar"
      "WDM Streaming TvTuner Devices"
         "Testcap TvTuner"
      "WDM Streaming TvAudio Devices"
         "Testcap TvAudio"
   - Connect up the graph by rendering the output pins on the TvTuner
   - Graph.Play
   - Right Click on TvTuner filter to change the TV channel
   - Right Click on TvAudio filter to change TV audio properties
   - Right Click on Crossbar filter to change the input selection.  
     Testcap generates different color bar patterns depending on the 
     input selected.
   - Right Click on Capture filter to change capture filter properties
   
   
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

   - Testcap does not persist property settings between reboots. It should.
   
   - Testcap demonstrates one extreme of including all four capture 
components within a single driver.  Generally, code reuse is maximized by 
splitting apart functionality as is done in the sample drivers for the 
AIW.

   - When calculating timestamps, always use the 
KSCONVERT_PERFORMANCE_TIME macro provided in ks.h.  Many of the 
beta drivers used an incorrect implementation of this calculation 
which ignored the remainder in the HighPart of the LARGE_INTEGER.