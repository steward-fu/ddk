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

DRVNAME = XGA
RESDIR  = ..\res

L16EXE  = obj\i386\$(DRVNAME).drv        # Name of exe.
L16DEF  = $(DRVNAME).def        # Our def file.
L16MAP  = obj\i386\$(DRVNAME).map        # Our map file.
L16SYM  = obj\i386\$(DRVNAME).sym        # Our sym file.
L16LIBS = libw.lib dibeng.lib
L16RES  = $(RESDIR)\display.res # Resource file.
LFLAGS  = /NOE/NOD/LI/MAP/AL:16

AFLAGS = -c -Zm -Cp -nologo -I$(DDKROOT)\src\display\inc -I$(INC32) -I$(INC16)
ASM     = ml
ASMENV  = ML
LIB = $(LIB16);$(C16_ROOT)\lib

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1 = obj\i386\diblink.obj obj\i386\disable.obj  obj\i386\control.obj obj\i386\sswitch.obj 
OBJ2 = obj\i386\init.obj    obj\i386\inkready.obj obj\i386\ssb.obj     obj\i386\cursor.obj
OBJ3 = obj\i386\bitblt.obj  obj\i386\xgablt.obj   obj\i386\vga.obj     obj\i386\enable.obj
OBJ4 = obj\i386\palette.obj obj\i386\getresid.obj obj\i386\strblt.obj  obj\i386\access.obj

L16OBJS= $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) 

.asm{obj\i386}.obj:
        set $(ASMENV)=$(AFLAGS)
        $(ASM) -Fo$*.obj $< 

#-----------------------------------------------------------------------
# Goals
#-----------------------------------------------------------------------

default: $(L16EXE) $(L16RES)

$(L16RES): 
        copy $(DRVNAME).rcv $(RESDIR)\display.rcv
        cd $(RESDIR)
        $(MAKE) 
        cd ..\$(DRVNAME)

$(L16EXE): $(L16OBJS) $(L16RES) $(L16DEF)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        $(LINK16) $(LFLAGS) @<<
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4)
$(L16EXE)
$(L16MAP)
$(L16LIBS)
$(L16DEF)
<<
        $(RC16B) -b $(L16RES) $(L16EXE)
        copy $(L16MAP) obj\i386\display.map
        -mapsym -n  -o obj\i386\display.sym obj\i386\display.map
        del obj\i386\display.map
        del $(L16SYM)
        rename obj\i386\display.sym $(L16SYM)
!endif

clean:
    -@if exist obj\i386\display*.obj del obj\i386\display*.obj
    -@if exist obj\i386\display*.map del obj\i386\display*.map
    -@if exist obj\i386\display*.drv del obj\i386\display*.drv
    -@if exist obj\i386\display*.sym del obj\i386\display*.sym
    -@if exist *.bak del *.bak
