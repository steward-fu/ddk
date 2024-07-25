# Microsoft Developer Studio Project File - Name="DAYTIME1" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=DAYTIME1 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DAYTIME1.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DAYTIME1.MAK" CFG="DAYTIME1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DAYTIME1 - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "DAYTIME1 - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0

!IF  "$(CFG)" == "DAYTIME1 - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f DAYTIME.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "DAYTIME.exe"
# PROP BASE Bsc_Name "DAYTIME.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f DAYTIME.mak DEBUG=0"
# PROP Rebuild_Opt "/a"
# PROP Target_File "DAYTIME1.EXE"
# PROP Bsc_Name "DAYTIME1.BSC"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "DAYTIME1 - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f DAYTIME.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "DAYTIME.exe"
# PROP BASE Bsc_Name "DAYTIME.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "NMAKE /f DAYTIME.mak DEBUG=1"
# PROP Rebuild_Opt "/a"
# PROP Target_File "DAYTIME1.EXE"
# PROP Bsc_Name "DAYTIME1.BSC"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "DAYTIME1 - Win32 Release"
# Name "DAYTIME1 - Win32 Debug"

!IF  "$(CFG)" == "DAYTIME1 - Win32 Release"

!ELSEIF  "$(CFG)" == "DAYTIME1 - Win32 Debug"

!ENDIF 

# Begin Source File

SOURCE=.\DAYTIME.cpp
# End Source File
# Begin Source File

SOURCE=.\DAYTIME.h
# End Source File
# Begin Source File

SOURCE=.\DAYTIME.mak
# End Source File
# End Target
# End Project
