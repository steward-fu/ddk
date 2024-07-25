# Microsoft Developer Studio Project File - Name="TestHello" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=TestHello - Win32 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TestHello.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestHello.mak" CFG="TestHello - Win32 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestHello - Win32 Checked" (based on "Win32 (x86) Console Application")
!MESSAGE "TestHello - Win32 Free" (based on "Win32 (x86) Console Application")
!MESSAGE "TestHello - Win32 AMD64 Checked" (based on "Win32 (x86) Console Application")
!MESSAGE "TestHello - Win32 AMD64 Free" (based on "Win32 (x86) Console Application")
!MESSAGE "TestHello - Win32 IA64 Checked" (based on "Win32 (x86) Console Application")
!MESSAGE "TestHello - Win32 IA64 Free" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TestHello - Win32 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objchk\i386"
# PROP BASE Intermediate_Dir "objchk\i386"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk\i386"
# PROP Intermediate_Dir "objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD CPP /nologo /Gz /W3 /Gm /GX /Zi /Od /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(ATL_INC_PATH)\atl30" /I "..\." /D _X86_=1 /D i386=1 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D DBG=1 /D "DEPRECATE_DDK_FUNCTIONS" /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /D "STD_CALL" /D "_CONSOLE" /FR /YX /Fd"objchk\i386/TestHello.pdb" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib     /nologo /base:"0x01000000" /subsystem:console /debug /machine:I386 /libpath:"$(TARGET_LIB_PATH)" /ignore:4010,4037,4039,4065,4070,4078,4087,4089,4221,4210    /ignore:4010,4037,4039,4065,4070,4078,4087,4089,4221,4210

!ELSEIF  "$(CFG)" == "TestHello - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objfre\i386"
# PROP BASE Intermediate_Dir "objfre\i386"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre\i386"
# PROP Intermediate_Dir "objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD CPP /nologo /Gz /W3 /GX /O2 /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(ATL_INC_PATH)\atl30" /I "..\." /D _X86_=1 /D i386=1 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /D "STD_CALL" /D "_CONSOLE" /FR /YX /Fd"objfre/i386/TestHello.pdb" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib     /nologo /base:"0x01000000" /subsystem:console /machine:I386 /libpath:"$(TARGET_LIB_PATH)" /ignore:4010,4037,4039,4065,4070,4078,4087,4089,4221,4210

!ELSEIF  "$(CFG)" == "TestHello - Win32 AMD64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objchk/amd64"
# PROP BASE Intermediate_Dir "objchk/amd64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk/amd64"
# PROP Intermediate_Dir "objchk/amd64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(ATL_INC_PATH)\atl30" /I "..\." /D "_WIN64" /D "_AMD64_" /D "AMD64" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D "_AMD64_WORKAROUND_" /D DEVL=1 /D "NDEBUG" /D DBG=1 /D "DEPRECATE_DDK_FUNCTIONS" /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /D "_CONSOLE" /FR /YX /Fd".\objchk\amd64\TestHello.pdb" /Wp64        /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib libc.lib advapi32.lib kernel32.lib libci.lib     /nologo /base:"0x01000000" /subsystem:console /debug /machine:IX86 /nodefaultlib /libpath:"$(TARGET_LIB_PATH)" /machine:amd64     /ignore:4198,4010,4037,4039,4065,4070,4078,4087,4089,4221

!ELSEIF  "$(CFG)" == "TestHello - Win32 AMD64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objfre/amd64"
# PROP BASE Intermediate_Dir "objfre/amd64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre/amd64"
# PROP Intermediate_Dir "objfre/amd64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD CPP /nologo /W3 /WX /GX /O2 /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(ATL_INC_PATH)\atl30" /I "..\." /D "_WIN64" /D "_AMD64_" /D "AMD64" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D "_AMD64_WORKAROUND_" /D DEVL=1 /D "NDEBUG" /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /D "_CONSOLE" /FR /YX -Wp64        /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib libc.lib advapi32.lib kernel32.lib libci.lib     /nologo /base:"0x01000000" /subsystem:console /machine:IX86 /nodefaultlib /libpath:"$(TARGET_LIB_PATH)" /machine:amd64     /ignore:4198,4010,4037,4039,4065,4070,4078,4087,4089,4221
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "TestHello - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objchk/ia64"
# PROP BASE Intermediate_Dir "objchk/ia64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk/ia64"
# PROP Intermediate_Dir "objchk/ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD CPP /nologo /W3 /WX /Gm /GX /Zi /Od /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(ATL_INC_PATH)\atl30" /I "..\." /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D DBG=1 /D "DEPRECATE_DDK_FUNCTIONS" /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /D "_CONSOLE" /FR /YX /Fd".\objchk\ia64\TestHello.pdb" /Wp64        /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib libc.lib advapi32.lib kernel32.lib libci.lib     /nologo /base:"0x01000000" /subsystem:console /debug /machine:IX86 /nodefaultlib /libpath:"$(TARGET_LIB_PATH)" /machine:IA64     /ignore:4010,4037,4039,4065,4070,4078,4087,4089,4221,4210,4108,4088,4218

!ELSEIF  "$(CFG)" == "TestHello - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objfre/ia64"
# PROP BASE Intermediate_Dir "objfre/ia64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre/ia64"
# PROP Intermediate_Dir "objfre/ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD CPP /nologo /W3 /WX /GX /O2 /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(ATL_INC_PATH)\atl30" /I "..\." /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /D "_CONSOLE" /FR /YX -Wp64        /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 setupapi.lib libc.lib advapi32.lib kernel32.lib libci.lib     /nologo /base:"0x01000000" /subsystem:console /machine:IX86 /nodefaultlib /libpath:"$(TARGET_LIB_PATH)" /machine:IA64     /ignore:4010,4037,4039,4065,4070,4078,4087,4089,4221,4210,4108,4088,4218
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "TestHello - Win32 Checked"
# Name "TestHello - Win32 Free"
# Name "TestHello - Win32 AMD64 Checked"
# Name "TestHello - Win32 AMD64 Free"
# Name "TestHello - Win32 IA64 Checked"
# Name "TestHello - Win32 IA64 Free"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp;.idl"
# Begin Source File

SOURCE=.\testhello.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project
