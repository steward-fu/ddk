Readme.txt for BT829


Description:
------------

   BT829 is a sample WDM stream class Capture driver which supports the 
BT829 and BT829A video digitizer chips from Brooktree. It manifests
itself in a DShow graph as two filters, "BT829 Capture", and "BT829 
Crossbar".  

   This is a fairly complex driver, in that it supports an input stream 
representing analog video, two capture streams for video and VBI data, and 
two output connection points for DDraw surface allocation filters (Overlay 
Mixer", and "VBI Surface Allocator".  It also supports input selection via 
"BT829 Crossbar".

   The actual capture operation is implemented by an interface to Kernel 
DirectDraw.  Capture buffers are passed to KDDraw, where they are filled 
by a bus master operation.

Installation:
-------------

   BT829 is not a true PnP driver, since it is accessed via the I2C
bus controlled by the graphics device for which no bus enumerator exists.

   BT829 exists as a child device of the display device.  It is force
installed by the following lines in \windows\inf\DxATI.inf:

[ATI_M64.AIW.AddReg]
HKR,"ENUM\Device2",pnpid,,"bt829pro"

                                                       
Testing:
--------               

   1.  WebTV for Windows
   
   - Install an ATI All-In-Wonder or All-In-Wonder Pro adapter.
   - Since subvendor IDs are not available in the Rage3 to distinguish 
     different hardware configurations, you must manually select the 
     appropriate driver to install:
      All-In-Wonder (ati_m64)   
      All-In-Wonder PRO (atir3)
   - Install WebTV for Windows
      StartMenu.Settings.ControlPanel.Add/Remove Programs.WindowsSetup.
      WebTV for Windows
   - Click the TV icon in the tray, and watch TV
   
      
   2.  GraphEdt.exe
   
   - Make sure you're using a version of DShow 5.2 or better 
      GraphEdt.exe. This is needed to support new PnP features.
   - Graph.InsertFilters
      "WDM Streaming Capture Devices"
         "BT829 Capture"
      "WDM Streaming Crossbar Devices"
         "ATI Crossbar"
         "BT829 Crossbar"
      "WDM Streaming TvTuner Devices"
         "ATI TvTuner"
      "Direct Show filters"
         "Overlay Mixer"
   - Connect up the graph by rendering the output pins on the TvTuner
   - Graph.Play
   - Right Click on TvTuner filter to change the TV channel
   - Right Click on Crossbar filter to change the input selection.  
   - Right Click on Capture filter to change capture filter properties
   
   
   3.  AMCap.exe
   
   This capture application is provided in the DShow SDK, and does all of 
   the graph building automatically for both WDM and VFW capture filters.


Other stuff:
------------

   - We have identified the code blocks in this driver which are specific 
     to the ATI All-In-Wonder card as follows:
   
   // =========================================================
   // This block is specific to the ATI All-In-Wonder card. 
        CATIHwConfiguration CATIConfig( pHwDevExt->pI2cScript);
   // End of ATI All-In-Wonder specific code.
   // ========================================================= 
   
   - Prior to capturing on either the "Capture" or "VBI" pin, a connection 
     must be made to the corresponding surface allocator.  For example, to use 
     the "Capture" output pin, you must first connect the "VP" pin to 
     the Overlay Mixer filter.

   - To test VBI capture operation, use VBIScope.ax.  This DShow filter 
     gives a graphical display of any two selected VBI lines and provides 
     precise timing information.
   
   
