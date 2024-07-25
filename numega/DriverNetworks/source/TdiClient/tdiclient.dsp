# Microsoft Developer Studio Project File - Name="tdiclient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=tdiclient - Win32 Release W9X SYS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "tdiclient.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "tdiclient.mak" CFG="tdiclient - Win32 Release W9X SYS"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "tdiclient - Win32 Debug NT 5" (based on "Win32 (x86) Static Library")
!MESSAGE "tdiclient - Win32 Release NT 5" (based on "Win32 (x86) Static Library")
!MESSAGE "tdiclient - Win32 Debug W9X SYS" (based on "Win32 (x86) Static Library")
!MESSAGE "tdiclient - Win32 Debug W9X VxD" (based on "Win32 (x86) Static Library")
!MESSAGE "tdiclient - Win32 Release W9X VxD" (based on "Win32 (x86) Static Library")
!MESSAGE "tdiclient - Win32 Release W9X SYS" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tdiclient___Win32_Debug_NT_5"
# PROP BASE Intermediate_Dir "tdiclient___Win32_Debug_NT_5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\nt5checked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I ".\..\..\include\stl" /I ".\..\..\include\stl\ndis" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /I "$(DRIVERNETWORKS)\include\dep_ndis" /FI"warning.h" /D TDI_WINDOWS_NT /D NDIS50=1 /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /Fd"obj\i386\nt5checked\tdint5.pdb" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"tdint4d.lib"
# ADD LIB32 /nologo /out:"..\..\lib\i386\checked\tdint5.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\checked
TargetName=tdint5
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tdiclient___Win32_Release_NT_5"
# PROP BASE Intermediate_Dir "tdiclient___Win32_Release_NT_5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\nt5free"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Og /Oi /Ob2 /Gy /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I ".\..\..\include\stl" /I ".\..\..\include\stl\ndis" /I "$(DRIVERNETWORKS)\include\tdiclient\dep_vxd" /I "$(DRIVERNETWORKS)\include\dep_ndis" /FI"warning.h" /D TDI_WINDOWS_NT /D NDIS50=1 /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\i386\checked\tdint5.lib"
# ADD LIB32 /nologo /out:"..\..\lib\i386\free\tdint5.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\free
TargetName=tdint5
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tdiclient___Win32_Debug_W9X SYS"
# PROP BASE Intermediate_Dir "tdiclient___Win32_Debug_W9X SYS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\w9sysChecked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Z7 /Oi /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(VTOOLSD)\include" /I "$(DRIVERNETWORKS)\include\dep_ndis" /FI"warning.h" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D i386=1 /D uint=UINT /D ushort=USHORT /D ulong=ULONG /D uchar=UCHAR /D "_X86_" /D "IS_32" /D "_M_IX86" /D "WANTVXDWRAPS" /D "TDI_WINDOWS_95" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"tdi9x.lib"
# ADD LIB32 /nologo /out:"..\..\lib\i386\checked\tdiw9sys.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\checked
TargetName=tdiw9sys
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tdiclient___Win32_Debug_W9X_VxD"
# PROP BASE Intermediate_Dir "tdiclient___Win32_Debug_W9X_VxD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\w9vxdChecked"
# PROP Target_Dir ""
# ADD CPP /nologo /Zp1 /W3 /Z7 /Oi /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(VTOOLSD)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D i386=1 /D uint=UINT /D ushort=USHORT /D ulong=ULONG /D uchar=UCHAR /D "_X86_" /D "IS_32" /D "_M_IX86" /D "WANTVXDWRAPS" /D "TDI_WINDOWS_95" /D "VTOOLSD" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\i386\checked\tdiw9x.lib"
# ADD LIB32 /nologo /out:"..\..\lib\i386\checked\tdiw9vxd.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\checked
TargetName=tdiw9vxd
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tdiclient___Win32_Release_W9X_VxD"
# PROP BASE Intermediate_Dir "tdiclient___Win32_Release_W9X_VxD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\w9vxdFree"
# PROP Target_Dir ""
# ADD CPP /nologo /Zp1 /ML /W3 /Ob2 /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(VTOOLSD)\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /D "WIN32" /D "_WINDOWS" /D i386=1 /D uint=UINT /D ushort=USHORT /D ulong=ULONG /D uchar=UCHAR /D "_X86_" /D "IS_32" /D "_M_IX86" /D "WANTVXDWRAPS" /D "TDI_WINDOWS_95" /D "VTOOLSD" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\i386\checked\tdiw9vxd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\i386\free\tdiw9vxd.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\free
TargetName=tdiw9vxd
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tdiclient___Win32_Release_W9X_SYS"
# PROP BASE Intermediate_Dir "tdiclient___Win32_Release_W9X_SYS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\w9sysFree"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /ML /W3 /Ob1 /I ".\..\..\include" /I ".\..\..\include\tdiclient" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(BASEDIR)\src\network\wshsmple" /I "$(VTOOLSD)\include" /I "$(DRIVERNETWORKS)\include\dep_ndis" /FI"warning.h" /D DBG=0 /D "WIN32" /D "_WINDOWS" /D i386=1 /D uint=UINT /D ushort=USHORT /D ulong=ULONG /D uchar=UCHAR /D "_X86_" /D "IS_32" /D "_M_IX86" /D "WANTVXDWRAPS" /D "TDI_WINDOWS_95" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\i386\checked\tdiw9sys.lib"
# ADD LIB32 /nologo /out:"..\..\lib\i386\free\tdiw9sys.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\free
TargetName=tdiw9sys
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ENDIF

# Begin Target

# Name "tdiclient - Win32 Debug NT 5"
# Name "tdiclient - Win32 Release NT 5"
# Name "tdiclient - Win32 Debug W9X SYS"
# Name "tdiclient - Win32 Debug W9X VxD"
# Name "tdiclient - Win32 Release W9X VxD"
# Name "tdiclient - Win32 Release W9X SYS"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KIPIntr.cpp
# End Source File
# Begin Source File

SOURCE=.\RouTable.cpp
# End Source File
# Begin Source File

SOURCE=.\tdi9xiface.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiclient.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiconnobj.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiDtgSocket.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdinew.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdintiface.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\TdiPnpCl.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiquery.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiRawSocket.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiSclient.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiSsession.cpp
# End Source File
# Begin Source File

SOURCE=.\tdiStrSocket.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\tdiutil.cpp

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1
# PROP BASE Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\tdiclient\ipnumbers.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\KIpIntr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\KIrpPool.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\KTdiIrp.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\RouTable.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdi9xiface.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiclient.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiconnobj.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdidef9x.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdintiface.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiPartialReceive.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdipnpcl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiSclient.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdisocket.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdistatus.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\tdiclient\tdiStrServer.h

!IF  "$(CFG)" == "tdiclient - Win32 Debug NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release NT 5"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X SYS"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Debug W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X VxD"

!ELSEIF  "$(CFG)" == "tdiclient - Win32 Release W9X SYS"

!ENDIF

# End Source File
# End Group
# End Target
# End Project
