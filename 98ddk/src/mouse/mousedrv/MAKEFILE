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
#
#    Makefile for mouse.drv
#
#############################################################################

BUILD_TYPE=mouse
!INCLUDE $(BASEDIR)\inc\master.mk

LFLAGS  = /L /MAP /NOPACKCODE /NOE /ALIGN:16
MFLAGS  = -m
ASM     = ml
AFLAGS  = -W2 -Zd -c -Cx -Zm -DIS_16 -DNot_VxD -Sg -I$(INC16) -I$(INC32)
ASMENV  = ML

.asm{obj\i386}.obj:
        set $(ASMENV)=$(AFLAGS)
        $(ASM) -Fo$*.obj $< 

.rc{obj\i386}.res:
	$(RC16B) -r -i $(INC16) -fo$@ $(@B).rc

obj\i386\mouse.drv: obj\i386\mouse.obj obj\i386\mouse.res mouse.def
!ifndef PASS0ONLY
		@echo link -OUT:$@
        $(LINK16) $(LFLAGS) @<<obj\i386\mouse.lnk
obj\i386\mouse.obj
obj\i386\mouse.drv 
obj\i386\mouse.map 
$(LIB16)\libw.lib 
mouse.def;
<<
        $(RC16B) obj\i386\mouse.res obj\i386\mouse.drv
	mapsym -m -s -o obj\i386\mouse.sym obj\i386\mouse.map
!endif


obj\i386\mouse.obj: mouse.asm 

obj\i386\mouse.res: mouse.rc 

clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.res del obj\i386\*.res
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.drv del obj\i386\*.drv
    -@if exist *.bak del *.bak
