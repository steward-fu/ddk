# Microsoft Developer Studio Project File - Name="kbdclass" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kbdclass - Win32 IA64 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kbdclass.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kbdclass.mak" CFG="kbdclass - Win32 IA64 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kbdclass - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kbdclass - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kbdclass - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kbdclass - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kbdclass - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "kbdclass___Win32_IA64_Checked"
# PROP BASE Intermediate_Dir "kbdclass___Win32_IA64_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\ia64\"
# PROP Intermediate_Dir ".\objchk\ia64\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /ML /W3 /WX /Zi /Oi /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D DBG=1 /D FPO=0 /Fd".\objchk\ia64\\kbdclass.pdb" /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /i "..\inc"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /i "..\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objchk\i386/kbdclass.bsc"
# ADD BSC32 /nologo /o"objchk\i386/kbdclass.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wdm.lib ntoskrnl.lib "$(DRIVERWORKS)\lib\IA64\checked\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:".\objchk\ia64\kbdclass.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /machine:IA64 /debugtype:pdata
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kbdclass - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "kbdclass___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "kbdclass___Win32_IA64_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64\"
# PROP Intermediate_Dir ".\objfre\ia64\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /WX /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D "_WIN64" /D _IA64_=1 /D IA64=1 /D "NO_HW_DETECT" /D "_MSC_EXTENSIONS" /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x501 /D WINVER=0x501 /D _WIN32_IE=0x600 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D FPO=0 /Zel -cbstring /GF /Wp64 /Zel /QIA64_fr32 /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /i "..\inc" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /i "..\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objfre\i386/kbdclass.bsc"
# ADD BSC32 /nologo /o"objfre\i386/kbdclass.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wdm.lib ntoskrnl.lib "$(DRIVERWORKS)\lib\IA64\free\vdw_wdm.lib" /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\ia64\kbdclass.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /machine:IA64 /debugtype:pdata
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kbdclass - Win32 Free"

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
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /i "..\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objfre\i386/kbdclass.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 ntoskrnl.lib wdm.lib wmilib.lib $(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre\i386\kbdclass.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kbdclass - Win32 Checked"

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
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /FR /Fd".\objchk\i386\\kbdclass.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc\ddk\wdm" /i "$(BASEDIR)\inc\ddk" /i "$(BASEDIR)\inc" /i "$(BASEDIR)\inc\Win98" /i "..\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk\i386/kbdclass.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 ntoskrnl.lib wdm.lib wmilib.lib $(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:"objchk\i386\kbdclass.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "kbdclass - Win32 IA64 Checked"
# Name "kbdclass - Win32 IA64 Free"
# Name "kbdclass - Win32 Free"
# Name "kbdclass - Win32 Checked"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\kbdclass.cpp
	
# End Source File
# Begin Source File

SOURCE=.\kbdlog.mc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\kbdclass.h
# End Source File
# Begin Source File

SOURCE=.\newkbd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"
# Begin Source File

SOURCE=.\kbdclass.rc
# End Source File
# Begin Source File

SOURCE=.\kbdlog.mc

!IF  "$(CFG)" == "kbdclass - Win32 IA64 Checked"

# Begin Custom Build - Running message compiler
InputPath=.\kbdlog.mc

BuildCmds= \
	mc kbdlog.mc

"kbdlog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"kbdlog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "kbdclass - Win32 IA64 Free"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Running message compiler
InputPath=.\kbdlog.mc

BuildCmds= \
	mc kbdlog.mc

"kbdlog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"kbdlog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "kbdclass - Win32 Free"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Running message compiler
InputPath=.\kbdlog.mc

BuildCmds= \
	mc kbdlog.mc

"kbdlog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"kbdlog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "kbdclass - Win32 Checked"

# Begin Custom Build - Running message compiler
InputPath=.\kbdlog.mc

BuildCmds= \
	mc kbdlog.mc

"kbdlog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"kbdlog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
