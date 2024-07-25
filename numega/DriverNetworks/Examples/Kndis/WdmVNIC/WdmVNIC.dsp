# Microsoft Developer Studio Project File - Name="wdmvnic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=wdmvnic - Win32 NDIS 5_1 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "WdmVNIC.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "WdmVNIC.mak" CFG="wdmvnic - Win32 NDIS 5_1 Checked"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "wdmvnic - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wdmvnic - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wdmvnic - Win32 NDIS 5 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wdmvnic - Win32 NDIS 5 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wdmvnic - Win32 NDIS 5_1 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wdmvnic - Win32 NDIS 5_1 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wdmvnic - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wdmvnic___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "wdmvnic___Win32_IA64_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Ob2 /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\ddk\wdm" /I "$(BASEDIR)\inc\ddk" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"$(BASEDIR)\inc\warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=1 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /WX /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DBG=0 /D DEVL=1 /D "NDEBUG" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=0 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /Oxs /Wp64 /Zel /QIA64_fr32 -cbstring /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wdm.lib hal.lib kndiswdm.lib ndis.lib int64.lib ntoskrnl.lib hal.lib kndis5mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /debug /debugtype:coff /machine:IX86 /nodefaultlib /out:".\i386\free5\WdmVNIC.sys" /libpath:"$(BASEDIR)\libfre\i386" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERWORKS)\lib\i386\free\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# ADD LINK32 usbd.lib wdm.lib hal.lib ndis.lib ntoskrnl.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"objfre\ia64\wdmvnic.pdb" /machine:IX86 /nodefaultlib /out:".\objfre\ia64\wdmvnic.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\ia64\free" /libpath:"$(DRIVERWORKS)\lib\ia64\free" /machine:IA64 /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)	echo DRIVERWORKS $(DRIVERWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wdmvnic___Win32_IA64_Checked"
# PROP BASE Intermediate_Dir "wdmvnic___Win32_IA64_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Z7 /Od /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\ddk\wdm" /I "$(BASEDIR)\inc\ddk" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"$(BASEDIR)\inc\warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=1 /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /ML /W3 /WX /Zi /Od /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DBG=1 /D DEVL=1 /D "NDEBUG" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=0 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /Fd".\objchk\ia64\wdmvnic.pdb" /Wp64 /Zel /GF /QIA64_fr32 -cbstring /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wdm.lib hal.lib kndiswdm.lib ndis.lib int64.lib ntoskrnl.lib hal.lib kndis5mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\i386\checked5\WdmVNIC.sys" /libpath:"$(BASEDIR)\libchk\i386" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(DRIVERWORKS)\lib\i386\checked\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 usbd.lib wdm.lib hal.lib ndis.lib ntoskrnl.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /pdb:"objchk\ia64\wdmvnic.pdb" /debug /machine:IX86 /nodefaultlib /out:".\objchk\ia64\wdmvnic.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\ia64\checked" /libpath:"$(DRIVERWORKS)\lib\ia64\checked" /machine:IA64 /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)	echo DRIVERWORKS $(DRIVERWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wdmvnic___Win32_NDIS_5_Checked"
# PROP BASE Intermediate_Dir "wdmvnic___Win32_NDIS_5_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Z7 /Od /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"$(BASEDIR)\inc\warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Zi /Od /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=0 /D KNDIS_WDM_STRICT=1 /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 $(BASEDIR)\lib\i386\checked\ndis.lib int64.lib ntoskrnl.lib hal.lib kndis4mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\i386\checked4\WdmVNIC.sys" /libpath:"$(BASEDIR)\libchk\i386" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# ADD LINK32 int64.lib wdm.lib hal.lib ndis.lib ntoskrnl.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:".\objchk\i386\WdmVNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\checked\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\i386
TargetPath=.\objchk\i386\WdmVNIC.sys
TargetName=WdmVNIC
InputPath=.\objchk\i386\WdmVNIC.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)	echo DRIVERWORKS $(DRIVERWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wdmvnic___Win32_NDIS_5_Free"
# PROP BASE Intermediate_Dir "wdmvnic___Win32_NDIS_5_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"$(BASEDIR)\inc\warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=0 /D KNDIS_WDM_STRICT=1 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 $(BASEDIR)\lib\i386\free\ndis.lib int64.lib ntoskrnl.lib hal.lib kndis4mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /debug /debugtype:coff /machine:IX86 /nodefaultlib /out:".\i386\free4\WdmVNIC.sys" /libpath:"$(BASEDIR)\libfre\i386" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# ADD LINK32 int64.lib wdm.lib hal.lib ndis.lib ntoskrnl.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"objfre\i386\WdmVNIC.pdb" /machine:IX86 /nodefaultlib /out:".\objfre\i386\WdmVNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\free\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)	echo DRIVERWORKS $(DRIVERWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wdmvnic___Win32_NDIS_5_1_Checked"
# PROP BASE Intermediate_Dir "wdmvnic___Win32_NDIS_5_1_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386xp"
# PROP Intermediate_Dir ".\objchk\i386xp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Z7 /Od /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=0 /D KNDIS_WDM_STRICT=1 /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Zi /Od /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS51_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=0 /D KNDIS_WDM_STRICT=1 /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wdm.lib hal.lib kndiswdm.lib ndis.lib int64.lib ntoskrnl.lib hal.lib kndis5mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:".\objchk\i386\WdmVNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\checked\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 int64.lib wdm.lib hal.lib ndis.lib ntoskrnl.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:".\objchk\i386xp\WdmVNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\checked\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\i386xp
TargetPath=.\objchk\i386xp\WdmVNIC.sys
TargetName=WdmVNIC
InputPath=.\objchk\i386xp\WdmVNIC.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)	echo DRIVERWORKS $(DRIVERWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wdmvnic___Win32_NDIS_5_1_Free"
# PROP BASE Intermediate_Dir "wdmvnic___Win32_NDIS_5_1_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386xp"
# PROP Intermediate_Dir ".\objfre\i386xp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=0 /D KNDIS_WDM_STRICT=1 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /D "NDIS_MINIPORT_DRIVER" /D NDIS51_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NDIS_WDM=1 /D USE_KLOCKS=0 /D KNDIS_WDM_STRICT=1 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wdm.lib hal.lib kndiswdm.lib ndis.lib int64.lib ntoskrnl.lib hal.lib kndis5mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"objfre\i386\WdmVNIC.pdb" /machine:IX86 /nodefaultlib /out:".\objfre\i386\WdmVNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\free\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 int64.lib wdm.lib hal.lib ndis.lib ntoskrnl.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"objfre\i386\WdmVNIC.pdb" /machine:IX86 /nodefaultlib /out:".\objfre\i386xp\WdmVNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERWORKS)\lib\i386\free\\" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)	echo DRIVERWORKS $(DRIVERWORKS)
# End Special Build Tool

!ENDIF

# Begin Target

# Name "wdmvnic - Win32 IA64 Free"
# Name "wdmvnic - Win32 IA64 Checked"
# Name "wdmvnic - Win32 NDIS 5 Checked"
# Name "wdmvnic - Win32 NDIS 5 Free"
# Name "wdmvnic - Win32 NDIS 5_1 Checked"
# Name "wdmvnic - Win32 NDIS 5_1 Free"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\BusDeviceSim.cpp

!IF  "$(CFG)" == "wdmvnic - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Free"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\WdmVNICAdap.cpp

!IF  "$(CFG)" == "wdmvnic - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Free"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\WdmVNICDrv.cpp

!IF  "$(CFG)" == "wdmvnic - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 IA64 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Free"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\WdmVNICOids.cpp

!IF  "$(CFG)" == "wdmvnic - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 IA64 Checked"

# ADD BASE CPP /Ob1
# ADD CPP /Og /Ob1

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Checked"

# ADD CPP /Og /Ob1

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Checked"

# ADD BASE CPP /Og /Ob1
# ADD CPP /Og /Ob1

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Free"

!ENDIF

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\BusDeviceSim.h
# End Source File
# Begin Source File

SOURCE=.\Characteristics.h
# End Source File
# Begin Source File

SOURCE=.\WdmVNICAdap.h
# End Source File
# Begin Source File

SOURCE=.\WdmVNICDrv.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc"
# Begin Source File

SOURCE=.\WdmVNIC.mof

!IF  "$(CFG)" == "wdmvnic - Win32 IA64 Free"

# Begin Custom Build
TargetName=wdmvnic
InputPath=.\WdmVNIC.mof

"wdmvnic.bmf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mofcomp -WMI -B:$(TargetName).bmf "$(InputPath)"
	wmimofck -h$(TargetName)_wmi.h $(TargetName).bmf

# End Custom Build

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 IA64 Checked"

# Begin Custom Build
TargetName=wdmvnic
InputPath=.\WdmVNIC.mof

"wdmvnic.bmf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mofcomp -WMI -B:$(TargetName).bmf "$(InputPath)"
	wmimofck -h$(TargetName)_wmi.h $(TargetName).bmf

# End Custom Build

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Checked"

# Begin Custom Build
TargetName=WdmVNIC
InputPath=.\WdmVNIC.mof

"wdmvnic.bmf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mofcomp -WMI -B:$(TargetName).bmf "$(InputPath)"
	wmimofck -h$(TargetName)_wmi.h $(TargetName).bmf

# End Custom Build

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5 Free"

# Begin Custom Build
TargetName=WdmVNIC
InputPath=.\WdmVNIC.mof

"wdmvnic.bmf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mofcomp -WMI -B:$(TargetName).bmf "$(InputPath)"
	wmimofck -h$(TargetName)_wmi.h $(TargetName).bmf

# End Custom Build

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Checked"

# Begin Custom Build
TargetName=WdmVNIC
InputPath=.\WdmVNIC.mof

"wdmvnic.bmf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mofcomp -WMI -B:$(TargetName).bmf "$(InputPath)"
	wmimofck -h$(TargetName)_wmi.h $(TargetName).bmf

# End Custom Build

!ELSEIF  "$(CFG)" == "wdmvnic - Win32 NDIS 5_1 Free"

# Begin Custom Build
TargetName=WdmVNIC
InputPath=.\WdmVNIC.mof

"wdmvnic.bmf" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mofcomp -WMI -B:$(TargetName).bmf "$(InputPath)"
	wmimofck -h$(TargetName)_wmi.h $(TargetName).bmf

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\wdmvnic.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(DRIVERNETWORKS)\include"
# SUBTRACT RSC /i "$(BASEDIR)\inc"
# End Source File
# End Group
# Begin Source File

SOURCE=.\makefile.inc
# End Source File
# Begin Source File

SOURCE=.\netWdmVNIC.inf
# End Source File
# Begin Source File

SOURCE=.\sources
# End Source File
# Begin Source File

SOURCE=.\WdmVNIC.bmf
# End Source File
# End Target
# End Project
