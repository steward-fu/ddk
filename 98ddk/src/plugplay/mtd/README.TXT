Readme.txt for SRAM MTD sample


Description:
------------

   SRAMMTD.VXD is the MTD driver shipped in Windows 98 for SRAM PCMCIA cards.  
               
Installation:
-------------

   The SRAM MTD sample includes an INF file to install the sample driver.
   To install the sample SRAM MTD driver using this INF (for a standard
   PCMCIA SRAM card), the MTD INF file shipped in Windows 98
   (\windows\inf\mtd.inf) must be either moved or renamed.  This will
   prevent Windows 98 from installing the default SRAM MTD driver when an
   SRAM card is first inserted.  This assumes that an SRAM card has not
   previously been used with the machine.

   Once the MTD has been installed, edit CONFIG.SYS and add the following
   two lines:

      device=c:\windows\system\csmapper.sys
      device=c:\windows\system\carddrv.exe /slot=XX

   Note: XX should be the number of PCMCIA slots in your machine.

   You must reboot the machine after editing CONFIG.SYS. 
