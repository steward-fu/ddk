For more help on using the win9x debugger, see the Start Menu\Programs\
Development Kits\Windows 2000 DDK\DDK Documentation\Millennium DDK\
Windows Me Debugging and Other Tools\Windows 9x Debugging.


==================================================================
TO INSTALL THE DEBUGGER:

To install the debugger, run dbginst.exe from %NTDDK%\bin\win_me\fre.


==================================================================
TO INSTALL THE DEBUGGER FOR USE WITH DEBUG BINARIES:

NOTE:  Before running dbginst.exe to install the debugger for 
       use with debug binaries, the debugger.inf file must be modified.

To install the debugger with debug binaries and symbols,
run dbginst.exe from %NTDDK%\bin\win_me\chk.  BEFORE
running dbginst.exe, make sure you modify the debugger.inf
in %NTDDK%\bin\win_me\chk directory.  See the section
"Modifying the Debugger Installs" below.

The debugger installer for the chk directory installs a
certified catalog that contains updated file signatures for the
new binaries it installs.  For this reason, you can install
and uninstall the debugger without overwrites from System File
Protection, which monitors unauthorized modifications to system
files.  For more information on SFP, see the section
"SFP Information" below.


==================================================================
MODIFYING THE DEBUGGER INSTALLS

By default, the dbginst.exe in the %ntddk%\bin\win_me\chk directory
does not install any debug binaries.  To install a debug binary
and its symbol file, you must first modify the debugger.inf in the
%ntddk%\bin\win_me\chk directory.

To add a debug binary:
First, find the binary you wish to replace in debugger.inf.  It will
be listed in a [DBG.DRV], [DBG.SYS], [DBG.VXD], or [DBG.IOS] section.
Remove the semi-colon before the binary to uncomment out.

Second, find the line ";%debug_file%=1" in the [SourceDisksFiles] section.
Remove the semi-colon to uncomment out.

Note: Do not modify the below sections:
dbg.tool
dbg.wb
ret.wb
ret.drv
ret.sys
ret.ios

To install a symbol file:
First, find the symbol file you wish to add in the [DBG.SYM] section
of the debugger.inf. Remove the semi-colon before the symbol file to
uncomment out.

Second, find the line ";%symbol%.sym=1" in the [SourceDisksFiles] section.
Remove the semi-colon to uncomment out.

To load a symbol file:
Find the symbol file you wish to load in the [systemini.Inis]
section.  Remove the semi-colon before the system.ini line to
uncomment out.  Repeat for the [debugvxd.inis.remove] section.


==================================================================
TO UNINSTALL THE DEBUGGER:

To uninstall the debugger, run dbginst.exe from the directory
which you first ran dbginst.exe from, either from the
%NTDDK%\bin\win_me\fre or %NTDDK%\bin\win_me\chk directory.

NOTE: The dbginst.exe unintaller from the %NTDDK%\bin\win_me\chk
directory will return binaries on your retail system to retail
versions.


==================================================================
TO LOAD ADDITIONAL SYMBOL FILES AFTER INSTALLING THE DEBUGGER:

1. Edit the system.ini file and uncomment out the below line
   in the system.ini file for the symbol you wish to load:

   system.ini,386Enh,,"debugsym=%25%\%SymbolName%.sym,1

2. Caution: Keep in mind that each symbol file that loads consumes
            additional system resources.

3. Reboot


==================================================================
TO PREVENT A SYMBOL FILE FROM LOADING:

1. Edit the system.ini file and comment out the below line
   in the system.ini file for the symbol you wish to remove:

   ;system.ini,386Enh,,"debugsym=%25%\%SymbolName%.sym,1

2. Reboot


==================================================================
SFP INFORMATION:

For more information about SFP, see either %NTDDK%\SFP.HTM,
or check the online help for Millennium DDK\Installing Drivers
on Windows Millennium\SYSTEM FILE PROTECTION.  In this document,
a list of protected files can be found in the "Protected File
List" section.

