# Microsoft Developer Studio Generated NMAKE File, Based on hDmp.dsp
!IF "$(CFG)" == ""
CFG=hDmp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hDmp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hDmp - Win32 Release" && "$(CFG)" != "hDmp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hDmp.mak" CFG="hDmp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hDmp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "hDmp - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "hDmp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\CCDump.exe"

!ELSE 

ALL : "$(OUTDIR)\CCDump.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\hDmp.obj"
	-@erase "$(INTDIR)\hDmp.pch"
	-@erase "$(INTDIR)\hDmp.res"
	-@erase "$(INTDIR)\hDmpDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\CCDump.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98"\
 /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL"\
 /Fp"$(INTDIR)\hDmp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=$(INTDIR)/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\hDmp.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hDmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=setupapi.lib ksuser.lib icodec.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\CCDump.pdb" /machine:I386\
 /out:"$(OUTDIR)\CCDump.exe" /libpath:"$(BASEDIR)\lib\i386\free"\
 /libpath:"$(BASEDIR)\src\bpc\lib" 
LINK32_OBJS= \
	"$(INTDIR)\hDmp.obj" \
	"$(INTDIR)\hDmp.res" \
	"$(INTDIR)\hDmpDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\CCDump.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CCDump.exe"
   copy .\Release\ccdump.exe $(BASEDIR)\lib\i386\free
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "hDmp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\CCDump.exe"

!ELSE 

ALL : "$(OUTDIR)\CCDump.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\hDmp.obj"
	-@erase "$(INTDIR)\hDmp.pch"
	-@erase "$(INTDIR)\hDmp.res"
	-@erase "$(INTDIR)\hDmpDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\CCDump.exe"
	-@erase "$(OUTDIR)\CCDump.ilk"
	-@erase "$(OUTDIR)\CCDump.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(BASEDIR)\inc" /I\
 "$(BASEDIR)\inc\win98" /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\hDmp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=$(INTDIR)/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\hDmp.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hDmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=setupapi.lib ksuser.lib icodecd.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\CCDump.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\CCDump.exe" /pdbtype:sept /libpath:"$(BASEDIR)\lib\i386\free"\
 /libpath:"$(BASEDIR)\src\bpc\lib" 
LINK32_OBJS= \
	"$(INTDIR)\hDmp.obj" \
	"$(INTDIR)\hDmp.res" \
	"$(INTDIR)\hDmpDlg.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\CCDump.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CCDump.exe"
   copy .\Debug\ccdump.exe $(BASEDIR)\lib\i386\free
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


!IF "$(CFG)" == "hDmp - Win32 Release" || "$(CFG)" == "hDmp - Win32 Debug"
SOURCE=.\hDmp.cpp
DEP_CPP_HDMP_=\
	".\hDmp.h"\
	".\hDmpDlg.h"\
	".\Resource.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\hDmp.obj" : $(SOURCE) $(DEP_CPP_HDMP_) "$(INTDIR)"\
 "$(INTDIR)\hDmp.pch"


SOURCE=.\hDmp.rc
DEP_RSC_HDMP_R=\
	".\res\hDmp.ico"\
	".\res\hDmp.rc2"\
	".\Resource.h"\
	

"$(INTDIR)\hDmp.res" : $(SOURCE) $(DEP_RSC_HDMP_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\hDmpDlg.cpp
DEP_CPP_HDMPD=\
	"$(BASEDIR)\inc\win98\ks.h"\
	"$(BASEDIR)\inc\win98\ksmedia.h"\
	"$(BASEDIR)\src\bpc\inc\ccdecode.h"\
	"$(BASEDIR)\src\bpc\inc\icodec.h"\
	"$(BASEDIR)\src\bpc\inc\iks.h"\
	".\hDmp.h"\
	".\hDmpDlg.h"\
	".\Resource.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\hDmpDlg.obj" : $(SOURCE) $(DEP_CPP_HDMPD) "$(INTDIR)"\
 "$(INTDIR)\hDmp.pch"


SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "hDmp - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(BASEDIR)\inc" /I\
 "$(BASEDIR)\inc\win98" /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\hDmp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\hDmp.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "hDmp - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(BASEDIR)\inc" /I\
 "$(BASEDIR)\inc\win98" /I "$(BASEDIR)\src\bpc\inc" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /Fp"$(INTDIR)\hDmp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\hDmp.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

