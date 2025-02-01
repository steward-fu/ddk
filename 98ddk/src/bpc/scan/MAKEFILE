# Microsoft Developer Studio Generated NMAKE File, Based on scan.dsp
!IF "$(CFG)" == ""
CFG=scan - Win32 Debug
!MESSAGE No configuration specified. Defaulting to scan - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "scan - Win32 Release" && "$(CFG)" != "scan - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scan.mak" CFG="scan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scan - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "scan - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "scan - Win32 Release"

OUTDIR=.\Release
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scan.exe"

!ELSE 

ALL : "$(OUTDIR)\scan.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\scan.obj"
	-@erase "$(INTDIR)\scan.pch"
	-@erase "$(INTDIR)\scan.res"
	-@erase "$(INTDIR)\scanDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\scan.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98"\
 /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL"\
 /Fp"$(INTDIR)\scan.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scan.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scan.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=setupapi.lib ksuser.lib icodec.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\scan.pdb" /machine:I386\
 /out:"$(OUTDIR)\scan.exe" /libpath:"$(BASEDIR)\lib\i386\free"\
 /libpath:"$(BASEDIR)\src\bpc\lib" 
LINK32_OBJS= \
	"$(INTDIR)\scan.obj" \
	"$(INTDIR)\scan.res" \
	"$(INTDIR)\scanDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\scan.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\scan.exe"
   copy .\Release\scan.exe $(BASEDIR)\lib\i386\free
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "scan - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scan.exe"

!ELSE 

ALL : "$(OUTDIR)\scan.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\scan.obj"
	-@erase "$(INTDIR)\scan.pch"
	-@erase "$(INTDIR)\scan.res"
	-@erase "$(INTDIR)\scanDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\scan.exe"
	-@erase "$(OUTDIR)\scan.ilk"
	-@erase "$(OUTDIR)\scan.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(BASEDIR)\inc" /I\
 "$(BASEDIR)\inc\win98" /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\scan.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scan.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scan.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=setupapi.lib ksuser.lib icodecd.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\scan.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\scan.exe" /pdbtype:sept /libpath:"$(BASEDIR)\lib\i386\free"\
 /libpath:"$(BASEDIR)\src\bpc\lib" 
LINK32_OBJS= \
	"$(INTDIR)\scan.obj" \
	"$(INTDIR)\scan.res" \
	"$(INTDIR)\scanDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\scan.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\scan.exe"
   copy .\Debug\scan.exe $(BASEDIR)\lib\i386\free
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


!IF "$(CFG)" == "scan - Win32 Release" || "$(CFG)" == "scan - Win32 Debug"
SOURCE=.\scan.cpp

!IF  "$(CFG)" == "scan - Win32 Release"

DEP_CPP_SCAN_=\
	"$(BASEDIR)\inc\win98\ks.h"\
	"$(BASEDIR)\inc\win98\ksmedia.h"\
	"$(BASEDIR)\src\bpc\inc\ccdecode.h"\
	"$(BASEDIR)\src\bpc\inc\icodec.h"\
	"$(BASEDIR)\src\bpc\inc\iks.h"\
	"$(BASEDIR)\src\bpc\inc\nabts.h"\
	"$(BASEDIR)\src\bpc\inc\nabtsfec.h"\
	".\Resource.h"\
	".\scan.h"\
	".\scanDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\scan.obj" : $(SOURCE) $(DEP_CPP_SCAN_) "$(INTDIR)"\
 "$(INTDIR)\scan.pch"


!ELSEIF  "$(CFG)" == "scan - Win32 Debug"


"$(INTDIR)\scan.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\scan.pch"


!ENDIF 

SOURCE=.\scan.rc
DEP_RSC_SCAN_R=\
	".\res\scan.ico"\
	".\res\scan.rc2"\
	".\Resource.h"\
	

"$(INTDIR)\scan.res" : $(SOURCE) $(DEP_RSC_SCAN_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\scanDlg.cpp

!IF  "$(CFG)" == "scan - Win32 Release"

DEP_CPP_SCAND=\
	"$(BASEDIR)\inc\win98\ks.h"\
	"$(BASEDIR)\inc\win98\ksmedia.h"\
	"$(BASEDIR)\src\bpc\inc\ccdecode.h"\
	"$(BASEDIR)\src\bpc\inc\icodec.h"\
	"$(BASEDIR)\src\bpc\inc\iks.h"\
	"$(BASEDIR)\src\bpc\inc\nabts.h"\
	"$(BASEDIR)\src\bpc\inc\nabtsfec.h"\
	".\Resource.h"\
	".\scan.h"\
	".\scanDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\scanDlg.obj" : $(SOURCE) $(DEP_CPP_SCAND) "$(INTDIR)"\
 "$(INTDIR)\scan.pch"


!ELSEIF  "$(CFG)" == "scan - Win32 Debug"


"$(INTDIR)\scanDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\scan.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "scan - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(BASEDIR)\inc" /I\
 "$(BASEDIR)\inc\win98" /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\scan.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\scan.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "scan - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(BASEDIR)\inc" /I\
 "$(BASEDIR)\inc\win98" /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\scan.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\scan.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

