# Microsoft Developer Studio Project File - Name="BMDmaTst" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=BMDmaTst - Win32 Checked
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BMDmaTst.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BMDmaTst.mak" CFG="BMDmaTst - Win32 Checked"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BMDmaTst - Win32 Free" (based on "Win32 (x86) Application")
!MESSAGE "BMDmaTst - Win32 Checked" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BMDmaTst - Win32 Free"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objfre\i386\"
# PROP Intermediate_Dir ".\objfre\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /W3 /Zel -cbstring /Gy /GF /c /FI"warning.h" /Gz /FR /QIfdiv- /QIf /Oxs /Oy /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis"
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "$(DRIVERWORKS)\include" /d "NDEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objfre/i386/BMDmaTst.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib $(DRIVERWORKS)\lib\i386\free\vdw.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:MINIMAL /out:"objfre\i386\BMDmaTst.sys"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objfre\i386\BMDmaTst.sys
TargetName=BMDmaTst
InputPath=.\objfre\i386\BMDmaTst.sys
SOURCE="$(InputPath)"

".\objfre\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source;package;always $(TARGETPATH)

# End Custom Build

!ELSEIF  "$(CFG)" == "BMDmaTst - Win32 Checked"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\objchk\i386\"
# PROP Intermediate_Dir ".\objchk\i386\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP
# ADD CPP /nologo /ML /W3 /Zel -cbstring /Gy /GF /c /FI"warning.h" /Gz /FR /QIfdiv- /QIf /Zi /Oi /D NTVERSION=400 /D _X86_=1 /D i386=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D DBG=1 /D FPO=0 /I "..\..\common\include\stl" /I "$(TARGET_INC_PATH)" /I "$(CRT_INC_PATH)" /I "$(DDK_INC_PATH)" /I "$(WDM_INC_PATH)" /I "$(DRIVERWORKS)\Include" /I "$(DRIVERWORKS)\source" /I "$(DRIVERWORKS)\include\dep_vxd" /I "$(DRIVERWORKS)\include\dep_wdm" /I "$(DRIVERWORKS)\include\dep_ndis" /Fd".\objchk\i386\\BMDmaTst.pdb"
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "$(DRIVERWORKS)\include" /d "_DEBUG" /d WIN32_LEAN_AND_MEAN=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk/i386/BMDmaTst.bsc"
LINK32=link.exe
# ADD BASE LINK32
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib $(DRIVERWORKS)\lib\i386\checked\vdw.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  /nologo /base:"0x10000" /version:5.0 /stack:0x40000,0x1000 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /INCREMENTAL:NO /libpath:"$(TARGET_LIB_PATH)" /driver /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096,4210 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /OPT:REF /OPT:ICF /align:0x80 /osversion:5.00 /subsystem:native,1.10 /debug:FULL /debugtype:cv /out:"objchk\i386\BMDmaTst.sys"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - ---------------------------Build SoftICE Symbols----------------------------
TargetPath=.\objchk\i386\BMDmaTst.sys
TargetName=BMDmaTst
InputPath=.\objchk\i386\BMDmaTst.sys
SOURCE="$(InputPath)"

".\objchk\i386\$(TARGETNAME).nms" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(DRIVERWORKS)\bin\nmsym /trans:source;package;always $(TARGETPATH)

# End Custom Build

!ENDIF 

# Begin Target

# Name "BMDmaTst - Win32 Free"
# Name "BMDmaTst - Win32 Checked"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BMDmaTst.cpp
# End Source File
# Begin Source File

SOURCE=.\BMDmaTst.rc
# End Source File
# Begin Source File

SOURCE=.\BMDmaTstDevice.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BMDmaTst.h
# End Source File
# Begin Source File

SOURCE=.\BMDmaTstDevice.h
# End Source File
# Begin Source File

SOURCE=.\function.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\BMDmaTst.ini
# End Source File
# End Target
# End Project
