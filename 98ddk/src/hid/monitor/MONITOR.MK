#############################################################################
#
#	Copyright (C) Microsoft Corporation 1992
#	All Rights Reserved.
#
#	Makefile for $(PROJ)
#
#############################################################################


PROJ=MONITOR

!IFDEF BLDROOT
ROOT=$(BLDROOT)
!else
ROOT=c:\win32
!endif

SRCDIR=..
IS_32=TRUE
IS_SDK=TRUE
IS_PRIVATE = TRUE			# IShellView is internal
DEPENDNAME=..\depend.mk
WANT_C1132=TRUE
BUILD_COFF=TRUE

BUILDDLL=TRUE

L32EXE=$(PROJ).DLL
L32RES=$(PROJ).RES
L32DEF=$(SRCDIR)\$(PROJ).DEF

DLLENTRY=Entry32
DEFENTRY=Entry32
L32FLAGS=$(L32FLAGS) -entry:$(DLLENTRY) -def:$(L32DEF)

L32OBJS=MAP.OBJ MAPCF.OBJ MAPPSX.OBJ MEM.OBJ COMMON.OBJ ASSERT.OBJ MONITOR.OBJ PNP.OBJ

TARGETS=$(L32EXE)

L32LIBSNODEP=kernel32.lib advapi32.lib user32.lib shell32.lib uuid.lib \
             comctl32.lib hid.lib cfgmgr32.lib setupapi.lib msvcrt.lib gdi32.lib

INCFLAGS=$(INCFLAGS) -nwindows.h -noleidl.h -nole2.h -ncommctrl.h -nshlobj.h -nwindowsx.h -nshell2.h -nshellapi.h -ninitguid.h -n..\shelldll\views.h -n..\shelldll\defview.h -nstdarg.h 

L32FLAGS=$(L32FLAGS) -base:0x40400000

!include $(ROOT)\dev\master.mk

LIB=$(LIB);$(ROOT)\wdm10\ddk\lib\i386;$(ROOT)\dev\ddk\lib;$(ROOT)\dev\lib
INCLUDE=$(ROOT)\win\core\shell\inc;$(INCLUDE);$(ROOT)\wdm10\ddk\inc;$(ROOT)\dev\ddk\inc;$(ROOT)\dev\msdev\include
CFLAGS=$(CFLAGS) -YX -Zp1 -Oxs -W3 -WX -Gz

