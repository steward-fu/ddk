# Microsoft Developer Studio Project File - Name="tdi64" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tdi64 - Win32 IA64 Release NT 51
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "tdi64.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "tdi64.mak" CFG="tdi64 - Win32 IA64 Release NT 51"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "tdi64 - Win32 IA64 Debug NT 51" (based on "Win32 (x86) Static Library")
!MESSAGE "tdi64 - Win32 IA64 Release NT 51" (based on "Win32 (x86) Static Library")
!MESSAGE "tdi64 - Win32 AMD64 Debug NT 51" (based on "Win32 (x86) Static Library")
!MESSAGE "tdi64 - Win32 AMD64 Release NT 51" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tdi64 - Win32 IA64 Debug NT 51"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug NT 51"
# PROP BASE Intermediate_Dir "Debug NT 51"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\ia64\checked"
# PROP Intermediate_Dir "obj\ia64\nt5checked"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /ML /W3 /WX /Z7 /Oi /Gy /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I ".\..\..\include\stl" /I ".\..\..\include\stl\ndis" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /I "$(DRIVERNETWORKS)\include\dep_ndis" /D TDI_WINDOWS_NT /D NDIS51=1 /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fp"obj\ia64\checked/tdint5.pch" /YX /Fo"obj\ia64\checked/" /Wp64 /Zel /GF -cbstring /GF /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\ia64\checked\tdint5.lib"
# Begin Special Build Tool
TargetDir=..\ia64\checked
TargetName=tdint5
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdi64 - Win32 IA64 Release NT 51"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release NT 51"
# PROP BASE Intermediate_Dir "Release NT 51"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\ia64\free"
# PROP Intermediate_Dir "obj\ia64\nt5free"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /ML /W3 /WX /Gy /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I ".\..\..\include\stl" /I ".\..\..\include\stl\ndis" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /I "$(DRIVERNETWORKS)\include\dep_ndis" /D TDI_WINDOWS_NT /D NDIS51=1 /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Fp"obj\ia64\free/VdwLibs.pch" /YX /Fo"obj\ia64\free/" /Oxs /Wp64 /Zel /GF -cbstring /GF /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\ia64\free\tdint5.lib"
# Begin Special Build Tool
TargetDir=..\lib\ia64\free
TargetName=tdint5
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdi64 - Win32 AMD64 Debug NT 51"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug NT 51"
# PROP BASE Intermediate_Dir "Debug NT 51"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\amd64\checked"
# PROP Intermediate_Dir "obj\amd64\nt5checked"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /ML /W3 /WX /Z7 /Oi /Gy /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I ".\..\..\include\stl" /I ".\..\..\include\stl\ndis" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /I "$(DRIVERNETWORKS)\include\dep_ndis" /D TDI_WINDOWS_NT /D NDIS51=1 /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D DBG=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fp"obj\amd64\checked/tdint5.pch" /YX /Fo"obj\amd64\checked/" /Wp64 /Zel /GF -cbstring /GF /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\amd64\checked\tdint5.lib"
# Begin Special Build Tool
TargetDir=..\lib\amd64\checked
TargetName=tdint5
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdi64 - Win32 AMD64 Release NT 51"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release NT 51"
# PROP BASE Intermediate_Dir "Release NT 51"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\amd64\free"
# PROP Intermediate_Dir "obj\amd64\nt5free"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /Gz /ML /W3 /WX /Gy /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I ".\..\..\include\stl" /I ".\..\..\include\stl\ndis" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /I "$(DRIVERNETWORKS)\include\dep_ndis" /D TDI_WINDOWS_NT /D NDIS51=1 /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Fp"obj\amd64\free/tdint5.pch" /YX /Fo"obj\amd64\free/" /Oxs /Wp64 /Zel /GF -cbstring /GF /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\amd64\free\tdint5.lib"
# Begin Special Build Tool
TargetDir=..\lib\amd64\free
TargetName=tdint5
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ENDIF

# Begin Target

# Name "tdi64 - Win32 IA64 Debug NT 51"
# Name "tdi64 - Win32 IA64 Release NT 51"
# Name "tdi64 - Win32 AMD64 Debug NT 51"
# Name "tdi64 - Win32 AMD64 Release NT 51"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\KIPIntr.cpp
# End Source File
# Begin Source File

SOURCE=.\RouTable.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiclient.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiconnobj.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiDtgSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\tdintiface.cpp
# End Source File
# Begin Source File

SOURCE=.\TdiPnpCl.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiquery.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiRawSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiSclient.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiSsession.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiStrSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiutil.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\tdiclient\ipnumbers.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\KIpIntr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\KTdiIrp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\RouTable.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiclient.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiconnobj.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdintiface.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiPartialReceive.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdipnpcl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiSclient.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdisocket.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdistatus.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiStrServer.h
# End Source File
# End Group
# End Target
# End Project
