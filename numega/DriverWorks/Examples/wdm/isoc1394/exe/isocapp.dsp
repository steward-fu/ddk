# Microsoft Developer Studio Project File - Name="IsocApp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=IsocApp - Win32 IA64 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IsocApp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IsocApp.mak" CFG="IsocApp - Win32 IA64 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IsocApp - Win32 IA64 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "IsocApp - Win32 IA64 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "IsocApp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "IsocApp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "IsocApp"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IsocApp - Win32 IA64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IsocApp___Win32_IA64_Debug"
# PROP BASE Intermediate_Dir "IsocApp___Win32_IA64_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "$(DRIVERWORKS)\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /I "$(DRIVERWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /D "WIN64" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX /Fd".\objchk\ia64\isocapp.pdb" /Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib kernel32.lib libc.lib /nologo /subsystem:console /pdb:"debug/IsocApp.pdb" /debug /machine:IX86 /nodefaultlib /out:"objchk/ia64/IsocApp.exe" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64 /ignore:4099
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IsocApp - Win32 IA64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IsocApp___Win32_IA64_Release"
# PROP BASE Intermediate_Dir "IsocApp___Win32_IA64_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "$(DRIVERWORKS)\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /ML /W3 /WX /GX /O2 /I "$(DRIVERWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /D "WIN64" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX -Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib libc.lib kernel32.lib /nologo /subsystem:console /incremental:no /pdb:"release/IsocApp.pdb" /machine:IX86 /nodefaultlib /out:"objfre/ia64/IsocApp.exe" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IsocApp - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "$(DRIVERWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"$(TARGET_LIB_PATH)" /IGNORE:4078
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IsocApp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IsocApp___Win32_Debug"
# PROP BASE Intermediate_Dir "IsocApp___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(DRIVERWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /libpath:"$(TARGET_LIB_PATH)"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "IsocApp - Win32 IA64 Debug"
# Name "IsocApp - Win32 IA64 Release"
# Name "IsocApp - Win32 Release"
# Name "IsocApp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\1394Isoc.cpp
DEP_CPP_1394I=\
	"..\Isoc1394DeviceInterface.h"\
	"..\Isoc1394ioctl.h"\
	"..\Isoc1394types.h"\
	

!IF  "$(CFG)" == "IsocApp - Win32 IA64 Debug"

!ELSEIF  "$(CFG)" == "IsocApp - Win32 IA64 Release"

!ELSEIF  "$(CFG)" == "IsocApp - Win32 Release"

!ELSEIF  "$(CFG)" == "IsocApp - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenByIntf.cpp
DEP_CPP_OPENB=\
	"..\..\..\..\include\devintf.h"\
	

!IF  "$(CFG)" == "IsocApp - Win32 IA64 Debug"

!ELSEIF  "$(CFG)" == "IsocApp - Win32 IA64 Release"

!ELSEIF  "$(CFG)" == "IsocApp - Win32 Release"

!ELSEIF  "$(CFG)" == "IsocApp - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Isoc1394DeviceInterface.h
# End Source File
# Begin Source File

SOURCE=..\Isoc1394ioctl.h
# End Source File
# Begin Source File

SOURCE=..\Isoc1394types.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
