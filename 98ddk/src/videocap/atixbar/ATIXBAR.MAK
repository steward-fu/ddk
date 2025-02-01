# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

!IF "$(CFG)" == ""
CFG=ATIXBar - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ATIXBar - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ATIXBar - Win32 Release" && "$(CFG)" !=\
 "ATIXBar - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ATIXBar.mak" CFG="ATIXBar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ATIXBar - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "ATIXBar - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "ATIXBar - Win32 Debug"

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f Makefile"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Makefile.exe"
# PROP BASE Bsc_Name "Makefile.bsc"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /f Makefile"
# PROP Rebuild_Opt "/a"
# PROP Target_File "ATIXBar.exe"
# PROP Bsc_Name "ATIXBar.bsc"
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f Makefile"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Makefile.exe"
# PROP BASE Bsc_Name "Makefile.bsc"
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /f Makefile"
# PROP Rebuild_Opt "/a"
# PROP Target_File "ATIXBar.exe"
# PROP Bsc_Name "ATIXBar.bsc"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ENDIF 

################################################################################
# Begin Target

# Name "ATIXBar - Win32 Release"
# Name "ATIXBar - Win32 Debug"

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

".\ATIXBar.exe" : 
   CD D:\Redrum\wdm10\capture\mini\ATIXBar
   NMAKE /f Makefile

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

".\ATIXBar.exe" : 
   CD D:\Redrum\wdm10\capture\mini\ATIXBar
   NMAKE /f Makefile

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Makefile

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ATIXBAR.cpp

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ATIXBar.h

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WDMXBar.h

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WDMDebug.h

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WDMXBar.cpp

!IF  "$(CFG)" == "ATIXBar - Win32 Release"

!ELSEIF  "$(CFG)" == "ATIXBar - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
