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

BUILD_TYPE=printer
!INCLUDE $(BASEDIR)\inc\master.mk

COPTS = -nologo -W3 -Zdp -Gs -c -DIS_32 -D_X86_ -DWIN32 -DDEBUG -Gzfy -Oxsy
LINK = link
LINK_OPTS = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib kernel32.lib $(BASEDIR)\LIB\WIN98\spoolss.lib /ENTRY:DllEntryPoint /DLL
MASM = ml -coff -c -Zi -DMASM6
OBJS_EXT =
LIBS = $(BASEDIR)\LIB\WIN98\spoolss.lib libc.lib advapi32.lib mpr.lib shell32.lib
MAPFILE_OPTION = -map:obj\i386\localmon.map

CC   = cl $(COPTS) 

INCLUDE=$(INCLUDE);$(INC32);$(INC16)

all: obj\i386\localmon.dll

obj\i386\localmon.obj: localmon.c localmon.h
        $(CC) -Fo$@ localmon.c

obj\i386\util.obj: util.c
	$(CC) -Fo$@ util.c

obj\i386\data.obj: data.c
	$(CC) -Fo$@ data.c

obj\i386\dialogs.obj: dialogs.c dialogs.h
	$(CC) -Fo$@ dialogs.c

obj\i386\helper.obj: helper.asm
    $(MASM) -I$(BASEDIR)\inc\win98 -Fo$@ helper.asm

obj\i386\localmon.res: localmon.rc
    rc -r -DWIN32 -fo obj\i386\localmon.res localmon.rc

obj\i386\localmon.dll: obj\i386\localmon.obj obj\i386\util.obj obj\i386\dialogs.obj obj\i386\data.obj obj\i386\helper.obj obj\i386\localmon.res
!ifndef PASS0ONLY
        echo @<<LOCALMON.CRF
$(LINK_OPTS) 
obj\i386\LOCALMON.OBJ obj\i386\DATA.OBJ obj\i386\UTIL.OBJ obj\i386\DIALOGS.OBJ obj\i386\HELPER.OBJ
$(OBJS_EXT)
-OUT:obj\i386\LOCALMON.DLL
$(MAPFILE_OPTION)
obj\i386\LOCALMON.RES
$(LIBS)
-def:localmon.def
<<
		@echo link -OUT:$@
        link @LOCALMON.CRF

        mapsym -s -o obj\i386\localmon.sym obj\i386\localmon.map
!endif


clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.dll del obj\i386\*.dll
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist *.pdb del *.pdb
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.res del obj\i386\*.res
    -@if exist *.bak del *.bak
