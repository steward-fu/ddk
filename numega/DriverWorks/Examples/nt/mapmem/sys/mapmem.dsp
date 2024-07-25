# Microsoft Developer Studio Project File - Name="mapmem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mapmem - Win32 IA64 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mapmem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mapmem.mak" CFG="mapmem - Win32 IA64 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mapmem - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mapmem - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mapmem - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mapmem - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mapmem - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mapmem___Win32_IA64_Checked"
# PROP BASE Intermediate_Dir "mapmem___Win32_IA64_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I ".." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /FR /Fd".\objchk\i386\\mapmem.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD CPP /nologo /ML /W3 /WX /Zi /Gy /I "." /I ".." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D "DBGMSG_NT" /D "DBGMSG_DRIVER" /D NTVERSION=400 /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D DBG=1 /D FPO=0 /FR /Fd".\objchk\ia64\mapmem.pdb" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i ".." /i "$(DRIVERWORKS)\include" /i "$(DRIVERWORKS)\source" /i "$(DRIVERWORKS)\include\dep_vxd" /i "$(DRIVERWORKS)\include\dep_wdm" /d "_DEBUG" /d NTVERSION=400
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i ".." /i "$(DRIVERWORKS)\include" /i "$(DRIVERWORKS)\source" /i "$(DRIVERWORKS)\include\dep_vxd" /i "$(DRIVERWORKS)\include\dep_wdm" /d "_DEBUG" /d NTVERSION=400
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 int64.lib ntoskrnl.lib hal.lib "$(DRIVERWORKS)\lib\IA64\checked\vdw.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:".\objchk\i386\mapmem.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ntoskrnl.lib hal.lib "$(DRIVERWORKS)\lib\IA64\checked\vdw.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\objchk\ia64\mapmem.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /machine:IA64 /debugtype:pdata
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mapmem - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mapmem___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "mapmem___Win32_IA64_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Oy /Gy /I ".." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD CPP /nologo /W3 /WX /Zi /Gy /I "." /I ".." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='400' /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D FPO=0 /FR /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i ".." /i "$(DRIVERWORKS)\include" /i "$(DRIVERWORKS)\source" /i "$(DRIVERWORKS)\include\dep_vxd" /i "$(DRIVERWORKS)\include\dep_wdm" /d "NDEBUG" /d NTVERSION=400
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i ".." /i "$(DRIVERWORKS)\include" /i "$(DRIVERWORKS)\source" /i "$(DRIVERWORKS)\include\dep_vxd" /i "$(DRIVERWORKS)\include\dep_wdm" /d "NDEBUG" /d NTVERSION=400
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 int64.lib ntoskrnl.lib hal.lib "$(DRIVERWORKS)\lib\IA64\free\vdw.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\i386\mapmem.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ntoskrnl.lib hal.lib "$(DRIVERWORKS)\lib\IA64\free\vdw.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\objfre\ia64\mapmem.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /machine:IA64 /debugtype:pdata
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mapmem - Win32 Free"

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
# ADD CPP /nologo /Gz /W3 /Zi /Oy /Gy /I ".." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i ".." /i "$(DRIVERWORKS)\include" /i "$(DRIVERWORKS)\source" /i "$(DRIVERWORKS)\include\dep_vxd" /i "$(DRIVERWORKS)\include\dep_wdm" /d "NDEBUG" /d NTVERSION=400
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib "$(DRIVERWORKS)\lib\i386\free\vdw.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\objfre\i386\mapmem.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objfre\i386\mapmem.sys
TargetName=mapmem
InputPath=.\objfre\i386\mapmem.sys
SOURCE="$(InputPath)"

".\objfre\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source,package,always $(TARGETPATH)

# End Custom Build

!ELSEIF  "$(CFG)" == "mapmem - Win32 Checked"

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
# ADD CPP /nologo /Gz /ML /W3 /Zi /Gy /I ".." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /FR /Fd".\objchk\i386\\mapmem.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i ".." /i "$(DRIVERWORKS)\include" /i "$(DRIVERWORKS)\source" /i "$(DRIVERWORKS)\include\dep_vxd" /i "$(DRIVERWORKS)\include\dep_wdm" /d "_DEBUG" /d NTVERSION=400
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib "$(DRIVERWORKS)\lib\i386\checked\vdw.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\objchk\i386\mapmem.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objchk\i386\mapmem.sys
TargetName=mapmem
InputPath=.\objchk\i386\mapmem.sys
SOURCE="$(InputPath)"

".\objchk\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source,package,always $(TARGETPATH)

# End Custom Build

!ENDIF 

# Begin Target

# Name "mapmem - Win32 IA64 Checked"
# Name "mapmem - Win32 IA64 Free"
# Name "mapmem - Win32 Free"
# Name "mapmem - Win32 Checked"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mapmem.cpp
DEP_CPP_MAPME=\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\common\include\stcinit.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\cpprt.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\cright.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_ndis\kndiscpp.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_ndis\KNdisList.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_ndis\KNdisSpinLock.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_vxd\vtoolscp.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_vxd\vxdntlib.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\dwcontrl.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394async.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394Irb.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394isoc.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kadapter.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kaddress.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\karray.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kbus.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kchecker.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kcontrol.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kcsq.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdevice.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdevque.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdispobj.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdmaxfer.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdmqcsq.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdmqex.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdpc.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdriver.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kerrlog.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kevent.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kfifo.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kfile.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kfilter.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kgenlock.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kheap.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kicount.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kimgsect.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kintrupt.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kiocparm.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kirp.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\klist.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\klower.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kmemory.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kmutex.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kndisvdw.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kpcicfg.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kphysdev.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kpnpdev.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kpnplow.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kquery.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kregkey.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kresreq.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ksemaphr.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ksfifo.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kspin.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kstdwmi.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ksysthrd.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ktimer.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ktrace.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kunitnam.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kustring.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kvirtualcom.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kvxdintf.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\Kwdmfltr.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kwmi.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kwmiblock.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kwmistr.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kworkitm.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ShFifo.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\vdw.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\source\shfifo.cpp"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wdm\wnet\1394.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wdm\wnet\wdm.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\afilter.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\csq.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ddk_ext.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ia64reg.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\mce.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ndis.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ntddk.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\xfilter.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\basetsd.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\bugcodes.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\evntrace.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\guiddef.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ia64reg.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\netevent.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntddndis.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntddser.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntdef.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntiologc.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntstatus.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\wmistr.h"\
	".\function.h"\
	".\mapmem.h"\
	".\mmdevice.h"\
	
# End Source File
# Begin Source File

SOURCE=.\mmdevice.cpp
DEP_CPP_MMDEV=\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\common\include\stcinit.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\cpprt.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\cright.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_ndis\kndiscpp.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_ndis\KNdisList.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_ndis\KNdisSpinLock.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_vxd\vtoolscp.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\include\dep_vxd\vxdntlib.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\dwcontrl.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394async.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394Irb.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\k1394isoc.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kadapter.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kaddress.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\karray.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kbus.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kchecker.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kcontrol.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kcsq.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdevice.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdevque.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdispobj.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdmaxfer.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdmqcsq.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdmqex.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdpc.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kdriver.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kerrlog.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kevent.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kfifo.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kfile.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kfilter.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kgenlock.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kheap.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kicount.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kimgsect.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kintrupt.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kiocparm.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kirp.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\klist.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\klower.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kmemory.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kmutex.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kndisvdw.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kpcicfg.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kphysdev.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kpnpdev.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kpnplow.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kquery.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kregkey.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kresreq.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ksemaphr.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ksfifo.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kspin.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kstdwmi.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ksysthrd.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ktimer.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ktrace.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kunitnam.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kustring.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kvirtualcom.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kvxdintf.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\Kwdmfltr.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kwmi.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kwmiblock.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kwmistr.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\kworkitm.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\ShFifo.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\Include\vdw.h"\
	"..\..\..\..\..\..\..\PROGRA~1\Compuware\DriverStudio\DriverWorks\source\shfifo.cpp"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wdm\wnet\1394.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wdm\wnet\wdm.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\afilter.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\csq.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ddk_ext.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ia64reg.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\mce.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ndis.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\ntddk.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\ddk\wnet\xfilter.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\basetsd.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\bugcodes.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\evntrace.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\guiddef.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ia64reg.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\netevent.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntddndis.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntddser.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntdef.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntiologc.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\ntstatus.h"\
	"..\..\..\..\..\..\..\WINDDK\3790\inc\wnet\wmistr.h"\
	"..\mmioctl.h"\
	".\function.h"\
	".\mmdevice.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\mapmem.h
# End Source File
# Begin Source File

SOURCE=.\mmdevice.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mapmem.rc
# End Source File
# End Group
# End Target
# End Project
