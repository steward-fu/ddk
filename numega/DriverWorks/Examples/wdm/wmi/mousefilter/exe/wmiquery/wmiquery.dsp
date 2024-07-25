# Microsoft Developer Studio Project File - Name="wmiquery" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=wmiquery - Win32 IA64 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WmiQuery.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WmiQuery.mak" CFG="wmiquery - Win32 IA64 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wmiquery - Win32 IA64 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "wmiquery - Win32 IA64 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wmiquery - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wmiquery - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wmiquery - Win32 IA64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wmiquery___Win32_IA64_Debug"
# PROP BASE Intermediate_Dir "wmiquery___Win32_IA64_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D _WIN32_WINNT=0x400 /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN64" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x501 /D "WIN32_LEAN_AND_MEAN" /YX /Fd".\objchk\ia64\wmiquery.pdb" /Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wbemuuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wbemuuid.lib setupapi.lib kernel32.lib libc.lib ole32.lib oleaut32.lib uuid.lib oldnames.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib /out:"objchk/ia64/wmiquery.exe" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64 /ignore:4099
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wmiquery - Win32 IA64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wmiquery___Win32_IA64_Release"
# PROP BASE Intermediate_Dir "wmiquery___Win32_IA64_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D _WIN32_WINNT=0x400 /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /ML /W3 /WX /GX /O2 /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /D "WIN64" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x501 /D "WIN32_LEAN_AND_MEAN" /YX -Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wbemuuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wbemuuid.lib setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /incremental:no /machine:IX86 /out:"objfre/ia64/wmiquery.exe" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wmiquery - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp8 /W3 /GX /O2 /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN32" /D _WIN32_WINNT=0x400 /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /Yu"stdafx.h" /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /pdbtype:sept
# ADD LINK32 libcd.lib advapi32.lib kernel32.lib ntdll.lib oleaut32.lib ole32.lib wbemuuid.lib uuid.lib setupapi.lib oldnames.lib /nologo /subsystem:console /machine:I386 /nodefaultlib /libpath:"$(TARGET_LIB_PATH)"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wmiquery - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN32" /D _WIN32_WINNT=0x400 /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wbemuuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /libpath:"$(TARGET_LIB_PATH)"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "wmiquery - Win32 IA64 Debug"
# Name "wmiquery - Win32 IA64 Release"
# Name "wmiquery - Win32 Release"
# Name "wmiquery - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "wmiquery - Win32 IA64 Debug"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "wmiquery - Win32 IA64 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "wmiquery - Win32 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "wmiquery - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wmiquery.cpp

!IF  "$(CFG)" == "wmiquery - Win32 IA64 Debug"

!ELSEIF  "$(CFG)" == "wmiquery - Win32 IA64 Release"

!ELSEIF  "$(CFG)" == "wmiquery - Win32 Release"

!ELSEIF  "$(CFG)" == "wmiquery - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
