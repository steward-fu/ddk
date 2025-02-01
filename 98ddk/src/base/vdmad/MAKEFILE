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

BUILD_TYPE=base
!INCLUDE $(BASEDIR)\inc\master.mk

# sample make file for building VDMAD virtual device for Windows

NAME = VDMAD

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for MASM 6 Assembler

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zd -DMASM6 -I$(INC32)
ASMENV = ML
LFLAGS = /VXD /NOD

#       MASM 6 only inference rules

.asm{obj\i386}.obj:
    set $(ASMENV)=$(AFLAGS)
    $(ASM) -Fo$*.obj $< 

all : obj\i386\$(NAME).vxd

OBJS = obj\i386\dmaeisa.obj  \
       obj\i386\vdmad.obj    \
       obj\i386\vdmadapi.obj \
       obj\i386\vdmadio.obj  \
       obj\i386\vdmadmsg.obj \
       obj\i386\vdmadps2.obj \
       obj\i386\vdmadarb.obj

obj\i386\vdmadmsg.obj: vdmadmsg.asm

obj\i386\vdmad.obj: vdmad.asm

obj\i386\vdmadio.obj: vdmadio.asm

obj\i386\vdmadapi.obj: vdmadapi.asm

obj\i386\vdmadarb.obj: vdmadarb.asm

obj\i386\dmaeisa.obj: dmaeisa.asm

obj\i386\vdmadps2.obj: vdmadps2.asm

obj\i386\$(NAME).vxd: $(OBJS)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<obj\i386\$(NAME).lnk /DEF:<<obj\i386\$(NAME).def
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(NAME).vxd
/MAP:obj\i386\$(NAME).map
$(OBJS)
<<
VXD $(NAME)
DESCRIPTION 'VDMAD Sample VxD for Microsoft Windows'
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
    $(NAME)_DDB @1
<<
     mapsym -s -o obj\i386\$(NAME).sym obj\i386\$(NAME).map
!endif


clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist *.bak del *.bak
