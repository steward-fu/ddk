# Microsoft Developer Studio Project File - Name="Test_simple1394" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Test_simple1394 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Test_simple1394.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Test_simple1394.mak" CFG="Test_simple1394 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Test_simple1394 - Win32 IA64 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Test_simple1394 - Win32 IA64 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Test_simple1394 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Test_simple1394 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Test_simple1394 - Win32 IA64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "release64"
# PROP BASE Intermediate_Dir "release64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /ML /W3 /WX /GX /O2 /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN64" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX -Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:IA64 /out:"objchk/ia64/Test_simple1394.exe" /pdbtype:sept
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /incremental:no /machine:IX86 /out:"objfre/ia64/Test_simple1394.exe" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 IA64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug64"
# PROP BASE Intermediate_Dir "debug64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN64" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX /Wp64 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:IA64 /out:"objchk/ia64/Test_simple1394.exe" /pdbtype:sept
# ADD LINK32 setupapi.lib libc.lib kernel32.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib /out:"objchk/ia64/Test_simple1394.exe" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64 /ignore:4099
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386 /out:"objfre\i386\Test_simple1394.exe" /libpath:"$(TARGET_LIB_PATH)"

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "$(CRT_INC_PATH)" /I "$(TARGET_INC_PATH)" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386 /out:"objchk\i386\Test_simple1394.exe" /libpath:"$(TARGET_LIB_PATH)"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Test_simple1394 - Win32 IA64 Release"
# Name "Test_simple1394 - Win32 IA64 Debug"
# Name "Test_simple1394 - Win32 Release"
# Name "Test_simple1394 - Win32 Debug"
# Begin Source File

SOURCE=.\OpenByIntf.cpp

!IF  "$(CFG)" == "Test_simple1394 - Win32 IA64 Release"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98"

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 IA64 Debug"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98"

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 Release"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98"

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 Debug"

# ADD CPP /I "$(DRIVERWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\README.TXT
# End Source File
# Begin Source File

SOURCE=..\Simple1394DeviceInterface.h
# End Source File
# Begin Source File

SOURCE=.\Test_simple1394.cpp

!IF  "$(CFG)" == "Test_simple1394 - Win32 IA64 Release"

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 IA64 Debug"

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 Release"

!ELSEIF  "$(CFG)" == "Test_simple1394 - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
