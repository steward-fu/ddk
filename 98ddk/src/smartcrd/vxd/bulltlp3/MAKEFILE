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

!ifdef MASTER_MAKE
BUILD_BITS=32
BUILD_TYPE=block
!INCLUDE $(DDKROOT)\master.mk
!endif

#
# remove comment from the next line to compile your driver with debugging info
# 
!if "$(DDKBUILDENV)" == "checked"
DEBUG=1
!endif

BIN=obj\i386
!ifdef DEBUG
DBG=DEBUG
!else
DBG=NODEBUG
!endif

DEVICE      = BULLTLP3
OBJS        = $(BIN)\tlp3Strt.obj $(BIN)\tlp3scr.obj $(BIN)\tlp3cb.obj $(BIN)\tlp3Wrap.obj
CVXDFLAGS   = -Zd -Gs -c -DIS_32 -Zl -D$(DBG) -DSMCLIB_VXD -Fr -Oi -I. -I..\..\inc -I.. -Fa
LVXDFLAGS   = -MACHINE:i386 -PDB:NONE -NOD
AFLAGS      = -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -DDEBLEVEL=1 -DDEBUG -Fl
DESCRIPTION = Bull SmarTLP3 Serial Smart Card Driver

all: $(BIN)\bulltlp3.vxd

$(BIN)\tlp3scr.obj: tlp3scr.c
        cl $(CVXDFLAGS) -Fo$@ %s

$(BIN)\tlp3cb.obj: tlp3cb.c
        cl $(CVXDFLAGS) -Fo$@ %s

$(BIN)\tlp3Strt.obj: tlp3Strt.asm
        ml $(AFLAGS) -Fo$@ %s

$(BIN)\tlp3Wrap.obj: tlp3Wrap.asm
        ml -coff -DBLD_COFF -W2 -c -Cx -DMASM6 -DSEGNUM=1 -Fo$@ %s

$(BIN)\$(DEVICE).res: 
        $(BASEDIR)\bin\win98\bin16\rc -r -fo $@ <<$(DEVICE).rc

#include <version.h>
#define Not_VxD
#include <vmm.h>
#define VER_FILETYPE			VFT_VXD
#define VER_FILESUBTYPE 		0x0004E
#define VER_FILEDESCRIPTION_STR 	"$(DESCRIPTION)"
#define VER_INTERNALNAME_STR		"$(DEVICE)"
#define VER_LEGALCOPYRIGHT_YEARS	"1998"
#define VER_LEGALCOPYRIGHT_STR	"Copyright (C) Bull CP8." VER_LEGALCOPYRIGHT_YEARS

#define VER_FILEVERSION			VER_PRODUCTVERSION
#define VER_FILEVERSION_STR		"1.00"
#define VER_ORIGINALFILENAME_STR	VER_INTERNALNAME_STR

#undef VER_PRODUCTNAME_STR
#define VER_PRODUCTNAME_STR		"Bull SmarTLP3 Serial Smart Card Reader"

#undef VER_COMPANYNAME_STR
#define VER_COMPANYNAME_STR        	"Bull"
#include <common.ver>
<<

$(BIN)\$(DEVICE).vxd: $(OBJS) $(BIN)\$(DEVICE).res
!ifndef PASS0ONLY
		echo link -OUT:$@
        link @<<$(DEVICE).lnk /def:<<$(DEVICE).def
$(LVXDFLAGS) /vxd
-ignore:4078 -ignore:4039
/out:$(BIN)\$(DEVICE).vxd
/map:$(BIN)\$(DEVICE).map
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
        adrc2vxd.exe $@ $(BIN)\$(DEVICE).res
        mapsym -s -o $(BIN)\$(DEVICE).sym $(BIN)\$(DEVICE).map
		bscmake /o smcvxd /n *.sbr 
        del $(BIN)\$(DEVICE).exp
        del $(BIN)\$(DEVICE).lib
!endif

clean:
		-@del $(BIN)\*.obj
		-@del $(BIN)\*.vxd
		-@del $(BIN)\*.exe
		-@del $(BIN)\*.map
		-@del $(BIN)\*.sym
		-@del $(BIN)\*.res
		-@rd  $(BIN)
		-@del *.pdb
      













