# Microsoft Developer Studio Project File - Name="OID Handlers" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=OID Handlers - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "_OID Handlers.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "_OID Handlers.mak" CFG="OID Handlers - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OID Handlers - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "_OID Handlers"
# PROP Scc_LocalPath "."
MTL=midl.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# Begin Target

# Name "OID Handlers - Win32 Release"
# Begin Source File

SOURCE=.\CoIsdnOidBrowser.h
# PROP Ignore_Default_Tool 1
# Begin Custom Build - *** This configuration exists only to allow browsing of OID handlers.
InputPath=.\CoIsdnOidBrowser.h

"null" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo *** It is not meant to be compiled, please choose a Win32 NDIS 4/5 Configuration in Build menu to build your driver.

# End Custom Build
# End Source File
# Begin Source File

SOURCE=.\CoIsdnOids.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
