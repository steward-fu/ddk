# Microsoft Developer Studio Project File - Name="sonydcam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sonydcam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sonydcam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sonydcam.mak" CFG="sonydcam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sonydcam - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sonydcam - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sonydcam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Rel98"
# PROP Intermediate_Dir "Rel98"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /O2 /X /I "..\..\..\..\dev\ntddk\inc" /I "..\..\..\..\dev\tools\c32\inc" /I ".\\" /I "..\..\..\ddk\inc" /I "..\..\bus" /D "NDEBUG" /D "USE_DMA_MACROS" /D "WIN95_BUILD" /D "_PNP_POWER_" /D "NTKERN" /D "_X86_" /D "_PNP_POWER_STUB_ENABLED_" /D "PNP" /D "WIN32_LEAN_AND_MEAN" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /x /i "..\..\..\..\dev\inc" /i "..\..\..\..\dev\tools\c32\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 wdm.lib ks.lib stream.lib ksguid.lib libcmt.lib /nologo /entry:"DriverEntry@8" /dll /map /machine:I386 /nodefaultlib /out:"Rel98/sonydcam.sys" /libpath:"..\..\..\ddk\lib\i386" /libpath:"..\..\..\..\dev\tools\c32\lib" /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
InputPath=.\Rel98\sonydcam.sys
SOURCE=$(InputPath)

".\Rel98\sonydcam.sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym.exe -o .\Rel98\sonydcam.sym\
                               .\Rel98\sonydcam.map

# End Custom Build

!ELSEIF  "$(CFG)" == "sonydcam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sonydcam"
# PROP BASE Intermediate_Dir "sonydcam"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Dbg98"
# PROP Intermediate_Dir "Dbg98"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MTd /W3 /Zi /Od /Gf /Gy /X /I "..\..\..\..\dev\ntddk\inc" /I "..\..\..\..\dev\tools\c32\inc" /I ".\\" /I "..\..\..\ddk\inc" /I "..\..\bus" /D "_DEBUG" /D "DEBUG" /D "DBG" /D "USE_DMA_MACROS" /D "WIN95_BUILD" /D "_PNP_POWER_" /D "NTKERN" /D "_X86_" /D "_PNP_POWER_STUB_ENABLED_" /D "PNP" /D "WIN32_LEAN_AND_MEAN" /FR /YX /FD /c
# SUBTRACT CPP /u
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /x /i "..\..\..\..\dev\inc" /i "..\..\..\..\dev\tools\c32\inc" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wdm.lib ks.lib stream.lib ksguid.lib libcmt.lib /nologo /entry:"DriverEntry@8" /dll /incremental:no /map /machine:I386 /nodefaultlib /out:"Dbg98/sonydcam.sys" /libpath:"..\..\..\ddk\lib\i386" /libpath:"..\..\..\..\dev\tools\c32\lib" /debug:FULL /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
InputPath=.\Dbg98\sonydcam.sys
SOURCE=$(InputPath)

".\Dbg98\sonydcam.sym" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	mapsym.exe -o .\Dbg98\sonydcam.sym\
                               .\Dbg98\sonydcam.map

# End Custom Build

!ENDIF 

# Begin Target

# Name "sonydcam - Win32 Release"
# Name "sonydcam - Win32 Debug"
# Begin Source File

SOURCE=.\dbg.c
# End Source File
# Begin Source File

SOURCE=.\dcampkt.c
# End Source File
# Begin Source File

SOURCE=.\sonydcam.c
# End Source File
# Begin Source File

SOURCE=.\sonydcam.rc

!IF  "$(CFG)" == "sonydcam - Win32 Release"

!ELSEIF  "$(CFG)" == "sonydcam - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
