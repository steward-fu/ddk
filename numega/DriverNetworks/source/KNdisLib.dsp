# Microsoft Developer Studio Project File - Name="KNdisLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=KNdisLib - Win32 NDIS 5 Miniport Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "KNdisLib.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "KNdisLib.mak" CFG="KNdisLib - Win32 NDIS 5 Miniport Checked"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "KNdisLib - Win32 NDIS 4 Miniport Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "KNdisLib - Win32 NDIS 4 Miniport Free" (based on "Win32 (x86) Static Library")
!MESSAGE "KNdisLib - Win32 NDIS 5 Miniport Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "KNdisLib - Win32 NDIS 5 Miniport Free" (based on "Win32 (x86) Static Library")
!MESSAGE "KNdisLib - Win32 NDIS 5 Protocol Checked" (based on "Win32 (x86) Static Library")
!MESSAGE "KNdisLib - Win32 NDIS 5 Protocol Free" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KNdisLib___Win32_NDIS_4_Miniport_Checked"
# PROP BASE Intermediate_Dir "KNdisLib___Win32_NDIS_4_Miniport_Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\n4mpchecked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /MLd /W3 /Z7 /Oi /Gy /I "..\..\Common\Include" /I "..\..\Common\Include\Stl" /I ".\..\include" /I "." /I ".\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"warning.h" /D DBG=1 /D FPO=0 /D NDIS40_MINIPORT=1 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D NTVERSION=400 /D "NDIS_MINIPORT_DRIVER" /D BINARY_COMPATIBLE=1 /D "KNDIS_LIB_BUILD" /FR /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked/kndis.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked/kndis.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\checked\vdw.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\checked\kndis4mp.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\checked
TargetName=kndis4mp
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KNdisLib___Win32_NDIS_4_Miniport_Free"
# PROP BASE Intermediate_Dir "KNdisLib___Win32_NDIS_4_Miniport_Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\n4mpfree"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W3 /Gy /I "..\..\Common\Include" /I "..\..\Common\Include\Stl" /I "..\include" /I ".\..\include" /I "." /I ".\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"warning.h" /D FPO=1 /D "NDEBUG" /D DBG=0 /D NDIS40_MINIPORT=1 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D NTVERSION=400 /D "NDIS_MINIPORT_DRIVER" /D BINARY_COMPATIBLE=1 /D "KNDIS_LIB_BUILD" /FR /FD /Oxs /cbstring /GF /Oxs /QIfdiv- /QIf /Zel /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\free/kndis.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\free/kndis.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\free\vdw.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\free\kndis4mp.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\free
TargetName=kndis4mp
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KNdisLib___Win32_NDIS_5_Miniport_Checked"
# PROP BASE Intermediate_Dir "KNdisLib___Win32_NDIS_5_Miniport_Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\n5mpchecked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /MLd /W4 /Z7 /Oi /Gy /I "..\..\Common\Include" /I "..\..\Common\Include\Stl" /I ".\..\include" /I "." /I ".\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"warning.h" /D NDIS50=1 /D NDIS50_MINIPORT=1 /D "NDIS_MINIPORT_DRIVER" /D "KNDIS_LIB_BUILD" /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D DEPRECATE_DDK_FUNCTIONS /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked/kndis.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked/kndis.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\checked\kndis4mp.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\checked\kndis5mp.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\checked
TargetName=kndis5mp
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KNdisLib___Win32_NDIS_5_Miniport_Free"
# PROP BASE Intermediate_Dir "KNdisLib___Win32_NDIS_5_Miniport_Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\n5mpfree"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W4 /Gy /I "..\..\Common\Include" /I "..\..\Common\Include\Stl" /I "..\include" /I ".\..\include" /I "." /I ".\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"warning.h" /D NDIS50=1 /D NDIS50_MINIPORT=1 /D "NDIS_MINIPORT_DRIVER" /D "KNDIS_LIB_BUILD" /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Oxs /cbstring /GF /Oxs /QIfdiv- /QIf /Zel /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\free/kndis.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\free/kndis.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\free\kndis4mp.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\free\kndis5mp.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\free
TargetName=kndis5mp
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KNdisLib___Win32_NDIS_5_Protocol_Checked"
# PROP BASE Intermediate_Dir "KNdisLib___Win32_NDIS_5_Protocol_Checked"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\checked"
# PROP Intermediate_Dir "obj\i386\n5ptchecked"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /MLd /W4 /Z7 /Oi /Gy /I "..\..\Common\Include" /I ".\..\include" /I "." /I ".\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"warning.h" /D NDIS50=1 /D "KNDIS_LIB_BUILD" /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D DBG=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\checked/kndis.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\checked/kndis.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\checked\kndis5mp.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\checked\kndis5pt.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\checked
TargetName=kndis5pt
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KNdisLib___Win32_NDIS_5_Protocol_Free"
# PROP BASE Intermediate_Dir "KNdisLib___Win32_NDIS_5_Protocol_Free"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\i386\free"
# PROP Intermediate_Dir "obj\i386\n5ptfree"
# PROP Target_Dir ""
# ADD CPP /nologo /Gz /W4 /Gy /I "..\..\Common\Include" /I "..\include" /I "." /I ".\include" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(BASEDIR)\src\network\inc" /I "$(DRIVERWORKS)\include" /FI"warning.h" /D NDIS50=1 /D "KNDIS_LIB_BUILD" /D _X86_=1 /D i386=1 /D STD_CALL /D CONDITION_HANDLING=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D NDEBUG /D _DLL=1 /D _WIN32_WINNT=$(_WIN32_WINNT) /D WINVER=$(WINVER) /D _WIN32_IE=$(_WIN32_IE) /D NTDDI_VERSION=$(NTDDI_VERSION) /FR /FD /Oxs /cbstring /GF /Oxs /QIfdiv- /QIf /Zel /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\lib\i386\free/kndis.bsc"
# ADD BSC32 /nologo /o"..\lib\i386\free/kndis.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\i386\free\kndis5mp.lib"
# ADD LIB32 /nologo /out:"..\lib\i386\free\kndis5pt.lib"
# Begin Special Build Tool
TargetDir=\DS\DSSource\FrameWork\DriverNetWorks\lib\i386\free
TargetName=kndis5pt
SOURCE="$(InputPath)"
PostBuild_Desc=Build Environment Variables:
PostBuild_Cmds=$(DRIVERWORKS)\bin\saveenv "$(TargetDir)" "$(TargetName)"
# End Special Build Tool

!ENDIF

# Begin Target

# Name "KNdisLib - Win32 NDIS 4 Miniport Checked"
# Name "KNdisLib - Win32 NDIS 4 Miniport Free"
# Name "KNdisLib - Win32 NDIS 5 Miniport Checked"
# Name "KNdisLib - Win32 NDIS 5 Miniport Free"
# Name "KNdisLib - Win32 NDIS 5 Protocol Checked"
# Name "KNdisLib - Win32 NDIS 5 Protocol Free"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\KNdisBufferHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\KNdisBundle.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\kndiscpp.cpp
# End Source File
# Begin Source File

SOURCE=.\KNdisFilterAdapter.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisFilterAdapter51.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisFilterBinding.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisInterrupt.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisList.cpp
# End Source File
# Begin Source File

SOURCE=.\KNdisMiniDriver.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisOidDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\KNdisPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\KNdisPacketsTransfer.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisParamResource.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisPciResource.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisPnpResource.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisProtocolBinding.cpp
# End Source File
# Begin Source File

SOURCE=.\KNdisReceiveArea.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\KNdisSharedMemory.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KNdisTrace.cpp
# End Source File
# Begin Source File

SOURCE="$(DRIVERNETWORKS)\..\Common\source\kstl.cpp"

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE="$(DRIVERNETWORKS)\..\Common\source\kstllmcb.cpp"

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE="$(DRIVERNETWORKS)\..\Common\source\kstlnphs.cpp"

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\KTransportPnp.cpp

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# ADD CPP /D BINARY_COMPATIBLE=0
# SUBTRACT CPP /D BINARY_COMPATIBLE=1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# ADD CPP /D BINARY_COMPATIBLE=0
# SUBTRACT CPP /D BINARY_COMPATIBLE=1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\Common\Source\stcinit.cpp
# End Source File
# Begin Source File

SOURCE="$(DRIVERNETWORKS)\..\Common\source\suballoc.cpp"

!IF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 4 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Checked"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Miniport Free"

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Checked"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "KNdisLib - Win32 NDIS 5 Protocol Free"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\KIrpPool.h
# End Source File
# Begin Source File

SOURCE=..\include\KListBrowser.h
# End Source File
# Begin Source File

SOURCE=..\include\kndis.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisAdapterTraits.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisBuffer.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisBufferHeap.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisBufferPool.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisBundle.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCallManagerChars.h
# End Source File
# Begin Source File

SOURCE=..\include\KNDisCallManagerVc.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCallManagerVcTraits.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCallManagerWrapper.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisChars.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCoAddressFamily.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCoMiniportChars.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisConfig.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCoOid.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCoOidDefs.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisCoWrapper.h
# End Source File
# Begin Source File

SOURCE=..\include\kndiscpp.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisEvent.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisFilterAdapter.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisFilterAdapter51.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisFilterBinding.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisFilterPacketPool.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisFilterWrapper.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisHeap.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisInterlocked.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisInterrupt.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisList.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisLookahead.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisLookaside.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisMedium.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisMiniAdapter.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisMiniDriver.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisMiniportVc.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisMiniportVcTraits.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisOid.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisOidDebug.h
# End Source File
# Begin Source File

SOURCE=.\KNdisOidDebugStrings.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisOidDefs.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisOidDefs4.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisOidDefs5.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisOidTraits.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPacket.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPacketCancelId.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPacketList.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPacketPool.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPacketsTransfer.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPartialPacket.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPciInformation.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPeripheralAddress.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPhysAddr.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisPnpMode.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisProtocolBinding.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisProtocolDriver.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisProtocolTraits.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisProtocolWrapper.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisReceiveArea.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisReceiveAreaTraits.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisRequest.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisResource.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisRxAreaAsyncMgr.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisRxAreaNoAsync.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisSharedMemory.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisSharedReceiveArea.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisSpinLock.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisStats.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisString.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisSystemReceiveArea.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisTimer.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisTrace.h
# End Source File
# Begin Source File

SOURCE=..\include\KNdisWrapper.h
# End Source File
# Begin Source File

SOURCE=..\include\KObject.h
# End Source File
# Begin Source File

SOURCE="$(DRIVERWORKS)\include\kstl.h"
# End Source File
# Begin Source File

SOURCE=..\include\KTA_ADDRESS.h
# End Source File
# Begin Source File

SOURCE=..\include\KTransport.h
# End Source File
# Begin Source File

SOURCE=..\include\KTransportIrpDispatch.h
# End Source File
# Begin Source File

SOURCE=..\include\KTransportPnp.h
# End Source File
# Begin Source File

SOURCE=..\include\KTransportTraits.h
# End Source File
# Begin Source File

SOURCE="$(DRIVERWORKS)\include\lockable.h"
# End Source File
# Begin Source File

SOURCE=..\..\Common\Include\stcinit.h
# End Source File
# Begin Source File

SOURCE="$(DRIVERWORKS)\include\suballoc.h"
# End Source File
# End Group
# End Target
# End Project
