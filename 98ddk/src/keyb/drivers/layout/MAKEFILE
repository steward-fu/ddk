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

DLL=kbdus.kbd  \
    kbdgr.kbd  \
    kbdgr1.kbd \
    kbddv.kbd  \
    kbdfr.kbd  \
    kbdsf.kbd  \
    kbdsg.kbd  \
    kbdusx.kbd \
    kbdusx2.kbd

BIN=xlat850.bin

ASM = ml
TARGET : $(DLL) $(BIN)

kbdus.kbd: kbdus.asm
        @echo $(LINK16) $* /NOD/NOE;
        $(ASM) $(AFLAGS) -Fo$*.obj -Fl$*.lst $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj


kbdgr.kbd: kbdgr.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj
 
kbdgr1.kbd: kbdgr1.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj
 
kbddv.kbd: kbddv.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj
 

kbdfr.kbd: kbdfr.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj
 
kbdsf.kbd: kbdsf.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj
 
kbdsg.kbd: kbdsg.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj
 
kbdusx.kbd: kbdusx.asm
        $(ASM) $(AFLAGS) -Fo$*.obj $*.asm
        $(LINK16) $* /NOD/NOE;
        exe2bin $*.exe $*.kbd
        del $*.exe
        del $*.obj
 
kbdusx2.kbd: kbdusx2.asm
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
