Readme.txt for HPSJUSD


Description
-----------
   
   HPSJUSD is an actual user-mode minidriver for Hewlett-Packard scanners.  
   You will need an HP scanner to use this sample, which you can use for 
   guidance in writing your own user-mode minidriver.  You can also use 
   the debugger to watch how the minidriver works.  
      

Installation
------------

   Although a copy of hpscan.inf is included in this directory, it is not 
   needed.  It is installed in \windows\inf as a part of Windows(R) 98.  
   
   After building HPSJUSD, install an HP scanner on your system.  For the 
   SCSI scanners, simply shut down your machine, plug the scanner into a 
   SCSI adapter, and reboot.  The scanner will be detected by Plug and 
   Play during boot.  
    
   After all the files are installed, copy the version of HPSJUSD.DLL 
   you built over the version in \windows\system (save a copy of the 
   original).  Reboot.  
   
   
Testing
-------
   
   Launch Imaging for Windows(R) or another imaging application and use an 
   HP TWAIN data source to access the scanner.  The TWAIN data source will 
   call your version of HPSJUSD to communicate with the scanner.  
   
