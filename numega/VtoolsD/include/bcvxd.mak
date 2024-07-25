# BCVXD.MAK  
# Copyright (c) 1994-1998 Compuware Corporation

PELE		= $(VTOOLSD)\bin\pele.exe

DEFNAME		= $(OBJPATHSLASH)$(DEVICENAME).DEF
PELNAME		= $(OBJPATHSLASH)$(DEVICENAME).PEL
DLLNAME		= $(OBJPATHSLASH)$(DEVICENAME).DLL
MAPNAME		= $(OUTPATHSLASH)$(DEVICENAME).MAP
SMFNAME		= $(OBJPATHSLASH)$(DEVICENAME).SMF

VXD_LIBS	= $(FWLIB) $(DAALIB) $(NTLIB) $(W0)  $(W1)  $(W2)  $(W3)  $(CRTLIB)

! ifdef USER_LIB
VXD_LIBS	= $(VXD_LIBS)  $(USER_LIB)
! endif

#
# If we are building just a lib do just the following section
#
!if	"$(DEVICETYPE)" == "LIB"
$(TARGNAME): $(OBJPATH) $(OUTPATH) $(FINALOBJECTS)
	echo Building library "$(TARGNAME)"
	$(TLIB) $(OUTPATH)$(TARGNAME)	$(FINALOBJECTS: =+^
)

#
# The rest of the bulk of this file deals with building a VxD
# Start of section to build Vxd
#
!else	# "$(DEVICETYPE)" == "LIB"

# Default for non-debug build
DEBUGCMD	=
LINKDBG		= -s

! if ($(DEBUG)==1) || defined(MAKE_RETAIL_SYMBOLS)
!   if "$(DEBUGGER)" == "SOFTICE2"
DEBUGCMD	= $(DBG2MAP) $(DLLNAME)
#LINKDBG		= -x -v
LINKDBG		=  -v
PELE		= $(PELE) -d -m $(MAPNAME)
!   else if "$(DEBUGGER)" == "SOFTICE3"
#LINKDBG		= -x -v
LINKDBG		=  -v
PELE		= $(PELE) -d -s $(SMFNAME)
MAKESYM		= $(NMSYM) $(SMFNAME)
!   else
LINKDBG		= -m
PELE		= $(PELE) -m $(MAPNAME)
!   endif
! endif

! ifdef REALINIT
PELE		= $(PELE) -r $(REALINIT)
! endif

! ifndef MAKE_RETAIL_SYMBOLS
! 	ifndef DEBUG
MAKESYM =
! 	endif
!	if "$(DEBUG)" == "0" 
MAKESYM =
!	endif
! endif

#ifdef WIN31

! if "$(TARGET)" == "WIN31" 


$(TARGNAME): $(OBJPATH) $(OUTPATH) $(FINALOBJECTS) $(DEFNAME) $(PELNAME)
	$(CLEAR)
	$(TLINK32) $(XLFLAGS) -c -Tpd -n -P- -M $(LINKDBG) @&&!
$(VTOOLSD)\LIB\ICRTBOR.OBJ+
$(FINALOBJECTS: =+^
)
$(DLLNAME)
$(MAPNAME)
$(VXD_LIBS)
$(DEFNAME)
!
	$(DEBUGCMD)
	$(PELE) -c $(PELNAME) -m $(MAPNAME) -o $(TARGNAME) $(DLLNAME)
	$(VTOOLSD)\bin\sethdr -n $(DEVICENAME) -x $(TARGNAME)	
	$(MAKESYM)

$(DEFNAME): $(FINALOBJECTS)
	copy &&|
EXPORTS
	_The_DDB	@1
| $(DEFNAME) >C:NUL

$(PELNAME):
	copy &&|
ATTRIB ICODE INIT
ATTRIB LCODE LOCKED
MERGE  ICODE INITDAT0 INITDATA 
MERGE  LCODE CODE DATA BSS EXITDATA
MERGE  PCODE PDATA
| $(PELNAME) >C:NUL

!endif

! if "$(TARGET)" == "WFW311" 

! if $(NDIS) == 1
VXD_LIBS	= $(VXD_LIBS)  $(NDISLIB)
! endif	

! if "$(DYNASWITCH)" == "DYNAMIC"
DDKVERSTR = "30A"
! else
DDKVERSTR = "30B"
! endif

$(TARGNAME): $(OBJPATH) $(OUTPATH) $(FINALOBJECTS) $(DEFNAME) $(PELNAME)
	$(CLEAR)
	$(TLINK32) $(XLFLAGS) -c -Tpd -n -P- -M $(LINKDBG) @&&!
$(VTOOLSD)\LIB\ICRTBOR.OBJ+
$(FINALOBJECTS: =+^
)
$(DLLNAME)
$(MAPNAME)
$(VXD_LIBS)
$(DEFNAME)
!
	$(DEBUGCMD)
	$(PELE) -c $(PELNAME) -o $(TARGNAME) -k $(DDKVERSTR) $(DLLNAME)
	$(VTOOLSD)\bin\sethdr -n $(DEVICENAME) -v $(DDKVERSTR) $(TARGNAME)	
	$(MAKESYM)

$(DEFNAME): $(FINALOBJECTS)
	copy &&|
EXPORTS
	_The_DDB	@1
| $(DEFNAME) >C:NUL

$(PELNAME):
	copy &&|
$(DYNASWITCH)
ATTRIB ICODE INIT
ATTRIB LCODE LOCKED
ATTRIB PCODE PAGEABLE
ATTRIB SCODE STATIC
MERGE  LCODE CODE DATA BSS EXITDATA
MERGE  ICODE INITDAT0 INITDATA 
MERGE  PCODE PDATA
| $(PELNAME) >C:NUL

! endif

#endif	WIN31


! if "$(TARGET)" == "WIN95" || "$(TARGET)" == "WIN98"

RESNAME		= $(OBJPATHSLASH)$(DEVICENAME).RES
!ifndef VRCNAME
VRCNAME		= $(DEVICENAME).VRC
!endif

! if $(NDIS) == 1
VXD_LIBS	= $(VXD_LIBS)  $(NDISLIB)
! endif	

$(TARGNAME): $(OBJPATH) $(OUTPATH) $(FINALOBJECTS) $(DEFNAME) $(PELNAME) $(RESNAME)
	$(CLEAR)
	$(TLINK32) $(XLFLAGS) -c -Tpd -n -P- -m $(LINKDBG) @&&!
$(VTOOLSD)\LIB\ICRTBOR.OBJ+
$(FINALOBJECTS: =+^
)
$(DLLNAME)
$(MAPNAME)
$(VXD_LIBS)
$(DEFNAME)
!
	$(DEBUGCMD)
	$(PELE) -c $(PELNAME) -o $(TARGNAME) -k 400 $(DLLNAME)
	$(VTOOLSD)\bin\sethdr -n $(DEVICENAME) -x $(TARGNAME) -r $(RESNAME)
	$(MAKESYM)

$(DEFNAME): $(FINALOBJECTS)
	copy &&|
EXPORTS
	_The_DDB	@1
| $(DEFNAME) >C:NUL

$(RESNAME):	$(VRCNAME)
	$(VTOOLSD)\bin\vxdver $(VRCNAME) $(RESNAME)

$(VRCNAME):	$(VTOOLSD)\include\default.vrc
	if not exist $(VRCNAME) copy $(VTOOLSD)\include\default.vrc $(VRCNAME)

$(PELNAME):
	copy &&|
$(DYNASWITCH)
ATTRIB ICODE INIT
ATTRIB LCODE LOCKED
ATTRIB PCODE PAGEABLE
ATTRIB SCODE STATIC
ATTRIB DBOCODE DEBUG
MERGE  ICODE INITDAT0 INITDATA 
MERGE  PCODE PDATA
MERGE  LCODE CODE DATA BSS EXITDATA
| $(PELNAME) >C:NUL

! endif

#
# End of section to build Vxd
#
!endif	# "$(DEVICETYPE)" == "LIB"

!ifdef OBJPATH
$(OBJPATH):
	@echo Checking for existence of build dir : $(OBJPATH)
	@if not exist $(OBJPATH)\NUL mkdir $(OBJPATH)
!endif

!ifdef OUTPATH
!if	"$(OUTPATH)" != "$(OBJPATH)"
$(OUTPATH):
	@echo Checking for existence of output dir : $(OUTPATH)
	@if not exist $(OUTPATH)\NUL mkdir $(OUTPATH)
!endif
!endif
