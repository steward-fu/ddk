#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
#                                                                           *
#****************************************************************************

BUILD_TYPE=base
LOCAL_CLEAN=1
!INCLUDE $(BASEDIR)\inc\master.mk

DEVICE      = ASYNCW32
OBJS        = obj\i386\asyncw32.obj obj\i386\sysctrl.obj obj\i386\wrappers.obj
CFLAGS      = -DWIN32 -DCON -Di386 -D_X86_ -D_NTWIN -W3 -Gs -D_DEBUG -Zi -I$(INC32)
CVXDFLAGS   = -Zdp -Gs -c -DIS_32 -Zl -DDEBLEVEL=1 -DDEBUG -I$(INC32)
LFLAGS      = -machine:i386 -debug:notmapped,full -debugtype:cv -subsystem:console kernel32.lib 
LVXDFLAGS   = -MACHINE:i386 -DEBUG:NONE -PDB:NONE -NOD
AFLAGS      = -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -DDEBLEVEL=1 -DDEBUG -I$(INC32)
DESCRIPTION = DOS386 Async W32 Devioctl Sample Device

all: obj\i386\con_aw32.exe obj\i386\asyncw32.vxd

obj\i386\asyncw32.obj: asyncw32.c
        cl $(CVXDFLAGS) -Fo$@ %s

obj\i386\sysctrl.obj: sysctrl.asm
        ml $(AFLAGS) -Fo$@ %s

obj\i386\wrappers.obj: wrappers.asm
        ml -coff -I$(INC32) -DBLD_COFF -W2 -c -Cx -DMASM6 -DSEGNUM=1 -Fo$@ %s

obj\i386\$(DEVICE).res: 
        $(RC16B) -v -r -i $(INC16);$(INC32) -fo $@ <<$(DEVICE).rc
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
#define VER_LEGALCOPYRIGHT_YEARS	"1995"
#define VER_ORIGINALFILENAME_STR	"$(DEVICE).VXD"

#include <common.ver>
<<

obj\i386\$(DEVICE).vxd: $(OBJS) obj\i386\$(DEVICE).res
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<$(DEVICE).lnk /def:<<$(DEVICE).def
$(LVXDFLAGS) /vxd /libpath:$(LIB32)
-ignore:4078 -ignore:4039
/out:obj\i386\$(DEVICE).vxd
/map:obj\i386\$(DEVICE).map
$(OBJS)
$(BASEDIR)\lib\i386\free\vxdwraps.clb
<<
VXD $(DEVICE) DYNAMIC
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
    $(DEVICE)_DDB @1
<<
        adrc2vxd.exe $@ obj\i386\$(DEVICE).res
        mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map
        del obj\i386\$(DEVICE).exp
        del obj\i386\$(DEVICE).lib
!endif

obj\i386\con_aw32.obj: con_aw32.c
        cl $(CFLAGS) -c -Fo$@ %s

obj\i386\con_aw32.exe: obj\i386\con_aw32.obj
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<con_aw32.crf
$(LFLAGS)
-PDB:NONE -OUT:obj\i386\con_aw32.exe -MAP:obj\i386\con_aw32.map obj\i386\con_aw32.obj
<<
!endif

clean:
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.exe del obj\i386\*.exe
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist obj\i386\*.res del obj\i386\*.res
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist *.pdb del *.pdb
        -@if exist *.bak del *.bak
