#
#  MYNDI
#

BUILD_TYPE=net
!INCLUDE $(BASEDIR)\inc\master.mk

BIN = obj\i386\$(DDKBUILDENV)
TARGETPATH = $(BASEDIR)\LIB\i386\$(DDKBUILDENV)
PROJ = MYNDI
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS =
INCLUDE = .;$(INC16);$(C16_ROOT)\include;$(INC32);$(INCLUDE)
LIB = $(C16_ROOT)\lib;$(LIB16);$(LIB)
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = $(BASEDIR)\SRC\NET\MYNDI
USEMFC = 0
CC = $(CL16)
CPP = $(CL16)
CXX = $(CL16)
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = MYNDI.C     
FIRSTCPP =             
RC = $(RC16B)
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Od /D "_DEBLEVEL"="1" /D "_DEBUG" /FR /GD /Fd"$(BIN)\MYNDI.PDB"
CFLAGS_R_WDLL = /nologo /G3 /W3 /ALw /O1 /D "NDEBUG" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /NOE /ALIGN:16 /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /NOE /ALIGN:16 /MAP:FULL
LIBS_D_WDLL = oldnames libw shell ldllcew
LIBS_R_WDLL = oldnames libw shell ldllcew
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
SBRS = $(BIN)\MYNDI.SBR \
					 $(BIN)\INSTALL.SBR \
					 $(BIN)\DEBUG.SBR \
					 $(BIN)\BIND.SBR \
					 $(BIN)\CREATE.SBR \
					 $(BIN)\VALIDATE.SBR \
					 $(BIN)\PROPPAGE.SBR \
					 $(BIN)\MYCOPY.SBR \
					 $(BIN)\VNBTINFO.SBR


SETUPX_DEP = 

MYNDI_RCDEP = $(PROJPATH)\resource.h


MYNDI_RCDEP = $(PROJPATH)\myndi.rcv


NETDI_DEP = 

all: $(BIN) $(TARGETPATH)\$(PROJ).DLL $(BIN)\$(PROJ).BSC

$(BIN):
						if not exist obj md obj
						if not exist obj\i386 md obj\i386
		  if not exist $(BIN) md $(BIN)

$(BIN)\MYNDI.OBJ:      MYNDI.C $(MYNDI_DEP)
		  $(CC) $(CFLAGS) $(CCREATEPCHFLAG) /Fo$@ /FR$*.SBR /c MYNDI.C

$(BIN)\INSTALL.OBJ:    INSTALL.C $(INSTALL_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c INSTALL.C

$(BIN)\MYNDI.RES:      MYNDI.RC $(MYNDI_RCDEP)
		  $(RC) $(RCFLAGS) $(RCDEFINES) -r MYNDI.RC
						if exist $(BIN)\MYNDI.res del $(BIN)\MYNDI.res
						move MYNDI.RES $(BIN)\MYNDI.RES

$(BIN)\DEBUG.OBJ:      DEBUG.C $(DEBUG_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c DEBUG.C

$(BIN)\BIND.OBJ:       BIND.C $(BIND_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c BIND.C

$(BIN)\CREATE.OBJ:     CREATE.C $(CREATE_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c CREATE.C

$(BIN)\VALIDATE.OBJ:   VALIDATE.C $(VALIDATE_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c VALIDATE.C

$(BIN)\PROPPAGE.OBJ:   PROPPAGE.C $(PROPPAGE_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c PROPPAGE.C

$(BIN)\MYCOPY.OBJ:     MYCOPY.C $(MYCOPY_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c MYCOPY.C

$(BIN)\VNBTINFO.OBJ:   VNBTINFO.C $(VNBTINFO_DEP)
		  $(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$*.SBR /c VNBTINFO.C


$(TARGETPATH)\$(PROJ).DLL::   $(BIN)\MYNDI.RES

$(TARGETPATH)\$(PROJ).DLL::   $(BIN)\MYNDI.OBJ \
		  $(BIN)\INSTALL.OBJ \
		  $(BIN)\DEBUG.OBJ \
		  $(BIN)\BIND.OBJ \
		  $(BIN)\CREATE.OBJ \
		  $(BIN)\VALIDATE.OBJ \
		  $(BIN)\PROPPAGE.OBJ \
		  $(BIN)\MYCOPY.OBJ \
		  $(BIN)\VNBTINFO.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
$(BIN)\MYNDI.OBJ +
$(BIN)\INSTALL.OBJ +
$(BIN)\DEBUG.OBJ +
$(BIN)\BIND.OBJ +
$(BIN)\CREATE.OBJ +
$(BIN)\VALIDATE.OBJ +
$(BIN)\PROPPAGE.OBJ +
$(BIN)\MYCOPY.OBJ +
$(BIN)\VNBTINFO.OBJ +
$(OBJS_EXT)
$@
$(BIN)\$(MAPFILE)
$(C16_ROOT)\lib\+
$(LIB16)\SETUPX.LIB+
$(LIB16)\NETDI.LIB+
$(LIBS)
$(DEFFILE);
<<
		  $(LINK16) $(LFLAGS) @$(PROJ).CRF
		  $(RC) $(RESFLAGS) $(BIN)\MYNDI.RES $@
		  @copy $(PROJ).CRF $(BIN)\MSVC.BND

$(TARGETPATH)\$(PROJ).DLL::   $(BIN)\MYNDI.RES
		  if not exist $(BIN)\MSVC.BND  $(RC) $(RESFLAGS) $(BIN)\MYNDI.RES $@

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
		  -@if exist $(BIN)\*.dll del $(BIN)\*.dll
		  -@if exist $(BIN)\*.lib del $(BIN)\*.lib
		  -@if exist $(BIN)\*.sbr del $(BIN)\*.sbr
		  -@if exist $(BIN)\*.bsc del $(BIN)\*.bsc
		  -@if exist $(BIN)\*.pdb del $(BIN)\*.pdb
		  -@if exist *.pch del *.pch
		  -@if exist pch.c del pch.c
		  -@if exist *.bak del *.bak
		  

