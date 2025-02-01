# Microsoft Developer Studio Generated NMAKE File, Based on Ccbeacon.dsp
!IF "$(CFG)" == ""
CFG=ccbeacon - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ccbeacon - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ccbeacon - Win32 Release" && "$(CFG)" !=\
 "ccbeacon - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ccbeacon.mak" CFG="ccbeacon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ccbeacon - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "ccbeacon - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ccbeacon - Win32 Release"

OUTDIR=.\Release
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Ccbeacon.exe"

!ELSE 

ALL : "$(OUTDIR)\Ccbeacon.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ccbeacon.obj"
	-@erase "$(INTDIR)\Getopt.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Ccbeacon.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\Ccbeacon.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ccbeacon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\Ccbeacon.pdb" /machine:I386 /out:"$(OUTDIR)\Ccbeacon.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ccbeacon.obj" \
	"$(INTDIR)\Getopt.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\Ccbeacon.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Ccbeacon.exe"
   copy .\Release\ccbeacon.exe $(BASEDIR)\lib\i386\free
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ccbeacon - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Ccbeacon.exe"

!ELSE 

ALL : "$(OUTDIR)\Ccbeacon.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ccbeacon.obj"
	-@erase "$(INTDIR)\Getopt.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Ccbeacon.exe"
	-@erase "$(OUTDIR)\Ccbeacon.ilk"
	-@erase "$(OUTDIR)\Ccbeacon.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\Ccbeacon.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Ccbeacon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\Ccbeacon.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\Ccbeacon.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ccbeacon.obj" \
	"$(INTDIR)\Getopt.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\Ccbeacon.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Ccbeacon.exe"
   copy .\Debug\ccbeacon.exe $(BASEDIR)\lib\i386\free
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "ccbeacon - Win32 Release" || "$(CFG)" ==\
 "ccbeacon - Win32 Debug"
SOURCE=.\ccbeacon.cpp
DEP_CPP_CCBEA=\
	".\ccbeacon.h"\
	

"$(INTDIR)\ccbeacon.obj" : $(SOURCE) $(DEP_CPP_CCBEA) "$(INTDIR)"


SOURCE=.\Getopt.c

"$(INTDIR)\Getopt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp
DEP_CPP_MAIN_=\
	".\ccbeacon.h"\
	".\Getopt.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"



!ENDIF 

