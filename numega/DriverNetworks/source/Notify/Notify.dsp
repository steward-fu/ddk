# Microsoft Developer Studio Project File - Name="Notify" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Notify - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "Notify.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Notify.mak" CFG="Notify - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Notify - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Notify - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Notify - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\Include\Notify" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(BASEDIR)\inc\atl30" /I "$(DDK_INC_PATH)" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\i386\Checked\Notify.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Notify - Win32 Release"

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
# ADD BASE CPP /nologo /Gz /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\Include\Notify" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(BASEDIR)\inc\atl30" /I "$(DDK_INC_PATH)" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\i386\Free\Notify.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ENDIF

# Begin Target

# Name "Notify - Win32 Debug"
# Name "Notify - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BindingPolicy.cpp
# End Source File
# Begin Source File

SOURCE=.\NetCfgComponent.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\Notify\BindingPolicy.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Notify\NetCfg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Notify\NetCfgBindingInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Notify\NetCfgBindingPath.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Notify\NetCfgComponent.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Notify\Notify.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Notify\NotifyObject.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Notify\PropertyUi.h
# End Source File
# End Group
# End Target
# End Project
