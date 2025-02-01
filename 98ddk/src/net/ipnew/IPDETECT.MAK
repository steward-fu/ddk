# Microsoft Visual C++ generated build script - Do not modify

PROJ = IPDETECT
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\98DDK\SRC\NET\IPNEW\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = IPDETDOC.CPP
RC = rc
CFLAGS_D_WEXE = /nologo /G3 /W3 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"IPDETECT.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AM /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:32768
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:32768
LIBS_D_WEXE = lafxcwd oldnames libw commdlg shell olecli olesvr llibcew
LIBS_R_WEXE = mafxcw oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = IPDETECT.DEF
OBJS_EXT = 
LIBS_EXT = ..\..\..\LIB\WIN98\NETDI.LIB ..\..\..\LIB\WIN98\SETUPX.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = IPDETDOC.SBR \
		IPDETECT.SBR \
		IPDETVW.SBR \
		IPDO.SBR \
		IPMODEM.SBR \
		IPNDI.SBR \
		IPOPTION.SBR \
		MAINFRM.SBR \
		STDAFX.SBR


IPDETDOC_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\ipdetdoc.h \
	d:\98ddk\src\net\ipnew\ipdetvw.h


IPDETECT_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\mainfrm.h \
	d:\98ddk\src\net\ipnew\ipdetdoc.h \
	d:\98ddk\src\net\ipnew\ipdetvw.h


IPDETVW_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\ipdetdoc.h \
	d:\98ddk\src\net\ipnew\ipdetvw.h


IPDO_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\mainfrm.h \
	d:\98ddk\src\net\ipnew\ipdetdoc.h \
	d:\98ddk\src\net\ipnew\ipdetvw.h


IPMODEM_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\mainfrm.h \
	d:\98ddk\src\net\ipnew\ipdetdoc.h \
	d:\98ddk\src\net\ipnew\ipdetvw.h


IPNDI_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\mainfrm.h \
	d:\98ddk\src\net\ipnew\ipdetdoc.h \
	d:\98ddk\src\net\ipnew\ipdetvw.h


IPOPTION_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\mainfrm.h \
	d:\98ddk\src\net\ipnew\ipdetdoc.h \
	d:\98ddk\src\net\ipnew\ipdetvw.h


MAINFRM_DEP = d:\98ddk\src\net\ipnew\stdafx.h \
	d:\98ddk\src\net\ipnew\ipdetect.h \
	d:\98ddk\src\net\ipnew\mainfrm.h


STDAFX_DEP = d:\98ddk\src\net\ipnew\stdafx.h


IPDETECT_RCDEP = d:\98ddk\src\net\ipnew\res\ipdetect.ico \
	d:\98ddk\src\net\ipnew\res\ipdetect.rc2


NETDI_DEP = 

SETUPX_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

IPDETDOC.OBJ:	IPDETDOC.CPP $(IPDETDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c IPDETDOC.CPP

IPDETECT.OBJ:	IPDETECT.CPP $(IPDETECT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPDETECT.CPP

IPDETVW.OBJ:	IPDETVW.CPP $(IPDETVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPDETVW.CPP

IPDO.OBJ:	IPDO.CPP $(IPDO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPDO.CPP

IPMODEM.OBJ:	IPMODEM.CPP $(IPMODEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPMODEM.CPP

IPNDI.OBJ:	IPNDI.CPP $(IPNDI_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPNDI.CPP

IPOPTION.OBJ:	IPOPTION.CPP $(IPOPTION_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c IPOPTION.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c STDAFX.CPP

IPDETECT.RES:	IPDETECT.RC $(IPDETECT_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r IPDETECT.RC


$(PROJ).EXE::	IPDETECT.RES

$(PROJ).EXE::	IPDETDOC.OBJ IPDETECT.OBJ IPDETVW.OBJ IPDO.OBJ IPMODEM.OBJ IPNDI.OBJ \
	IPOPTION.OBJ MAINFRM.OBJ STDAFX.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
IPDETDOC.OBJ +
IPDETECT.OBJ +
IPDETVW.OBJ +
IPDO.OBJ +
IPMODEM.OBJ +
IPNDI.OBJ +
IPOPTION.OBJ +
MAINFRM.OBJ +
STDAFX.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
..\..\..\LIB\WIN98\NETDI.LIB+
..\..\..\LIB\WIN98\SETUPX.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) IPDETECT.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	IPDETECT.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) IPDETECT.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
