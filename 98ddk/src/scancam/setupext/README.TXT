Readme.txt for SetupExt


Description
-----------

   SetupExt is an example of a setup extension.  Setup extensions are more 
   fully explained in the documentation, but they are basically used to 
   extend the default Windows(R) 98 setup process.
   
   This particular example fixes a small problem in the StillImage class 
   installer where Plug and Play parallel scanners are not installed 
   correctly. This problem was discovered too late to fix in Windows 98 
   Gold, but will be fixed in later releases.
   
   The problem is that the StillImage class installer does not store the 
   port name that the user-mode minidriver normally uses to communicate 
   with the device. To solve this problem, this example setup extension 
   looks in a particular place in the registry for the port name and 
   then stores it correctly in the STI section of the registry.
   
   Even though this extension is written to solve a particular problem, the 
   same general format can be used for any setup extension.
   
   
Installation
------------

   The .inf in this directory needs to be changed as commented in the file.  
   Then a Plug and Play scanner should be plugged in.  Unfortunately, not 
   many of the parallel scanners on the market are Plug and Play.  
