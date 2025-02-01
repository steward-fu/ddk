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
#       Makefile for vsd device
#
##########################################################################

BUILD_TYPE=block
!INCLUDE $(BASEDIR)\inc\master.mk

DEVICE  = HELPVSD
OBJS    = obj\i386\helpvsd.obj obj\i386\AER.obj obj\i386\VSDCO.obj obj\i386\helpreq.obj

######### Definitions for the debug level #########

!ifdef DEBUG
DDEBUG =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG =-DDEBLEVEL=0
!endif

########## Definitions for the Assembler ##########

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -nologo -W3 -Zd -c -Cx -DMASM6 -DINITLOG $(DDEBUG)
AFLAGS = $(AFLAGS) -I$(DDKROOT)\src\block\inc -I$(INC32)
ASMENV = ML

########## Definitions for linker #############

LFLAGS  = /VXD /NOD

########## Base inference rules ###################

.asm{obj\i386}.obj:
    set $(ASMENV)=$(AFLAGS)
    $(ASM) -Fo$*.obj $< 

############### VxD device section ####################

obj\i386\$(DEVICE).sym: obj\i386\$(DEVICE).map
    mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map

obj\i386\$(DEVICE).map: obj\i386\$(DEVICE).vxd

obj\i386\$(DEVICE).vxd : $(OBJS)
!ifndef PASS0ONLY
	@echo link -OUT:$@
    link @<<obj\i386\$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(LFLAGS)
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(DEVICE).vxd
/MAP:obj\i386\$(DEVICE).map
$(OBJS)
<<
VXD $(DEVICE) DYNAMIC
DESCRIPTION 'DOS386 $(DEVICE) Device  (Version 4.0)'

SEGMENTS
        _LPTEXT         CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _LTEXT          CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _LDATA          CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _TEXT           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _DATA           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        CONST           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _TLS            CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _BSS            CLASS 'LCODE'   PRELOAD NONDISCARDABLE
        _ITEXT          CLASS 'ICODE'   DISCARDABLE
        _IDATA          CLASS 'ICODE'   DISCARDABLE
        _PTEXT          CLASS 'PCODE'   NONDISCARDABLE
        _PDATA          CLASS 'PDATA'   NONDISCARDABLE SHARED
        _STEXT          CLASS 'SCODE'   RESIDENT
        _SDATA          CLASS 'SCODE'   RESIDENT
        _DBOSTART       CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
        _DBOCODE        CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
        _DBODATA        CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
        _16ICODE        CLASS '16ICODE' PRELOAD DISCARDABLE
        _RCODE          CLASS 'RCODE'

EXPORTS
        $(DEVICE)_DDB @1
<<
!endif

clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist *.bak del *.bak
