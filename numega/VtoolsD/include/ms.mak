# ms.mak -  Makefile for Microsoft command line compiler
# Copyright (c) 1994-1998 Compuware Corporation

MSBASEFLAGS		= -c -nologo -GB -GF -Gy -Gs -Zp1 -DWANTVXDWRAPS -DVTOOLSD -DALT_INIT_SEG -DIS_32 -DNTVERSION='WDM' -DVXD_COMPATLIB
MSDEBUGFLAGS		= -Zi -Fd$(PDBNAME) -DDEBUG=1
MSRETAILFLAGS		= 
MS3FLAGS		= -DWIN31 -DWIN31_OR_LATER 
MS311FLAGS		= -DWIN31 -DWFW311 -DWFW311_OR_LATER 
MS4FLAGS		= -DWIN40 -DWIN40_OR_LATER -DWFW311_OR_LATER 
MS41FLAGS		= -DWIN40 -DWIN41_OR_LATER -DWIN41 -DWIN40_OR_LATER -DWFW311_OR_LATER 
MASMBASE		= -DIS_32 -DMASM6 -coff -DCOFF -c -W2 -Cx -nologo 
MASMDEBUG		= -DDEBUG -Zd
MASMRETAIL		=

! ifdef MAKE_RETAIL_SYMBOLS
MSRETAILFLAGS	= $(MSRETAILFLAGS) -Zi -Fd$(PDBNAME)
! endif

COMPILEFILTER = | $(VTOOLSD)\bin\fltrwarn LNK4039 LNK4069
! ifdef NOFILTER
COMPILEFILTER = 
! endif			   

! ifndef COPTFLAGS
! if $(DEBUG) != 0
COPTFLAGS = /Od /Oi
!else
COPTFLAGS = /Ox /Oi
! endif
! endif

! ifndef CFLAGS
CFLAGS		= $(MSBASEFLAGS) $(COPTFLAGS)
! endif

! ifndef ASMFLAGS
ASMFLAGS	= $(MASMBASE)
! endif

! ifdef BROWSE
CFLAGS		= $(CFLAGS) /Fr$(OBJPATHSLASH)$(<B)
MAKEBROWSE	= $(BSCMAKE) /n /v /o $(OUTPATHSLASH)$(DEVICENAME).BSC $(OBJPATHSLASH)*.sbr
! else
MAKEBROWSE	=
! endif

!ifndef TARGET
TARGET=WIN95
!endif

! if $(DEBUG) != 0
CFLAGS		= $(CFLAGS) $(MSDEBUGFLAGS)
ASMFLAGS	= $(ASMFLAGS) $(MASMDEBUG)
! else
CFLAGS		= $(CFLAGS) $(MSRETAILFLAGS)
ASMFLAGS	= $(ASMFLAGS) $(MASMRETAIL)
! endif

! if "$(TARGET)" == "WIN31" 
CFLAGS		= $(CFLAGS) $(MS3FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(MS3FLAGS)
! endif

! if "$(TARGET)" == "WFW311" 
CFLAGS		= $(CFLAGS) $(MS311FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(MS311FLAGS)
! endif

! if "$(TARGET)" == "WIN95" 
CFLAGS		= $(CFLAGS) $(MS4FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(MS4FLAGS)
! endif

! if "$(TARGET)" == "WIN98" 
CFLAGS		= $(CFLAGS) $(MS41FLAGS)
ASMFLAGS	= $(ASMFLAGS) $(MS41FLAGS)
! endif

CFLAGS = $(CFLAGS) $(XFLAGS)
ASMFLAGS = $(ASMFLAGS) $(XAFLAGS)

{}.c{$(OBJPATH)}.obj:
	set INCLUDE= .;$(INCLUDEVXD)
	set CL= $(CFLAGS) /Fo$@
	$(C32) $<
	$(EDITBIN) @$(VTOOLSD)\include\ebin.cmd $@ $(COMPILEFILTER)

{}.cpp{$(OBJPATH)}.obj:
	set INCLUDE= .;$(INCLUDEVXD)
	set CL= $(CFLAGS) /DALT_INIT_SEG /Fo$@
	$(C32) $<
	$(EDITBIN) @$(VTOOLSD)\include\ebin.cmd $@ $(COMPILEFILTER)

{}.asm{$(OBJPATH)}.obj:
	set INCLUDE= .;$(INCLUDEVXD)
	$(ASM6) $(ASMFLAGS) /Fo$@ $<

# SOURCEPATH can be used to specify an alternate directory for sources.
# Don't define these rules unless SOURCEPATH exists and is not the current directory
# 
! if defined(SOURCEPATH) && ("$(SOURCEPATH)" != ".")

SOURCEINCPATH = $(SOURCEPATH)

{$(SOURCEPATH)}.c{$(OBJPATH)}.obj:
	set INCLUDE=$(SOURCEINCPATH);$(INCLUDEVXD)
	set CL= $(CFLAGS) /Fo$@
	$(C32) $<
	$(EDITBIN) @$(VTOOLSD)\include\ebin.cmd $@ $(COMPILEFILTER)

{$(SOURCEPATH)}.cpp{$(OBJPATH)}.obj:
	set INCLUDE=$(SOURCEINCPATH);$(INCLUDEVXD)
	set CL= $(CFLAGS) /DALT_INIT_SEG /Fo$@
	$(C32) $<
	$(EDITBIN) @$(VTOOLSD)\include\ebin.cmd $@ $(COMPILEFILTER)

{$(SOURCEPATH)}.asm{$(OBJPATH)}.obj:
	set INCLUDE=$(SOURCEINCPATH);$(INCLUDEVXD)
	$(ASM6) $(ASMFLAGS) /Fo$@ $<

! endif

