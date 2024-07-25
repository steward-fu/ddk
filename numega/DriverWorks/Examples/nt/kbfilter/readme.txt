****************NT KBFilter example Readme*******************

The NT keyboard sample illustrates the use of KFilterDevice in a driver.  Note that this
  method of filtering is for NT 4 only.  KWdmFilterDevice should be used for WDM filter 
  drivers.

The driver works by intercepting read IRPs going to the keyboard driver and putting a 
  completion routine on them.  When the IRP completes, the filter driver is able to see
  the scancode data in the IRP.

This driver has two settings:
-------------------------------------------------------------------------------------------
 1) If the driver is loaded manually, it will hook the keyboard driver only when 
   the acompanying application is open. The driver may be unloaded in this setting,
   but it will always miss the first scancode.
 2) If the driver is loaded at boot time, Start = 1 Group = "Keyboard Class", it will hook 
   the keyboard driver when it loads.  No scancodes will be missed, but the filter cannot 
   be unloaded.


The reason for the missing scancode when loaded manually:
----------------------------------------------------------------------------------------
   	 Windows constantly sends read IRPs to the keyboard driver to poll for key strokes.
At the time KbFilter sets up a hook, it cannot set a completion routine on the IRP currently
in progress.  It can only set completion routines if new IRPs come in.  The next scancode
will complete the IRP that was already in progress and will not be hooked by the driver.
	When loaded at boot time, Windows has not yet sent a read IRP to the keyboard driver
so no keystroke is missed.

The reason the driver cannot be unloaded when loaded at boot time:
----------------------------------------------------------------------------------------
  	As long as a completion routine is set on an IRP in progress, the driver cannot 
unload. As there is always an IRP in progress, the driver cannot unload unless it first
stops hooking IRPs, and the currently hooked IRP completes.  In manual mode, the hooking 
stops when the application unloads, and the next keystroke will complete the last hooked IRP. 
	When the driver loads in boot mode, it never unhooks, so there is always an IRP
in progress with the completion routine set.  Therefore, the driver cannot unload in this
situation. 