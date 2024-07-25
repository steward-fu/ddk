# Microsoft Developer Studio Project File - Name="Hello" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Hello - Win32 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "Hello.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Hello.mak" CFG="Hello - Win32 Checked"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Hello - Win32 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Hello - Win32 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Hello - Win32 AMD64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Hello - Win32 AMD64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Hello - Win32 IA64 Checked" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Hello - Win32 IA64 Free" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Hello - Win32 Checked"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386"
# PROP Intermediate_Dir ".\objchk\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /Zel -cbstring /Gy /GF /c /FI"warning.h" /Zi /Oi /Gz
# ADD CPP /W3
# ADD CPP /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION)
# ADD CPP /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"
# ADD CPP /Fd".\objchk\i386\Hello.pdb"
# ADD CPP /I "$(DRIVERWORKS)\include;"
# ADD CPP /I "$(DRIVERWORKS)\source;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD CPP -DNTVERSION=400
# ADD CPP /FR
# ADD BASE MTL /nologo /mktyplib203 /o NUL /win32 /d "_DEBUG"
# ADD MTL /nologo /mktyplib203 /o NUL /win32 /d "_DEBUG"
# ADD BASE RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "_DEBUG"
# ADD RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "_DEBUG"
# ADD RSC /I "$(DRIVERWORKS)\include;"
# ADD RSC /I "$(DRIVERWORKS)\source;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD RSC -DNTVERSION=400
BSC32=bscmake.exe
# ADD BSC32 /nologo /o".\objchk\i386/Hello.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO
# ADD LINK32 /libpath:"$(TARGET_LIB_PATH);$(DRIVERWORKS)\lib\i386\checked" /driver /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text
# ADD LINK32 /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10
# ADD LINK32 /debug:FULL /debugtype:cv /out:".\objchk\i386\Hello.sys"
# ADD LINK32 /ignore:4210,4010,4037,4039,4065,4070,4078,4087,4089,4221,4210
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib
# ADD LINK32 vdw.lib

# Begin Special Build Tool
PostBuild_Cmds=$(DRIVERWORKS)\bin\nmsym /trans:source,package,always .\objchk\i386\Hello.sys
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Hello - Win32 Free"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386"
# PROP Intermediate_Dir ".\objfre\i386"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /Zel -cbstring /Gy /GF /c /FI"warning.h" /Oxs /Oy /Gz
# ADD CPP /W3
# ADD CPP /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION)
# ADD CPP /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"
# ADD CPP /I "$(DRIVERWORKS)\include;"
# ADD CPP /I "$(DRIVERWORKS)\source;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD CPP -DNTVERSION=400
# ADD CPP /FR
# ADD BASE MTL /nologo /mktyplib203 /o NUL /win32 /d "NDEBUG"
# ADD MTL /nologo /mktyplib203 /o NUL /win32 /d "NDEBUG"
# ADD BASE RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "NDEBUG"
# ADD RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "NDEBUG"
# ADD RSC /I "$(DRIVERWORKS)\include;"
# ADD RSC /I "$(DRIVERWORKS)\source;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD RSC -DNTVERSION=400
BSC32=bscmake.exe
# ADD BSC32 /nologo /o".\objfre\i386/Hello.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO
# ADD LINK32 /libpath:"$(TARGET_LIB_PATH);$(DRIVERWORKS)\lib\i386\free" /driver /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text
# ADD LINK32 /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10
# ADD LINK32 /debug:MINIMAL /out:"objfre\i386\Hello.sys"
# ADD LINK32 /ignore:4210,4010,4037,4039,4065,4070,4078,4087,4089,4221,4210
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib
# ADD LINK32 vdw.lib

!ELSEIF  "$(CFG)" == "Hello - Win32 AMD64 Checked"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\amd64"
# PROP Intermediate_Dir ".\objchk\amd64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /Zel /Zp8 -cbstring /Gy /GF /Gi- /GX- /GR- /Z7 /c /FI"warning.h" /Zi /Od /Oi
# ADD CPP /W3
# ADD CPP /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION)
# ADD CPP /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"
# ADD CPP /Fd".\objchk\amd64\Hello.pdb"
# ADD CPP /I "$(DRIVERWORKS)\include;"
# ADD CPP /I "$(DRIVERWORKS)\source;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD CPP -DNTVERSION=400
# ADD CPP /FR
# ADD BASE MTL /nologo /mktyplib203 /o NUL /win32 /d "_DEBUG"
# ADD MTL /nologo /mktyplib203 /o NUL /win32 /d "_DEBUG"
# ADD BASE RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "_DEBUG"
# ADD RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "_DEBUG"
# ADD RSC /I "$(DRIVERWORKS)\include;"
# ADD RSC /I "$(DRIVERWORKS)\source;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD RSC -DNTVERSION=400
BSC32=bscmake.exe
# ADD BSC32 /nologo /o".\objchk\amd64/Hello.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 /nologo /base:"0x10000" /version:5.2 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:amd64 /nodefaultlib /INCREMENTAL:NO
# ADD LINK32 /libpath:"$(TARGET_LIB_PATH);$(DRIVERWORKS)\lib\amd64\checked" /driver /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text
# ADD LINK32 /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.2 /subsystem:native,1.10
# ADD LINK32 /debug:FULL /debugtype:cv /out:".\objchk\amd64\Hello.sys"
# ADD LINK32 /ignore:4210,4198,4010,4037,4039,4065,4070,4078,4087,4089,4221
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib
# ADD LINK32 vdw.lib

!ELSEIF  "$(CFG)" == "Hello - Win32 AMD64 Free"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\amd64"
# PROP Intermediate_Dir ".\objfre\amd64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /Zel /Zp8 /Gy /Gi- -cbstring /Wp64 /WX /GX- /GR- /GF -Z7 / Oxt /c /FI"warning.h"
# ADD CPP /W3
# ADD CPP /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION)
# ADD CPP /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"
# ADD CPP /I "$(DRIVERWORKS)\include;"
# ADD CPP /I "$(DRIVERWORKS)\source;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD CPP -DNTVERSION=400
# ADD CPP /FR
# ADD BASE MTL /nologo /mktyplib203 /o NUL /win32 /d "NDEBUG"
# ADD MTL /nologo /mktyplib203 /o NUL /win32 /d "NDEBUG"
# ADD BASE RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "NDEBUG"
# ADD RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "NDEBUG"
# ADD RSC /I "$(DRIVERWORKS)\include;"
# ADD RSC /I "$(DRIVERWORKS)\source;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD RSC -DNTVERSION=400
BSC32=bscmake.exe
# ADD BSC32 /nologo /o".\objfre\amd64/Hello.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 /nologo /base:"0x10000" /version:5.2 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:amd64 /nodefaultlib /INCREMENTAL:NO
# ADD LINK32 /libpath:"$(TARGET_LIB_PATH);$(DRIVERWORKS)\lib\amd64\free" /driver /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text
# ADD LINK32 /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.2 /subsystem:native,1.10
# ADD LINK32 /debug:MINIMAL /out:"objfre\amd64\Hello.sys"
# ADD LINK32 /ignore:4210,4198,4010,4037,4039,4065,4070,4078,4087,4089,4221
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib
# ADD LINK32 vdw.lib

!ELSEIF  "$(CFG)" == "Hello - Win32 IA64 Checked"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objchk\ia64"
# PROP Intermediate_Dir ".\objchk\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /Zel -cbstring /Gy /GF /c /FI"warning.h" /Zi /Oi
# ADD CPP /W3
# ADD CPP /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION)
# ADD CPP /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"
# ADD CPP /Fd".\objchk\ia64\Hello.pdb"
# ADD CPP /I "$(DRIVERWORKS)\include;"
# ADD CPP /I "$(DRIVERWORKS)\source;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD CPP -DNTVERSION=400
# ADD CPP /FR
# ADD BASE MTL /nologo /mktyplib203 /o NUL /win32 /d "_DEBUG"
# ADD MTL /nologo /mktyplib203 /o NUL /win32 /d "_DEBUG"
# ADD BASE RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "_DEBUG"
# ADD RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "_DEBUG"
# ADD RSC /I "$(DRIVERWORKS)\include;"
# ADD RSC /I "$(DRIVERWORKS)\source;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD RSC -DNTVERSION=400
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 /debug:FULL /debugtype:cv /machine:IA64 /debugtype:pdata /out:".\objchk\ia64\Hello.sys"
# ADD LINK32 /nologo /base:"0x10000" /version:5.2 /stack:0x40000,0x1000 /entry:"DriverEntry" /nodefaultlib /INCREMENTAL:NO
# ADD LINK32 /libpath:"$(TARGET_LIB_PATH);$(DRIVERWORKS)\lib\ia64\checked" /driver /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text
# ADD LINK32 /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.2 /subsystem:native,1.10
# ADD LINK32 /ignore:4210,4010,4037,4039,4065,4070,4078,4087,4089,4221,4210,4108,4088,4218
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib
# ADD LINK32 vdw.lib

!ELSEIF  "$(CFG)" == "Hello - Win32 IA64 Free"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\ia64"
# PROP Intermediate_Dir ".\objfre\ia64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /Zel -cbstring /Gy /GF /c /FI"warning.h" /WX /Wp64 /Zel /Oxs
# ADD CPP /W3
# ADD CPP /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION)
# ADD CPP /I "." /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)"
# ADD CPP /I "$(DRIVERWORKS)\include;"
# ADD CPP /I "$(DRIVERWORKS)\source;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD CPP /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD CPP -DNTVERSION=400
# ADD CPP /FR
# ADD BASE MTL /nologo /mktyplib203 /o NUL /win32 /d "NDEBUG"
# ADD MTL /nologo /mktyplib203 /o NUL /win32 /d "NDEBUG"
# ADD BASE RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "NDEBUG"
# ADD RSC /l 0x409 /I "$(CRT_INC_PATH)" /d "NDEBUG"
# ADD RSC /I "$(DRIVERWORKS)\include;"
# ADD RSC /I "$(DRIVERWORKS)\source;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_vxd;"
# ADD RSC /I "$(DRIVERWORKS)\include\dep_wdm"
# ADD RSC -DNTVERSION=400
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 /debug:MINIMAL /machine:IA64 /debugtype:pdata /out:"objfre\ia64\Hello.sys"
# ADD LINK32 /nologo /base:"0x10000" /version:5.2 /stack:0x40000,0x1000 /entry:"DriverEntry" /nodefaultlib /INCREMENTAL:NO
# ADD LINK32 /libpath:"$(TARGET_LIB_PATH);$(DRIVERWORKS)\lib\ia64\free" /driver /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text
# ADD LINK32 /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.2 /subsystem:native,1.10
# ADD LINK32 /ignore:4210,4010,4037,4039,4065,4070,4078,4087,4089,4221,4210,4108,4088,4218
# ADD LINK32 ntoskrnl.lib hal.lib wmilib.lib
# ADD LINK32 vdw.lib

!ENDIF

# Begin Target

# Name "Hello - Win32 Checked"
# Name "Hello - Win32 Free"
# Name "Hello - Win32 IA64 Checked"
# Name "Hello - Win32 IA64 Free"
# Name "Hello - Win32 AMD64 Checked"
# Name "Hello - Win32 AMD64 Free"

# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"

# Begin Source File

SOURCE=.\hello.cpp
# End Source File
# Begin Source File

SOURCE=.\hellodev.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"

# Begin Source File

SOURCE=.\FUNCTION.H
# End Source File
# Begin Source File

SOURCE=.\HELLO.H
# End Source File
# Begin Source File

SOURCE=.\HELLODEV.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ".rc;.mc;.mof"

# Begin Source File

SOURCE=.\hello.rc
# End Source File


# End Group
# Begin Source File

SOURCE=.\sources
# End Source File
# End Target
# End Project
