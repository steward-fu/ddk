# Microsoft Developer Studio Project File - Name="vdev1394con" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vdev1394con - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vdev1394con.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdev1394con.mak" CFG="vdev1394con - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdev1394con - Win32 IA64 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vdev1394con - Win32 IA64 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "vdev1394con - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vdev1394con - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vdev1394con - Win32 IA64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objfre/ia64"
# PROP BASE Intermediate_Dir "release64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objfre/ia64"
# PROP Intermediate_Dir "release64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /ML /W3 /WX /GX /O2 /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN64" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /D "WIN32_LEAN_AND_MEAN" /YX"pch.h" -Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /incremental:no /machine:IX86 /libpath:"$(TARGET_LIB_PATH)" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 IA64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objchk/ia64"
# PROP BASE Intermediate_Dir "debug64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk/ia64"
# PROP Intermediate_Dir "debug64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN64" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /D "WIN32_LEAN_AND_MEAN" /YX"pch.h" /Fd".\objchk\ia64\vdev1394con.pdb" /Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib libc.lib advapi32.lib kernel32.lib libci.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib /libpath:"$(TARGET_LIB_PATH)" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objfre\i386"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre\i386"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /D "WIN32_LEAN_AND_MEAN" /YX"pch.h" /Fd"objfre/i386/vdev1394con.pdb" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386 /libpath:"$(TARGET_LIB_PATH)" /ignore:4078

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objchk\i386"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk\i386"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /D "WIN32_LEAN_AND_MEAN" /YX"pch.h" /Fd"objchk\i386/vdev1394con.pdb" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386 /libpath:"$(TARGET_LIB_PATH)" /IGNORE:4099

!ENDIF 

# Begin Target

# Name "vdev1394con - Win32 IA64 Release"
# Name "vdev1394con - Win32 IA64 Debug"
# Name "vdev1394con - Win32 Release"
# Name "vdev1394con - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\CBus1394.h
# End Source File
# Begin Source File

SOURCE=.\pch.h
# End Source File
# Begin Source File

SOURCE=..\VDev1394DeviceInterface.h
# End Source File
# Begin Source File

SOURCE=..\vdev1394ioctl.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\CBus1394.cpp

!IF  "$(CFG)" == "vdev1394con - Win32 IA64 Release"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 IA64 Debug"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Release"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenByIntf.cpp

!IF  "$(CFG)" == "vdev1394con - Win32 IA64 Release"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 IA64 Debug"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Release"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Debug"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vdev1394con.cpp

!IF  "$(CFG)" == "vdev1394con - Win32 IA64 Release"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 IA64 Debug"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Release"

!ELSEIF  "$(CFG)" == "vdev1394con - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\makefile
# End Source File
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project
