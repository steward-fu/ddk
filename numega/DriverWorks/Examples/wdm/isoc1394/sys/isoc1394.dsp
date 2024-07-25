# Microsoft Developer Studio Project File - Name="Isoc1394" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Isoc1394 - Win32 IA64 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Isoc1394.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Isoc1394.mak" CFG="Isoc1394 - Win32 IA64 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Isoc1394 - Win32 IA64 Checked" (based on "Win32 (x86) Application")
!MESSAGE "Isoc1394 - Win32 IA64 Free" (based on "Win32 (x86) Application")
!MESSAGE "Isoc1394 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Isoc1394 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Isoc1394"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Isoc1394 - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Isoc1394___Win32_IA64_Checked"
# PROP BASE Intermediate_Dir "Isoc1394___Win32_IA64_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64\"
# PROP Intermediate_Dir ".\objchk\ia64\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /ML /W3 /WX /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D DBG=1 /D FPO=0 /Fd".\objchk\ia64\\isoc1394.pdb" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\IA64\checked\vdw_wdm.lib" usbd.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:".\objchk\ia64\isoc1394.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /machine:IA64 /debugtype:pdata /driver:WDM
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Isoc1394 - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Isoc1394___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "Isoc1394___Win32_IA64_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64\"
# PROP Intermediate_Dir ".\objfre\ia64\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /WX /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D FPO=0 /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib" usbd.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\ia64\isoc1394.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /machine:IA64 /debugtype:pdata /driver:WDM
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Isoc1394 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386\"
# PROP Intermediate_Dir ".\objfre\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\i386\isoc1394.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /driver:WDM
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Isoc1394 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386\"
# PROP Intermediate_Dir ".\objchk\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /FR /Fd".\objchk\i386\\Isoc1394.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:".\objchk\i386\Isoc1394.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /driver:WDM
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file Isoc1394.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objchk\i386\Isoc1394.sys
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Isoc1394 - Win32 IA64 Checked"
# Name "Isoc1394 - Win32 IA64 Free"
# Name "Isoc1394 - Win32 Release"
# Name "Isoc1394 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AsyncCommands.cpp
DEP_CPP_ASYNC=\
	"..\..\..\..\..\common\include\stcinit.h"\
	"..\..\..\..\Include\cpprt.h"\
	"..\..\..\..\Include\cright.h"\
	"..\..\..\..\include\dep_ndis\kndiscpp.h"\
	"..\..\..\..\include\dep_ndis\KNdisList.h"\
	"..\..\..\..\include\dep_ndis\KNdisSpinLock.h"\
	"..\..\..\..\include\dep_vxd\vtoolscp.h"\
	"..\..\..\..\include\dep_vxd\vxdntlib.h"\
	"..\..\..\..\Include\dwcontrl.h"\
	"..\..\..\..\Include\k1394.h"\
	"..\..\..\..\Include\k1394async.h"\
	"..\..\..\..\Include\k1394Irb.h"\
	"..\..\..\..\Include\k1394isoc.h"\
	"..\..\..\..\Include\kadapter.h"\
	"..\..\..\..\Include\kaddress.h"\
	"..\..\..\..\Include\karray.h"\
	"..\..\..\..\Include\kbus.h"\
	"..\..\..\..\Include\kchecker.h"\
	"..\..\..\..\Include\kcontrol.h"\
	"..\..\..\..\Include\kdevice.h"\
	"..\..\..\..\Include\kdevque.h"\
	"..\..\..\..\Include\kdispobj.h"\
	"..\..\..\..\Include\kdmaxfer.h"\
	"..\..\..\..\Include\kdmqex.h"\
	"..\..\..\..\Include\kdmqueue.h"\
	"..\..\..\..\Include\kdpc.h"\
	"..\..\..\..\Include\kdriver.h"\
	"..\..\..\..\Include\kerrlog.h"\
	"..\..\..\..\Include\kevent.h"\
	"..\..\..\..\Include\kfifo.h"\
	"..\..\..\..\Include\kfile.h"\
	"..\..\..\..\Include\kfilter.h"\
	"..\..\..\..\Include\kgenlock.h"\
	"..\..\..\..\Include\kheap.h"\
	"..\..\..\..\Include\kicount.h"\
	"..\..\..\..\Include\kimgsect.h"\
	"..\..\..\..\Include\kintrupt.h"\
	"..\..\..\..\Include\kiocparm.h"\
	"..\..\..\..\Include\kirp.h"\
	"..\..\..\..\Include\klist.h"\
	"..\..\..\..\Include\klower.h"\
	"..\..\..\..\Include\kmemory.h"\
	"..\..\..\..\Include\kmutex.h"\
	"..\..\..\..\Include\kndisvdw.h"\
	"..\..\..\..\Include\kpcicfg.h"\
	"..\..\..\..\Include\kphysdev.h"\
	"..\..\..\..\Include\kpnpdev.h"\
	"..\..\..\..\Include\kpnplow.h"\
	"..\..\..\..\Include\kquery.h"\
	"..\..\..\..\Include\kregkey.h"\
	"..\..\..\..\Include\kresreq.h"\
	"..\..\..\..\Include\ksemaphr.h"\
	"..\..\..\..\Include\ksfifo.h"\
	"..\..\..\..\Include\kspin.h"\
	"..\..\..\..\Include\kstdwmi.h"\
	"..\..\..\..\Include\ksysthrd.h"\
	"..\..\..\..\Include\ktimer.h"\
	"..\..\..\..\Include\ktrace.h"\
	"..\..\..\..\Include\kunitnam.h"\
	"..\..\..\..\Include\kustring.h"\
	"..\..\..\..\Include\kvxdintf.h"\
	"..\..\..\..\Include\Kwdmfltr.h"\
	"..\..\..\..\Include\kwmi.h"\
	"..\..\..\..\Include\kwmiblock.h"\
	"..\..\..\..\Include\kwmistr.h"\
	"..\..\..\..\Include\kworkitm.h"\
	"..\..\..\..\Include\ShFifo.h"\
	"..\..\..\..\Include\vdw.h"\
	"..\..\..\..\source\shfifo.cpp"\
	".\AsyncCommands.h"\
	".\function.h"\
	"D:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\1394.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\mce.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\afilter.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ndis.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ntddk.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\xfilter.h"\
	"D:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"D:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"D:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\wxp\netevent.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntddndis.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"D:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Isoc1394.cpp
DEP_CPP_ISOC1=\
	"..\..\..\..\..\common\include\stcinit.h"\
	"..\..\..\..\Include\cpprt.h"\
	"..\..\..\..\Include\cright.h"\
	"..\..\..\..\include\dep_ndis\kndiscpp.h"\
	"..\..\..\..\include\dep_ndis\KNdisList.h"\
	"..\..\..\..\include\dep_ndis\KNdisSpinLock.h"\
	"..\..\..\..\include\dep_vxd\vtoolscp.h"\
	"..\..\..\..\include\dep_vxd\vxdntlib.h"\
	"..\..\..\..\Include\dwcontrl.h"\
	"..\..\..\..\Include\k1394.h"\
	"..\..\..\..\Include\k1394async.h"\
	"..\..\..\..\Include\k1394Irb.h"\
	"..\..\..\..\Include\k1394isoc.h"\
	"..\..\..\..\Include\kadapter.h"\
	"..\..\..\..\Include\kaddress.h"\
	"..\..\..\..\Include\karray.h"\
	"..\..\..\..\Include\kbus.h"\
	"..\..\..\..\Include\kchecker.h"\
	"..\..\..\..\Include\kcontrol.h"\
	"..\..\..\..\Include\kdevice.h"\
	"..\..\..\..\Include\kdevque.h"\
	"..\..\..\..\Include\kdispobj.h"\
	"..\..\..\..\Include\kdmaxfer.h"\
	"..\..\..\..\Include\kdmqex.h"\
	"..\..\..\..\Include\kdmqueue.h"\
	"..\..\..\..\Include\kdpc.h"\
	"..\..\..\..\Include\kdriver.h"\
	"..\..\..\..\Include\kerrlog.h"\
	"..\..\..\..\Include\kevent.h"\
	"..\..\..\..\Include\kfifo.h"\
	"..\..\..\..\Include\kfile.h"\
	"..\..\..\..\Include\kfilter.h"\
	"..\..\..\..\Include\kgenlock.h"\
	"..\..\..\..\Include\kheap.h"\
	"..\..\..\..\Include\kicount.h"\
	"..\..\..\..\Include\kimgsect.h"\
	"..\..\..\..\Include\kintrupt.h"\
	"..\..\..\..\Include\kiocparm.h"\
	"..\..\..\..\Include\kirp.h"\
	"..\..\..\..\Include\klist.h"\
	"..\..\..\..\Include\klower.h"\
	"..\..\..\..\Include\kmemory.h"\
	"..\..\..\..\Include\kmutex.h"\
	"..\..\..\..\Include\kndisvdw.h"\
	"..\..\..\..\Include\kpcicfg.h"\
	"..\..\..\..\Include\kphysdev.h"\
	"..\..\..\..\Include\kpnpdev.h"\
	"..\..\..\..\Include\kpnplow.h"\
	"..\..\..\..\Include\kquery.h"\
	"..\..\..\..\Include\kregkey.h"\
	"..\..\..\..\Include\kresreq.h"\
	"..\..\..\..\Include\ksemaphr.h"\
	"..\..\..\..\Include\ksfifo.h"\
	"..\..\..\..\Include\kspin.h"\
	"..\..\..\..\Include\kstdwmi.h"\
	"..\..\..\..\Include\ksysthrd.h"\
	"..\..\..\..\Include\ktimer.h"\
	"..\..\..\..\Include\ktrace.h"\
	"..\..\..\..\Include\kunitnam.h"\
	"..\..\..\..\Include\kustring.h"\
	"..\..\..\..\Include\kvxdintf.h"\
	"..\..\..\..\Include\Kwdmfltr.h"\
	"..\..\..\..\Include\kwmi.h"\
	"..\..\..\..\Include\kwmiblock.h"\
	"..\..\..\..\Include\kwmistr.h"\
	"..\..\..\..\Include\kworkitm.h"\
	"..\..\..\..\Include\ShFifo.h"\
	"..\..\..\..\Include\vdw.h"\
	"..\..\..\..\source\shfifo.cpp"\
	"..\Isoc1394types.h"\
	".\AsyncCommands.h"\
	".\function.h"\
	".\Isoc1394.h"\
	".\Isoc1394Device.h"\
	".\isocClient.h"\
	"D:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\1394.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\mce.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\afilter.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ndis.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ntddk.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\xfilter.h"\
	"D:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"D:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"D:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\wxp\netevent.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntddndis.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"D:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Isoc1394.rc
# End Source File
# Begin Source File

SOURCE=.\Isoc1394Device.cpp
DEP_CPP_ISOC13=\
	"..\..\..\..\..\common\include\stcinit.h"\
	"..\..\..\..\Include\cpprt.h"\
	"..\..\..\..\Include\cright.h"\
	"..\..\..\..\include\dep_ndis\kndiscpp.h"\
	"..\..\..\..\include\dep_ndis\KNdisList.h"\
	"..\..\..\..\include\dep_ndis\KNdisSpinLock.h"\
	"..\..\..\..\include\dep_vxd\vtoolscp.h"\
	"..\..\..\..\include\dep_vxd\vxdntlib.h"\
	"..\..\..\..\Include\dwcontrl.h"\
	"..\..\..\..\Include\k1394.h"\
	"..\..\..\..\Include\k1394async.h"\
	"..\..\..\..\Include\k1394Irb.h"\
	"..\..\..\..\Include\k1394isoc.h"\
	"..\..\..\..\Include\kadapter.h"\
	"..\..\..\..\Include\kaddress.h"\
	"..\..\..\..\Include\karray.h"\
	"..\..\..\..\Include\kbus.h"\
	"..\..\..\..\Include\kchecker.h"\
	"..\..\..\..\Include\kcontrol.h"\
	"..\..\..\..\Include\kdevice.h"\
	"..\..\..\..\Include\kdevque.h"\
	"..\..\..\..\Include\kdispobj.h"\
	"..\..\..\..\Include\kdmaxfer.h"\
	"..\..\..\..\Include\kdmqex.h"\
	"..\..\..\..\Include\kdmqueue.h"\
	"..\..\..\..\Include\kdpc.h"\
	"..\..\..\..\Include\kdriver.h"\
	"..\..\..\..\Include\kerrlog.h"\
	"..\..\..\..\Include\kevent.h"\
	"..\..\..\..\Include\kfifo.h"\
	"..\..\..\..\Include\kfile.h"\
	"..\..\..\..\Include\kfilter.h"\
	"..\..\..\..\Include\kgenlock.h"\
	"..\..\..\..\Include\kheap.h"\
	"..\..\..\..\Include\kicount.h"\
	"..\..\..\..\Include\kimgsect.h"\
	"..\..\..\..\Include\kintrupt.h"\
	"..\..\..\..\Include\kiocparm.h"\
	"..\..\..\..\Include\kirp.h"\
	"..\..\..\..\Include\klist.h"\
	"..\..\..\..\Include\klower.h"\
	"..\..\..\..\Include\kmemory.h"\
	"..\..\..\..\Include\kmutex.h"\
	"..\..\..\..\Include\kndisvdw.h"\
	"..\..\..\..\Include\kpcicfg.h"\
	"..\..\..\..\Include\kphysdev.h"\
	"..\..\..\..\Include\kpnpdev.h"\
	"..\..\..\..\Include\kpnplow.h"\
	"..\..\..\..\Include\kquery.h"\
	"..\..\..\..\Include\kregkey.h"\
	"..\..\..\..\Include\kresreq.h"\
	"..\..\..\..\Include\ksemaphr.h"\
	"..\..\..\..\Include\ksfifo.h"\
	"..\..\..\..\Include\kspin.h"\
	"..\..\..\..\Include\kstdwmi.h"\
	"..\..\..\..\Include\ksysthrd.h"\
	"..\..\..\..\Include\ktimer.h"\
	"..\..\..\..\Include\ktrace.h"\
	"..\..\..\..\Include\kunitnam.h"\
	"..\..\..\..\Include\kustring.h"\
	"..\..\..\..\Include\kvxdintf.h"\
	"..\..\..\..\Include\Kwdmfltr.h"\
	"..\..\..\..\Include\kwmi.h"\
	"..\..\..\..\Include\kwmiblock.h"\
	"..\..\..\..\Include\kwmistr.h"\
	"..\..\..\..\Include\kworkitm.h"\
	"..\..\..\..\Include\ShFifo.h"\
	"..\..\..\..\Include\vdw.h"\
	"..\..\..\..\source\shfifo.cpp"\
	"..\Isoc1394DeviceInterface.h"\
	"..\Isoc1394ioctl.h"\
	"..\Isoc1394types.h"\
	".\AsyncCommands.h"\
	".\function.h"\
	".\Isoc1394.h"\
	".\Isoc1394Device.h"\
	".\isocClient.h"\
	"D:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\1394.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\mce.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\afilter.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ndis.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ntddk.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\xfilter.h"\
	"D:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"D:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"D:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\wxp\netevent.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntddndis.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"D:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# Begin Source File

SOURCE=.\isocClient.cpp
DEP_CPP_ISOCC=\
	"..\..\..\..\..\common\include\stcinit.h"\
	"..\..\..\..\Include\cpprt.h"\
	"..\..\..\..\Include\cright.h"\
	"..\..\..\..\include\dep_ndis\kndiscpp.h"\
	"..\..\..\..\include\dep_ndis\KNdisList.h"\
	"..\..\..\..\include\dep_ndis\KNdisSpinLock.h"\
	"..\..\..\..\include\dep_vxd\vtoolscp.h"\
	"..\..\..\..\include\dep_vxd\vxdntlib.h"\
	"..\..\..\..\Include\dwcontrl.h"\
	"..\..\..\..\Include\k1394.h"\
	"..\..\..\..\Include\k1394async.h"\
	"..\..\..\..\Include\k1394Irb.h"\
	"..\..\..\..\Include\k1394isoc.h"\
	"..\..\..\..\Include\kadapter.h"\
	"..\..\..\..\Include\kaddress.h"\
	"..\..\..\..\Include\karray.h"\
	"..\..\..\..\Include\kbus.h"\
	"..\..\..\..\Include\kchecker.h"\
	"..\..\..\..\Include\kcontrol.h"\
	"..\..\..\..\Include\kdevice.h"\
	"..\..\..\..\Include\kdevque.h"\
	"..\..\..\..\Include\kdispobj.h"\
	"..\..\..\..\Include\kdmaxfer.h"\
	"..\..\..\..\Include\kdmqex.h"\
	"..\..\..\..\Include\kdmqueue.h"\
	"..\..\..\..\Include\kdpc.h"\
	"..\..\..\..\Include\kdriver.h"\
	"..\..\..\..\Include\kerrlog.h"\
	"..\..\..\..\Include\kevent.h"\
	"..\..\..\..\Include\kfifo.h"\
	"..\..\..\..\Include\kfile.h"\
	"..\..\..\..\Include\kfilter.h"\
	"..\..\..\..\Include\kgenlock.h"\
	"..\..\..\..\Include\kheap.h"\
	"..\..\..\..\Include\kicount.h"\
	"..\..\..\..\Include\kimgsect.h"\
	"..\..\..\..\Include\kintrupt.h"\
	"..\..\..\..\Include\kiocparm.h"\
	"..\..\..\..\Include\kirp.h"\
	"..\..\..\..\Include\klist.h"\
	"..\..\..\..\Include\klower.h"\
	"..\..\..\..\Include\kmemory.h"\
	"..\..\..\..\Include\kmutex.h"\
	"..\..\..\..\Include\kndisvdw.h"\
	"..\..\..\..\Include\kpcicfg.h"\
	"..\..\..\..\Include\kphysdev.h"\
	"..\..\..\..\Include\kpnpdev.h"\
	"..\..\..\..\Include\kpnplow.h"\
	"..\..\..\..\Include\kquery.h"\
	"..\..\..\..\Include\kregkey.h"\
	"..\..\..\..\Include\kresreq.h"\
	"..\..\..\..\Include\ksemaphr.h"\
	"..\..\..\..\Include\ksfifo.h"\
	"..\..\..\..\Include\kspin.h"\
	"..\..\..\..\Include\kstdwmi.h"\
	"..\..\..\..\Include\ksysthrd.h"\
	"..\..\..\..\Include\ktimer.h"\
	"..\..\..\..\Include\ktrace.h"\
	"..\..\..\..\Include\kunitnam.h"\
	"..\..\..\..\Include\kustring.h"\
	"..\..\..\..\Include\kvxdintf.h"\
	"..\..\..\..\Include\Kwdmfltr.h"\
	"..\..\..\..\Include\kwmi.h"\
	"..\..\..\..\Include\kwmiblock.h"\
	"..\..\..\..\Include\kwmistr.h"\
	"..\..\..\..\Include\kworkitm.h"\
	"..\..\..\..\Include\ShFifo.h"\
	"..\..\..\..\Include\vdw.h"\
	"..\..\..\..\source\shfifo.cpp"\
	"..\Isoc1394types.h"\
	".\AsyncCommands.h"\
	".\function.h"\
	".\Isoc1394Device.h"\
	".\isocClient.h"\
	"D:\WINDDK\2600.1106\inc\crt\basetsd.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\1394.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\mce.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wdm\wxp\wdm.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\afilter.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ndis.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\ntddk.h"\
	"D:\WINDDK\2600.1106\inc\ddk\wxp\xfilter.h"\
	"D:\WINDDK\2600.1106\inc\wxp\bugcodes.h"\
	"D:\WINDDK\2600.1106\inc\wxp\evntrace.h"\
	"D:\WINDDK\2600.1106\inc\wxp\guiddef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ia64reg.h"\
	"D:\WINDDK\2600.1106\inc\wxp\netevent.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntddndis.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntdef.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntiologc.h"\
	"D:\WINDDK\2600.1106\inc\wxp\ntstatus.h"\
	"D:\WINDDK\2600.1106\inc\wxp\wmistr.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AsyncCommands.h
# End Source File
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\Isoc1394.h
# End Source File
# Begin Source File

SOURCE=.\Isoc1394Device.h
# End Source File
# Begin Source File

SOURCE=.\isocClient.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
