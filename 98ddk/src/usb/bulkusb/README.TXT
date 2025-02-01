Readme.txt for BulkUsb


Installing the Driver
---------------------

1. Make an OEM installation diskette or equivalent installation source directory for BulkUsb.sys by copying BulkUsb.sys and BulkUsb.inf into it. The sample BulkUsb.inf is in the DDK BulkUsb\Sys source directory. 

2. Make sure your device has been programmed with the device VID/PID in the Bulkusb.inf file. If not, edit the device VID, PID, and description text to match your test board/device.

3. Depending on the operating system you are using:

On Windows(R) 2000:

  When you plug in the device, the "Found New Hardware Wizard" dialog box will appear with the subheading "Install Hardware Device Drivers." Select the radio button labeled "Search for a suitable driver for your device (Recommended)," and then click the Next button.
  In the next screen, specify your installation source floppy or directory, and then click the Next button. The next screen should indicate that Windows 2000 is ready to install the driver.  Near the middle of the box, you should see the full source path to BulkUsb.inf.  Click the Next button. You'll briefly see a "Copying files" message box, then once again the "Found New Hardware Wizard" box, displaying the subheading, "Hardware Install: The hardware installation is complete." Click the Finish button.
  You should now have a copy of BulkUsb.sys in your \System32\Drivers directory, a BulkUsb.inf in your \Winnt\Inf directory, and a newly created BulkUsb.pnf file, which is a precompiled setup info file that Windows 2000 creates.  If the final "Add New Hardware Wizard" box indicates any error or the OS says you must reboot to finish installation of this device, something has gone wrong.
  Check your .inf file, Install directory, or driver code, follow the instructions in the "Simulating a First-Time Install" section later in this document, and start over.
 
On Windows(R) 98: 

  Plug in the device.  The "Add New Hardware Wizard" dialog box will appear, stating, "This wizard searches for new drivers for: [your deviceName as programmed into your board's hardware]."
  Click the Next button. On the next screen, hit the radio button labeled "Search for the best driver for your device (Recommended)." Click the Next button.  In the next screen, specify your installation source floppy or directory.  Then click the Next button.  The next screen should indicate that Windows is ready to install the driver. You should see the full source path to BulkUsb.inf under the label "Location of Driver."  Click the Next button.  You will see the "Building Driver Information Database" message box. If the installation and loading of the driver have succeeded, you will see the final wizard box that states, "Windows has finished installing the software that your new hardware device requires."  Click the Finish button.  If the final "Add New Hardware Wizard" box indicates any error or the OS says you must reboot to finish installation of this device, something has gone wrong. Check your .inf file, Install directory, or driver code, follow the instructions in the "Simulating a First-time Install" section later in this document, and start over.


Updating the Driver
-------------------

To install a new version of your driver after a successful initial installation, simply replace the binary in \System32\Drivers. If the initial or last installation failed for any reason, follow the instructions below on simulating a first-time install.


Simulating a First-Time Install 
---------------------------------

If you want to test the .inf file or installation program modifications, or if your first install failed and you need to do a "first-time" driver installation without reinstalling Windows 98 or Windows 2000, do the following:

1. Delete BulkUsb.inf from the \Windows\Inf or \Winnt\Inf directory. On Windows 2000,
   also delete BulkUsb.pnf from the \Winnt\Inf directory.

2. Delete BulkUsb.sys from the \System32\Drivers directory.

3. Using RegEdit on Windows 98, or RegEdt32 on Windows 2000, purge the registry
   of the following driver information:

For Windows 98: 

	Delete the following registry key:
	\LocalMachine\System\Enum\USB\[the key with your device ID/PID] 
        In the case of our sample as published, this would be
        \LocalMachine\System\Enum\USB\VID_045E&PID_930A

For Windows 2000:

	Delete the following registry key:
	\LocalMachine\System\CurrentControlSet\Enum\USB\[the key with your deviceId/PID]
	On Windows 2000, you must restart the system before reinstalling the driver. 
	This is not necessary on Windows 98.


BulkUsb.inf is in the Sys subdirectory.

It allows you to set registry overrides for:

1. DebugLevel: Debug verbosity level, where 0 == no debug output, 1 == default, higher == more verbose. 

2. MaximumTransferSize: The chunk size (default 4k) that large IO requests are broken into by the driver.








