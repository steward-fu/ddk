#############################################################
#
#  IPDETECT
#
#  Win98 16-bit network installation application
#
#############################################################

BUILD_TYPE=net
!INCLUDE $(BASEDIR)\inc\master.mk

PROJ = IPDETECT
DEFFILE = $(PROJ).DEF
BIN = obj\i386\$(DDKBUILDENV)
TARGETPATH = $(BASEDIR)\LIB\i386\$(DDKBUILDENV)
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
INCLUDE = .;$(INC16);$(C16_ROOT)\include;$(C16_ROOT)\mfc\include;$(INC32);$(INCLUDE)
LIB = $(BASEDIR)\lib\win98;$(C16_ROOT)\lib;$(C16_ROOT)\mfc\lib;$(LIB)
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = $(BASEDIR)\SRC\NET\IPNEW
USEMFC = 1
CC = $(CL16)
CPP = $(CL16)
CXX = $(CL16)
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = 
FIRSTCPP = IPDETDOC.CPP
RC = $(RC16B)
CFLAGS_D_WEXE = /nologo /W3 /Zi /AL /Od /D "_DEBLEVEL"="1" /D "_DEBUG" /FR /GA /Fd"$(BIN)\$(PROJ).PDB"
CFLAGS_R_WEXE = /nologo /G3 /W3 /AL /O1 /D "NDEBUG" /FR /GA
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:32768
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:32768
LIBS_D_WEXE = lafxcwd oldnames libw commdlg shell olecli olesvr llibcew
LIBS_R_WEXE = lafxcw oldnames libw commdlg shell olecli olesvr llibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = $(LIB16)\SETUPX.LIB $(LIB16)\WIN98\NETDI.LIB 
!if "$(DDKBUILDENV)" == "checked"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = $(PROJ).MAP
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist $(BIN)\MSVC.BND del $(BIN)\MSVC.BND]
!endif
SBRS = $(BIN)\IPDETDOC.SBR \
                $(BIN)\IPDETECT.SBR \
                $(BIN)\IPDETVW.SBR \
                $(BIN)\IPDO.SBR \
                $(BIN)\IPMODEM.SBR \
                $(BIN)\IPNDI.SBR \
                $(BIN)\IPOPTION.SBR \
                $(BIN)\MAINFRM.SBR \
                $(BIN)\STDAFX.SBR

SETUPX_DEP = 
        
IPDETDOC_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\ipdetdoc.h \
        $(PROJPATH)\ipdetvw.h


IPDETECT_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\mainfrm.h \
        $(PROJPATH)\ipdetdoc.h \
        $(PROJPATH)\ipdetvw.h \
        $(INC32)\winerror.h \
        $(INC32)\regstr.h


IPDETVW_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\ipdetdoc.h \
        $(PROJPATH)\ipdetvw.h


IPDO_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\mainfrm.h \
        $(PROJPATH)\ipdetdoc.h \
        $(PROJPATH)\ipdetvw.h \
        $(INC32)\winerror.h \
        $(INC32)\regstr.h


IPMODEM_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\mainfrm.h \
        $(PROJPATH)\ipdetdoc.h \
        $(PROJPATH)\ipdetvw.h \
        $(INC32)\winerror.h \
        $(INC32)\dbt.h \
        $(INC32)\regstr.h


IPNDI_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\mainfrm.h \
        $(PROJPATH)\ipdetdoc.h \
        $(PROJPATH)\ipdetvw.h \
        $(INC32)\winerror.h \
        $(INC32)\regstr.h


IPOPTION_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\mainfrm.h \
        $(PROJPATH)\ipdetdoc.h \
        $(PROJPATH)\ipdetvw.h \
        $(INC32)\winerror.h \
        $(INC32)\regstr.h


MAINFRM_DEP = $(PROJPATH)\stdafx.h \
        $(PROJPATH)\ipdetect.h \
        $(INC16)\setupx.h \
        $(INC16)\prsht.h \
        $(PROJPATH)\mainfrm.h


STDAFX_DEP = $(PROJPATH)\stdafx.h


IPDETECT_RCDEP = $(PROJPATH)\res\ipdetect.ico \
        $(PROJPATH)\res\ipdetect.rc2 \
        $(PROJPATH)\resource.h

NETDI_DEP = 

all: $(BIN) $(TARGETPATH)\$(PROJ).EXE $(BIN)\$(PROJ).BSC

$(BIN):
        if not exist obj md obj
        if not exist obj\i386 md obj\i386
        if not exist $(BIN) md $(BIN)

$(BIN)\IPDETDOC.OBJ:   IPDETDOC.CPP $(IPDETDOC_DEP)
        $(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /Fo$@ /FR$*.SBR /c IPDETDOC.CPP


$(BIN)\IPDETECT.OBJ:   IPDETECT.CPP $(IPDETECT_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c IPDETECT.CPP

$(BIN)\IPDETVW.OBJ:    IPDETVW.CPP $(IPDETVW_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c IPDETVW.CPP

$(BIN)\IPDO.OBJ:       IPDO.CPP $(IPDO_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c IPDO.CPP

$(BIN)\IPMODEM.OBJ:    IPMODEM.CPP $(IPMODEM_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c IPMODEM.CPP

$(BIN)\IPNDI.OBJ:      IPNDI.CPP $(IPNDI_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c IPNDI.CPP

$(BIN)\IPOPTION.OBJ:   IPOPTION.CPP $(IPOPTION_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c IPOPTION.CPP

$(BIN)\MAINFRM.OBJ:    MAINFRM.CPP $(MAINFRM_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c MAINFRM.CPP

$(BIN)\STDAFX.OBJ:     STDAFX.CPP $(STDAFX_DEP)
        $(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$*.SBR /c STDAFX.CPP

$(BIN)\$(PROJ).RES:   $(PROJ).RC $(IPDETECT_RCDEP)
        $(RC) $(RCFLAGS) $(RCDEFINES) -r $(PROJ).RC
        if exist $(BIN)\$(PROJ).res del $(BIN)\$(PROJ).res
        move $(PROJ).RES $(BIN)\$(PROJ).RES

$(TARGETPATH)\$(PROJ).EXE::   $(BIN)\$(PROJ).RES

$(TARGETPATH)\$(PROJ).EXE::   $(BIN)\IPDETDOC.OBJ \
        $(BIN)\IPDETECT.OBJ \
        $(BIN)\IPDETVW.OBJ \
        $(BIN)\IPDO.OBJ \
        $(BIN)\IPMODEM.OBJ \
        $(BIN)\IPNDI.OBJ \
        $(BIN)\IPOPTION.OBJ \
        $(BIN)\MAINFRM.OBJ \
        $(BIN)\STDAFX.OBJ $(OBJS_EXT) $(DEFFILE)
        echo >NUL @<<$(PROJ).CRF
$(BIN)\IPDETDOC.OBJ +
$(BIN)\IPDETECT.OBJ +
$(BIN)\IPDETVW.OBJ +
$(BIN)\IPDO.OBJ +
$(BIN)\IPMODEM.OBJ +
$(BIN)\IPNDI.OBJ +
$(BIN)\IPOPTION.OBJ +
$(BIN)\MAINFRM.OBJ +
$(BIN)\STDAFX.OBJ +
$(OBJS_EXT)
$@
$(BIN)\$(MAPFILE)
$(C16_ROOT)\lib\+
$(C16_ROOT)\mfc\lib\+
$(LIB16)\SETUPX.LIB+
$(LIB16)\NETDI.LIB+
$(LIBS)
$(DEFFILE);
<<
        $(LINK16) $(LFLAGS) @$(PROJ).CRF
        $(RC) $(RESFLAGS) $(BIN)\$(PROJ).RES $@
        @copy $(PROJ).CRF $(BIN)\MSVC.BND

$(TARGETPATH)\$(PROJ).EXE::   $(BIN)\$(PROJ).RES
        if not exist $(BIN)\MSVC.BND  $(RC) $(RESFLAGS) $(BIN)\$(PROJ).RES $@

run: $(TARGETPATH)\$(PROJ).EXE
        $(TARGETPATH)\$(PROJ) $(RUNFLAGS)


$(BIN)\$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<

clean:
		  -@if exist $(BIN)\*.obj del $(BIN)\*.obj
		  -@if exist $(BIN)\*.map del $(BIN)\*.map
		  -@if exist $(BIN)\*.sym del $(BIN)\*.sym
		  -@if exist $(BIN)\*.res del $(BIN)\*.res
		  -@if exist $(BIN)\*.dll del $(BIN)\*.dll
		  -@if exist $(BIN)\*.lib del $(BIN)\*.lib
		  -@if exist $(BIN)\*.sbr del $(BIN)\*.sbr
		  -@if exist $(BIN)\*.bsc del $(BIN)\*.bsc
		  -@if exist $(BIN)\*.pdb del $(BIN)\*.pdb
		  -@if exist *.pch del *.pch
		  -@if exist pch.c del pch.c
		  -@if exist *.bak del *.bak
