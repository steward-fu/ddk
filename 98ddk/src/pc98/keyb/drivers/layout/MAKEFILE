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

DLL=kbdjp.kbd kbdca.kbd

ASM = ml
TARGET : $(DLL) $(BIN)

kbdjp.kbd: kbdjp.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj

kbdca.kbd: kbdca.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj

xlat850.bin: xlat850.asm xlat850.inc
        $(ASM) $(AFLAGS) -Foxlat850.obj xlat850.asm
        $(LINK16) xlat850 /NOD /NOE;
        exe2bin xlat850
        del xlat850.obj
        del xlat850.exe


clean:
    -@if exist *.kbd del *.kbd
    -@if exist *.lst del *.lst
    -@if exist *.bin del *.bin
    -@if exist *.bak del *.bak
