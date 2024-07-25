# Microsoft Developer Studio Project File - Name="Daytime" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Daytime - Win32 IA64 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Daytime.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Daytime.mak" CFG="Daytime - Win32 IA64 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Daytime - Win32 IA64 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Daytime - Win32 IA64 Release" (based on "Win32 (x86) Application")
!MESSAGE "Daytime - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Daytime - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Daytime - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /MD /W3 /Z7 /Oy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\Include\tdiclient\dep_vxd" /FI"warning.h" /D FPO=1 /D "WIN32" /D "_WINDOWS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D "TDI_WINDOWS_NT" /FR /Zel -cbstring /QIfdiv- /QIf /GF /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ntoskrnl.lib hal.lib vdw.lib ndis.lib tdint5.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /machine:I386 /nodefaultlib /out:".\objfre\i386\Daytime.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\free" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -OPT:ICF -RELEASE -FULLBUILD -IGNORE:4210,4001,4037,4039,4065,4070,4078,4087,4089,4096 -MERGE:.rdata=.text -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:MINIMAL
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Daytime - Win32 Debug"

# PROP BASE Use_MFC 2
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
# ADD CPP /nologo /Gz /W3 /Z7 /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\Include\tdiclient\dep_vxd" /FI"warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D "WIN32" /D "_WINDOWS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D "TDI_WINDOWS_NT" /FR /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ntoskrnl.lib hal.lib vdw.lib ndis.lib tdint5.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib /out:".\objchk\i386\Daytime.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\checked" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -OPT:ICF -RELEASE -FULLBUILD -IGNORE:4210,4001,4037,4039,4065,4070,4078,4087,4089,4096 -MERGE:.rdata=.text -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:FULL
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\i386
TargetPath=.\objchk\i386\Daytime.sys
TargetName=Daytime
InputPath=.\objchk\i386\Daytime.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).NMS" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym  /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "Daytime - Win32 IA64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Daytime___Win32_IA64_Debug"
# PROP BASE Intermediate_Dir "Daytime___Win32_IA64_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Z7 /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\Include\tdiclient\dep_vxd" /FI"warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D "WIN32" /D "_WINDOWS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D "TDI_WINDOWS_NT" /FR /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /ML /W3 /WX /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\Include\tdiclient\dep_vxd" /FI"warning.h" /D "TDI_WINDOWS_NT" /D NTVERSION=400 /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D DBG=1 /D FPO=0 /Fd".\objchk\ia64\DayTime.pdb" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ntoskrnl.lib hal.lib vdw.lib ndis.lib tdint5.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib /out:".\objchk\i386\Daytime.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\checked" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -OPT:ICF -RELEASE -FULLBUILD -IGNORE:4210,4001,4037,4039,4065,4070,4078,4087,4089,4096 -MERGE:.rdata=.text -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:FULL
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ntoskrnl.lib vdw.lib ndis.lib tdint5.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:".\objchk\ia64\DayTime.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\ia64\checked" /libpath:"$(DRIVERNETWORKS)\lib\ia64\checked" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /machine:IA64 /debugtype:pdata
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Daytime - Win32 IA64 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Daytime___Win32_IA64_Release"
# PROP BASE Intermediate_Dir "Daytime___Win32_IA64_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /MD /W3 /Z7 /Oy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\Include\tdiclient\dep_vxd" /FI"warning.h" /D FPO=1 /D "WIN32" /D "_WINDOWS" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D "TDI_WINDOWS_NT" /FR /Zel -cbstring /QIfdiv- /QIf /GF /Oxs /c
# ADD CPP /nologo /W3 /WX /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\Include\tdiclient\dep_vxd" /FI"warning.h" /D "TDI_WINDOWS_NT" /D NTVERSION='400' /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D FPO=0 /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ntoskrnl.lib hal.lib vdw.lib ndis.lib tdint5.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry@8" /subsystem:windows /machine:I386 /nodefaultlib /out:".\objfre\i386\Daytime.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\free" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -OPT:ICF -RELEASE -FULLBUILD -IGNORE:4210,4001,4037,4039,4065,4070,4078,4087,4089,4096 -MERGE:.rdata=.text -driver -align:0x20 -osversion:4.00 -subsystem:native,4.00 -debug:MINIMAL
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ntoskrnl.lib vdw.lib ndis.lib tdint5.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\ia64\DayTime.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\ia64\free" /libpath:"$(DRIVERNETWORKS)\lib\ia64\free" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /machine:IA64 /debugtype:pdata
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Daytime - Win32 Release"
# Name "Daytime - Win32 Debug"
# Name "Daytime - Win32 IA64 Debug"
# Name "Daytime - Win32 IA64 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Daytime.cpp
# End Source File
# Begin Source File

SOURCE=.\Daytime.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(DRIVERNETWORKS)\include"
# SUBTRACT RSC /i "$(BASEDIR)\inc"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Daytime.h
# End Source File
# Begin Source File

SOURCE=.\function.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Daytime.ini
# End Source File
# Begin Source File

SOURCE=.\makefile.x
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\sources.x
# End Source File
# End Target
# End Project
