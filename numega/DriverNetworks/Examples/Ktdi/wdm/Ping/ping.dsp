# Microsoft Developer Studio Project File - Name="ping" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ping - Win32 Checked Win9X
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ping.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ping.mak" CFG="ping - Win32 Checked Win9X"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ping - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ping - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ping - Win32 Checked Win9X" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ping - Win32 Free Win9X" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ping - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ping - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ping - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /D TDI_WINDOWS_NT=1 /D NTVERSION='WDM' /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objfre\i386/ping.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 wdm.lib ntoskrnl.lib "$(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib" ndis.lib tdint5.lib tdi.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:".\objfre\i386\ping1.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver:WDM /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file ping1.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objfre\i386\ping1.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ping - Win32 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Checked"
# PROP BASE Intermediate_Dir "Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\Include\tdiclient\dep_vxd" /D NTVERSION='WDM' /D TDI_WINDOWS_NT=1 /D WIN32=100 /D "RDRDBG" /D "SRVDBG" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FR /Fd".\objchk\i386\ping1.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk\i386/ping.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 wdm.lib ntoskrnl.lib tdi.lib "$(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib" wdm.lib ndis.lib tdint5.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /debug /machine:IX86 /nodefaultlib /out:".\objchk\i386\ping1.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file ping1.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objchk\i386\ping1.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ping - Win32 Checked Win9X"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ping___Win32_Checked_Win9X"
# PROP BASE Intermediate_Dir "ping___Win32_Checked_Win9X"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\w9x"
# PROP Intermediate_Dir ".\objchk\w9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "..\sys" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(VTOOLSD)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /D TDI_WINDOWS_95=1 /D WIN32=100 /D "RDRDBG" /D "SRVDBG" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "__KVXDINTF__" /FR /Fd".\objchk\w9x\ping1.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objchk\i386/ping.bsc"
# ADD BSC32 /nologo /o"objchk\i386/ping.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib" tdiw9sys.lib ndis.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:".\objchk\w9x\ping1.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file ping1.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objchk\w9x\ping1.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ping - Win32 Free Win9X"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ping___Win32_Free_Win9X"
# PROP BASE Intermediate_Dir "ping___Win32_Free_Win9X"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\w9x"
# PROP Intermediate_Dir ".\objfre\w9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "..\sys" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(VTOOLSD)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /D TDI_WINDOWS_95=1 /D NTVERSION='WDM' /D "__KVXDINTF__" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FR /YX /FD /Oxs /QI0f /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objfre\i386/ping.bsc"
# ADD BSC32 /nologo /o"objfre\i386/ping.bsc"
LINK32=link.exe
# ADD BASE LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:".\objfre\i386\ping.sys" /libpath:"$(BASEDIR)\libfre\i386" /libpath:"$(BASEDIR)\lib\i386\free" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:5.00 /subsystem:native,1.10
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib" tdiw9sys.lib ndis.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:".\objfre\w9x\ping1.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver:WDM /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file ping1.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objfre\w9x\ping1.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ping___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "ping___Win32_IA64_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /D NTVERSION='WDM' /D "TDI_WINDOWS_NT" /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DBG=0 /D DEVL=1 /D "NDEBUG" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objfre\i386/ping.bsc"
# ADD BSC32 /nologo /o"objfre\i386/ping.bsc"
LINK32=link.exe
# ADD BASE LINK32 wdm.lib "$(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib" tdiw9sys.lib ndis.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:".\objfre\w9x\ping1.sys" /libpath:"$(BASEDIR)\libfre\i386" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:5.00 /subsystem:native,1.10
# ADD LINK32 wdm.lib vdw_wdm.lib usbd.lib ndis.lib tdint5.lib kndis5pt.lib ntoskrnl.lib tdi.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"objfre\ia64\ping1.pdb" /machine:IX86 /nodefaultlib /out:".\objfre\ia64\ping1.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\ia64\free" /libpath:"$(DRIVERWORKS)\lib\ia64\free" /machine:IA64 /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ping___Win32_IA64_Checked"
# PROP BASE Intermediate_Dir "ping___Win32_IA64_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /D NTVERSION='WDM' /D "TDI_WINDOWS_NT" /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DBG=1 /D DEVL=1 /D "NDEBUG" /Fo"objchk\ia64/" /Fd"objchk\ia64\ping1.pdb" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objfre\i386/ping.bsc"
# ADD BSC32 /nologo /o"objfre\i386/ping.bsc"
LINK32=link.exe
# ADD BASE LINK32 wdm.lib "$(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib" tdiw9sys.lib ndis.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:".\objfre\w9x\ping1.sys" /libpath:"$(BASEDIR)\libfre\i386" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:5.00 /subsystem:native,1.10
# ADD LINK32 wdm.lib vdw_wdm.lib usbd.lib ndis.lib tdint5.lib kndis5pt.lib ntoskrnl.lib tdi.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"objchk\ia64\ping1.pdb" /debug /machine:IX86 /nodefaultlib /out:".\objchk\ia64\ping1.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\ia64\checked" /libpath:"$(DRIVERWORKS)\lib\ia64\checked" /machine:IA64 /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ping - Win32 Free"
# Name "ping - Win32 Checked"
# Name "ping - Win32 Checked Win9X"
# Name "ping - Win32 Free Win9X"
# Name "ping - Win32 IA64 Free"
# Name "ping - Win32 IA64 Checked"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\Ping.cpp
!IF  "$(CFG)" == "ping - Win32 Free"

!ELSEIF  "$(CFG)" == "ping - Win32 Checked"

!ELSEIF  "$(CFG)" == "ping - Win32 Checked Win9X"

!ELSEIF  "$(CFG)" == "ping - Win32 Free Win9X"

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PingDevice.cpp
!IF  "$(CFG)" == "ping - Win32 Free"

!ELSEIF  "$(CFG)" == "ping - Win32 Checked"

!ELSEIF  "$(CFG)" == "ping - Win32 Checked Win9X"

!ELSEIF  "$(CFG)" == "ping - Win32 Free Win9X"

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PingSocket.cpp
!IF  "$(CFG)" == "ping - Win32 Free"

!ELSEIF  "$(CFG)" == "ping - Win32 Checked"

!ELSEIF  "$(CFG)" == "ping - Win32 Checked Win9X"

!ELSEIF  "$(CFG)" == "ping - Win32 Free Win9X"

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "ping - Win32 IA64 Checked"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\Ping.h
# End Source File
# Begin Source File

SOURCE=.\PingDevice.h
# End Source File
# Begin Source File

SOURCE=.\PingDeviceInterface.h
# End Source File
# Begin Source File

SOURCE=.\PingSocket.h
# End Source File
# Begin Source File

SOURCE=.\tdipnp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# Begin Source File

SOURCE=.\Ping.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\ping1.inf
# End Source File
# End Target
# End Project
