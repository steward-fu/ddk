# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=mynp - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to mynp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mynp - Win32 Release" && "$(CFG)" != "mynp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mynp.mak" CFG="mynp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mynp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mynp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "mynp - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mynp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "..\..\..\..\temp\donecode\npcode\mynp.dll"

CLEAN : 
	-@erase "$(INTDIR)\mynpcaps.obj"
	-@erase "$(INTDIR)\mynpconn.obj"
	-@erase "$(INTDIR)\mynpenum.obj"
	-@erase "$(INTDIR)\mynplog.obj"
	-@erase "$(OUTDIR)\mynp.exp"
	-@erase "$(OUTDIR)\mynp.lib"
	-@erase "..\..\..\..\temp\donecode\npcode\mynp.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/mynp.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mynp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"c:\temp\donecode\npcode\mynp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/mynp.pdb" /machine:I386 /def:".\mynp.def"\
 /out:"c:\temp\donecode\npcode\mynp.dll" /implib:"$(OUTDIR)/mynp.lib" 
DEF_FILE= \
	".\mynp.def"
LINK32_OBJS= \
	"$(INTDIR)\mynpcaps.obj" \
	"$(INTDIR)\mynpconn.obj" \
	"$(INTDIR)\mynpenum.obj" \
	"$(INTDIR)\mynplog.obj"

"..\..\..\..\temp\donecode\npcode\mynp.dll" : "$(OUTDIR)" $(DEF_FILE)\
 $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mynp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : ".\mynp.dll"

CLEAN : 
	-@erase "$(INTDIR)\mynpcaps.obj"
	-@erase "$(INTDIR)\mynpconn.obj"
	-@erase "$(INTDIR)\mynpenum.obj"
	-@erase "$(INTDIR)\mynplog.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mynp.exp"
	-@erase "$(OUTDIR)\mynp.lib"
	-@erase "$(OUTDIR)\mynp.pdb"
	-@erase ".\mynp.dll"
	-@erase ".\mynp.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/mynp.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mynp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"mynp.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/mynp.pdb" /debug /machine:I386 /def:".\mynp.def"\
 /out:"mynp.dll" /implib:"$(OUTDIR)/mynp.lib" 
DEF_FILE= \
	".\mynp.def"
LINK32_OBJS= \
	"$(INTDIR)\mynpcaps.obj" \
	"$(INTDIR)\mynpconn.obj" \
	"$(INTDIR)\mynpenum.obj" \
	"$(INTDIR)\mynplog.obj"

".\mynp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "mynp - Win32 Release"
# Name "mynp - Win32 Debug"

!IF  "$(CFG)" == "mynp - Win32 Release"

!ELSEIF  "$(CFG)" == "mynp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\mynpcaps.c
DEP_CPP_MYNPC=\
	{$(INCLUDE)}"\netmpr.h"\
	{$(INCLUDE)}"\netspi.h"\
	

"$(INTDIR)\mynpcaps.obj" : $(SOURCE) $(DEP_CPP_MYNPC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mynp.def

!IF  "$(CFG)" == "mynp - Win32 Release"

!ELSEIF  "$(CFG)" == "mynp - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mynpenum.c
DEP_CPP_MYNPE=\
	{$(INCLUDE)}"\netmpr.h"\
	{$(INCLUDE)}"\netspi.h"\
	

"$(INTDIR)\mynpenum.obj" : $(SOURCE) $(DEP_CPP_MYNPE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mynplog.c
DEP_CPP_MYNPL=\
	{$(INCLUDE)}"\netmpr.h"\
	{$(INCLUDE)}"\netspi.h"\
	{$(INCLUDE)}"\pwdspi.h"\
	

"$(INTDIR)\mynplog.obj" : $(SOURCE) $(DEP_CPP_MYNPL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mynpconn.c
DEP_CPP_MYNPCO=\
	{$(INCLUDE)}"\netmpr.h"\
	{$(INCLUDE)}"\netspi.h"\
	

"$(INTDIR)\mynpconn.obj" : $(SOURCE) $(DEP_CPP_MYNPCO) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
