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
#                                                                          
#       Makefile for serial device
#
##########################################################################

BUILD_TYPE=comm
!INCLUDE $(BASEDIR)\inc\master.mk

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

DEVICE = SER8251

OBJS = obj\i386\serial.obj  \
       obj\i386\serutil.obj \
       obj\i386\serfunc.obj \
       obj\i386\serinit.obj \
       obj\i386\serdbg.obj

########## Definitions ###############################

ASM     = ml
AFLAGS  = -coff -DBLD_COFF -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6 -I$(INC32) $(DDEBUG)
AFLAGS  = $(AFLAGS) -DNEC_98
ASMENV  = ML

LFLAGS  = /VXD /NOD

############## VxD device link rule ###################

.asm{obj\i386}.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

obj\i386\$(DEVICE).SYM: obj\i386\$(DEVICE).MAP 
	mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map

obj\i386\$(DEVICE).VXD obj\i386\$(DEVICE).MAP: $(OBJS)
!ifndef PASS0ONLY
	@echo link -OUT:$*.vxd
	link @<<obj\i386\$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(LFLAGS)
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(DEVICE).VXD
/MAP:obj\i386\$(DEVICE).map
$(OBJS)
<<
VXD $(DEVICE) DYNAMIC
DESCRIPTION '$(DEVICE) Device'

SEGMENTS
	_LPTEXT  CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_LTEXT   CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_LDATA   CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_TEXT    CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_DATA    CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	CONST    CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_BSS     CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_ITEXT   CLASS 'ICODE'   DISCARDABLE
	_IDATA   CLASS 'ICODE'   DISCARDABLE
	_PTEXT   CLASS 'PCODE'   NONDISCARDABLE
	_PDATA   CLASS 'PCODE'   NONDISCARDABLE
	_STEXT   CLASS 'SCODE'   RESIDENT
	_SDATA   CLASS 'SCODE'   RESIDENT
	_16ICODE CLASS '16ICODE' PRELOAD DISCARDABLE
	_RCODE   CLASS 'RCODE'
EXPORTS
	$(DEVICE)_DDB @1
<<
!endif

clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist *.bak del *.bak
