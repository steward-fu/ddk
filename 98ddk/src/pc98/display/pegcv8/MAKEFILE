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

BUILD_TYPE=display
!INCLUDE $(BASEDIR)\inc\master.mk

DRVNAME=pegcv8
SRCDIR=.
RESDIR=$(BASEDIR)\src\display\mini\res
TARGETS = obj\i386\$(DRVNAME).drv

L16EXE=$(TARGETS)               # Name of exe.
L16DEF=$(DRVNAME).def           # Our def file.
L16MAP=obj\i386\$(DRVNAME).map           # Our map file.
L16SYM=obj\i386\$(DRVNAME).sym           # Our sym file.
L16LIBS=libw.lib dibeng.lib
L16RES =$(RESDIR)\display.res   # Resource file.
LFLAGS = /NOE/NOD/LI/MAP/AL:16
AFLAGS = -c -Zm -Cp -nologo -I$(DDKROOT)\src\display\inc -I$(INC32) -I$(INC16)
ASM = ml
ASMENV  = ML
LIB=$(LIB16)

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1 = obj\i386\control.obj obj\i386\diblink.obj
OBJ2 = obj\i386\disable.obj obj\i386\enable.obj obj\i386\getresid.obj obj\i386\init.obj
OBJ3 = obj\i386\inkready.obj obj\i386\palette.obj
OBJ4 = obj\i386\sswitch.obj
OBJ5 = obj\i386\pegc.obj
OBJ6 = obj\i386\vga.obj

L16OBJS= $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6)

#-----------------------------------------------------------------------
# Goals
#-----------------------------------------------------------------------

default: $(L16EXE) $(L16RES)

.asm{obj\i386}.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $<

$(L16RES):
    copy $(DRVNAME).rcv $(RESDIR)\display.rcv
    cd $(RESDIR)
    $(MAKE)
    cd ..\..\..\pc98\display\$(DRVNAME)

$(L16EXE): $(L16OBJS) $(L16RES) $(L16DEF)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        $(LINK16) $(LFLAGS) @<<
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4) +
$(OBJ5) +
$(OBJ6)
$(L16EXE)
$(L16MAP)
$(L16LIBS)
$(L16DEF)
<<
        $(RC16B) -40 -b $(L16RES) $(L16EXE)
	copy $(L16MAP) $(@D)\display.map
	-mapsym -n -o $(@D)\display.sym $(@D)\display.map
	del $(@D)\display.map
	if exist $(L16SYM) del $(L16SYM)
	rename $(@D)\display.sym $(DRVNAME).sym
!endif

clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.drv del obj\i386\*.drv
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist *.bak del *.bak
