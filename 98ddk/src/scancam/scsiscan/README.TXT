Readme.txt for SCSISCAN


Description
-----------

   SCSISCAN is a scanner SCSI bus driver.  It is provided mainly for 
   documentation on how to write a SCSI driver.  
      

Installation
------------

   Install a SCSI scanner such as a Umax 600S, 610S, 1200S, or any SCSI Hewlett-Packard 
   scanner.  After all of the files have been copied, copy your version of 
   scsiscan.sys over the version in \windows\system32\drivers.  
   Reboot.  
   
   Although a copy of hpscan.inf is included in this directory, it is not 
   needed.  It is installed in \windows\inf as a part of Windows(R) 98.   
   

Testing
-------
   
   Launch Imaging for Windows(R) or another imaging application and use a 
   TWAIN data source to access the scanner.  The TWAIN data source will 
   talk through scsiscan.sys to communicate with the scanner.  
   
