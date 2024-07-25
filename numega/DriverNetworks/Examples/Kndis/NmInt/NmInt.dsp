# Microsoft Developer Studio Project File - Name="nmint" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=nmint - Win32 NDIS 5 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NmInt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NmInt.mak" CFG="nmint - Win32 NDIS 5 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nmint - Win32 NDIS 5 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nmint - Win32 NDIS 5 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nmint - Win32 NT4 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nmint - Win32 NT4 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nmint - Win32 NDIS 5 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "nmint___Win32_NDIS_5_Checked"
# PROP BASE Intermediate_Dir "nmint___Win32_NDIS_5_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386\"
# PROP Intermediate_Dir ".\objchk\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Z7 /Oi /Ob2 /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /FI"$(BASEDIR)\inc\warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Zi /Od /Oi /Gy /I "..\..\..\..\common\include\stl" /I "$(DRIVERNETWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /FI"warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D NDIS50=1 /D KNDIS_W9X_COMPATIBLE=0 /FR /YX /Fd"objchk/i386/NmInt.pdb" /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 $(BASEDIR)\lib\i386\checked\ndis.lib int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\i386\checked4\NmInt.sys" /libpath:"$(BASEDIR)\libchk\i386" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# ADD LINK32 ndis.lib int64.lib ntoskrnl.lib hal.lib kndis5mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:".\objchk\i386\nmint.sys" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /libpath:"$(TARGET_LIB_PATH)" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\i386
TargetPath=.\objchk\i386\nmint.sys
TargetName=nmint
InputPath=.\objchk\i386\nmint.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "nmint - Win32 NDIS 5 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "nmint___Win32_NDIS_5_Free"
# PROP BASE Intermediate_Dir "nmint___Win32_NDIS_5_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386\"
# PROP Intermediate_Dir ".\objfre\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /FI"$(BASEDIR)\inc\warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /WX /Oy /Ob2 /Gy /I "..\..\..\..\common\include\stl" /I "$(DRIVERNETWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /FI"warning.h" /D FPO=1 /D "_IDWBUILD" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS50_MINIPORT=1 /D NDIS50=1 /D KNDIS_W9X_COMPATIBLE=0 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\include" /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /d "NDEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 $(BASEDIR)\lib\i386\free\ndis.lib int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /machine:IX86 /nodefaultlib /out:".\i386\free4\NmInt.sys" /libpath:"$(BASEDIR)\libfre\i386" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# ADD LINK32 ndis.lib int64.lib ntoskrnl.lib hal.lib kndis5mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:".\objfre\i386\NmInt.sys" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /libpath:"$(TARGET_LIB_PATH)" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objfre\i386
TargetPath=.\objfre\i386\NmInt.sys
TargetName=NmInt
InputPath=.\objfre\i386\NmInt.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "nmint___Win32_NT4_Checked"
# PROP BASE Intermediate_Dir "nmint___Win32_NT4_Checked"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\nt4\"
# PROP Intermediate_Dir ".\objchk\nt4\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Z7 /Oi /Ob2 /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /FI"$(BASEDIR)\inc\warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /Zi /Od /Oi /Gy /I "..\..\..\..\common\include\stl" /I "$(DRIVERNETWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /FI"warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /D NDIS40=1 /D KNDIS_W9X_COMPATIBLE=0 /FR /YX /Fd"objchk/i386/NmInt.pdb" /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
# ADD RSC /l 0x409 /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 $(BASEDIR)\lib\i386\checked\ndis.lib int64.lib ntoskrnl.lib hal.lib kndis4mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\i386\checked4\NmInt.sys" /libpath:"$(BASEDIR)\libchk\i386" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# ADD LINK32 ndis.lib int64.lib ntoskrnl.lib hal.lib kndis4mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /map /debug /machine:IX86 /nodefaultlib /out:".\objchk\nt4\nmint.sys" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /libpath:"$(TARGET_LIB_PATH)" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objchk\nt4
TargetPath=.\objchk\nt4\nmint.sys
TargetName=nmint
InputPath=.\objchk\nt4\nmint.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "nmint___Win32_NT4_Free"
# PROP BASE Intermediate_Dir "nmint___Win32_NT4_Free"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\nt4\"
# PROP Intermediate_Dir ".\objfre\nt4\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /WX /Z7 /Oi /Oy /Ob2 /Gy /I "$(DRIVERNETWORKS)\include" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\..\..\common\include\stl" /FI"$(BASEDIR)\inc\warning.h" /D "_DEBUG" /D DBG=1 /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /D FPO=1 /D "_IDWBUILD" /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD CPP /nologo /Gz /W3 /WX /Zi /Oy /Ob2 /Gy /I "..\..\..\..\common\include\stl" /I "$(DRIVERNETWORKS)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /FI"warning.h" /D FPO=0 /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D NDIS40_MINIPORT=1 /D NDIS40=1 /D KNDIS_W9X_COMPATIBLE=0 /D FPO=1 /D "_IDWBUILD" /FR /YX /Fd"objchk/i386/NmInt.pdb" /FD /Zel -cbstring /QIfdiv- /QIf /GF /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /D "NDEBUG" /mktyplib203 /o "NUL" /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /D "NDEBUG" /mktyplib203 /o "NUL" /win32 /win32
# ADD BASE RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\include" /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER" /d "NDEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "NDIS_MINIPORT_DRIVER" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 $(BASEDIR)\lib\i386\checked\ndis.lib int64.lib ntoskrnl.lib hal.lib kndis4mp.lib $(BASEDIR)\lib\i386\free\ndis.lib int64.lib ntoskrnl.lib hal.lib kndis4mp.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:".\i386\free4\NmInt.sys" /libpath:"$(BASEDIR)\libchk\i386" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /libpath:"$(BASEDIR)\libfre\i386" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# ADD LINK32 ndis.lib int64.lib ntoskrnl.lib hal.lib kndis4mp.lib ndis.lib int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /map /debug /machine:IX86 /nodefaultlib /out:".\objfre\nt4\NmInt.sys" /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /libpath:"$(TARGET_LIB_PATH)" /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native /driver /debug:FULL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none /incremental:yes
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\objfre\nt4
TargetPath=.\objfre\nt4\NmInt.sys
TargetName=NmInt
InputPath=.\objfre\nt4\NmInt.sys
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Build Under Environment:
PostBuild_Cmds=echo BASEDIR $(BASEDIR)	echo DRIVERNETWORKS $(DRIVERNETWORKS)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "nmint - Win32 NDIS 5 Checked"
# Name "nmint - Win32 NDIS 5 Free"
# Name "nmint - Win32 NT4 Checked"
# Name "nmint - Win32 NT4 Free"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\NmIntAdap.cpp

!IF  "$(CFG)" == "nmint - Win32 NDIS 5 Checked"

!ELSEIF  "$(CFG)" == "nmint - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NmIntBind.cpp

!IF  "$(CFG)" == "nmint - Win32 NDIS 5 Checked"

!ELSEIF  "$(CFG)" == "nmint - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NmIntDrv.cpp

!IF  "$(CFG)" == "nmint - Win32 NDIS 5 Checked"

!ELSEIF  "$(CFG)" == "nmint - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Checked"

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Free"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NmIntOids.cpp

!IF  "$(CFG)" == "nmint - Win32 NDIS 5 Checked"

# ADD CPP /Og /Oi /Ob1

!ELSEIF  "$(CFG)" == "nmint - Win32 NDIS 5 Free"

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Checked"

# ADD CPP /Og /Oi /Ob1

!ELSEIF  "$(CFG)" == "nmint - Win32 NT4 Free"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\Characteristics.h
# End Source File
# Begin Source File

SOURCE=.\NmIntAdap.h
# End Source File
# Begin Source File

SOURCE=.\NmIntBind.h
# End Source File
# Begin Source File

SOURCE=.\NmIntDrv.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc"
# Begin Source File

SOURCE=.\nmint.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(DRIVERNETWORKS)\include"
# End Source File
# End Group
# Begin Source File

SOURCE=.\netNmInt.inf
# End Source File
# Begin Source File

SOURCE=.\netNmIntMP.inf
# End Source File
# Begin Source File

SOURCE=.\OEMSETUP.INF
# End Source File
# End Target
# End Project
