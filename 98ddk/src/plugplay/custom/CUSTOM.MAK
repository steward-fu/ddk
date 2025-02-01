# Microsoft Visual C++ generated build script - Do not modify

PROJ = CUSTOM
DEBUG = 1
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\MSVC\PROJ\CUSTOM\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = CUSTOM.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Od /D "_DEBLEVEL"="1" /D "_DEBUG" /FR /GD /Fd"CUSTOM.PDB"
CFLAGS_R_WDLL = /nologo /G3 /W3 /ALw /O1 /D "NDEBUG" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /NOE /ALIGN:16 /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /NOE /ALIGN:16 /MAP:FULL
LIBS_D_WDLL = oldnames libw commdlg shell olecli olesvr ldllcew
LIBS_R_WDLL = oldnames libw commdlg shell olecli olesvr ldllcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = CUSTOM.DEF
OBJS_EXT = 
LIBS_EXT = ..\..\..\DDK\LIB\SETUPX.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = CUSTOM.SBR \
		DEBUG.SBR


SETUPX_DEP = 

CUSTOM_DEP = c:\ddk\inc16\netdi.h \
	c:\ddk\inc16\prsht.h \
	c:\ddk\inc16\setupx.h


CUSTOM_RCDEP = c:\msvc\proj\custom\resource.h


all:	$(PROJ).DLL $(PROJ).BSC

CUSTOM.OBJ:	CUSTOM.C $(CUSTOM_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c CUSTOM.C

CUSTOM.RES:	CUSTOM.RC $(CUSTOM_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r CUSTOM.RC

DEBUG.OBJ:	DEBUG.C $(DEBUG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DEBUG.C


$(PROJ).DLL::	CUSTOM.RES

$(PROJ).DLL::	CUSTOM.OBJ DEBUG.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
CUSTOM.OBJ +
DEBUG.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
c:\ddk\lib\+
c:\msvc\lib\+
c:\msvc\mfc\lib\+
..\..\..\DDK\LIB\SETUPX.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) CUSTOM.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	CUSTOM.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) CUSTOM.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
