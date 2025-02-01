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

BUILD_TYPE=keyb
!INCLUDE $(BASEDIR)\inc\master.mk

#Makefile for Windows 95 keyboard driver.

# The following drivers are built:
#	KBD.DRV (==> KEYBOARD.DRV on release) for IBM-compatible systems.

AFLAGS = -I$(BASEDIR)\src\keyb\inc -I$(INC16) -I$(INC32)

LFLAGS = /align:16 /map /NOD

OBJ1 = obj\i386\trap.obj obj\i386\init.obj obj\i386\toascii.obj obj\i386\xlatus.obj
OBJ2 = obj\i386\enable.obj obj\i386\vkscan.obj obj\i386\setspeed.obj obj\i386\tabs.obj
OBJ3 = obj\i386\oemscan.obj obj\i386\getname.obj obj\i386\wep.obj obj\i386\mapvirt.obj


OPT = -c -Zm -DROM=0

# define assembler command
ASM = ml $(OPT)
LIB = $(LIB16)

# define some dependencies for DATAxx.obj assembly
DATAINCS = keyboard.inc vkwin.inc vkoem.inc

all:  tables obj\i386\kbd.drv

tables:
		cd layout
        nmake
        copy kbdus.kbd ..
        cd..

####### Assemble object files.. ####################################

obj\i386\setspeed.obj: setspeed.asm keyboard.inc
        $(ASM) $(AFLAGS) -Fo$@ setspeed.asm

# INIT() -- INIT code
obj\i386\init.obj: init.asm keyboard.inc
       $(ASM) $(AFLAGS) -Fo$@ init.asm

obj\i386\trap.obj: trap.asm keyboard.inc vkwin.inc vkoem.inc
    $(ASM) $(AFLAGS) -Fo$@ trap.asm

obj\i386\toascii.obj: toascii.asm  keyboard.inc vkoem.inc vkwin.inc
    $(ASM) $(AFLAGS) -DDEAD -Fo$@ toascii.asm

obj\i386\oemscan.obj: oemscan.asm  keyboard.inc
    $(ASM) $(AFLAGS) -DDEAD -Fo$@ oemscan.asm

obj\i386\vkscan.obj: vkscan.asm  keyboard.inc
    $(ASM) $(AFLAGS) -DDEAD -Fo$@ vkscan.asm

obj\i386\mapvirt.obj: mapvirt.asm  keyboard.inc
    $(ASM) $(AFLAGS) -DDEAD -Fo$@ mapvirt.asm

obj\i386\xlatus.obj: xlat.asm xlatus.inc
    $(ASM) $(AFLAGS) -Fo$@ xlatus.obj xlat.asm

ENABLEOPTS =

obj\i386\enable.obj: enable.asm vkwin.inc vkoem.inc keyboard.inc equate.inc
        $(ASM) $(AFLAGS) $(ENABLEOPTS) -Fo$@ enable.asm

obj\i386\tabs.obj: tabs.asm vkwin.inc vkoem.inc 
        $(ASM) $(AFLAGS) -Fo$@ tabs.asm

obj\i386\getname.obj: getname.asm
    $(ASM) $(AFLAGS) -Fo$@ getname.asm

obj\i386\wep.obj: wep.asm
    $(ASM) $(AFLAGS) -Fo$@ wep.asm

#### link driver versions ########################################

# ALL the drivers are first linked as 'keyboard.exe'.
# RC is then run to mark them as executables.
# Keyboard.exe is then renamed to KBDx.DRV.
# Mapsym is run on KEYBOARD.MAP, and KEYBOARD.SYM is renamed to
# KBDx.SYM.
# IBM-compatible (includes XT, AT, Enhanced) keyboard driver KBD.DRV.

obj\i386\kbd.drv: kbd.def $(OBJ1) $(OBJ2) $(OBJ3)
!ifndef PASS0ONLY
    copy kbd.def keyboard.def
	@echo link -OUT:$@
	$(LINK16) $(LFLAGS) @<<
$(OBJ1) +
$(OBJ2) +
$(OBJ3)
obj\i386\keyboard.exe
obj\i386\keyboard.map
libw kernel
keyboard.def
<<
    $(RC16B) -i $(INC16) -r -v -foobj\i386\kbd.res kbd.rc
    $(RC16B) obj\i386\kbd.res obj\i386\keyboard.exe
    -del obj\i386\kbd.drv
    copy obj\i386\keyboard.exe obj\i386\kbd.drv
    mapsym -o obj\i386\keyboard.sym obj\i386\keyboard.map
    copy obj\i386\keyboard.sym obj\i386\kbd.sym
    copy obj\i386\keyboard.map obj\i386\kbd.map
    -del keyboard.def
    -del obj\i386\keyboard.exe
    -del obj\i386\keyboard.map
    -del obj\i386\keyboard.sym
    -del kbdus.kbd
!endif

clean:
    -@cd layout
    -@$(CMDLINE)
    -@cd..
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.res del obj\i386\*.res
    -@if exist obj\i386\*.drv del obj\i386\*.drv
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist *.bak del *.bak
