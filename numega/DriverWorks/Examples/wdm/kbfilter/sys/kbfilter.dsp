# Microsoft Developer Studio Project File - Name="kbfilter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kbfilter - Win32 IA64 checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kbfilter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kbfilter.mak" CFG="kbfilter - Win32 IA64 checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kbfilter - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kbfilter - Win32 IA64 checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kbfilter - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kbfilter - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kbfilter - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386\"
# PROP Intermediate_Dir ".\objfre\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /W3 /Zel -cbstring /Gy /GF /c /FI"warning.h" /Gz /FR /QIfdiv- /QIf /Oxs /Oy /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" 
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objfre\i386/kbfilter.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /driver:WDM /out:".\objfre\i386\kbfilter.sys"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file kbfilter.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objfre\i386\kbfilter.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "kbfilter - Win32 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Checked"
# PROP BASE Intermediate_Dir "Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386\"
# PROP Intermediate_Dir ".\objchk\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /W3 /Zel -cbstring /Gy /GF /c /FI"warning.h" /Gz /FR /QIfdiv- /QIf /Zi /Oi /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /Fd".\objchk\i386\\kbfilter.pdb"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk\i386/kbfilter.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /debugtype:cv /driver:WDM /out:".\objchk\i386\kbfilter.sys"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file kbfilter.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objchk\i386\kbfilter.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "kbfilter - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "kbfilter___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "kbfilter___Win32_IA64_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64\"
# PROP Intermediate_Dir ".\objfre\ia64\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /W3 /Zel -cbstring /Gy /GF /c /FI"warning.h" /WX /Wp64 /Zel /QIA64_fr32 /Oxs /D NTVERSION='WDM' /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG"  /D _DLL=1 /D FPO=0 /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" 
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objfre\i386/kbfilter.bsc"
# ADD BSC32 /nologo /o"objfre\i386/kbfilter.bsc"
LINK32=link.exe
# ADD BASE LINK32
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib" usbd.lib  /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /machine:IA64 /debugtype:pdata /driver:WDM /out:".\objfre\ia64\kbfilter.sys"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kbfilter - Win32 IA64 checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "kbfilter___Win32_IA64_checked"
# PROP BASE Intermediate_Dir "kbfilter___Win32_IA64_checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64\"
# PROP Intermediate_Dir ".\objchk\ia64\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /W3 /Zel -cbstring /Gy /GF /c /FI"warning.h" /WX /Wp64 /Zel /QIA64_fr32 /Zi /Oi /D NTVERSION='WDM' /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG"  /D _DLL=1 /D DBG=1 /D FPO=0 /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /Fd".\objchk\ia64\\kbfilter.pdb"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objchk\i386/kbfilter.bsc"
# ADD BSC32 /nologo /o"objchk\i386/kbfilter.bsc"
LINK32=link.exe
# ADD BASE LINK32
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\IA64\checked\vdw_wdm.lib" usbd.lib  /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /debugtype:cv /machine:IA64 /debugtype:pdata /driver:WDM /out:".\objchk\ia64\kbfilter.sys"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "kbfilter - Win32 Free"
# Name "kbfilter - Win32 Checked"
# Name "kbfilter - Win32 IA64 Free"
# Name "kbfilter - Win32 IA64 checked"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\Kbfilter.cpp
	
# End Source File
# Begin Source File

SOURCE=.\KbfilterDevice.cpp
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\Kbfilter.h
# End Source File
# Begin Source File

SOURCE=.\KbfilterDevice.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# Begin Source File

SOURCE=.\Kbfilter.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\kbfilter.inf
# End Source File
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# End Target
# End Project
