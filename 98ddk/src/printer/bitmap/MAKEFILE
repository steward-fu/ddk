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

#-------------------------------------------------------------
# 
# MINI-DRIVER Make file template
#
# The follwing entries should updated
#-------------------------------------------------------------
#
#-------------------------------------------------------------
# Enter the DRV file name (without extension) after DRVNAME =
#-------------------------------------------------------------
# DRIVER NAME
DRVNAME = BITMAP

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
FONTS = STANDARD.PFM

#-------------------------------------------------------------
# Enter the names of all of the version resource files
# (with extensions) after RCV =
#-------------------------------------------------------------
RCV =	BITMAP.RCV

NOFUNCS = -DNODEVSTRETCHBLT

#**********************************************************************
# Set up AFLAGS, CFLAGS and LFLAGS
#**********************************************************************

!ifdef DEBUG

CFLAGS=-nologo -u -c -Asnw -PLM -G2sw -W3 -Od -Zipe -DDEBUG
LFLAGS=/ALIGN:16/NOD/map/Co

!else

CFLAGS=-nologo -u -c -Asnw -PLM -Gsw -W3 -Oasceob1 -Zpe
LFLAGS=/ALIGN:16/NOD/map

!endif

AFLAGS=-DIS_16 -nologo -W2 -Zd -c -Cx -DMASM6

INCLUDE=$(BASEDIR)\src\printer\inc;$(INC16);$(C16_ROOT)\include
LIB=$(LIB16);$(C16_ROOT)\lib


#**********************************************************************
# Do not edit below this line
#**********************************************************************

TARGET:  obj\i386\$(DRVNAME).DRV

obj\i386\$(DRVNAME).res:  $(DRVNAME).rc $(DRVNAME).GPC $(FONTS) $(RCV)
    $(RC16B) -r -I. -fo obj\i386\$(DRVNAME).res $(DRVNAME).RC

obj\i386\libinit.obj: ..\common\libinit.asm
    set ML=$(AFLAGS)
	@echo ml -Fo$@ $(@B).asm
    @ml -Fo$@ ..\common\libinit.asm > NUL

obj\i386\minidriv.obj: ..\common\minidriv.c
    set CL=$(CFLAGS) $(NOFUNCS)
	@echo cl -Fo$@ $(@B).c
    $(CL16) -Fo$@ ..\common\minidriv.c


obj\i386\$(DRVNAME).obj:  $(DRVNAME).c
    set CL=$(CFLAGS)
	@echo cl -Fo$@ $(@B).c
    $(CL16) -Fo$@ $(DRVNAME).c

obj\i386\$(DRVNAME).exe: obj\i386\libinit.obj obj\i386\minidriv.obj obj\i386\$(DRVNAME).obj $(DRVNAME).def
!ifndef PASS0ONLY
	@echo link -OUT:$@
    $(LINK16) $(LFLAGS) @<<$*.lnk
obj\i386\libinit.obj obj\i386\minidriv.obj obj\i386\$(DRVNAME).obj
obj\i386\$(DRVNAME).exe
obj\i386\$(DRVNAME).map
sdllcew libw
$(DRVNAME).def
<<
    mapsym -o obj\i386\$(DRVNAME).sym obj\i386\$(DRVNAME).map
!endif


obj\i386\$(DRVNAME).drv: obj\i386\$(DRVNAME).res obj\i386\$(DRVNAME).exe
!ifndef PASS0ONLY
    $(RC16B) -40 -t -I. obj\i386\$(DRVNAME).res obj\i386\$(DRVNAME).exe
    copy obj\i386\$(DRVNAME).exe obj\i386\$(DRVNAME).drv
!endif


clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.drv del obj\i386\*.drv
    -@if exist obj\i386\*.exe del obj\i386\*.exe
    -@if exist *.pdb del *.pdb
    -@if exist obj\i386\*.res del obj\i386\*.res
    -@if exist *.bak del *.bak
