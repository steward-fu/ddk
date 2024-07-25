# Microsoft Developer Studio Project File - Name="vcap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=vcap - Win32 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vcap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vcap.mak" CFG="vcap - Win32 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vcap - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vcap - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vcap - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vcap - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vcap - Win32 IA64 Free"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /WX /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\include\dep_vxd" /FI"warning.h" /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D _MERCED_A0_=1 /D "NDEBUG" /D _DLL=1 /D FPO=0 /D NTVERSION='WDM' /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "UNICODE" /D "_UNICODE" /D DEBUG_LEVEL=DEBUGLVL_BLAB /D "STD_CALL" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /i "$(CRT_INC_PATH)" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ks.lib $(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre\ia64\vcap.sys" /libpath:"$(TARGET_LIB_PATH)" /debug:MINIMAL /machine:IA64 /debugtype:pdata /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /driver:WDM

!ELSEIF  "$(CFG)" == "vcap - Win32 IA64 Checked"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /I "." /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\include\dep_vxd" /FI"warning.h" /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D _MERCED_A0_=1 /D "NDEBUG" /D _DLL=1 /D DBG=1 /D FPO=0 /D NTVERSION='WDM' /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "UNICODE" /D "_UNICODE" /D "STD_CALL" /Fd".\objchk\ia64\vcap.pdb" /Zel -cbstring /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /i "$(CRT_INC_PATH)" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ks.lib $(DRIVERWORKS)\lib\IA64\checked\vdw_wdm.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objchk\ia64\vcap.sys" /libpath:"$(TARGET_LIB_PATH)" /debug:FULL /machine:IA64 /debugtype:pdata /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /driver:WDM

!ELSEIF  "$(CFG)" == "vcap - Win32 Free"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\include\dep_vxd" /FI"warning.h" /D _X86_=1 /D i386=1 /D NT_UP=1 /D _WIN32_WINNT=0x0400 /D FPO=1 /D NTVERSION='WDM' /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "UNICODE" /D "_UNICODE" /D DEBUG_LEVEL=DEBUGLVL_BLAB /D "STD_CALL" /Zel -cbstring /GF /Oxs /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(CRT_INC_PATH)" /i "$(DRIVERWORKS)\include" /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d DEBUG_LEVEL=DEBUGLVL_BLAB
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ks.lib $(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre\i386\vcap.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /driver:WDM

!ELSEIF  "$(CFG)" == "vcap - Win32 Checked"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(DRIVERWORKS)\include" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include\dep_vxd" /FI"warning.h" /D _X86_=1 /D i386=1 /D NT_UP=1 /D _WIN32_WINNT=0x0400 /D DBG=1 /D FPO=0 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "UNICODE" /D "_UNICODE" /D "STD_CALL" /D NTVERSION='WDM' /Fd".\objchk\i386\vcap.pdb" /Zel -cbstring /GF /c
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(CRT_INC_PATH)" /i "$(DRIVERWORKS)\include" /i "$(DRIVERWORKS)\source" /i "$(DRIVERWORKS)\..\common\include\stl" /i "$(DRIVERWORKS)\include\dep_vxd" /d "UNICODE" /d "_UNICODE" /d DEBUG_LEVEL=DEBUGLVL_BLAB
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib ks.lib $(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:".\objchk\i386\vcap.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /driver:WDM
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objchk\i386\vcap.sys
TargetName=vcap
InputPath=.\objchk\i386\vcap.sys
SOURCE="$(InputPath)"

".\objchk\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source;package;always $(TARGETPATH)

# End Custom Build

!ENDIF 

# Begin Target

# Name "vcap - Win32 IA64 Free"
# Name "vcap - Win32 IA64 Checked"
# Name "vcap - Win32 Free"
# Name "vcap - Win32 Checked"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\Vcap.cpp
# End Source File
# Begin Source File

SOURCE=.\VcapDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\VcapFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\VcapPin.cpp
# End Source File
# Begin Source File

SOURCE=.\VcapTimer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\Vcap.h
# End Source File
# Begin Source File

SOURCE=.\VcapDevice.h
# End Source File
# Begin Source File

SOURCE=.\VcapFilter.h
# End Source File
# Begin Source File

SOURCE=.\VcapFormats.h
# End Source File
# Begin Source File

SOURCE=.\VcapPin.h
# End Source File
# Begin Source File

SOURCE=.\VcapTimer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# Begin Source File

SOURCE=.\Vcap.rc
# End Source File
# End Group
# Begin Group "KS Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\..\..\include\kavstream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\avstream\KsDevice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\avstream\KsFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\avstream\KsFormats.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\avstream\KsObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\avstream\KsPin.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# Begin Source File

SOURCE=.\Vcap.inf
# End Source File
# End Target
# End Project
