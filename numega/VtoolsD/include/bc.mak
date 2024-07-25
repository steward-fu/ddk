# BC.MAK -  Master makefile for Borland Code Builder builds
# Copyright (c) 1994-1998 Compuware Corporation
#

.SUFFIXES:

BORBASEFLAGS	= -c -Oi -a1 -RT- -x- -DIS_32 -DWANTVXDWRAPS -DVTOOLSD -I$(INCLUDEVXD) -DNTVERSION='WDM' -DVXD_COMPATLIB
BORDEBUGFLAGS	= -v -DDEBUG=1
BORRETAILFLAGS	= -O2 -O -Oc
BORCPPFLAGS		= -ps -w-hid
BOR3FLAGS		= -DWIN31 
BOR311FLAGS		= -DWIN31 -DWFW311 -DWFW311_OR_LATER
BOR4FLAGS		= -DWIN40 -DWIN40_OR_LATER  -DWFW311_OR_LATER
BOR41FLAGS		= -DWIN41 -DWIN40 -DWIN40_OR_LATER -DWIN41_OR_LATER -DWFW311_OR_LATER
TASMPARAMS		= @tasm.cmd >> $$(LOG)
TASMBASE		= -m9 -mx -d__BORLANDC__=1 -t -I$(VTOOLSD)\INCLUDE -I$(VTOOLSD)\LIB\INCLUDE
TASMDEBUG		= -DDEBUG -zi 
TASMRETAIL		=
BCWRAPSEGA 		= if exist $$O segalias -p ..\wraps.seg $$O
TASMCMD			= $(TASM) $(TASMPARAMS)

! ifdef MAKE_RETAIL_SYMBOLS
BORRETAILFLAGS	= $(BORRETAILFLAGS) -v
! endif

LOCKFLAGS	= -DDEFSEG=1 -zC_LTEXT -zALCODE -zR_LDATA -zTLCODE
INITFLAGS	= -DDEFSEG=2 -zC_ITEXT -zAICODE -zR_IDATA -zTICODE
!if "$(TARGET)" == "WIN31"
PAGEABLEFLAGS	= -DDEFSEG=3 -zC_PTEXT -zAPCODE -zR_PDATA -zTPCODE
!endif
!if "$(TARGET)" == "WIN95"
PAGEABLEFLAGS	= -DDEFSEG=3 -zC_PTEXT -zAPCODE -zR_PDATA -zTPDATA
STATICFLAGS	= -DDEFSEG=4 -zC_STEXT -zASCODE -zR_SDATA -zTSCODE
DEBUGFLAGS	= -DDEFSEG=5 -zC_DBOTEXT -zDBOCODE -zR_DBODATA -zTDBOCODE
!endif

! ifndef COPTFLAGS
! if $(DEBUG) == 0
COPTFLAGS	= -Ox -Oa 
!else
COPTFLAGS       = -Od
! endif
! endif

! ifndef CFLAGS
CFLAGS			= $(BORBASEFLAGS) $(COPTFLAGS)
! endif

! ifndef ASMFLAGS
ASMFLAGS	= $(TASMBASE)
! endif

ASMDEBUG	= $(TASMDEBUG)
CDEBUG		= $(BORDEBUGFLAGS)
ASMRETAIL	= $(TASMRETAIL)
CRETAIL		= $(BORRETAILFLAGS)

! if $(DEBUG) != 0
CFLAGS		= $(CFLAGS) $(CDEBUG)
ASMFLAGS	= $(ASMFLAGS) $(ASMDEBUG)
! else
CFLAGS		= $(CFLAGS) $(CRETAIL)
ASMFLAGS	= $(ASMFLAGS) $(ASMRETAIL)
! endif


! if "$(TARGET)" == "WIN31" 

CFLAGS		= $(CFLAGS) $(BOR3FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(BOR3FLAGS)

! endif


! if "$(TARGET)" == "WFW311" 

CFLAGS		= $(CFLAGS) $(BOR311FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(BOR311FLAGS)

! endif


!if "$(TARGET)" == "WIN95"

CFLAGS		= $(CFLAGS) $(BOR4FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(BOR4FLAGS)

!endif 

!if "$(TARGET)" == "WIN98"

CFLAGS		= $(CFLAGS) $(BOR41FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(BOR41FLAGS)

!endif 

CFLAGS = $(CFLAGS) $(XFLAGS)
ASMFLAGS = $(ASMFLAGS) $(XAFLAGS)


# Inference rules

{.}.c{$(OBJPATH)}.obj:
	$(C32) @&&|
$(CFLAGS) -I. $(LOCKFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.cpp{$(OBJPATH)}.obj:
	$(C32) @&&|
$(CFLAGS) -I. $(LOCKFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.c{$(OBJPATH)}.obl:
	$(C32) @&&|
$(CFLAGS) -I. $(LOCKFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.cpp{$(OBJPATH)}.obl:
	$(C32) @&&|
$(CFLAGS) -I. $(LOCKFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@


{.}.c{$(OBJPATH)}.obi:
	$(C32) @&&|
$(CFLAGS) -I. $(INITFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.cpp{$(OBJPATH)}.obi:
	$(C32) @&&|
$(CFLAGS) -I. $(INITFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

#ifdef WIN40
{.}.c{$(OBJPATH)}.obs:
	$(C32) @&&|
$(CFLAGS) -I. $(STATICFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.cpp{$(OBJPATH)}.obs:
	$(C32) @&&|
$(CFLAGS) -I. $(STATICFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@


{.}.c{$(OBJPATH)}.obd:
	$(C32) @&&|
$(CFLAGS) -I. $(DEBUGFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.cpp{$(OBJPATH)}.obd:
	$(C32) @&&|
$(CFLAGS) -I. $(DEBUGFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

#endif

{.}.c{$(OBJPATH)}.obp:
	$(C32) @&&|
$(CFLAGS) -I. $(PAGEABLEFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.cpp{$(OBJPATH)}.obp:
	$(C32) @&&|
$(CFLAGS) -I. $(PAGEABLEFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{.}.asm{$(OBJPATH)}.obj:
	$(TASM32) @&&|
$(ASMFLAGS) -I$(VTOOLSD)\include
| $<, $@

# SOURCEPATH can be used to specify an alternate directory for sources.
# Don't define these rules unless SOURCEPATH exists and is not the current directory
# 
! ifdef SOURCEPATH
! if "$(SOURCEPATH)" != "."

SOURCEINCPATH = $(SOURCEPATH)

{$(SOURCEPATH)}.c{$(OBJPATH)}.obj:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(LOCKFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.cpp{$(OBJPATH)}.obj:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(LOCKFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.c{$(OBJPATH)}.obl:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(LOCKFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.cpp{$(OBJPATH)}.obl:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(LOCKFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@


{$(SOURCEPATH)}.c{$(OBJPATH)}.obi:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(INITFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.cpp{$(OBJPATH)}.obi:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(INITFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

#ifdef WIN40
{$(SOURCEPATH)}.c{$(OBJPATH)}.obs:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(STATICFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.cpp{$(OBJPATH)}.obs:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(STATICFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@


{$(SOURCEPATH)}.c{$(OBJPATH)}.obd:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(DEBUGFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.cpp{$(OBJPATH)}.obd:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(DEBUGFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

#endif

{$(SOURCEPATH)}.c{$(OBJPATH)}.obp:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(PAGEABLEFLAGS) -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.cpp{$(OBJPATH)}.obp:
	$(C32) @&&|
$(CFLAGS) -I$(SOURCEINCPATH) $(PAGEABLEFLAGS) -ps -o$@ $<
|
	$(SEGALIAS) -p $(SEGFILE) $@

{$(SOURCEPATH)}.asm{$(OBJPATH)}.obj:
	$(TASM32) @&&|
$(ASMFLAGS) -I$(VTOOLSD)\include
| $<, $@

! endif # SOURCEPATH
! endif # "$(SOURCEPATH)" != "."
