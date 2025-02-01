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

#-----------------------------------------------------------------------
# SuperVGA (800x600) Display Driver Makefile
#-----------------------------------------------------------------------
# Notes:
# 1) To build a ROM driver, set environment variable ROM=1, delete
#    all object files, and rebuild.
#-----------------------------------------------------------------------

!ifdef ROM
NAME    = SVGAR
!else
NAME    = SUPERVGA
!endif
RESDIR  = ..\rc_high
RESFILE = $(RESDIR)\rc_high.res
MASM    = ml $(MOPT)
LFLAGS  = $(LOPT)/NOE/NOD/LI/MAP/AL:16
INCS    = -I$(DDKROOT)\src\display\inc -I$(INC16) -I$(INC32) -I..
ASM     = $(MASM) $(INCS) $(OPT)
LIB     = $(LIB16)

#-----------------------------------------------------------------------
# Options
#-----------------------------------------------------------------------
# MASMFLAGS definitions:
#   EGA    =  0001h             ;Build the EGA driver.
#   VGA    =  0002h             ;Build the VGA driver.
#   SVGA   =  0004h             ;Build the SuperVGA driver.
#   ROM    =  0008h             ;Build A ROMable driver.
#   PUBDEFS=  4000h             ;Make symbols public.
#   DEBUG  =  8000h             ;Build a debug driver.
#-----------------------------------------------------------------------
!ifdef ROM
MOPT = -DMASMFLAGS=0400Eh -D?QUIET /c /Zm /Cp /W2 /nologo
!else
MOPT = -DMASMFLAGS=04006h -D?QUIET /c /Zm /Cp /W2 /nologo
!endif

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1 = obj\i386\3xswitch.obj obj\i386\bitblt.obj obj\i386\blkwhite.obj obj\i386\bmc_eti.obj obj\i386\bmc_ite.obj
OBJ2 = obj\i386\bmc_main.obj obj\i386\buildstr.obj obj\i386\cblt.obj obj\i386\charwdth.obj obj\i386\chkstk.obj
OBJ3 = obj\i386\colorinf.obj obj\i386\colors.obj obj\i386\control.obj obj\i386\trail.obj obj\i386\cursor.obj
OBJ4 = obj\i386\disable.obj obj\i386\discreen.obj obj\i386\hidither.obj obj\i386\dithers.obj
OBJ5 = obj\i386\egainit.obj obj\i386\enable.obj obj\i386\enum.obj obj\i386\fb.obj obj\i386\output.obj
OBJ6 = obj\i386\pixel.obj obj\i386\polyline.obj obj\i386\inkready.obj
OBJ7 = obj\i386\rlebm.obj obj\i386\robject.obj obj\i386\scanline.obj obj\i386\scanlr.obj
OBJ8 = obj\i386\setmode.obj obj\i386\ssb.obj obj\i386\sswitch.obj obj\i386\strblt.obj
OBJ9 = obj\i386\vgastate.obj obj\i386\ds.obj obj\i386\dss.obj obj\i386\mode.obj

OBJ = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) $(OBJ8) $(OBJ9)

#-----------------------------------------------------------------------
# Goals
#-----------------------------------------------------------------------
!ifdef ROM
default:        romres2 obj\i386\$(NAME)2.drv romres3 obj\i386\$(NAME)3.drv 
!else
default:        res $(NAME).rcv obj\i386\$(NAME).drv 
!endif

!ifdef ROM
romres2:
    copy $(NAME)2.rcv $(RESDIR)\display.rcv
    del $(RESFILE)
    cd $(RESDIR)
    $(MAKE) OPT="-D_SVGA=1"
    cd ..\svga

romres3:
    del $(RESFILE)
    copy $(NAME)2.rcv $(RESDIR)\display.rcv
    cd $(RESDIR)
    $(MAKE) OPT="-D_SVGA=1"
    cd ..\svga

!endif

res: $(RESDIR)\$(NAME).mrk
    copy $(NAME).rcv $(RESDIR)\display.rcv
    cd $(RESDIR)
    $(MAKE) OPT="-D_SVGA=1"
    cd ..\svga

$(RESDIR)\$(NAME).mrk:
	del $(RESDIR)\*.mrk
	del $(RESDIR)\*.res
	echo $(NAME) > $(RESDIR)\$(NAME).mrk

!ifdef ROM
obj\i386\$(NAME)2.drv: obj\i386\$(NAME)2.def $(OBJ) res obj\i386\svgar2.obj 
!	ifndef PASS0ONLY
	@echo link -OUT:$@
	$(LINK16) @<<
obj\i386\svgar2.obj +
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4) +
$(OBJ5) +
$(OBJ6) +
$(OBJ7) +
$(OBJ8) +
$(OBJ9) 
obj\i386\$(NAME)2.drv
obj\i386\$(NAME)2.map
libw
$(NAME)2.def
<<
	$(RC16B) -b $(RESFILE) obj\i386\$(NAME)2.drv
	copy obj\i386\$(NAME)2.map obj\i386\display.map
	-mapsym -n -o obj\i386\display.sym obj\i386\display.map
	del obj\i386\display$(NAME)2.sym
	rename obj\i386\displaydisplay.sym obj\i386\display$(NAME)2.sym
!	endif
#-----------------------------------------------------------------------
# Note: The -b option for the RC compiler is an undocumentated switch that
# prevents LOADONCALL FIXED segments from being marked as PRELOAD.
#-----------------------------------------------------------------------

obj\i386\$(NAME)3.drv: $(NAME)3.def $(OBJ) res obj\i386\supervga.obj
!	ifndef PASS0ONLY
	@echo link -OUT:$@
	$(LINK16) @<<
obj\i386\supervga.obj +
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4) +
$(OBJ5) +
$(OBJ6) +
$(OBJ7) +
$(OBJ8) +
$(OBJ9) 
obj\i386\$(NAME)3.drv
obj\i386\$(NAME)3.map
libw
$(NAME)3.def
<<
	$(RC16B) -b $(RESFILE) obj\i386\$(NAME)3.drv
	copy obj\i386\$(NAME)3.map obj\i386\display.map
	-mapsym -n -o obj\i386\display.sym obj\i386\display.map
	del obj\i386\$(NAME)3.sym
	rename obj\i386\display.sym obj\i386\$(NAME)3.sym

!	endif
!else
obj\i386\$(NAME).drv: $(NAME).def $(OBJ) res obj\i386\supervga.obj  
!ifndef PASS0ONLY
	@echo link -OUT:$@
	$(LINK16) @<<
obj\i386\supervga.obj +
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4) +
$(OBJ5) +
$(OBJ6) +
$(OBJ7) +
$(OBJ8) +
$(OBJ9) 
obj\i386\$(NAME).drv
obj\i386\$(NAME).map
libw
$(NAME).def
<<
	$(RC16B) -b $(RESFILE) obj\i386\$(NAME).drv
	copy obj\i386\$(NAME).map obj\i386\display.map
	-mapsym -n -o obj\i386\display.sym obj\i386\display.map
	del obj\i386\$(NAME).sym
	rename obj\i386\display.sym obj\i386\$(NAME).sym
!endif
!endif

# This goal does not do a complete job of creating the dependancies.
# Some additional text editing will be necessary.
# NOTE: Because of the reference to "includes.exe" THIS GOAL SHOULD 
# NOT BE IN THE DDK VERSION OF THIS MAKEFILE.
depend:
	echo # START Dependencies > depend.txt
	-includes -sjnk -lobj -I.. ..\vga\*.asm >> depend.txt
	-includes -sjnk -lobj -I.. ..\*.asm >> depend.txt
	echo # END Dependencies >> depend.txt

#-----------------------------------------------------------------------
# Dependancies
#-----------------------------------------------------------------------
# START Dependencies 

obj\i386\supervga.obj: ..\vga\vga.asm ..\cursor.inc ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\macros.inc
	$(ASM) /Fo$@ ..\vga\vga.asm 

obj\i386\svgar2.obj: ..\vga\vga.asm ..\cursor.inc ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\macros.inc
	$(ASM) -DONLY286 /Fo$@ ..\vga\vga.asm

obj\i386\inkready.obj: ..\inkready.asm
	$(ASM) /Fo$@ ..\$(@B).asm 

obj\i386\3xswitch.obj: ..\3xswitch.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\bitblt.obj: ..\bitblt.asm ..\CLRCONST.INC ..\CLRDATA.INC \
	 ..\CLRLOCAL.INC ..\COMPUTEY.INC ..\COPYDEV.INC ..\CURSOR.INC \
	 ..\DEVCONST.INC ..\DEVLOCAL.INC ..\DISPLAY.INC ..\EGA.INC \
	 ..\EGAMEM.INC ..\EXIT.INC ..\GENCONST.INC ..\GENDATA.INC \
	 ..\GENLOCAL.INC ..\MACROS.INC ..\PATTERN.INC ..\PDEVICE.INC \
	 ..\ROPDEFS.INC ..\ROPTABLE.INC ..\SPECIAL.INC ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\blkwhite.obj: ..\blkwhite.asm
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\bmc_eti.obj: ..\bmc_eti.asm ..\display.inc ..\lines.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\bmc_ite.obj: ..\bmc_ite.asm ..\display.inc ..\lines.inc \
	 ..\macros.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\bmc_main.obj: ..\bmc_main.asm ..\macros.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\buildstr.obj: ..\buildstr.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\cblt.obj: ..\cblt.asm ..\CLRCONST.INC ..\CLRLOCAL.INC \
	 ..\DEVCONST.INC ..\DEVDATA.INC ..\DEVLOCAL.INC ..\DISPLAY.INC \
	 ..\EGA.INC ..\EGAMEM.INC ..\GENCONST.INC ..\GENLOCAL.INC \
	 ..\MACROS.INC ..\NJMP.INC ..\ROPDEFS.INC ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\charwdth.obj: ..\charwdth.asm ..\fontseg.inc ..\macros.inc \
	 ..\njmp.inc ..\debug.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\chkstk.obj: ..\chkstk.asm ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\colorinf.obj: ..\colorinf.asm ..\display.inc ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\colors.obj: ..\colors.asm
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\control.obj: ..\control.asm ..\cursor.inc ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\cursor.obj: ..\cursor.asm ..\CURSOR.INC ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\disable.obj: ..\disable.asm
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\discreen.obj: ..\discreen.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\lodither.obj: ..\dither.asm ..\mflags.inc
	$(ASM) -DLORES /Fo$@ ..\dither.asm

obj\i386\hidither.obj: ..\dither.asm ..\mflags.inc
	$(ASM) -DHIRES /Fo$@ ..\dither.asm

obj\i386\dithers.obj: ..\dithers.asm ..\display.inc ..\macros.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\ds.obj: ..\ds.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\dss.obj: ..\dss.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\egainit.obj: ..\egainit.asm
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\egastate.obj: ..\egastate.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\enable.obj: ..\enable.asm ..\debug.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\enum.obj: ..\enum.asm ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\fb.obj: ..\fb.asm ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\output.obj: ..\output.asm ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\pixel.obj: ..\pixel.asm ..\display.inc ..\drawmod2.inc \
	 ..\ega.inc ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\polyline.obj: ..\polyline.asm ..\clip.inc ..\display.inc \
	 ..\drawmod2.inc ..\ega.inc ..\egamem.inc ..\lines.inc ..\macros.inc \
	 ..\njmp.inc ..\plysolid.inc ..\polybitm.inc ..\polyline.inc \
	 ..\polystyl.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\rlebm.obj: ..\rlebm.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\njmp.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\robject.obj: ..\robject.asm ..\display.inc ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\scanline.obj: ..\scanline.asm ..\display.inc \
	 ..\drawmod2.inc ..\ega.inc ..\egamem.inc ..\macros.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\scanlr.obj: ..\scanlr.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\setmode.obj: ..\setmode.asm
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\ssb.obj: ..\ssb.asm ..\cursor.inc ..\ega.inc ..\egamem.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\sswitch.obj: ..\sswitch.asm ..\macros.inc ..\debug.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\strblt.obj: ..\strblt.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\trail.obj: ..\trail.asm ..\cursor.inc ..\ega.inc \
	 ..\egamemd.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\vgastate.obj: ..\vgastate.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) /Fo$@ ..\$(@B).asm

obj\i386\mode.obj: ..\mode.asm
      $(ASM) /Fo$@ ..\$(@B).asm

# END Dependencies

clean:
	-@cd $(RESDIR)
	-@$(CMDLINE)
	-@cd $(MAKEDIR)
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.drv del obj\i386\*.drv
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist *.bak del *.bak



