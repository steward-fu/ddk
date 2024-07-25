# Microsoft Developer Studio Project File - Name="vclassinstaller" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=vclassinstaller - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vclsinst.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vclsinst.mak" CFG="vclassinstaller - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vclassinstaller - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vclassinstaller - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vclassinstaller - Win32 IA64 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vclassinstaller - Win32 IA64 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vclassinstaller - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x400 /D "WIN32_LEAN_AND_MEAN" /FR /YX /Fd"objchk\i386\vbusclass.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /debug /machine:I386
# ADD LINK32 winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib setupapi.lib /nologo /dll /map /debug /machine:I386 /out:"objchk\i386/vbusclass.dll" /libpath:"$(TARGET_LIB_PATH)" /IGNORE:4099

!ELSEIF  "$(CFG)" == "vclassinstaller - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x400 /D "WIN32_LEAN_AND_MEAN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib setupapi.lib /nologo /dll /debug /machine:I386 /out:"objfre\i386/vbusclass.dll" /libpath:"$(TARGET_LIB_PATH)" /ignore:4089 /ignore:4078

!ELSEIF  "$(CFG)" == "vclassinstaller - Win32 IA64 Debug"

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
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "_DEBUG" /D "WIN64" /D "WIN32_LEAN_AND_MEAN" /YX /Wp64 /c
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IA64
# ADD LINK32 libcd.lib version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib setupapi.lib /nologo /dll /debug /nodefaultlib /out:"objchk/ia64\vbusclass.dll" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64 /ignore:4078

!ELSEIF  "$(CFG)" == "vclassinstaller - Win32 IA64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objfre/ia64"
# PROP BASE Intermediate_Dir "Release64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre/ia64"
# PROP Intermediate_Dir "Release64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "NDEBUG" /D "WIN64" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINNT=0x400 /D "WIN32_LEAN_AND_MEAN" /YX /Wp64 /c
# SUBTRACT CPP /u
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /out:"objfre/ia64/vclassinstallerxp.dll" /machine:IA64 /ignore:4089
# ADD LINK32 version.lib comctl32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib setupapi.lib /nologo /dll /debug /machine:IA64 /out:"objfre/ia64/vbusclass.dll" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64 /ignore:4089

!ENDIF 

# Begin Target

# Name "vclassinstaller - Win32 Debug"
# Name "vclassinstaller - Win32 Release"
# Name "vclassinstaller - Win32 IA64 Debug"
# Name "vclassinstaller - Win32 IA64 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\openintf.cpp
	
!IF  "$(CFG)" == "vclassinstaller - Win32 Debug"

# ADD CPP /I "$(DRIVERWORKS)\include"

!ELSEIF  "$(CFG)" == "vclassinstaller - Win32 Release"

# ADD CPP /I "$(DRIVERWORKS)\include"

!ELSEIF  "$(CFG)" == "vclassinstaller - Win32 IA64 Debug"

# ADD CPP /I "$(DRIVERWORKS)\include"

!ELSEIF  "$(CFG)" == "vclassinstaller - Win32 IA64 Release"

# ADD CPP /I "$(DRIVERWORKS)\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vbusclass.def
# End Source File
# Begin Source File

SOURCE=.\vclsinst.cpp
	
# End Source File
# Begin Source File

SOURCE=.\vclsinst.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\vbusioctl.h
# End Source File
# Begin Source File

SOURCE=..\vinterface.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\vbus.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project
