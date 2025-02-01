Readme.txt for SampUSD


Description
-----------
                      
   SampUSD is a sample STI user-mode minidriver.  The main purpose of this 
   sample is show how to implement events in the minidriver.  Events are 
   hardware-initiated, usually caused by the user pushing a button on the 
   device.  Rather than require some piece of hardware, this sample uses a 
   test file to simulate hardware events.  When the test file changes or 
   changes size, the minidriver is fooled into thinking that a hardware 
   event occurred.  
   
   This user-mode minidriver sample works in conjunction with a sample 
   control panel found in the SAMPCPL directory. To run this sample, both
   parts need to be built.

   
Installation
------------

   After compiling, put the .dll and .inf files in a temporary directory 
   together along with the .cpl file built from the SAMPCPL directory of 
   the DDK.  To install it, use the Add New Hardware Wizard control panel 
   or the Add button in the Scanners and Cameras control panel.  If you 
   use the wizard, it will ask you at one point if you want to search for 
   your new hardware.  Answer No, then select Imaging Device from the next 
   dialog.  Use the Have Disk button to point to the temporary directory 
   where the files are located.  The wizard will ask if the device is 
   attached to COM1 or COM2.  It doesn't matter which one is chosen, as 
   the choice will be changed later.  Open the Sample Device in the 
   Scanners and Cameras control panel.  On the Sample USD tab, type in the 
   name of a test file.  Create the test file if it does not already 
   exist.  
   
   
Testing
-------
   
   Once the sample is installed, changing the test file's size will cause 
   one event.  Altering the file without changing its size will cause a 
   different event.  To link these events to different applications, 
   use the Event tab in the Scanners and Cameras control panel. Try 
   causing both of these events and watch the applications launch.
