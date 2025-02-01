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

COPTS = -nologo -W3 -Zdp -Gs -c -DIS_32 -D_X86_ -DWIN32 -DDEBUG -Gzfy -Oxsy -I $(C32_ROOT)\include\winspool.h
LINK = link
LINK_OPTS = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib kernel32.lib $(BASEDIR)\LIB\WIN98\spoolss.lib /ENTRY:DllEntryPoint /DLL
OBJS_EXT =
LIBS = $(BASEDIR)\LIB\WIN98\spoolss.lib libc.lib advapi32.lib mpr.lib shell32.lib
MAPFILE_OPTION = -map:obj\i386\winprint.map

CC   = cl $(COPTS) 

INCLUDE=$(INC32);$(INCLUDE);$(INC16)

all: obj\i386\winprint.dll

obj\i386\winprint.obj: winprint.c winprint.h
        $(CC) -Fo$@ winprint.c

obj\i386\util.obj: util.c
	$(CC) -Fo$@ util.c

obj\i386\banner.obj: banner.c
	$(CC) -Fo$@ banner.c

obj\i386\winprint.res: winprint.rc
    rc -r -DWIN32 -fo $@ winprint.rc

obj\i386\winprint.dll: obj\i386\winprint.obj obj\i386\util.obj obj\i386\banner.obj obj\i386\winprint.res
!ifndef PASS0ONLY
        echo @<<WINPRINT.CRF
$(LINK_OPTS) 
obj\i386\WINPRINT.OBJ obj\i386\UTIL.OBJ obj\i386\BANNER.OBJ 
$(OBJS_EXT)
-OUT:obj\i386\WINPRINT.DLL
$(MAPFILE_OPTION)
obj\i386\WINPRINT.RES
$(LIBS)
-def:winprint.def
<<
		@echo link -OUT:$@
        link @WINPRINT.CRF

        mapsym -s -o obj\i386\winprint.sym obj\i386\winprint.map
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
