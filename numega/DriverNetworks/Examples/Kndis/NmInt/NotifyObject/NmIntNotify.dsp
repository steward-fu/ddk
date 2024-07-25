# Microsoft Developer Studio Project File - Name="NmIntNotify" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NmIntNotify - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "NmIntNotify.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "NmIntNotify.mak" CFG="NmIntNotify - Win32 Unicode Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "NmIntNotify - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NmIntNotify - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NmIntNotify - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NmIntNotify - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugU"
# PROP Intermediate_Dir "DebugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /X /I "$(DRIVERNETWORKS)\include\Notify" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\inc\atl30" /I "$(MFC_INCLUDES)" /D "WIN32_LEAN_AND_MEAN" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /Fd"DebugU/NmIntNotify.pdb" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /I "$(CRT_INC_PATH)" /I "$(MFC_INCLUDES)"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib oldnames.lib libcmtd.lib libcpmtd.lib Notify.lib kernel32.lib user32.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib setupapi.lib /def:NmIntNotify.def /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /nodefaultlib /libpath:"$(DRIVERNETWORKS)\lib\i386\checked" /libpath:"$(TARGET_LIB_PATH)" /IGNORE:4078,4099
# SUBTRACT LINK32 /verbose /pdb:none
# Begin Custom Build - Building SoftICE symbol file
TargetDir=.\DebugU
TargetPath=.\DebugU\NmIntNotify.dll
TargetName=NmIntNotify
InputPath=.\DebugU\NmIntNotify.dll
SOURCE="$(InputPath)"

"$(TargetDir)\$(TargetName).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /TRANS:SOURCE,PACKAGE $(TargetPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "NmIntNotify - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinSize"
# PROP Intermediate_Dir "ReleaseUMinSize"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O1 /X /I "$(DRIVERNETWORKS)\include\Notify" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\inc\atl30" /I "$(MFC_INCLUDES)" /D "WIN32_LEAN_AND_MEAN" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /I "$(CRT_INC_PATH)" /I "$(MFC_INCLUDES)"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib oldnames.lib libcmt.lib libcpmt.lib Notify.lib kernel32.lib user32.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib setupapi.lib atl.lib /def:NmIntNotify.def /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /libpath:"$(TARGET_LIB_PATH)" /ignore:4078,4089,4099
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\NmIntNotify.dll
InputPath=.\ReleaseUMinSize\NmIntNotify.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT
	if not "%OS%"=="Windows_NT" goto NOTNT
	regsvr32 /s /c "$(TargetPath)"
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"
	goto end
	:NOTNT
	echo Warning : Cannot register Unicode DLL on Windows 95
	:end

# End Custom Build

!ELSEIF  "$(CFG)" == "NmIntNotify - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUMinDependency"
# PROP Intermediate_Dir "ReleaseUMinDependency"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /X /I "$(DRIVERNETWORKS)\include\Notify" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(BASEDIR)\inc\atl30" /I "$(MFC_INCLUDES)" /D "WIN32_LEAN_AND_MEAN" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /I "$(CRT_INC_PATH)" /I "$(MFC_INCLUDES)"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib oldnames.lib libcmt.lib libcpmt.lib Notify.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib uuid.lib /def:NmIntNotify.def /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /libpath:"$(DRIVERNETWORKS)\lib\i386\free" /libpath:"$(TARGET_LIB_PATH)" /ignore:4078,4099
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing registration
OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\NmIntNotify.dll
InputPath=.\ReleaseUMinDependency\NmIntNotify.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT
	if not "%OS%"=="Windows_NT" goto NOTNT
	regsvr32 /s /c "$(TargetPath)"
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"
	goto end
	:NOTNT
	echo Warning : Cannot register Unicode DLL on Windows 95
	:end

# End Custom Build

!ENDIF

# Begin Target

# Name "NmIntNotify - Win32 Unicode Debug"
# Name "NmIntNotify - Win32 Unicode Release MinSize"
# Name "NmIntNotify - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MyBindingPolicy.cpp
# End Source File
# Begin Source File

SOURCE=.\MyPropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MyPropertyUi.cpp
# End Source File
# Begin Source File

SOURCE=.\NmIntNotify.cpp
# End Source File
# Begin Source File

SOURCE=.\NmIntNotify.def
# End Source File
# Begin Source File

SOURCE=.\NmIntNotify.idl
# ADD MTL /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /tlb "NmIntNotify.tlb"
# End Source File
# Begin Source File

SOURCE=.\NmIntNotify.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MyBindingPolicy.h
# End Source File
# Begin Source File

SOURCE=.\MyPropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\MyPropertyUi.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\NmIntNotify.RGS
# End Source File
# End Group
# End Target
# End Project
