This sample demonstrates basic Isochronous IO under WDM with the Intel i82930 USB controller. 

See the document for the corresponding Bulk IO sample, "BulkUsb.doc" for discussion of
basic architechtural issues,as well as Plug and Play and Power Management IRP handling,
as it is basically the same as in this sample.

The accompanying console test application for IsoUsb.sys, RwIso.exe, operates similarly to
RwBulk.exe, the console test application for BulkUsb.sys. See BulkUsb.doc.


Installing IsoUsb.sys
---------------------

1. Make an 'OEM installation diskette' or equivalent installation source directory for IsoUsb.sys by copying IsoUsb.sys and IsoUsb.inf into it. The sample IsoUsb.inf is in the DDK IsoUsb\Sys source directory. 

2. Make sure your device has been programmed with the device VID/PID in the IsoUsb.inf file. If not, edit the device VID, PID, and description text to match your test board/device.

3. Depending on the operating system you are using:

On Windows 2000:

  When you plug in the device, the "Found New Hardware Wizard" dialog box will appear with the subheading "Install Hardware Device Drivers". Hit the radio button labelled "Search for a suitable driver for your device (Recommended)" and then hit the "Next" button. The following screen has you specify your installation source floppy or directory. Do so, then hit the "Next" button. The next screen should indicate that Windows 2000 is ready to install the driver.  Near the middle of the box, you should see the full source path to IsoUsb.inf.  Hit the "Next" button. You'll briefly see a "Copying Files" message box, then once again the "Found New Hardware Wizard" box, now displaying the subheading, "Hardware Install: The hardware installation is complete." Hit the "Finish" button. You should now have a copy of IsoUsb.sys in your \System32\Drivers directory, a IsoUsb.inf in your \Winnt\Inf directory, and a newly-created IsoUsb.pnf file, which is a precompiled setup information file that Windows 2000 creates.  If the final "Add New Hardware Wizard" box indicates any error, or if the operating system issues a message that you must reboot to finish installation of this device, something has gone wrong.
  Check your INF file, install directory, or driver code, follow the instructions in the below section on simulating a 'first-time' install, and start over.
 
On Windows 98: 

  Plug in the device.  The "Add New Hardware Wizard" dialog box will appear, indicating that "This wizard searches for new drivers for: [your deviceName as programmed into your board's hardware].
  Hit the "Next" button. On the following screen, hit the radio button labelled "Search for the best driver for your device (Recommended)." Hit the "Next" button.  The next screen has you specify your installation source floppy or directory.  Do so, then hit the "Next" button.  The next screen should indicate that Windows is ready to install the driver. You should see the full source path to IsoUsb.inf under the label:  "Location of Driver".  Hit the "Next" button.  You will see the "Building Driver Information Database" message box, then, if the  installation and loading of the driver have succeeded, you will see the final wizard box  saying " Windows has finished installing the software that your new hardware device requires."  Hit the "Finish" button.  If the final "Add New Hardware Wizard" box indicates any error, or if the operating system issues a message that you must reboot to finish installation of this device, something has gone wrong. Check your INF file, install directory, or driver code, follow the below instructions on simulating a 'first-time' install, and start over.


Updating the Driver
-------------------

To install a new version of your driver after a successful initial installation, 
simply replace the binary in \System32\Drivers. If the  initial or last installation 
failed for any reason, follow the below instructions on simulating a 'first-time' install.

Simulating a 'First-Time' Install 
---------------------------------

If you want to test INF file or installation program modifications, or if your first install failed for any reason and you need to do, in effect, a new 'first-time' driver installation without reinstalling Windows 98 or Windows 2000, do the following:

1. Delete IsoUsb.inf from the \Windows\Inf or \Winnt\Inf directory. On Windows 2000,
   also delete IsoUsb.pnf from the \Winnt\Inf directory.

2. Delete IsoUsb.sys from the \System32\Drivers directory.

3. Using RegEdit on Windows 98, or RegEdt32 on Windows 2000, purge the registry of the following    driver information:

For Windows 98: 
	Delete the following registry key:
	\LocalMachine\System\Enum\USB\[the key with your device ID/PID], 
        (in the case of our sample as published, this would be:
        \LocalMachine\System\Enum\USB\VID_045E&PID_930A)

For Windows 2000:
	Delete the following registry key:
	\LocalMachine\System\CurrentControlSet\Enum\USB\[the key with your deviceId/PID]
        On Windows 2000, you must restart the system before reinstalling the driver;
	this is not neccesary on Windows 98.


IsoUsb.inf is in the Sys subdirectory.

It allows you to set registry overrides for:

	"DebugLevel" debug verbosity level, where 0 == no debug output, 
	1 == default, higher == more verbose. 

