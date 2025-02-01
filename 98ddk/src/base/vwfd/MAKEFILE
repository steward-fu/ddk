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

BUILD_TYPE=base
!INCLUDE $(BASEDIR)\inc\master.mk

# sample make file for building vwfd virtual device for Microsoft Windows

NAME = VWFD

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for MASM 6 Assembler

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DMINIVDD -DIS_32 -W2 -c -Cx -Zm -DMASM6 $(DDEBUG) -I$(INC32)
ASMENV = ML
LFLAGS = /VXD /NOD

#       MASM 6 only inference rules

.asm{obj\i386}.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $<
	set $(ASMENV)=

all : obj\i386\$(NAME).vxd obj\i386\tstwf.exe

obj\i386\tstwf.exe: obj\i386\tstwf.obj
!ifndef PASS0ONLY
	@echo link -OUT:$@
	cd obj\i386
	$(LINK16) tstwf.obj;
	cd ..\..
!endif

obj\i386\tstwf.obj: tstwf.asm vwfd.inc
        ml -W2 -c -Cx -Zm -DMASM6 $(DDEBUG) -Fo$@ tstwf.asm

obj\i386\vwfd.obj: vwfd.asm 

OBJS = obj\i386\vwfd.obj 

obj\i386\$(NAME).vxd: $(NAME).def $(OBJS)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<obj\i386\$(NAME).lnk
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(NAME).vxd
/MAP:obj\i386\$(NAME).map
/DEF:$(NAME).def
$(OBJS)
<<
        mapsym -s -o obj\i386\$(NAME).sym obj\i386\$(NAME).map
!endif


clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.exe del obj\i386\*.exe
    -@if exist *.bak del *.bak
