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
#    Makefile for vcd
#                                                                          
#############################################################################

BUILD_TYPE=comm
!INCLUDE $(BASEDIR)\inc\master.mk
INC98=..\..\inc\

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

DEVICE        = VCD
OBJS          = obj\i386\vcd.obj obj\i386\vcdint.obj obj\i386\vcdmsg.obj 
DDB_NAME      = $(DEVICE)_DDB
DESCRIPTION   = $(DEVICE) Virtual Device (Version 4.0)
LFLAGS        = /NOD
AFLAGS        = -DSTANDARD_CCALL -DIS_32 -coff -DBLD_COFF -W2 -Zd -c -Cx 
AFLAGS        = $(AFLAGS) -DMASM6 -Sg $(DDEBUG) -DSHARE_IRQ -I$(INC32)
AFLAGS        = $(AFLAGS) -DNEC_98 -I$(INC98)
BASECOPYRIGHT = 1988

default: obj\i386\$(DEVICE).vxd

obj\i386\vcd.obj: vcd.asm
        ml $(AFLAGS) -Fo$@ vcd.asm

obj\i386\vcdint.obj: vcdint.asm
        ml $(AFLAGS) -Fo$@ vcdint.asm

obj\i386\vcdmsg.obj: vcdmsg.asm
        ml $(AFLAGS) -Fo$@ vcdmsg.asm

obj\i386\vcd.res: 
        $(RC16B) -r -i $(INC16);$(INC32) -fo$@ <<$(DEVICE).rc

#include <version.h>
#define Not_VxD
#include <vmm.h>

#define VER_FILETYPE			VFT_VXD
#ifdef $(DEVICE)_DEVICE_ID
#define VER_FILESUBTYPE 		$(DEVICE)_DEVICE_ID
#else
#define VER_FILESUBTYPE 		UNDEFINED_DEVICE_ID
#endif
#define VER_FILEDESCRIPTION_STR 	"$(DESCRIPTION)"
#define VER_INTERNALNAME_STR		"$(DEVICE)"
#define VER_LEGALCOPYRIGHT_YEARS	"$(BASECOPYRIGHT)-1995"
#define VER_ORIGINALFILENAME_STR	"$(DEVICE).VXD"

#include <common.ver>
<<KEEP

obj\i386\$(DEVICE).vxd: $(OBJS) obj\i386\$(DEVICE).res
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<obj\i386\$(DEVICE).lnk /def:<<$(DEVICE).def
$(LFLAGS) /vxd
-ignore:4078 -ignore:4039
/out:obj\i386\$(DEVICE).vxd
/map:obj\i386\$(DEVICE).map
$(OBJS)
<<KEEP
VXD $(DEVICE)
DESCRIPTION '$(DESCRIPTION)'
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
<<KEEP
        $(DDKROOT)\bin\Win98\adrc2vxd.exe obj\i386\$(DEVICE).vxd obj\i386\$(DEVICE).res
        mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map
!endif

clean:
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist obj\i386\*.res del obj\i386\*.res
        -@if exist *.rc  del *.rc
        -@if exist obj\i386\*.lnk del obj\i386\*.lnk
        -@if exist *.def del *.def
        -@if exist *.bak del *.bak
