 #############################################################
#
#  Custom
#
#  Class installer
#
#  This is a 16-bit dll!
#
#############################################################

BUILD_TYPE=plugplay
!INCLUDE $(BASEDIR)\inc\master.mk

BIN = obj\i386
TARGETPATH = $(BASEDIR)\LIB\i386\$(DDKBUILDENV)
PROJ = CUSTOM
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS =
INCLUDE = .;$(INC16);$(C16_ROOT)\include;$(INC32);$(INCLUDE)
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = $(BASEDIR)\SRC\PLUGPLAY\CUSTOM
USEMFC = 0
CC = $(CL16)
CPP = $(CL16)
CXX = $(CL16)
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = CUSTOM.C     
FIRSTCPP =             
RC = $(RC16B)
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Od /D "_DEBLEVEL"="1" /D "_DEBUG" /FR /GD /Fd"$(BIN)\CUSTOM.PDB"
CFLAGS_R_WDLL = /nologo /G3 /W3 /ALw /O1 /D "NDEBUG" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /NOE /ALIGN:16 /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /NOE /ALIGN:16 /MAP:FULL
LIBS_D_WDLL = oldnames libw commdlg shell ldllcew
LIBS_R_WDLL = oldnames libw commdlg shell ldllcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = $(PROJ).DEF
OBJS_EXT = 
LIBS_EXT = $(LIB16)\SETUPX.LIB $(LIB16)\WIN98\NETDI.LIB 
!if "$(DDKBUILDENV)" == "checked"
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
!if [if exist $(BIN)\MSVC.BND del $(BIN)\MSVC.BND]
!endif
SBRS = $(BIN)\CUSTOM.SBR \
					 $(BIN)\DEBUG.SBR


SETUPX_DEP = 

CUSTOM_RCDEP = $(PROJPATH)\resource.h

CUSTOM_RCDEP = $(PROJPATH)\custom.rcv


NETDI_DEP = 

CUSTOM_DEP = $(INC16)\netdi.h \
		  $(INC16)\prsht.h \
		  $(INC16)\setupx.h

CUSTOM_RCDEP = $(PROJPATH)\resource.h

all: $(BIN) $(TARGETPATH)\$(PROJ).DLL $(BIN)\$(PROJ).BSC

$(BIN):
		  if not exist $(BIN) md $(BIN)

$(BIN)\CUSTOM.OBJ:     CUSTOM.C $(CUSTOM_DEP)
		  $(CC) $(CFLAGS) $(CCREATEPCHFLAG) /Fo$@ /FR$*.SBR /c CUSTOM.C

$(BIN)\CUSTOM.RES:     CUSTOM.RC $(CUSTOM_RCDEP)
		  $(RC) $(RCFLAGS) $(RCDEFINES) -r CUSTOM.RC
		  -@if exist $(BIN)\CUSTOM.RES del $(BIN)\CUSTOM.RES
		  move CUSTOM.RES $(BIN)

$(BIN)\DEBUG.OBJ:      DEBUG.C $(DEBUG_DEP)
		  $(CC) $(CFLAGS) $(CCREATEPCHFLAG) /Fo$@ /FR$*.SBR /c DEBUG.C

$(TARGETPATH)\$(PROJ).DLL::   $(BIN)\CUSTOM.RES

$(TARGETPATH)\$(PROJ).DLL::   $(BIN)\CUSTOM.OBJ \
		  $(BIN)\DEBUG.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
$(BIN)\CUSTOM.OBJ +
$(BIN)\DEBUG.OBJ +
$(OBJS_EXT)
$@
$(BIN)\$(MAPFILE)
$(C16_ROOT)\lib\+
$(LIB16)\SETUPX.LIB+
$(LIBS)
$(DEFFILE);
<<
		  $(LINK16) $(LFLAGS) @$(PROJ).CRF
		  $(RC) $(RESFLAGS) $(BIN)\CUSTOM.RES $@
		  @copy $(PROJ).CRF $(BIN)\MSVC.BND

$(TARGETPATH)\$(PROJ).DLL::   $(BIN)\CUSTOM.RES
		  if not exist $(BIN)\MSVC.BND  $(RC) $(RESFLAGS) $(BIN)\CUSTOM.RES $@

run: $(TARGETPATH)\$(PROJ).DLL
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
		  -@if exist $(BIN)\*.exe del $(BIN)\*.exe
		  -@if exist $(BIN)\*.lib del $(BIN)\*.lib
		  -@if exist $(BIN)\*.sbr del $(BIN)\*.sbr
		  -@if exist $(BIN)\*.bsc del $(BIN)\*.bsc
		  -@if exist $(BIN)\*.pdb del $(BIN)\*.pdb
		  -@if exist *.pch del *.pch
		  -@if exist pch.c del pch.c
		  -@if exist *.bak del *.bak
		  

