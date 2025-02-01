
MONITOR.DLL: A sample USB monitor control panel application


COMPONENTS
==========

monitor.c       Main dialog box procedure
pnp.c           Routines for finding HID monitors 
map.c           DLL entry routine
mapcf.c         IClassFactory interface
mappsx.c        Add pages routine
mem.c           Memory management
common.c        Shared components
assert.c        Assertion routines
hid.h           Data structures for a HID device
map.h           Main private header file
monitor.h       Contains USB monitor control class function information
monitor.rc      Resource file for dialog box GUI


INSTALLATION
============

To install the application, copy monitor.inf and monitor.dll to the
same location.  Using Windows(R) Explorer, right-click on monitor.inf
and select Install.  This will copy the files to the proper location
and make the necessary registry modifications.

To access the Control Panel, right-click on the desktop and then select
Properties.  Select the Settings tab and click the Advanced button.
You will see an MS-USB Monitor tab.

To uninstall, open the Control Panel and select Add/Remove Programs.  There
will be an option for MS-USB Monitor.

