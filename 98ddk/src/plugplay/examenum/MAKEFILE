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
# Makefile for EXAMENUM - Example Enumerator
#
##########################################################################

BUILD_TYPE=plugplay
!INCLUDE $(BASEDIR)\inc\master.mk

DEVICE  = EXAMENUM
OBJS    = obj\i386\pch.obj obj\i386\sysctrl.obj obj\i386\enum.obj obj\i386\examenum.obj
CL      = cl
CFLAGS  = -DBLD_COFF -Zdp -Gs -c -DWANTVXDWRAPS -DIS_32 -Zl -bzalign
CFLAGS  = -W4 -WX -Ogaisb1 -Gf -Yupch.h -I$(INC32) $(CFLAGS)
ASM     = ml
AFLAGS  = -coff -DBLD_COFF -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6 -I$(INC32)
ASMENV  = ML
LFLAGS  = /VXD /NOD

## Inference rules

.asm{obj\i386}.obj:
        set $(ASMENV)=$(AFLAGS)
        $(ASM) -Fo$*.obj $< 

.c{obj\i386}.obj:
        set CL=$(CFLAGS)
        $(CL) -Fo$*.obj $<

## Targets

default: obj\i386\$(DEVICE).vxd

obj\i386\pch.obj: pch.c
        set CL=$(CFLAGS)
        $(CL) -Ycpch.h -Fo$@ pch.c

obj\i386\sysctrl.obj: sysctrl.c

obj\i386\enum.obj: enum.c

obj\i386\examenum.obj: examenum.asm

obj\i386\$(DEVICE).vxd: $(OBJS)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        LINK @<<obj\i386\$(DEVICE).lnk /DEF:<<$(DEVICE).def
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(DEVICE).vxd
/MAP:obj\i386\$(DEVICE).map
$(OBJS)
$(BASEDIR)\lib\i386\free\vxdwraps.clb
<<$(KEEPFLAG)
VXD $(DEVICE) DYNAMIC
DESCRIPTION '$(DEVICE) Device (Version 4.0)'
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
    $(DEVICE)_DDB @1
<<
!endif

clean:
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist *.pch del *.pch
        -@if exist *.bak del *.bak
