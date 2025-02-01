############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#  Copyright (C) 1998 Microsoft Corporation.  All Rights Reserved.
#
#  File:    makefile.inc
#  Content: NT build file for building the parts that SOURCES can't build
#
############################################################################

!ifndef DDB_NAME
DDB_NAME = $(NAME)_DDB
!endif

#ifdef $(NAME)_DEVICE_ID
VER_FILESUBTYPE = $(NAME)_DEVICE_ID
#else
VER_FILESUBTYPE = VFT2_UNKNOWN
#endif

#ifdef $(PASS0ONLY)
#   We only need this for linking
$O\$(NAME).vxd:

#   Define a rule for the target that makefile.def will want to build
$(TARGET):

#endif

#   Notice, the resources are added with adrc2vxd because the linker won't
#   link resources to a vxd.
$O\$(NAME).VXD: $(OBJECTS) $O\$(NAME).RES makefile.inc sources
    $(LINKER) @<< /def:<<$(NAME).DEF
/nologo
/map
/out:$O\$(NAME).VXD
/nodefaultlib
/align:0x200
/ignore:4039
/ignore:4078
/vxd
/NOD
$(OBJECTS)
<<NOKEEP
VXD $(NAME) DYNAMIC
DESCRIPTION '$(DESCRIPTION)'
EXETYPE         DEV386

SEGMENTS
    _LPTEXT         CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _LTEXT          CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _LDATA          CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _TEXT           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _DATA           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    CONST           CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _TLS            CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _BSS            CLASS 'LCODE'   PRELOAD NONDISCARDABLE
    _LMSG           CLASS 'MCODE'   PRELOAD NONDISCARDABLE IOPL
    _IMSG           CLASS 'MCODE'   PRELOAD DISCARDABLE IOPL
    _ITEXT          CLASS 'ICODE'   DISCARDABLE
    _IDATA          CLASS 'ICODE'   DISCARDABLE
    _PTEXT          CLASS 'PCODE'   NONDISCARDABLE
    _PMSG           CLASS 'MCODE'   NONDISCARDABLE IOPL
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
<<KEEP
    $(MAPSYM_CMD)
    $(BASEDIR)\bin\win98\adrc2vxd $O\$(NAME).VXD $O\$(@B).res

# Make the version resource with the 16bit resource compiler because the 
# 32bit versions produce UNICODE strings which adrc2vxd can't handle
$O\$(NAME).res: makefile.inc sources
    $(BASEDIR)\bin\win98\bin16\rc -r -fo$O\$(NAME).res <<$(NAME).rc
#include <ver.h>
#include "$(MASTER_VERSION_FILE)"

#define VER_FILETYPE                VFT_VXD
#define VER_FILESUBTYPE             $(VER_FILESUBTYPE)
#define VER_FILEDESCRIPTION_STR     $(DESCRIPTION)
#define VER_INTERNALNAME_STR        "$(NAME).VXD"
#define VER_ORIGINALFILENAME_STR    "$(NAME).VXD"

#include "common.ver"
<<KEEP
