# Microsoft Developer Studio Generated NMAKE File, Based on ccbc.dsp
!IF "$(CFG)" == ""
CFG=ccbc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ccbc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ccbc - Win32 Release" && "$(CFG)" != "ccbc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ccbc.mak" CFG="ccbc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ccbc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ccbc - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "ccbc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ccbc.exe"

!ELSE 

ALL : "$(OUTDIR)\ccbc.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ccbc.obj"
	-@erase "$(INTDIR)\ccbc.pch"
	-@erase "$(INTDIR)\ccbc.res"
	-@erase "$(INTDIR)\ccbcDlg.obj"
	-@erase "$(INTDIR)\ccbeacon.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\ccbc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "$(BASEDIR)\src\bpc\inc" /I\
 "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\ccbc.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ccbc.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ccbc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=setupapi.lib ksuser.lib icodec.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\ccbc.pdb" /machine:I386\
 /out:"$(OUTDIR)\ccbc.exe" /libpath:"$(BASEDIR)\src\bpc\lib"\
 /libpath:"$(BASEDIR)\lib\i386\free" 
LINK32_OBJS= \
	"$(INTDIR)\ccbc.obj" \
	"$(INTDIR)\ccbc.res" \
	"$(INTDIR)\ccbcDlg.obj" \
	"$(INTDIR)\ccbeacon.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\ccbc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
PostBuild_Desc=Propagating...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ccbc.exe"
   copy .\Release\ccbc.exe $(BASEDIR)\lib\i386\free
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ccbc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ccbc.exe"

!ELSE 

ALL : "$(OUTDIR)\ccbc.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ccbc.obj"
	-@erase "$(INTDIR)\ccbc.pch"
	-@erase "$(INTDIR)\ccbc.res"
	-@erase "$(INTDIR)\ccbcDlg.obj"
	-@erase "$(INTDIR)\ccbeacon.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\ccbc.exe"
	-@erase "$(OUTDIR)\ccbc.ilk"
	-@erase "$(OUTDIR)\ccbc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(BASEDIR)\src\bpc\inc" /I\
 "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\ccbc.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ccbc.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ccbc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=setupapi.lib ksuser.lib icodecd.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\ccbc.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\ccbc.exe" /pdbtype:sept /libpath:"$(BASEDIR)\src\bpc\lib"\
 /libpath:"$(BASEDIR)\lib\i386\free" 
LINK32_OBJS= \
	"$(INTDIR)\ccbc.obj" \
	"$(INTDIR)\ccbc.res" \
	"$(INTDIR)\ccbcDlg.obj" \
	"$(INTDIR)\ccbeacon.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\ccbc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ccbc.exe"
   copy .\Debug\ccbc.exe $(BASEDIR)\lib\i386\free
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


!IF "$(CFG)" == "ccbc - Win32 Release" || "$(CFG)" == "ccbc - Win32 Debug"
SOURCE=.\ccbc.cpp

!IF  "$(CFG)" == "ccbc - Win32 Release"

DEP_CPP_CCBC_=\
	"$(BASEDIR)\inc\win98\ks.h"\
	"$(BASEDIR)\inc\win98\ksmedia.h"\
	"$(BASEDIR)\src\bpc\inc\ccdecode.h"\
	"$(BASEDIR)\src\bpc\inc\icodec.h"\
	"$(BASEDIR)\src\bpc\inc\iks.h"\
	".\Ccbeacon.h"\
	".\ccbc.h"\
	".\ccbcDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ccbc.obj" : $(SOURCE) $(DEP_CPP_CCBC_) "$(INTDIR)"\
 "$(INTDIR)\ccbc.pch"


!ELSEIF  "$(CFG)" == "ccbc - Win32 Debug"

DEP_CPP_CCBC_=\
	"$(BASEDIR)\inc\win98\ks.h"\
	"$(BASEDIR)\inc\win98\ksmedia.h"\
	"$(BASEDIR)\src\bpc\inc\ccdecode.h"\
	"$(BASEDIR)\src\bpc\inc\icodec.h"\
	"$(BASEDIR)\src\bpc\inc\iks.h"\
	".\Ccbeacon.h"\
	".\ccbc.h"\
	".\ccbcDlg.h"\
	

"$(INTDIR)\ccbc.obj" : $(SOURCE) $(DEP_CPP_CCBC_) "$(INTDIR)"\
 "$(INTDIR)\ccbc.pch"


!ENDIF 

SOURCE=.\ccbc.rc
DEP_RSC_CCBC_R=\
	".\res\ccbc.ico"\
	".\res\ccbc.rc2"\
	

"$(INTDIR)\ccbc.res" : $(SOURCE) $(DEP_RSC_CCBC_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ccbcDlg.cpp

!IF  "$(CFG)" == "ccbc - Win32 Release"

DEP_CPP_CCBCD=\
	"$(BASEDIR)\inc\win98\ks.h"\
	"$(BASEDIR)\inc\win98\ksmedia.h"\
	"$(BASEDIR)\src\bpc\inc\ccdecode.h"\
	"$(BASEDIR)\src\bpc\inc\icodec.h"\
	"$(BASEDIR)\src\bpc\inc\iks.h"\
	".\Ccbeacon.h"\
	".\ccbc.h"\
	".\ccbcDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ccbcDlg.obj" : $(SOURCE) $(DEP_CPP_CCBCD) "$(INTDIR)"\
 "$(INTDIR)\ccbc.pch"


!ELSEIF  "$(CFG)" == "ccbc - Win32 Debug"

DEP_CPP_CCBCD=\
	"$(BASEDIR)\inc\win98\ks.h"\
	"$(BASEDIR)\inc\win98\ksmedia.h"\
	"$(BASEDIR)\src\bpc\inc\ccdecode.h"\
	"$(BASEDIR)\src\bpc\inc\icodec.h"\
	"$(BASEDIR)\src\bpc\inc\iks.h"\
	".\Ccbeacon.h"\
	".\ccbc.h"\
	".\ccbcDlg.h"\
	

"$(INTDIR)\ccbcDlg.obj" : $(SOURCE) $(DEP_CPP_CCBCD) "$(INTDIR)"\
 "$(INTDIR)\ccbc.pch"


!ENDIF 

SOURCE=.\Ccbeacon.cpp
DEP_CPP_CCBEA=\
	".\Ccbeacon.h"\
	

!IF  "$(CFG)" == "ccbc - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(BASEDIR)\src\bpc\inc" /I\
 "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ccbeacon.obj" : $(SOURCE) $(DEP_CPP_CCBEA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ccbc - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(BASEDIR)\src\bpc\inc" /I\
 "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ccbeacon.obj" : $(SOURCE) $(DEP_CPP_CCBEA) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ccbc - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "$(BASEDIR)\src\bpc\inc" /I\
 "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\ccbc.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ccbc.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ccbc - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(BASEDIR)\src\bpc\inc" /I\
 "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\win98" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\ccbc.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ccbc.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

