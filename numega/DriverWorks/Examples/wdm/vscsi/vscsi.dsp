# Microsoft Developer Studio Project File - Name="vscsi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=vscsi - Win32 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vscsi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vscsi.mak" CFG="vscsi - Win32 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vscsi - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vscsi - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vscsi - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vscsi - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vscsi - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objfre\ia64"
# PROP BASE Intermediate_Dir "obj\ia64\free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre\ia64"
# PROP Intermediate_Dir "obj\ia64\free"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Zi /O1 /Ob2 /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source"
# ADD CPP /nologo /Gz /W3 /WX /Zi /Gy /I ".." /I "$(BASEDIR)\src\usb\inc" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D _MERCED_A0_=1 /D DBG=0 /D WIN32=100 /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D NTVERSION='WDM' /D TDI_WINDOWS_NT=1 /Fd".\objfre\ia64\vscsi.pdb" /Oxs /Wp64 /Zel /GF /QIA64_fr32 -cbstring /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wdm.lib hal.lib $(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib usbd.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry@8" /subsystem:windows /machine:IX86 /nodefaultlib /out:".\objfre\ia64\vscsi.sys" /machine:IA64 -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -FORCE:MULTIPLE -RELEASE -FULLBUILD -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 -MERGE:.rdata=.text -driver -align:0x20 -osversion:5.00 -subsystem:native,1.10 -driver:WDM -debug:MINIMAL
# SUBTRACT BASE LINK32 /pdb:none /map /debug
# ADD LINK32 "$(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib" usbd.lib wdm.lib "$(DRIVERNETWORKS)\lib\IA64\checked\tdint5.lib" tdi.lib ndis.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\ia64\vscsi.sys" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64 /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "vscsi - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objchk\ia64"
# PROP BASE Intermediate_Dir "obj\ia64\checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objchk\ia64"
# PROP Intermediate_Dir "obj\ia64\checked"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /WX /Zi /Oi /Gy /I ".." /I "$(BASEDIR)\src\usb\inc" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D _MERCED_A0_=1 /D DBG=1 /D WIN32=100 /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D NTVERSION='WDM' /D TDI_WINDOWS_NT=1 /YX /Fd".\objchk\ia64\vscsi.pdb" /Wp64 /Zel /GF /QIA64_fr32 -cbstring /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wdm.lib hal.lib $(DRIVERWORKS)\lib\IA64\checked\vdw_wdm.lib usbd.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry@8" /subsystem:windows /machine:IX86 /nodefaultlib /out:".\objchk\ia64\vscsi.sys" /machine:IA64 -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -FORCE:MULTIPLE -RELEASE -FULLBUILD -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 -MERGE:.rdata=.text -driver -align:0x20 -osversion:5.00 -subsystem:native,1.10 -driver:WDM -debug:MINIMAL
# SUBTRACT BASE LINK32 /pdb:none /map /debug
# ADD LINK32 "$(DRIVERWORKS)\lib\IA64\checked\vdw_wdm.lib" usbd.lib wdm.lib "$(DRIVERNETWORKS)\lib\IA64\checked\tdint5.lib" tdi.lib ndis.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /debug /machine:IX86 /nodefaultlib /out:".\objchk\ia64\vscsi.sys" /libpath:"$(TARGET_LIB_PATH)" /machine:IA64 /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "vscsi - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "objfre\i386"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre\i386"
# PROP Intermediate_Dir "Free"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Oy /Gy /D "WIN32" /YX /Oxs /c
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "." /I "$(BASEDIR)\inc\win98" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /D "STD_CALL" /D _WIN32_WINNT=0x0400 /D FPO=1 /D "_IDWBUILD" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D WIN32=100 /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D NTVERSION='WDM' /D TDI_WINDOWS_NT=1 /FR /YX /Fd".\objfre\i386\vscsi.pdb" /FD /Oxs /QI0f /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(TARGET_INC_PATH)" /i "$(CRT_INC_PATH)" /i "$(DDK_INC_PATH)" /i "$(WDM_INC_PATH)" /i "$(DRIVERWORKS)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib" wdm.lib "$(DRIVERNETWORKS)\lib\i386\checked\tdint5.lib" tdi.lib ndis.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:".\objfre\i386\vscsi.sys" /libpath:"$(TARGET_LIB_PATH)" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file vscsi.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objfre\i386\vscsi.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "vscsi - Win32 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "objchk\i386"
# PROP BASE Intermediate_Dir "Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk\i386"
# PROP Intermediate_Dir "Checked"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "." /I "$(BASEDIR)\inc\win98" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /D "RDRDBG" /D "SRVDBG" /D "STD_CALL" /D _WIN32_WINNT=0x0400 /D DBG=1 /D FPO=0 /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D WIN32=100 /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D NTVERSION='WDM' /D TDI_WINDOWS_NT=1 /FR /YX /Fd".\objchk\i386\vscsi.pdb" /FD /QI0f /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(TARGET_INC_PATH)" /i "$(CRT_INC_PATH)" /i "$(DDK_INC_PATH)" /i "$(WDM_INC_PATH)" /i "$(DRIVERWORKS)\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib" wdm.lib "$(DRIVERNETWORKS)\lib\i386\checked\tdint5.lib" tdi.lib ndis.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /map /machine:IX86 /nodefaultlib /out:".\objchk\i386\vscsi.sys" /libpath:"$(TARGET_LIB_PATH)" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file vscsi.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objchk\i386\vscsi.sys
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "vscsi - Win32 IA64 Free"
# Name "vscsi - Win32 IA64 Checked"
# Name "vscsi - Win32 Free"
# Name "vscsi - Win32 Checked"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\debug.cpp

!IF  "$(CFG)" == "vscsi - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sockets.cpp

!IF  "$(CFG)" == "vscsi - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vbus.cpp

!IF  "$(CFG)" == "vscsi - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vdisk.cpp

!IF  "$(CFG)" == "vscsi - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vscsi.cpp

!IF  "$(CFG)" == "vscsi - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vscsidev.cpp

!IF  "$(CFG)" == "vscsi - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Free"

!ELSEIF  "$(CFG)" == "vscsi - Win32 Checked"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\sockets.h
# End Source File
# Begin Source File

SOURCE=.\vbus.h
# End Source File
# Begin Source File

SOURCE=.\vdisk.h
# End Source File
# Begin Source File

SOURCE=.\vscsi.h
# End Source File
# Begin Source File

SOURCE=.\vscsidev.h
# End Source File
# Begin Source File

SOURCE=.\vscsintf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# Begin Source File

SOURCE=.\vscsi.rc
# End Source File
# End Group
# Begin Group "Inf Files"

# PROP Default_Filter "*.inf"
# Begin Source File

SOURCE=.\vscsi.inf
# End Source File
# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project