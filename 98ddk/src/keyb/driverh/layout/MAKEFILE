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

!INCLUDE $(BASEDIR)\inc\master.mk

AFLAGS = /c /nologo -I$(BASEDIR)\src\keyb\inc

# define assembler command

DLL= kbdus.kbd

ASM = ml
TARGET : $(DLL) $(BIN)

kbdus.kbd: kbdus.asm
        @echo $(LINK16) $* /NOD/NOE;
        $(ASM) $(AFLAGS) -Fo$*.obj -Fl$*.lst $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj


clean:
    -@if exist *.kbd del *.kbd
    -@if exist *.lst del *.lst
    -@if exist *.bin del *.bin
    -@if exist *.bak del *.bak
