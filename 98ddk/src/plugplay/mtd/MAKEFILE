#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************
BUILD_TYPE=plugplay
!INCLUDE $(BASEDIR)\inc\master.mk

INCLUDE =.;$(INCLUDE)

##########################################################################
#                                    
#       Makefile for Sample
#
##########################################################################

DEVICE = SRAMMTD
OBJS   = obj\i386\cardsvcs.obj obj\i386\mtdhdr.obj obj\i386\mtdio.obj 

######### Definitions for the debug level #########

!ifdef DEBUG
DDEBUG =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG =-DDEBLEVEL=0
!endif

########## Definitions for the Assembler ##########

ASM     = ml
AFLAGS  = -coff -DBLD_COFF $(DDEBUG) -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6 -I$(INC32)
ASMENV  = ML

########## Definitions for VxD linker #############

LFLAGS  = /VXD /NOD

########## Base inference rules ###################


#       MASM 6 only inference rules

.asm{obj\i386}.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 


############### VxD device section ####################

KEEPFLAG = NOKEEP
VERSION = 4.0
DDB_NAME = $(DEVICE)_DDB
DEVICETYPE = DYNAMIC

obj\i386\$(DEVICE).sym: obj\i386\$(DEVICE).map
	mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map

obj\i386\$(DEVICE).map: obj\i386\$(DEVICE).VXD 

obj\i386\$(DEVICE).VXD: $(OBJS) $(LIBS) $(STUB)
!ifndef PASS0ONLY
	@echo link -OUT:$@
	LINK @<<obj\i386\$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(DEVICE).vxd
/MAP:obj\i386\$(DEVICE).map
$(OBJS)
$(LIBS)
<<$(KEEPFLAG)
VXD $(DEVICE) $(DEVICETYPE)
DESCRIPTION '$(DEVICE) Device (Version $(VERSION))'
SEGMENTS
    _LPTEXT     CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _LTEXT      CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _LDATA      CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _TEXT       CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _DATA       CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    CONST       CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _TLS        CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _BSS        CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _ITEXT      CLASS 'ICODE'   DISCARDABLE
    _IDATA      CLASS 'ICODE'   DISCARDABLE
    _PTEXT      CLASS 'PCODE'   NONDISCARDABLE
    _PDATA      CLASS 'PDATA'   NONDISCARDABLE SHARED
    _STEXT      CLASS 'SCODE'   RESIDENT
    _SDATA      CLASS 'SCODE'   RESIDENT
    _DBOSTART   CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
    _DBOCODE    CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
    _DBODATA    CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
    _16ICODE    CLASS '16ICODE' PRELOAD DISCARDABLE
    _RCODE      CLASS 'RCODE'
EXPORTS
	$(DDB_NAME) @1
<<$(KEEPFLAG)
!endif


clean:
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist *.bak del *.bak
