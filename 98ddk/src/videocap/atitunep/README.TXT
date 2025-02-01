Readme.txt for ATITUNEP


Description:
------------

   ATITUNEP is a sample WDM stream class TVTuner driver. It manifests 
itself in a DShow graph as "ATI TVTuner". It controls the following tuner 
hardware (and any other hardware/software compatible devices):

FR1236 - Philips FR1200 series TV and FM tuner family, including:
         FI1216         - CCIR B, G System      (Western Europe)
         FI1216J        - RTMA M+N System       (Japan)
         FI1236         - RTMA M+N System       (USA)
         FI1246         - CCIR I System         (UK, Ireland, SA, H.K.)
         FI1256         - OIRT System           (China, Russia, E. Europe)
         FI1216         - CCIR B,G, L, L1 System (W. Europe, SE Asia, OZ, NZ)
         FR1236MK2      - RTMN M+N System + FM  (USA)


Installation:
-------------

   ATITUNEP is not a true PnP driver, since it is accessed via the I2C bus 
controlled by the graphics device for which no bus enumerator exists.

   ATITUNEP exists as a child device of the display device.  It is force
installed by the following lines in \windows\inf\DxATI.inf:

[ATI_M64.AIW.AddReg]
HKR,"ENUM\Device1",pnpid,,"atitunep"

                                                       
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
   
