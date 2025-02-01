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

BUILD_TYPE=display
!INCLUDE $(BASEDIR)\inc\master.mk

#############################################################################
#
#   Makefile for ATI Mini-VDD Port
#
##########################################################################

DEVICE   = ATI
DDB_NAME = ATI_DDB
OBJS     = obj\i386\ati.obj
AFLAGS   = -coff -DBLD_COFF -W2 -Zd -c -Cx -DMASM6 -Sg -DVGA -DVGA31 -DMINIVDD=1 -I$(INC32)
ASMENV   = ML
LFLAGS   = /VXD /NOD

.asm{obj\i386}.obj:
        set ml=$(AFLAGS)
        ml -Fo$@ $< 

obj\i386\$(DEVICE).vxd: $(OBJS)
!ifndef PASS0ONLY	
		@echo link -OUT:$@
        link $(LFLAGS) @<<obj\i386\$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(OBJS)
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(DEVICE).vxd
/MAP:obj\i386\$(DEVICE).map
<<
VXD $(DEVICE) DYNAMIC
DESCRIPTION 'DOS386 $(DEVICE) Device (Version 4.0)'

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
        $(DDB_NAME) @1
<<
        mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map
!endif


clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist *.bak del *.bak
