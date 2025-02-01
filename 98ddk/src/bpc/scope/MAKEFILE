# Microsoft Developer Studio Generated NMAKE File, Based on scope.dsp
!IF "$(CFG)" == ""
CFG=scope - Win32 Debug
!MESSAGE No configuration specified. Defaulting to scope - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "scope - Win32 Release" && "$(CFG)" != "scope - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scope.mak" CFG="scope - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scope - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "scope - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "scope - Win32 Release"

OUTDIR=.\Release
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scope.exe"

!ELSE 

ALL : "$(OUTDIR)\scope.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\guids.obj"
	-@erase "$(INTDIR)\scope.obj"
	-@erase "$(INTDIR)\scope.pch"
	-@erase "$(INTDIR)\scope.res"
	-@erase "$(INTDIR)\scopeDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\testgrf.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\scope.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\scope.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scope.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scope.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=quartz.lib strmbasd.lib strmbase.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\scope.pdb" /machine:I386\
 /out:"$(OUTDIR)\scope.exe" /libpath:".\DirectShow\lib"\
 /libpath:"$(BASEDIR)\lib\i386\free" 
LINK32_OBJS= \
	"$(INTDIR)\guids.obj" \
	"$(INTDIR)\scope.obj" \
	"$(INTDIR)\scope.res" \
	"$(INTDIR)\scopeDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\testgrf.obj"

"$(OUTDIR)\scope.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\scope.exe"
   copy .\Release\scope.exe $(BASEDIR)\lib\i386\free
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\obj\i386
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scope.exe"

!ELSE 

ALL : "$(OUTDIR)\scope.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\guids.obj"
	-@erase "$(INTDIR)\scope.obj"
	-@erase "$(INTDIR)\scope.pch"
	-@erase "$(INTDIR)\scope.res"
	-@erase "$(INTDIR)\scopeDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\testgrf.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\scope.exe"
	-@erase "$(OUTDIR)\scope.ilk"
	-@erase "$(OUTDIR)\scope.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\scope.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\obj\i386/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\scope.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scope.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=quartz.lib strmbasd.lib strmbase.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\scope.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\scope.exe" /pdbtype:sept /libpath:".\DirectShow\lib"\
 /libpath:"$(BASEDIR)\lib\i386\free" 
LINK32_OBJS= \
	"$(INTDIR)\guids.obj" \
	"$(INTDIR)\scope.obj" \
	"$(INTDIR)\scope.res" \
	"$(INTDIR)\scopeDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\testgrf.obj"

"$(OUTDIR)\scope.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE=$(InputPath)
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\scope.exe"
   copy .\Debug\scope.exe $(BASEDIR)\lib\i386\free
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


!IF "$(CFG)" == "scope - Win32 Release" || "$(CFG)" == "scope - Win32 Debug"
SOURCE=.\guids.cpp

!IF  "$(CFG)" == "scope - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\guids.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\guids.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\scope.cpp
DEP_CPP_SCOPE=\
	".\DirectShow\inc\amvideo.h"\
	".\DirectShow\inc\classes\base\amextra.h"\
	".\DirectShow\inc\classes\base\amfilter.h"\
	".\DirectShow\inc\classes\base\cache.h"\
	".\DirectShow\inc\classes\base\combase.h"\
	".\DirectShow\inc\classes\base\cprop.h"\
	".\DirectShow\inc\classes\base\ctlutil.h"\
	".\DirectShow\inc\classes\base\dllsetup.h"\
	".\DirectShow\inc\classes\base\fourcc.h"\
	".\DirectShow\inc\classes\base\measure.h"\
	".\DirectShow\inc\classes\base\msgthrd.h"\
	".\DirectShow\inc\classes\base\mtype.h"\
	".\DirectShow\inc\classes\base\outputq.h"\
	".\DirectShow\inc\classes\base\pstream.h"\
	".\DirectShow\inc\classes\base\refclock.h"\
	".\DirectShow\inc\classes\base\reftime.h"\
	".\DirectShow\inc\classes\base\renbase.h"\
	".\DirectShow\inc\classes\base\Schedule.h"\
	".\DirectShow\inc\classes\base\source.h"\
	".\DirectShow\inc\classes\base\streams.h"\
	".\DirectShow\inc\classes\base\strmctl.h"\
	".\DirectShow\inc\classes\base\sysclock.h"\
	".\DirectShow\inc\classes\base\transfrm.h"\
	".\DirectShow\inc\classes\base\transip.h"\
	".\DirectShow\inc\classes\base\videoctl.h"\
	".\DirectShow\inc\classes\base\vtrans.h"\
	".\DirectShow\inc\classes\base\winctrl.h"\
	".\DirectShow\inc\classes\base\winutil.h"\
	".\DirectShow\inc\classes\base\wxdebug.h"\
	".\DirectShow\inc\classes\base\wxlist.h"\
	".\DirectShow\inc\classes\base\wxutil.h"\
	".\DirectShow\inc\comlite.h"\
	".\DirectShow\inc\control.h"\
	".\DirectShow\inc\edevdefs.h"\
	".\DirectShow\inc\errors.h"\
	".\DirectShow\inc\evcode.h"\
	".\DirectShow\inc\ksuuids.h"\
	".\DirectShow\inc\strmif.h"\
	".\DirectShow\inc\uuids.h"\
	".\DirectShow\inc\VFWMSGS.H"\
	".\Resource.h"\
	".\scope.h"\
	".\scopeDlg.h"\
	".\StdAfx.h"\
	".\testgrf.h"\
	

"$(INTDIR)\scope.obj" : $(SOURCE) $(DEP_CPP_SCOPE) "$(INTDIR)"\
 "$(INTDIR)\scope.pch"


SOURCE=.\scope.rc
DEP_RSC_SCOPE_=\
	".\res\scope.ico"\
	".\res\scope.rc2"\
	".\Resource.h"\
	

"$(INTDIR)\scope.res" : $(SOURCE) $(DEP_RSC_SCOPE_) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\scopeDlg.cpp
DEP_CPP_SCOPED=\
	".\DirectShow\inc\amvideo.h"\
	".\DirectShow\inc\classes\base\amextra.h"\
	".\DirectShow\inc\classes\base\amfilter.h"\
	".\DirectShow\inc\classes\base\cache.h"\
	".\DirectShow\inc\classes\base\combase.h"\
	".\DirectShow\inc\classes\base\cprop.h"\
	".\DirectShow\inc\classes\base\ctlutil.h"\
	".\DirectShow\inc\classes\base\dllsetup.h"\
	".\DirectShow\inc\classes\base\fourcc.h"\
	".\DirectShow\inc\classes\base\measure.h"\
	".\DirectShow\inc\classes\base\msgthrd.h"\
	".\DirectShow\inc\classes\base\mtype.h"\
	".\DirectShow\inc\classes\base\outputq.h"\
	".\DirectShow\inc\classes\base\pstream.h"\
	".\DirectShow\inc\classes\base\refclock.h"\
	".\DirectShow\inc\classes\base\reftime.h"\
	".\DirectShow\inc\classes\base\renbase.h"\
	".\DirectShow\inc\classes\base\Schedule.h"\
	".\DirectShow\inc\classes\base\source.h"\
	".\DirectShow\inc\classes\base\streams.h"\
	".\DirectShow\inc\classes\base\strmctl.h"\
	".\DirectShow\inc\classes\base\sysclock.h"\
	".\DirectShow\inc\classes\base\transfrm.h"\
	".\DirectShow\inc\classes\base\transip.h"\
	".\DirectShow\inc\classes\base\videoctl.h"\
	".\DirectShow\inc\classes\base\vtrans.h"\
	".\DirectShow\inc\classes\base\winctrl.h"\
	".\DirectShow\inc\classes\base\winutil.h"\
	".\DirectShow\inc\classes\base\wxdebug.h"\
	".\DirectShow\inc\classes\base\wxlist.h"\
	".\DirectShow\inc\classes\base\wxutil.h"\
	".\DirectShow\inc\comlite.h"\
	".\DirectShow\inc\control.h"\
	".\DirectShow\inc\edevdefs.h"\
	".\DirectShow\inc\errors.h"\
	".\DirectShow\inc\evcode.h"\
	".\DirectShow\inc\ksuuids.h"\
	".\DirectShow\inc\strmif.h"\
	".\DirectShow\inc\uuids.h"\
	".\DirectShow\inc\VFWMSGS.H"\
	".\Resource.h"\
	".\scope.h"\
	".\scopeDlg.h"\
	".\StdAfx.h"\
	".\testgrf.h"\
	

"$(INTDIR)\scopeDlg.obj" : $(SOURCE) $(DEP_CPP_SCOPED) "$(INTDIR)"\
 "$(INTDIR)\scope.pch"


SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "scope - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\scope.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\scope.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\scope.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\scope.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\testgrf.cpp
DEP_CPP_TESTG=\
	".\DirectShow\inc\amvideo.h"\
	".\DirectShow\inc\classes\base\amextra.h"\
	".\DirectShow\inc\classes\base\amfilter.h"\
	".\DirectShow\inc\classes\base\cache.h"\
	".\DirectShow\inc\classes\base\combase.h"\
	".\DirectShow\inc\classes\base\cprop.h"\
	".\DirectShow\inc\classes\base\ctlutil.h"\
	".\DirectShow\inc\classes\base\dllsetup.h"\
	".\DirectShow\inc\classes\base\fourcc.h"\
	".\DirectShow\inc\classes\base\measure.h"\
	".\DirectShow\inc\classes\base\msgthrd.h"\
	".\DirectShow\inc\classes\base\mtype.h"\
	".\DirectShow\inc\classes\base\outputq.h"\
	".\DirectShow\inc\classes\base\pstream.h"\
	".\DirectShow\inc\classes\base\refclock.h"\
	".\DirectShow\inc\classes\base\reftime.h"\
	".\DirectShow\inc\classes\base\renbase.h"\
	".\DirectShow\inc\classes\base\Schedule.h"\
	".\DirectShow\inc\classes\base\source.h"\
	".\DirectShow\inc\classes\base\streams.h"\
	".\DirectShow\inc\classes\base\strmctl.h"\
	".\DirectShow\inc\classes\base\sysclock.h"\
	".\DirectShow\inc\classes\base\transfrm.h"\
	".\DirectShow\inc\classes\base\transip.h"\
	".\DirectShow\inc\classes\base\videoctl.h"\
	".\DirectShow\inc\classes\base\vtrans.h"\
	".\DirectShow\inc\classes\base\winctrl.h"\
	".\DirectShow\inc\classes\base\winutil.h"\
	".\DirectShow\inc\classes\base\wxdebug.h"\
	".\DirectShow\inc\classes\base\wxlist.h"\
	".\DirectShow\inc\classes\base\wxutil.h"\
	".\DirectShow\inc\comlite.h"\
	".\DirectShow\inc\control.h"\
	".\DirectShow\inc\edevdefs.h"\
	".\DirectShow\inc\errors.h"\
	".\DirectShow\inc\evcode.h"\
	".\DirectShow\inc\ksuuids.h"\
	".\DirectShow\inc\strmif.h"\
	".\DirectShow\inc\uuids.h"\
	".\DirectShow\inc\VFWMSGS.H"\
	".\testgrf.h"\
	

!IF  "$(CFG)" == "scope - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\testgrf.obj" : $(SOURCE) $(DEP_CPP_TESTG) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\DirectShow\inc" /I\
 ".\DirectShow\inc\classes\base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\testgrf.obj" : $(SOURCE) $(DEP_CPP_TESTG) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

