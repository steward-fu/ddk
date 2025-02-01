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

# sample make file for building vkd virtual device for Windows/386

NAME = VKD
BIN = obj\i386
TARGETPATH = $(BASEDIR)\lib\i386\$(DDKBUILDENV)
INCLUDE = $(INCLUDE);$(BASEDIR)\inc\win98\inc16;

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for the Assembler

ASM     = ml
AFLAGS  = -coff -DBLD_COFF -DMINIVDD -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6
AFLAGS  = $(AFLAGS) $(DDEBUG) -DSupport_Reboot
ASMENV  = ML
LFLAGS = /VXD /NOD

#       Base inference rules 

#       MASM 6 only inference rules

.asm{$(BIN)}.obj:
	set $(ASMENV)=$(AFLAGS)
	ml -Fo$*.obj $< 

all:    vkd.vxd

vkd{$(BIN)}.obj: vkd.asm 

vad{$(BIN)}.obj: vad.asm 

vkdhk{$(BIN)}.obj: vkdhk.asm 

vkdphys{$(BIN)}.obj: vkdphys.asm

vkdmsg{$(BIN)}.obj: vkdmsg.asm

vkdio{$(BIN)}.obj: vkdio.asm 

OBJS =  $(BIN)\vkd.obj $(BIN)\vkdhk.obj $(BIN)\vkdio.obj $(BIN)\vkdmsg.obj $(BIN)\vkdphys.obj $(BIN)\vad.obj

$(NAME).vxd: $(NAME).def $(OBJS)
!ifndef PASS0ONLY
        link @<<$(NAME).lnk
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:$(TARGETPATH)\$(NAME).vxd
/MAP:$(BIN)\$(NAME).map
/DEF:$(NAME).def
$(OBJS)
<<
        mapsym -s -o $(BIN)\$(NAME).sym $(BIN)\$(NAME).map

!endif