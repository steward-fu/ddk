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
OBJS_EXT =
LIBS = $(BASEDIR)\LIB\WIN98\spoolss.lib libc.lib advapi32.lib mpr.lib shell32.lib
MAPFILE_OPTION = -map:obj\i386\pjlmon.map

INCLUDE=$(INC32);$(INCLUDE);$(INC16)


CC   = cl $(COPTS) 

all: obj\i386\pjlmon.dll

obj\i386\pjlmon.obj: pjlmon.c parsepjl.h dialogs.h
        $(CC) -Fo$@ pjlmon.c

obj\i386\parsepjl.obj: parsepjl.obj
	copy $(@B).obj $@

obj\i386\utillm.obj: utillm.c
	$(CC) -Fo$@ utillm.c

obj\i386\pjlmon.res: pjlmon.rc
    rc -r -DWIN32 -fo $@ pjlmon.rc

obj\i386\pjlmon.dll: obj\i386\pjlmon.obj obj\i386\utillm.obj obj\i386\parsepjl.obj obj\i386\pjlmon.res
!ifndef PASS0ONLY
        echo @<<PJLMON.CRF
$(LINK_OPTS) 
obj\i386\PJLMON.OBJ obj\i386\UTILLM.OBJ obj\i386\PARSEPJL.OBJ 
$(OBJS_EXT)
-OUT:obj\i386\PJLMON.DLL
$(MAPFILE_OPTION)
obj\i386\PJLMON.RES
$(LIBS)
-def:pjlmon.def
<<
		@echo link -OUT:$@
        link @PJLMON.CRF

        mapsym -s -o obj\i386\pjlmon.sym obj\i386\pjlmon.map
!endif

clean:
    -@if exist obj\i386\pjlmon.obj del obj\i386\pjlmon.obj
    -@if exist obj\i386\utillm.obj del obj\i386\utillm.obj
    -@if exist obj\i386\*.dll del obj\i386\*.dll
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist *.pdb del *.pdb
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.res del obj\i386\*.res
    -@if exist *.bak del *.bak
