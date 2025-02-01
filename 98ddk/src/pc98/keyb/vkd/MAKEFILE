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

DEVICE = VKD
INCLUDE = $(INCLUDE);$(BASEDIR)\inc\win98\inc16;

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for the Assembler

OBJS =  obj\i386\vkd.obj obj\i386\vkdhk.obj obj\i386\vkdio.obj obj\i386\vkdmsg.obj obj\i386\vkdphys.obj obj\i386\vad.obj
ASM     = ml
AFLAGS  = -coff -DBLD_COFF -DMINIVDD -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6
AFLAGS  = $(AFLAGS) $(DDEBUG) -DSupport_Reboot -DNEC_98 -DNEC_WINKEY -DNEC_SHIFT
LFLAGS = /VXD /NOD

#       Base inference rules 

#       MASM 6 only inference rules

default:    obj\i386\$(DEVICE).vxd

obj\i386\vkd.obj: vkd.asm 
        ml $(AFLAGS) -Fo$@ vkd.asm

obj\i386\vad.obj: vad.asm 
        ml $(AFLAGS) -Fo$@ vad.asm

obj\i386\vkdhk.obj: vkdhk.asm 
        ml $(AFLAGS) -Fo$@ vkdhk.asm

obj\i386\vkdphys.obj: vkdphys.asm
        ml $(AFLAGS) -Fo$@ vkdphys.asm

obj\i386\vkdmsg.obj: vkdmsg.asm
        ml $(AFLAGS) -Fo$@ vkdmsg.asm

obj\i386\vkdio.obj: vkdio.asm 
        ml $(AFLAGS) -Fo$@ vkdio.asm
        

obj\i386\$(DEVICE).vxd: vkdnec95.def $(OBJS)
        link @<<obj\i386$(DEVICE).lnk
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(DEVICE).vxd
/MAP:obj\i386\$(DEVICE).map
/DEF:vkdnec95.def
$(OBJS)
<<
        mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map

clean:
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist obj\i386\*.res del obj\i386\*.res
        -@if exist *.bak del *.bak
