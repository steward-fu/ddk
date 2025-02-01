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
!INCLUDE $(BASEDIR)\inc\master.mk

CFLAGS     = -DWIN32 -DCON -Di386 -D_X86_ -D_NTWIN -W3 -Gs -D_DEBUG -Zi -I$(INC32)
CVXDFLAGS  = -Zdp -Gs -c -DIS_32 -Zl -DDEBLEVEL=1 -DDEBUG -I$(INC32)
LFLAGS     = -machine:i386 -debug:notmapped,full -debugtype:cv -subsystem:console kernel32.lib 
AFLAGS     = -coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -DDEBLEVEL=1 -DDEBUG -I$(INC32)

all: obj\i386\con_samp.exe obj\i386\cvxdsamp.vxd

obj\i386\cvxdsamp.obj: cvxdsamp.c
        cl $(CVXDFLAGS) -Fo$@ %s

obj\i386\cvxdctrl.obj: cvxdctrl.asm
        ml $(AFLAGS) -Fo$@ %s

obj\i386\vkdgko.obj: vkdgko.asm
        ml -coff -I$(INC32) -DBLD_COFF -W2 -c -Cx -DMASM6 -DSEGNUM=1 -Fo$@ %s

obj\i386\cvxdsamp.vxd: obj\i386\cvxdsamp.obj obj\i386\cvxdctrl.obj obj\i386\vkdgko.obj \
                  cvxdsamp.def
        echo >NUL @<<cvxdsamp.crf
-MACHINE:i386 -DEBUG:NONE -PDB:NONE -ignore:4078 -ignore:4039
-DEF:cvxdsamp.def -OUT:obj\i386\cvxdsamp.vxd -MAP:obj\i386\cvxdsamp.map -LIBPATH:$(LIB32)
-VXD $(BASEDIR)\lib\i386\free\vxdwraps.clb obj\i386\cvxdsamp.obj obj\i386\cvxdctrl.obj obj\i386\vkdgko.obj
<<
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @cvxdsamp.crf
        cd obj\i386
        mapsym cvxdsamp
        cd ..\..
!endif

obj\i386\con_samp.obj: con_samp.c 
        cl $(CFLAGS) -c -Fo$@ %s

obj\i386\con_samp.exe: obj\i386\con_samp.obj
        echo >NUL @<<con_samp.crf
$(LFLAGS)
-PDB:NONE -OUT:obj\i386\con_samp.exe -MAP:obj\i386\con_samp.map obj\i386\con_samp.obj 
<<
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @con_samp.crf
!endif

clean:
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.exe del obj\i386\*.exe
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist *.pdb del *.pdb
        -@if exist *.bak del *.bak
        
