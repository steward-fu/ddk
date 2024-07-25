# Microsoft Developer Studio Project File - Name="NmPacket" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NmPacket - Win32 NDIS 5_1 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NmPacket.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NmPacket.mak" CFG="NmPacket - Win32 NDIS 5_1 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NmPacket - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NmPacket - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NmPacket - Win32 NT4 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NmPacket - Win32 NT4 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NmPacket - Win32 NDIS 5_1 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NmPacket - Win32 NDIS 5_1 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NmPacket - Win32 Free"

# PROP BASE Use_MFC 4
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre\i386"
# PROP Intermediate_Dir "objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS50=1 /FR /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\free\vdw.lib" hal.lib ntoskrnl.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre\i386\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "NmPacket - Win32 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Checked"
# PROP BASE Intermediate_Dir "Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk\i386"
# PROP Intermediate_Dir "objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Od /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D DBG=1 /D FPO=0 /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS50=1 /FR /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\checked\vdw.lib" ntoskrnl.lib hal.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:"objchk\i386\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\i386
TargetPath=.\objchk\i386\NmPacket.sys
TargetName=NmPacket
InputPath=.\objchk\i386\NmPacket.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NT4_Checked"
# PROP BASE Intermediate_Dir "NT4_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk\nt4"
# PROP Intermediate_Dir "objchk\nt4"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Od /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D DBG=1 /D FPO=0 /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS40=1 /FR /YX /Fd"nmpacket.pdb" /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objchk\i386\NmPacket.bsc"
# ADD BSC32 /nologo /o"objchk\i386\NmPacket.bsc"
LINK32=link.exe
# ADD BASE LINK32 "$(DRIVERWORKS)\lib\i386\checked\vdw.lib" int64.lib ntoskrnl.lib hal.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:"objchk\i386\NmPacket.sys" /libpath:"$(BASEDIR)\libchk\i386" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(BASEDIR)\lib\i386" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\checked\vdw.lib" int64.lib ntoskrnl.lib hal.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:"objchk\nt4\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\nt4
TargetPath=.\objchk\nt4\NmPacket.sys
TargetName=NmPacket
InputPath=.\objchk\nt4\NmPacket.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NT4_Free"
# PROP BASE Intermediate_Dir "NT4_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objfre\nt4"
# PROP Intermediate_Dir "objfre\nt4"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS40=1 /FR /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"objfre\i386\NmPacket.bsc"
# ADD BSC32 /nologo /o"objfre\i386\NmPacket.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\free\vdw.lib" ntoskrnl.lib hal.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /machine:IX86 /nodefaultlib /out:"objfre\nt4\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "NmPacket___Win32_NDIS_5_1_Checked"
# PROP BASE Intermediate_Dir "NmPacket___Win32_NDIS_5_1_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objchk51\i386"
# PROP Intermediate_Dir "objchk51\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Z7 /Oi /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D DBG=1 /D FPO=0 /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS50=1 /FR /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Zi /Od /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D DBG=1 /D FPO=0 /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS51=1 /FR /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /FA<none>
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 "$(DRIVERWORKS)\lib\i386\checked\vdw.lib" ntoskrnl.lib hal.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:"objchk\i386\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\checked\vdw.lib" ntoskrnl.lib hal.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:"objchk51\i386\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk51\i386
TargetPath=.\objchk51\i386\NmPacket.sys
TargetName=NmPacket
InputPath=.\objchk51\i386\NmPacket.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NmPacket___Win32_NDIS_5_1_Free"
# PROP BASE Intermediate_Dir "NmPacket___Win32_NDIS_5_1_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objfre51\i386\"
# PROP Intermediate_Dir "objfre51\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS50=1 /FR /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Zi /Oy /Gy /I "$(DRIVERWORKS)\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\include" /I "." /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D NTVERSION=400 /D NDIS51=1 /FR /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 "$(DRIVERWORKS)\lib\i386\free\vdw.lib" hal.lib ntoskrnl.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre\i386\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 "$(DRIVERWORKS)\lib\i386\free\vdw.lib" hal.lib ntoskrnl.lib ndis.lib kndis5pt.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"objfre51\i386\NmPacket.sys" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:ICF /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "NmPacket - Win32 Free"
# Name "NmPacket - Win32 Checked"
# Name "NmPacket - Win32 NT4 Checked"
# Name "NmPacket - Win32 NT4 Free"
# Name "NmPacket - Win32 NDIS 5_1 Checked"
# Name "NmPacket - Win32 NDIS 5_1 Free"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\NmBindingPnp.cpp

!IF  "$(CFG)" == "NmPacket - Win32 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NmPacket.cpp

!IF  "$(CFG)" == "NmPacket - Win32 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NmPacketBinding.cpp

!IF  "$(CFG)" == "NmPacket - Win32 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NmPacketControlDevice.cpp

!IF  "$(CFG)" == "NmPacket - Win32 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NmPacketDevice.cpp

!IF  "$(CFG)" == "NmPacket - Win32 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NT4 Free"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Checked"

!ELSEIF  "$(CFG)" == "NmPacket - Win32 NDIS 5_1 Free"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\Characteristics.h
# End Source File
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\NmPacket.h
# End Source File
# Begin Source File

SOURCE=.\NmPacketBinding.h
# End Source File
# Begin Source File

SOURCE=.\NmPacketControlDevice.h
# End Source File
# Begin Source File

SOURCE=.\NmPacketDevice.h
# End Source File
# Begin Source File

SOURCE=.\NmPacketioctl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc"
# Begin Source File

SOURCE=.\NmPacket.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\NmPacket.inf
# End Source File
# Begin Source File

SOURCE=.\OEMSETUP.INF
# End Source File
# End Target
# End Project
