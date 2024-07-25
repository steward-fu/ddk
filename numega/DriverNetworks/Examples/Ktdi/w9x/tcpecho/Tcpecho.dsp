# Microsoft Developer Studio Project File - Name="tcpecho" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=tcpecho - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tcpecho.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tcpecho.mak" CFG="tcpecho - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcpecho - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "tcpecho - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0

!IF  "$(CFG)" == "tcpecho - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f tcpecho.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tcpecho.exe"
# PROP BASE Bsc_Name "tcpecho.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f tcpecho.mak DEBUG=0"
# PROP Rebuild_Opt "/a"
# PROP Target_File "tcpecho.exe"
# PROP Bsc_Name "tcpecho.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "tcpecho - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f tcpecho.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "tcpecho.exe"
# PROP BASE Bsc_Name "tcpecho.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "NMAKE /f tcpecho.mak DEBUG=1"
# PROP Rebuild_Opt "/a"
# PROP Target_File "tcpecho.exe"
# PROP Bsc_Name "tcpecho.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "tcpecho - Win32 Release"
# Name "tcpecho - Win32 Debug"

!IF  "$(CFG)" == "tcpecho - Win32 Release"

!ELSEIF  "$(CFG)" == "tcpecho - Win32 Debug"

!ENDIF 

# Begin Source File

SOURCE=.\tcpecho.cpp
# End Source File
# Begin Source File

SOURCE=.\tcpecho.h
# End Source File
# Begin Source File

SOURCE=.\tcpecho.mak
# End Source File
# End Target
# End Project
