# Microsoft Developer Studio Project File - Name="USBNIC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=USBNIC - Win32 NDIS 5 Win9X Compatible Free
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "USBNIC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "USBNIC.mak" CFG="USBNIC - Win32 NDIS 5 Win9X Compatible Free"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "USBNIC - Win32 NDIS 5 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "USBNIC - Win32 NDIS 5 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "USBNIC - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "USBNIC - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "USBNIC - Win32 NDIS 5 Win9X Compatible Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "USBNIC - Win32 NDIS 5 Win9X Compatible Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "checked5"
# PROP BASE Intermediate_Dir "checked5"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=0 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FR /Fd".\objchk\i386\USBNIC.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ndis.lib usbd.lib ntoskrnl.lib hal.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /debug /machine:IX86 /nodefaultlib /out:".\objchk\i386\USBNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /libpath:"$(DRIVERWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\i386
TargetPath=.\objchk\i386\USBNIC.sys
TargetName=USBNIC
InputPath=.\objchk\i386\USBNIC.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "free5"
# PROP BASE Intermediate_Dir "free5"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=0 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ndis.lib int64.lib usbd.lib ntoskrnl.lib hal.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\i386\USBNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /libpath:"$(DRIVERWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "USBNIC - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "USBNIC___Win32_IA64_Checked"
# PROP BASE Intermediate_Dir "USBNIC___Win32_IA64_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /ML /W3 /WX /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=0 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DBG=1 /D DEVL=1 /D "NDEBUG" /Fd".\objchk\ia64\USBNIC.pdb" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ndis.lib usbd.lib ntoskrnl.lib hal.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /debug /machine:IX86 /nodefaultlib /out:".\objchk\ia64\USBNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\ia64\checked" /libpath:"$(DRIVERWORKS)\lib\ia64\checked" /machine:IA64 /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "USBNIC - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "USBNIC___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "USBNIC___Win32_IA64_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /WX /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=0 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /D _IA64_=1 /D "_WIN64" /D "IA64" /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x560 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DBG=0 /D DEVL=1 /D "NDEBUG" /D $(CPU)=1 /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 ndis.lib ntoskrnl.lib hal.lib kndis5mp.lib kndiswdm.lib usbd.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\ia64\USBNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\ia64\free" /libpath:"$(DRIVERWORKS)\lib\ia64\free" /machine:IA64 /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Win9X Compatible Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "checked5_Win9X_Compatible"
# PROP BASE Intermediate_Dir "checked5_Win9X_Compatible"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\w9x"
# PROP Intermediate_Dir ".\objchk\w9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FR /Fd".\objchk\w9x\USBNIC.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ndis.lib usbd.lib int64.lib ntoskrnl.lib hal.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:".\objchk\w9x\USBNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /libpath:"$(DRIVERWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\w9x
TargetPath=.\objchk\w9x\USBNIC.sys
TargetName=USBNIC
InputPath=.\objchk\w9x\USBNIC.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Win9X Compatible Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "free5_Win9X_Compatible"
# PROP BASE Intermediate_Dir "free5_Win9X_Compatible"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\w9x"
# PROP Intermediate_Dir ".\objfre\w9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /FI"warning.h" /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D BINARY_COMPATIBLE=1 /D NTVERSION='WDM' /D NDIS_WDM=1 /D USE_KLOCKS=1 /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\incclude" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ndis.lib int64.lib usbd.lib ntoskrnl.lib hal.lib kndis5mp.lib kndiswdm.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\w9x\USBNIC.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /libpath:"$(DRIVERWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "USBNIC - Win32 NDIS 5 Checked"
# Name "USBNIC - Win32 NDIS 5 Free"
# Name "USBNIC - Win32 IA64 Checked"
# Name "USBNIC - Win32 IA64 Free"
# Name "USBNIC - Win32 NDIS 5 Win9X Compatible Checked"
# Name "USBNIC - Win32 NDIS 5 Win9X Compatible Free"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\USBNICAdap.cpp

# End Source File
# Begin Source File

SOURCE=.\USBNICDrv.cpp
	
# End Source File
# Begin Source File

SOURCE=.\USBNICOids.cpp

!IF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Checked"

# ADD CPP /Og /Ob1

!ELSEIF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "USBNIC - Win32 IA64 Checked"

# ADD BASE CPP /Ob1
# ADD CPP /Og /Ob1

!ELSEIF  "$(CFG)" == "USBNIC - Win32 IA64 Free"

!ELSEIF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Win9X Compatible Checked"

# ADD BASE CPP /Ob1
# ADD CPP /Og /Ob1

!ELSEIF  "$(CFG)" == "USBNIC - Win32 NDIS 5 Win9X Compatible Free"

# ADD BASE CPP /Ob1
# ADD CPP /Ob1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\Characteristics.h
# End Source File
# Begin Source File

SOURCE=.\USBNICAdap.h
# End Source File
# Begin Source File

SOURCE=.\USBNICDrv.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc"
# Begin Source File

SOURCE=.\USBNIC.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(DRIVERNETWORKS)\include"
# SUBTRACT RSC /i "$(BASEDIR)\inc" /i "..\incclude"
# End Source File
# End Group
# Begin Source File

SOURCE=.\netUSBNIC.inf
# End Source File
# End Target
# End Project
