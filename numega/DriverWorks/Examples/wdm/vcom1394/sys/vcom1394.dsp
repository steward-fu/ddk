# Microsoft Developer Studio Project File - Name="vcom1394" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=vcom1394 - Win32 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VCOM1394.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VCOM1394.mak" CFG="vcom1394 - Win32 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vcom1394 - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vcom1394 - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vcom1394 - Win32 Free"

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
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "." /I "$(BASEDIR)\inc\win98" /I ".." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /FR /YX /Fd".\objfre\i386\vcom1394.pdb" /FD /Oxs /QI0f /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(CRT_INC_PATH)" /i "$(DDK_INC_PATH)" /i "$(WDM_INC_PATH)" /i "$(DRIVERWORKS)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /map /machine:IX86 /nodefaultlib /out:".\objfre\i386\vcom1394.sys" /libpath:"$(TARGET_LIB_PATH)" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file vcom1394.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objfre\i386\vcom1394.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "vcom1394 - Win32 Checked"

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
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I ".." /I "." /I "$(BASEDIR)\inc\win98" /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /D WIN32=100 /D "RDRDBG" /D "SRVDBG" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION='WDM' /FR /YX /Fd".\objchk\i386\vcom1394.pdb" /FD /QI0f /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(CRT_INC_PATH)" /i "$(DDK_INC_PATH)" /i "$(WDM_INC_PATH)" /i "$(DRIVERWORKS)\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 wdm.lib "$(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /map /machine:IX86 /nodefaultlib /out:".\objchk\i386\vcom1394.sys" /libpath:"$(TARGET_LIB_PATH)" /driver:WDM /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:5.00 /subsystem:native,1.10
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Generating SoftICE Symbol file vcom1394.nms
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /translate:source,package,always .\objchk\i386\vcom1394.sys
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "vcom1394 - Win32 Free"
# Name "vcom1394 - Win32 Checked"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\vcom1394bus.cpp

!IF  "$(CFG)" == "vcom1394 - Win32 Free"

!ELSEIF  "$(CFG)" == "vcom1394 - Win32 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vcom1394driver.cpp

!IF  "$(CFG)" == "vcom1394 - Win32 Free"

!ELSEIF  "$(CFG)" == "vcom1394 - Win32 Checked"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vcom1394pdo.cpp

!IF  "$(CFG)" == "vcom1394 - Win32 Free"

!ELSEIF  "$(CFG)" == "vcom1394 - Win32 Checked"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\vcom1394bus.h
# End Source File
# Begin Source File

SOURCE=.\vcom1394driver.h
# End Source File
# Begin Source File

SOURCE=.\vcom1394pdo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# Begin Source File

SOURCE=.\vcom1394.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\vcom.inf
# End Source File
# Begin Source File

SOURCE=.\vcom1394.inf
# End Source File
# End Target
# End Project
