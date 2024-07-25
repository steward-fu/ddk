# Microsoft Developer Studio Project File - Name="HotII" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=HotII - Win32 WDM Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HotPCI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HotPCI.mak" CFG="HotII - Win32 WDM Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HotII - Win32 NT 4 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HotII - Win32 NT 4 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HotII - Win32 WDM Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HotII - Win32 WDM Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HotII - Win32 NT 4 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "HotII___"
# PROP BASE Intermediate_Dir "HotII___"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386\"
# PROP Intermediate_Dir ".\objchk\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd".\objchk\i386\\HotPci.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "_DEBUG"
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk/i386/HotPCI.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib $(DRIVERWORKS)\lib\i386\checked\vdw.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:"objchk\i386\HotPci.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objchk\i386\HotPci.sys
TargetName=HotPci
InputPath=.\objchk\i386\HotPci.sys
SOURCE="$(InputPath)"

".\objchk\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source,package,always $(TARGETPATH)

# End Custom Build

!ELSEIF  "$(CFG)" == "HotII - Win32 NT 4 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "HotII__0"
# PROP BASE Intermediate_Dir "HotII__0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386\"
# PROP Intermediate_Dir ".\objfre\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objfre/i386/HotPCI.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib $(DRIVERWORKS)\lib\i386\free\vdw.lib ntstrsafe.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre\i386\HotPci.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objfre\i386\HotPci.sys
TargetName=HotPci
InputPath=.\objfre\i386\HotPci.sys
SOURCE="$(InputPath)"

".\objfre\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source,package,always $(TARGETPATH)

# End Custom Build

!ELSEIF  "$(CFG)" == "HotII - Win32 WDM Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "HotII__1"
# PROP BASE Intermediate_Dir "HotII__1"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386\"
# PROP Intermediate_Dir ".\objfre\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objfre/i386/HotPCI.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib $(DRIVERWORKS)\lib\i386\free\vdw_wdm.lib wdm.lib ntstrsafe.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre\i386\HotPci.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /driver:WDM
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objfre\i386\HotPci.sys
TargetName=HotPci
InputPath=.\objfre\i386\HotPci.sys
SOURCE="$(InputPath)"

".\objfre\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source,package,always $(TARGETPATH)

# End Custom Build

!ELSEIF  "$(CFG)" == "HotII - Win32 WDM Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "HotII__2"
# PROP BASE Intermediate_Dir "HotII__2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386\"
# PROP Intermediate_Dir ".\objchk\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Zi /Oi /Gy /I "." /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(BASEDIR)\src\hid\inc" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd".\objchk\i386\\HotPci.pdb" /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "_DEBUG"
# ADD RSC /l 0x409 /i "$(DRIVERWORKS)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk/i386/HotPCI.bsc"
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib $(DRIVERWORKS)\lib\i386\checked\vdw_wdm.lib wdm.lib /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:"objchk\i386\HotPci.sys" /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /driver:WDM
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objchk\i386\HotPci.sys
TargetName=HotPci
InputPath=.\objchk\i386\HotPci.sys
SOURCE="$(InputPath)"

".\objchk\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source,package,always $(TARGETPATH)

# End Custom Build

!ENDIF 

# Begin Target

# Name "HotII - Win32 NT 4 Checked"
# Name "HotII - Win32 NT 4 Free"
# Name "HotII - Win32 WDM Free"
# Name "HotII - Win32 WDM Checked"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HotPciDev.cpp
	
# End Source File
# Begin Source File

SOURCE=.\HotPciDrv.cpp
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\HotPciCmn.h
# End Source File
# Begin Source File

SOURCE=.\HotPciDev.h
# End Source File
# Begin Source File

SOURCE=.\HotPciDrv.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HotPci.rc
# End Source File
# Begin Source File

SOURCE=.\HotPciLog.mc

!IF  "$(CFG)" == "HotII - Win32 NT 4 Checked"

# Begin Custom Build - Running message compiler . . .
InputPath=.\HotPciLog.mc

BuildCmds= \
	mc -v HotPciLog.mc

"hotpcilog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"hotpcilog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "HotII - Win32 NT 4 Free"

# Begin Custom Build - Running message compiler . . .
InputPath=.\HotPciLog.mc

BuildCmds= \
	mc -v HotPciLog.mc

"hotpcilog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"hotpcilog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "HotII - Win32 WDM Free"

# Begin Custom Build - Running message compiler . . .
InputPath=.\HotPciLog.mc

BuildCmds= \
	mc -v HotPciLog.mc

"hotpcilog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"hotpcilog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "HotII - Win32 WDM Checked"

# Begin Custom Build - Running message compiler . . .
InputPath=.\HotPciLog.mc

BuildCmds= \
	mc -v HotPciLog.mc

"hotpcilog.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"hotpcilog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\HotPci.inf
# End Source File
# End Target
# End Project
