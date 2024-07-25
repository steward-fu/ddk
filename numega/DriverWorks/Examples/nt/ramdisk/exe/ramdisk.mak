# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=ramdisk - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ramdisk - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ramdisk - Win32 Release" && "$(CFG)" !=\
 "ramdisk - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "RAMDISK.MAK" CFG="ramdisk - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ramdisk - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ramdisk - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "ramdisk - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "ramdisk - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\RAMDISK.exe"

CLEAN : 
	-@erase "$(INTDIR)\INSTDRV.OBJ"
	-@erase "$(INTDIR)\ramdisk.obj"
	-@erase "$(INTDIR)\ramdisk.res"
	-@erase "$(OUTDIR)\RAMDISK.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/RAMDISK.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ramdisk.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/RAMDISK.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/RAMDISK.pdb" /machine:I386\
 /out:"$(OUTDIR)/RAMDISK.exe" 
LINK32_OBJS= \
	"$(INTDIR)\INSTDRV.OBJ" \
	"$(INTDIR)\ramdisk.obj" \
	"$(INTDIR)\ramdisk.res"

"$(OUTDIR)\RAMDISK.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ramdisk - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\RAMDISK.exe" "$(OUTDIR)\RAMDISK.bsc"

CLEAN : 
	-@erase "$(INTDIR)\INSTDRV.OBJ"
	-@erase "$(INTDIR)\INSTDRV.SBR"
	-@erase "$(INTDIR)\ramdisk.obj"
	-@erase "$(INTDIR)\ramdisk.res"
	-@erase "$(INTDIR)\ramdisk.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\RAMDISK.bsc"
	-@erase "$(OUTDIR)\RAMDISK.exe"
	-@erase "$(OUTDIR)\RAMDISK.ilk"
	-@erase "$(OUTDIR)\RAMDISK.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/RAMDISK.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ramdisk.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/RAMDISK.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\INSTDRV.SBR" \
	"$(INTDIR)\ramdisk.sbr"

"$(OUTDIR)\RAMDISK.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/RAMDISK.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/RAMDISK.exe" 
LINK32_OBJS= \
	"$(INTDIR)\INSTDRV.OBJ" \
	"$(INTDIR)\ramdisk.obj" \
	"$(INTDIR)\ramdisk.res"

"$(OUTDIR)\RAMDISK.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "ramdisk - Win32 Release"
# Name "ramdisk - Win32 Debug"

!IF  "$(CFG)" == "ramdisk - Win32 Release"

!ELSEIF  "$(CFG)" == "ramdisk - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\RESOURCE.H

!IF  "$(CFG)" == "ramdisk - Win32 Release"

!ELSEIF  "$(CFG)" == "ramdisk - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ramdisk.rc

"$(INTDIR)\ramdisk.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ramdisk.c
DEP_CPP_RAMDI=\
	"..\common.h"\
	".\INSTDRV.H"\
	

!IF  "$(CFG)" == "ramdisk - Win32 Release"


"$(INTDIR)\ramdisk.obj" : $(SOURCE) $(DEP_CPP_RAMDI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ramdisk - Win32 Debug"


"$(INTDIR)\ramdisk.obj" : $(SOURCE) $(DEP_CPP_RAMDI) "$(INTDIR)"

"$(INTDIR)\ramdisk.sbr" : $(SOURCE) $(DEP_CPP_RAMDI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INSTDRV.H

!IF  "$(CFG)" == "ramdisk - Win32 Release"

!ELSEIF  "$(CFG)" == "ramdisk - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\INSTDRV.C
DEP_CPP_INSTD=\
	".\INSTDRV.H"\
	

!IF  "$(CFG)" == "ramdisk - Win32 Release"


"$(INTDIR)\INSTDRV.OBJ" : $(SOURCE) $(DEP_CPP_INSTD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ramdisk - Win32 Debug"


"$(INTDIR)\INSTDRV.OBJ" : $(SOURCE) $(DEP_CPP_INSTD) "$(INTDIR)"

"$(INTDIR)\INSTDRV.SBR" : $(SOURCE) $(DEP_CPP_INSTD) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
