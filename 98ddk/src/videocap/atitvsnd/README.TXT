Readme.txt for ATITVSND


Description:
------------

   ATITVSND is a sample WDM stream class TVSound driver. It manifests 
itself in a DShow graph as "ATI TVSound". It controls selection of audio 
subchannels (SAP) as well as mono/stereo audio decoding.


Installation:
-------------

   ATITVSND is not a true PnP driver, since it is accessed via the I2C bus 
controlled by the graphics device for which no bus enumerator exists.

   ATITVSND exists as a child device of the display device.  It is force
installed by the following lines in \windows\inf\DxATI.inf:

[ATI_M64.AIW.AddReg]
HKR,"ENUM\Device4",pnpid,,"atitvsnd"

                                                       
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
   
   1.  Note that WebTV for Windows, as shipped in the initial release 
of Windows 98, does not support a TVAudio filter.

   2.  Note that compile time options can be used to either disable or 
enable the use of the TvAudio filter in the All-In-Wonder graph.
