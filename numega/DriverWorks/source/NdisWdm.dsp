# Microsoft Developer Studio Project File - Name="NdisWdm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=NdisWdm - Win32 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "NdisWdm.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "NdisWdm.mak" CFG="NdisWdm - Win32 Checked"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "NdisWdm - Win32 Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "NdisWdm - Win32 Free" (based on "Win32 (x86) Static Library")
!MESSAGE "NdisWdm - Win32 AMD64 Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "NdisWdm - Win32 AMD64 Free" (based on "Win32 (x86) Static Library")
!MESSAGE "NdisWdm - Win32 IA64 Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "NdisWdm - Win32 IA64 Free" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NdisWdm - Win32 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NdisWdm___Win32_NDIS_WDM_Checked"
# PROP BASE Intermediate_Dir "NdisWdm___Win32_NDIS_WDM_Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\kndiswdmchecked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I ".\..\include\dep_vxd" /I ".\..\include\dep_wdm" /FI"warning.h" /D NTVERSION='WDM' /D KNDIS_WDM_LIBBLD=1 /D NDIS_WDM=1 /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd"obj\i386\kndiswdmchecked\kndiswdm.pdb" /FD /Zel -cbstring /GF /QIfdiv- /QIf /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\checked\vdw_wdm.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\checked\kndiswdm.lib"
# Begin Special Build Tool
TargetName=kndiswdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NdisWdm - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NdisWdm___Win32_NDIS_WDM_Free"
# PROP BASE Intermediate_Dir "NdisWdm___Win32_NDIS_WDM_Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\kndiswdmfree"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I ".\..\include\dep_vxd" /I ".\..\include\dep_wdm" /FI"warning.h" /D NTVERSION='WDM' /D KNDIS_WDM_LIBBLD=1 /D NDIS_WDM=1 /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Zel -cbstring /GF /QIfdiv- /QIf /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\free/vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\free/vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\free\vdw_wdm.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\free\kndiswdm.lib"
# Begin Special Build Tool
TargetName=kndiswdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NdisWdm - Win32 AMD64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\amd64\checked"
# PROP BASE Intermediate_Dir "obj\amd64\kndiswdmchecked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\amd64\checked"
# PROP Intermediate_Dir "obj\amd64\kndiswdmchecked"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Zi /Oi /Gy /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I ".\..\include\dep_vxd" /I ".\..\include\dep_wdm" /FI"warning.h" /D NTVERSION='WDM' /D KNDIS_WDM_LIBBLD=1 /D NDIS_WDM=1 /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /Fd"obj\amd64\kndiswdmchecked\kndiswdm.pdb" /FD /Zel -cbstring /GF /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\amd64\checked\vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\amd64\checked\vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\amd64\checked\vdw_wdm.lib"
# ADD LIB32 /nologo /out:"..\lib\amd64\checked\kndiswdm.lib"
# Begin Special Build Tool
TargetName=kndiswdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NdisWdm - Win32 AMD64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\amd64\free"
# PROP BASE Intermediate_Dir "obj\amd64\kndiswdmfree"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\amd64\free"
# PROP Intermediate_Dir "obj\amd64\kndiswdmfree"
# PROP Target_Dir ""
# ADD CPP /nologo /W3 /Oy /Gy /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\hid\inc" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I ".\..\include\dep_vxd" /I ".\..\include\dep_wdm" /FI"warning.h" /D NTVERSION='WDM' /D KNDIS_WDM_LIBBLD=1 /D NDIS_WDM=1 /D _WIN64 /D _AMD64_ /D AMD64 /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _AMD64_WORKAROUND_ /D DEVL=1 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Zel -cbstring /GF /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\amd64\free/vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\amd64\free/vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\amd64\free\vdw_wdm.lib"
# ADD LIB32 /nologo /out:"..\lib\amd64\free\kndiswdm.lib"
# Begin Special Build Tool
TargetName=kndiswdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NdisWdm - Win32 IA64 Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NdisWdm___Win32_IA64_Checked"
# PROP BASE Intermediate_Dir "NdisWdm___Win32_IA64_Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\ia64\checked"
# PROP Intermediate_Dir "obj\ia64\kndiswdmchecked"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Zi /Oi /Gy /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I ".\..\include\dep_vxd" /I ".\..\include\dep_wdm" /FI"warning.h" /D NTVERSION='WDM' /D KNDIS_WDM_LIBBLD=1 /D NDIS_WDM=1 /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Fo"obj\ia64\checked/" /Fd"obj\ia64\kndiswdmchecked\kndiswdm.pdb" /Zel -cbstring /GF /Wp64 /Zel /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\free/vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\free/vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\free\kndiswdm.lib"
# ADD LIB32 /nologo /out:"..\lib\ia64\checked\kndiswdm.lib"
# Begin Special Build Tool
TargetName=kndiswdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "NdisWdm - Win32 IA64 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NdisWdm___Win32_IA64_Free"
# PROP BASE Intermediate_Dir "NdisWdm___Win32_IA64_Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\ia64\free"
# PROP Intermediate_Dir "obj\ia64\kndiswdmfree"
# PROP Target_Dir ""
# ADD BASE CPP /nologo
# ADD CPP /nologo /W3 /WX /Gy /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I ".\..\include" /I "$(BASEDIR)\src\wdm\hid\inc" /I "$(DRIVERWORKS)\source" /I "$(DRIVERNETWORKS)\include" /I ".\..\include\dep_vxd" /I ".\..\include\dep_wdm" /FI"warning.h" /D NTVERSION='WDM' /D KNDIS_WDM_LIBBLD=1 /D NDIS_WDM=1 /D _WIN64 /D _IA64_=1 /D IA64=1 /D NO_HW_DETECT /D _MSC_EXTENSIONS /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D _MERCED_A0_=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Fo"obj\ia64\free/" /Zel -cbstring /GF /Wp64 /Zel /Oxs /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\free/vdw_wdm.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\free/vdw_wdm.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\free\kndiswdm.lib"
# ADD LIB32 /nologo /out:"..\lib\ia64\free\kndiswdm.lib"
# Begin Special Build Tool
TargetName=kndiswdm
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ENDIF

# Begin Target

# Name "NdisWdm - Win32 Checked"
# Name "NdisWdm - Win32 Free"
# Name "NdisWdm - Win32 AMD64 Checked"
# Name "NdisWdm - Win32 AMD64 Free"
# Name "NdisWdm - Win32 IA64 Checked"
# Name "NdisWdm - Win32 IA64 Free"
# Begin Group "Source Files"

# PROP Default_Filter ""
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

SOURCE=.\kevent.cpp
# End Source File
# Begin Source File

SOURCE=.\kexception.cpp
# End Source File
# Begin Source File

SOURCE=.\kirp.cpp
# End Source File
# Begin Source File

SOURCE=.\klower.cpp
# End Source File
# Begin Source File

SOURCE=.\kmutex.cpp
# End Source File
# Begin Source File

SOURCE=.\kpnplow.cpp
# End Source File
# Begin Source File

SOURCE=.\ksemaphr.cpp
# End Source File
# Begin Source File

SOURCE=.\kspin.cpp
# End Source File
# Begin Source File

SOURCE=.\ksysthrd.cpp
# End Source File
# Begin Source File

SOURCE=.\ktimer.cpp
# End Source File
# Begin Source File

SOURCE=.\kusb.cpp
# End Source File
# Begin Source File

SOURCE=.\kustring.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\include\dbgmsg.h
# End Source File
# Begin Source File

SOURCE=..\include\dmioctl.h
# End Source File
# Begin Source File

SOURCE=..\include\k1394.h
# End Source File
# Begin Source File

SOURCE=..\include\K1394Async.h
# End Source File
# Begin Source File

SOURCE=..\include\K1394isoc.h
# End Source File
# Begin Source File

SOURCE=..\include\karray.h
# End Source File
# Begin Source File

SOURCE=..\include\kdispobj.h
# End Source File
# Begin Source File

SOURCE=..\include\kdpc.h
# End Source File
# Begin Source File

SOURCE=..\include\kevent.h
# End Source File
# Begin Source File

SOURCE=..\include\kexception.h
# End Source File
# Begin Source File

SOURCE=..\include\kgenlock.h
# End Source File
# Begin Source File

SOURCE=..\include\kicount.h
# End Source File
# Begin Source File

SOURCE=..\include\kiocparm.h
# End Source File
# Begin Source File

SOURCE=..\include\kirp.h
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

SOURCE=..\include\kpnplow.h
# End Source File
# Begin Source File

SOURCE=..\include\ksemaphr.h
# End Source File
# Begin Source File

SOURCE=..\include\kspin.h
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

SOURCE=..\include\kustring.h
# End Source File
# Begin Source File

SOURCE=..\include\kworkitm.h
# End Source File
# Begin Source File

SOURCE=..\include\util.h
# End Source File
# End Group
# End Target
# End Project
