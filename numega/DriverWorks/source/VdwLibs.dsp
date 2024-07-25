# Microsoft Developer Studio Project File - Name="VdwLibs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=VdwLibs - Win32 NT4 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "VdwLibs.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "VdwLibs.mak" CFG="VdwLibs - Win32 NT4 Checked"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "VdwLibs - Win32 NT4 Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 NT4 Free" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 WDM Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 WDM Free" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 AMD64 NT4 Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 AMD64 NT4 Free" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 AMD64 WDM Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 AMD64 WDM Free" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 IA64 NT4 Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 IA64 NT4 Free" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 IA64 WDM Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "VdwLibs - Win32 IA64 WDM Free" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VdwLibs - Win32 NT4 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NT4 Checked"
# PROP BASE Intermediate_Dir "NT4 Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\nt4checked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "..\..\common\include" /I ".\..\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "." /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd"obj\i386\nt4checked\vdw.pdb" /FD /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\i386\checked/vdw.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\i386\checked\vdw.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\i386\checked
TargetName=vdw
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 NT4 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NT4 Free"
# PROP BASE Intermediate_Dir "NT4 Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\nt4free"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "..\..\common\include" /I "." /I "..\..\common\include\stl" /I "..\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\i386\free/vdw.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\i386\free\vdw.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\i386\free
TargetName=vdw
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WDM Checked"
# PROP BASE Intermediate_Dir "WDM Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\wdmchecked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd"obj\i386\wdmchecked\vdw_wdm.pdb" /FD /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\i386\checked\vdw_wdm.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\i386\checked
TargetName=vdw_wdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WDM Free"
# PROP BASE Intermediate_Dir "WDM Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\wdmfree"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\i386\free/vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\i386\free\vdw_wdm.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\i386\free
TargetName=vdw_wdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\amd64\checked"
# PROP BASE Intermediate_Dir "obj\amd64\nt4checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\amd64\checked"
# PROP Intermediate_Dir "obj\amd64\nt4checked"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Zi /Oi /Gy /I "..\..\common\include" /I ".\..\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "." /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D DBG=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd"obj\amd64\nt4checked\vdw.pdb" /FD /Zel -cbstring /GF /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\amd64\checked/vdw.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\amd64\checked\vdw.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\amd64\checked
TargetName=vdw
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\amd64\free"
# PROP BASE Intermediate_Dir "obj\amd64\nt4free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\amd64\free"
# PROP Intermediate_Dir "obj\amd64\nt4free"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Oy /Gy /I "..\..\common\include" /I "." /I "..\..\common\include\stl" /I "..\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION=400 /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Zel -cbstring /GF /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\amd64\free/vdw.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\amd64\free\vdw.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\amd64\free
TargetName=vdw
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\amd64\checked"
# PROP BASE Intermediate_Dir "obj\amd64\wdmchecked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\amd64\checked"
# PROP Intermediate_Dir "obj\amd64\wdmchecked"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Zi /Oi /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd"obj\amd64\wdmchecked\vdw_wdm.pdb" /FD /Zel -cbstring /GF /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\amd64\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\amd64\checked\vdw_wdm.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\amd64\checked
TargetName=vdw_wdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\amd64\free"
# PROP BASE Intermediate_Dir "obj\amd64\wdmfree"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\amd64\free"
# PROP Intermediate_Dir "obj\amd64\wdmfree"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Oy /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Zel -cbstring /GF /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\lib\amd64\free/vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\amd64\free\vdw_wdm.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\amd64\free
TargetName=vdw_wdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\ia64\checked"
# PROP BASE Intermediate_Dir "obj\ia64\NTchecked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\ia64\checked"
# PROP Intermediate_Dir "obj\ia64\NTchecked"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Zi /Oi /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I ".\..\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='400' /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Fd"obj\ia64\NTchecked\vdw.pdb" /Zel -cbstring /GF /Wp64 /Zel /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 crtsec.obj /nologo /out:"..\lib\IA64\checked\vdw.lib"
# ADD LIB32 /nologo /out:"..\lib\IA64\checked\vdw.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\IA64\checked
TargetName=vdw
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\ia64\free"
# PROP BASE Intermediate_Dir "obj\ia64\NTfree"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\ia64\free"
# PROP Intermediate_Dir "obj\ia64\NTfree"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I ".\..\include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='400' /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Zel -cbstring /GF /Wp64 /Zel /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 crtsec.obj /nologo /out:"..\lib\IA64\free\vdw.lib" /Ignore:4221
# ADD LIB32 /nologo /out:"..\lib\IA64\free\vdw.lib" /Ignore:4221
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\IA64\free
TargetName=vdw
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "IA64 WDM checked"
# PROP BASE Intermediate_Dir "IA64 WDM checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\ia64\checked"
# PROP Intermediate_Dir "obj\ia64\checked"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Zi /Oi /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Fd"obj\ia64\checked\vdw_wdm.pdb" /Zel -cbstring /GF /Wp64 /Zel /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\checked\vdw_wdm.lib"
# ADD LIB32 /nologo /out:"..\lib\IA64\checked\vdw_wdm.lib"
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\IA64\checked
TargetName=vdw_wdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "IA64 WDM Free"
# PROP BASE Intermediate_Dir "IA64 WDM Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\ia64\free"
# PROP Intermediate_Dir "obj\ia64\free"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Gy /I "..\..\common\include" /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /FI"warning.h" /D NTVERSION='WDM' /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Zel -cbstring /GF /Wp64 /Zel /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\IA64\checked\vdw_wdm.lib"
# ADD LIB32 /nologo /out:"..\lib\IA64\free\vdw_wdm.lib" /Ignore:4221
# Begin Special Build Tool
TargetDir=\DSSource\Framework\DriverWorks\lib\IA64\free
TargetName=vdw_wdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ENDIF

# Begin Target

# Name "VdwLibs - Win32 NT4 Checked"
# Name "VdwLibs - Win32 NT4 Free"
# Name "VdwLibs - Win32 WDM Checked"
# Name "VdwLibs - Win32 WDM Free"
# Name "VdwLibs - Win32 AMD64 NT4 Checked"
# Name "VdwLibs - Win32 AMD64 NT4 Free"
# Name "VdwLibs - Win32 AMD64 WDM Checked"
# Name "VdwLibs - Win32 AMD64 WDM Free"
# Name "VdwLibs - Win32 IA64 NT4 Checked"
# Name "VdwLibs - Win32 IA64 NT4 Free"
# Name "VdwLibs - Win32 IA64 WDM Checked"
# Name "VdwLibs - Win32 IA64 WDM Free"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpprt.cpp
# End Source File
# Begin Source File

SOURCE=.\dwcontrl.cpp
# End Source File
# Begin Source File

SOURCE=.\getnames.cpp
# End Source File
# Begin Source File

SOURCE=.\k1394.cpp
# End Source File
# Begin Source File

SOURCE=.\k1394Async.cpp
# End Source File
# Begin Source File

SOURCE=.\K1394Isoc.cpp
# End Source File
# Begin Source File

SOURCE=.\kadapter.cpp
# End Source File
# Begin Source File

SOURCE=.\kaddress.cpp
# End Source File
# Begin Source File

SOURCE=.\kbus.cpp
# End Source File
# Begin Source File

SOURCE=.\kchecker.cpp
# End Source File
# Begin Source File

SOURCE=.\kdevice.cpp
# End Source File
# Begin Source File

SOURCE=.\kdevque.cpp
# End Source File
# Begin Source File

SOURCE=.\kdmaxfer.cpp
# End Source File
# Begin Source File

SOURCE=.\kdmqcsq.cpp
# End Source File
# Begin Source File

SOURCE=.\kdmqex.cpp
# End Source File
# Begin Source File

SOURCE=.\kerrlog.cpp
# End Source File
# Begin Source File

SOURCE=.\kevent.cpp
# End Source File
# Begin Source File

SOURCE=.\kexception.cpp
# End Source File
# Begin Source File

SOURCE=.\kfifo.cpp
# End Source File
# Begin Source File

SOURCE=.\kfile.cpp
# End Source File
# Begin Source File

SOURCE=.\kfilter.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\khid.cpp

!IF  "$(CFG)" == "VdwLibs - Win32 NT4 Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 NT4 Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Checked"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Free"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Checked"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Free"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\khidaux.cpp
# End Source File
# Begin Source File

SOURCE=.\kintrupt.cpp
# End Source File
# Begin Source File

SOURCE=.\kirp.cpp
# End Source File
# Begin Source File

SOURCE=.\klist.cpp
# End Source File
# Begin Source File

SOURCE=.\klower.cpp
# End Source File
# Begin Source File

SOURCE=.\kmutex.cpp
# End Source File
# Begin Source File

SOURCE=.\kpcicfg.cpp
# End Source File
# Begin Source File

SOURCE=.\kphysdev.cpp
# End Source File
# Begin Source File

SOURCE=.\kpnpdev.cpp
# End Source File
# Begin Source File

SOURCE=.\kpnplow.cpp
# End Source File
# Begin Source File

SOURCE=.\kquery.cpp
# End Source File
# Begin Source File

SOURCE=.\kregkey.cpp
# End Source File
# Begin Source File

SOURCE=.\kresreq.cpp
# End Source File
# Begin Source File

SOURCE=.\ksemaphr.cpp
# End Source File
# Begin Source File

SOURCE=.\ksfifo.cpp
# End Source File
# Begin Source File

SOURCE=.\KsMiniDriver.cpp

!IF  "$(CFG)" == "VdwLibs - Win32 NT4 Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 NT4 Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Checked"

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Free"

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Checked"

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Free"

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Checked"

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Free"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\kspin.cpp
# End Source File
# Begin Source File

SOURCE=.\kstdwmi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Source\kstl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Source\kstllmcb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Source\kstlnphs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Source\kstlphs.cpp
# End Source File
# Begin Source File

SOURCE=.\Kstradpt.cpp
# End Source File
# Begin Source File

SOURCE=.\Kstream.cpp
# End Source File
# Begin Source File

SOURCE=.\Kstrmdrv.cpp
# End Source File
# Begin Source File

SOURCE=.\ksysthrd.cpp
# End Source File
# Begin Source File

SOURCE=.\ktimer.cpp
# End Source File
# Begin Source File

SOURCE=.\ktrace.cpp
# End Source File
# Begin Source File

SOURCE=.\kunitnam.cpp
# End Source File
# Begin Source File

SOURCE=.\kusb.cpp
# End Source File
# Begin Source File

SOURCE=.\kustring.cpp
# End Source File
# Begin Source File

SOURCE=.\kvxdintf.cpp
# End Source File
# Begin Source File

SOURCE=.\kwdmfltr.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\kwmi.cpp
# End Source File
# Begin Source File

SOURCE=.\kwmistr.cpp
# End Source File
# Begin Source File

SOURCE=.\shfifo.cpp

!IF  "$(CFG)" == "VdwLibs - Win32 NT4 Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 NT4 Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 WDM Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 NT4 Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 AMD64 WDM Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Checked"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 NT4 Free"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Checked"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "VdwLibs - Win32 IA64 WDM Free"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\Common\Source\stcinit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Source\suballoc.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\include\cpprt.h
# End Source File
# Begin Source File

SOURCE=..\include\cright.h
# End Source File
# Begin Source File

SOURCE=..\include\dbgmsg.h
# End Source File
# Begin Source File

SOURCE=..\include\devinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\devintf.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Include\stl\diswarn.h
# End Source File
# Begin Source File

SOURCE=..\include\dmioctl.h
# End Source File
# Begin Source File

SOURCE=..\include\DriverWorksSamples.h
# End Source File
# Begin Source File

SOURCE=..\include\dwcontrl.h
# End Source File
# Begin Source File

SOURCE=..\include\FilterInstall.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Include\stl\ios.h
# End Source File
# Begin Source File

SOURCE=..\include\k1394.h
# End Source File
# Begin Source File

SOURCE=..\include\K1394Async.h
# End Source File
# Begin Source File

SOURCE=..\include\K1394Irb.h
# End Source File
# Begin Source File

SOURCE=..\include\K1394isoc.h
# End Source File
# Begin Source File

SOURCE=..\include\kadapter.h
# End Source File
# Begin Source File

SOURCE=..\include\kaddress.h
# End Source File
# Begin Source File

SOURCE=..\include\karray.h
# End Source File
# Begin Source File

SOURCE=..\include\kautoptr.h
# End Source File
# Begin Source File

SOURCE=..\include\kavstream.h
# End Source File
# Begin Source File

SOURCE=..\include\kbus.h
# End Source File
# Begin Source File

SOURCE=..\include\kcallback.h
# End Source File
# Begin Source File

SOURCE=..\include\kchecker.h
# End Source File
# Begin Source File

SOURCE=..\include\kcontrol.h
# End Source File
# Begin Source File

SOURCE=..\include\kcsq.h
# End Source File
# Begin Source File

SOURCE=..\include\kdevice.h
# End Source File
# Begin Source File

SOURCE=..\include\kdevque.h
# End Source File
# Begin Source File

SOURCE=..\include\kdispobj.h
# End Source File
# Begin Source File

SOURCE=..\include\kdmaxfer.h
# End Source File
# Begin Source File

SOURCE=..\include\kdmqcsq.h
# End Source File
# Begin Source File

SOURCE=..\include\kdmqex.h
# End Source File
# Begin Source File

SOURCE=..\include\kdpc.h
# End Source File
# Begin Source File

SOURCE=..\include\kdriver.h
# End Source File
# Begin Source File

SOURCE=..\include\kerrlog.h
# End Source File
# Begin Source File

SOURCE=..\include\kevent.h
# End Source File
# Begin Source File

SOURCE=..\include\kexception.h
# End Source File
# Begin Source File

SOURCE=..\include\kfifo.h
# End Source File
# Begin Source File

SOURCE=..\include\kfile.h
# End Source File
# Begin Source File

SOURCE=..\include\kfilter.h
# End Source File
# Begin Source File

SOURCE=..\include\kgenlock.h
# End Source File
# Begin Source File

SOURCE=..\include\kheap.h
# End Source File
# Begin Source File

SOURCE=..\include\khid.h
# End Source File
# Begin Source File

SOURCE=..\include\kicount.h
# End Source File
# Begin Source File

SOURCE=..\include\kimgsect.h
# End Source File
# Begin Source File

SOURCE=..\include\kintrupt.h
# End Source File
# Begin Source File

SOURCE=..\include\kiocparm.h
# End Source File
# Begin Source File

SOURCE=..\include\kIPfilterhook.h
# End Source File
# Begin Source File

SOURCE=..\include\kirp.h
# End Source File
# Begin Source File

SOURCE=..\include\klist.h
# End Source File
# Begin Source File

SOURCE=..\include\klower.h
# End Source File
# Begin Source File

SOURCE=..\include\kmemory.h
# End Source File
# Begin Source File

SOURCE=..\include\kmutex.h
# End Source File
# Begin Source File

SOURCE=..\include\kndisvdw.h
# End Source File
# Begin Source File

SOURCE=..\include\kpcicfg.h
# End Source File
# Begin Source File

SOURCE=..\include\kphysdev.h
# End Source File
# Begin Source File

SOURCE=..\include\kplx9056.h
# End Source File
# Begin Source File

SOURCE=..\include\kplx9056reg.h
# End Source File
# Begin Source File

SOURCE=..\include\kpnpdev.h
# End Source File
# Begin Source File

SOURCE=..\include\kpnplow.h
# End Source File
# Begin Source File

SOURCE=..\include\kpownot.h
# End Source File
# Begin Source File

SOURCE=..\include\kquery.h
# End Source File
# Begin Source File

SOURCE=..\include\kregkey.h
# End Source File
# Begin Source File

SOURCE=..\include\kresreq.h
# End Source File
# Begin Source File

SOURCE=..\include\ks5920.h
# End Source File
# Begin Source File

SOURCE=..\include\ks5933.h
# End Source File
# Begin Source File

SOURCE=..\include\ks59xx.h
# End Source File
# Begin Source File

SOURCE=..\include\ks59xxrd.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsDevice.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsDma.h
# End Source File
# Begin Source File

SOURCE=..\include\ksemaphr.h
# End Source File
# Begin Source File

SOURCE=..\include\ksfifo.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsFilter.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsFormats.h
# End Source File
# Begin Source File

SOURCE=..\include\kslavdma.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsMiniDriver.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsObject.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsPin.h
# End Source File
# Begin Source File

SOURCE=..\include\kspin.h
# End Source File
# Begin Source File

SOURCE=..\include\avstream\KsStream.h
# End Source File
# Begin Source File

SOURCE=..\include\kstdwmi.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Include\stl\kstl.h
# End Source File
# Begin Source File

SOURCE=..\include\kstl.h
# End Source File
# Begin Source File

SOURCE=..\include\Kstream.h
# End Source File
# Begin Source File

SOURCE=..\include\ksysthrd.h
# End Source File
# Begin Source File

SOURCE=..\include\ktimer.h
# End Source File
# Begin Source File

SOURCE=..\include\ktrace.h
# End Source File
# Begin Source File

SOURCE=..\include\kunitnam.h
# End Source File
# Begin Source File

SOURCE=..\include\kusb.h
# End Source File
# Begin Source File

SOURCE=..\include\KUsbBusIntf.h
# End Source File
# Begin Source File

SOURCE=..\include\kustring.h
# End Source File
# Begin Source File

SOURCE=..\include\KVirtualCom.h
# End Source File
# Begin Source File

SOURCE=..\include\kvxdintf.h
# End Source File
# Begin Source File

SOURCE=..\include\kwdmfltr.h
# End Source File
# Begin Source File

SOURCE=..\include\kwmi.h
# End Source File
# Begin Source File

SOURCE=..\include\kwmiblock.h
# End Source File
# Begin Source File

SOURCE=..\include\kwmistr.h
# End Source File
# Begin Source File

SOURCE=..\include\kworkitm.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Include\stl\lockable.h
# End Source File
# Begin Source File

SOURCE=..\include\ntkmcpp.h
# End Source File
# Begin Source File

SOURCE=..\include\shfifo.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Include\stcinit.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Include\stl\suballoc.h
# End Source File
# Begin Source File

SOURCE=..\include\util.h
# End Source File
# Begin Source File

SOURCE=..\include\vdw.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\workspace.txt
# End Source File
# End Target
# End Project
