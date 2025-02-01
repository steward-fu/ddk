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

BUILD_TYPE=comm
!INCLUDE $(BASEDIR)\inc\master.mk

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

################################
#####                      #####  
##### THIS IS A 16-BIT DLL #####
#####                      #####  
################################

NAME   = serialui
OBJS   = obj\i386\libinit.obj obj\i386\pch.obj obj\i386\init.obj obj\i386\serialui.obj
OBJS   = $(OBJS) obj\i386\err.obj obj\i386\comm.obj obj\i386\cstrings.obj obj\i386\port.obj
OBJS   = $(OBJS) obj\i386\strings.obj obj\i386\profile.obj

LFLAGS = /ALIGN:16 /MAP /NOE /NOD
LIBS   = LIBW SHELL MNOCRTDW MLIBCEW COMMCTRL
CFLAGS = -c -AMw -G3Dcs -W3 -Oxs -Zldp $(DDEBUG)
INCL   = suiprv.h mcx16.h cstrings.h comm.h err.h    
INCLUDE = $(INC16);$(INC32);$(INCLUDE)

default: obj\i386\serialui.dll

pch.c:
        echo #include "suiprv.h" > pch.c

obj\i386\pch.obj: pch.c $(INCL)
		@echo cl -Fo$@ pch.c
        $(CL16) $(CFLAGS) -Ycsuiprv.h -NT _TEXT -Fo$@ pch.c

obj\i386\libinit.obj: libinit.asm
		@echo ml -Fo$@ libinit.asm
		@ml -Zm -Zd -c -W2 $(DDEBUG) -Fo$@ libinit.asm > NUL
        
obj\i386\init.obj: init.c suiprv.pch $(INCL)
		@echo cl -Fo$@ init.c
		$(CL16) $(CFLAGS) -Yusuiprv.h -NT INIT_TEXT -Fo$@ init.c

obj\i386\serialui.obj: serialui.c suiprv.pch $(INCL)
		@echo cl -Fo$@ serialui.c
        $(CL16) $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT -Fo$@ serialui.c

obj\i386\err.obj: err.c suiprv.pch $(INCL)
		@echo cl -Fo$@ err.c
        $(CL16) $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT -Fo$@ err.c

obj\i386\comm.obj: comm.c suiprv.pch $(INCL)
		@echo cl -Fo$@ comm.c
        $(CL16) $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT -Fo$@ comm.c

obj\i386\cstrings.obj: cstrings.c suiprv.pch $(INCL)
		@echo cl -Fo$@ cstrings.c
        $(CL16) $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT -Fo$@ cstrings.c

#/////BP added following two

obj\i386\profile.obj: profile.c suiprv.pch $(INCL)
		@echo cl -Fo$@ profile.c
        $(CL16) $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT -Fo$@ profile.c

obj\i386\strings.obj: strings.c suiprv.pch $(INCL)
		@echo cl -Fo$@ strings.c
        $(CL16) $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT -Fo$@ strings.c

#/////

obj\i386\port.obj: port.c suiprv.pch $(INCL)
		@echo cl -Fo$@ port.c
        $(CL16) $(CFLAGS) -Yusuiprv.h -NT COMMON_TEXT -Fo$@ port.c

obj\i386\$(NAME).res: $(NAME).rc
        $(RC16B) -r -fo $@ $(NAME).rc

obj\i386\$(NAME).dll:  $(OBJS) $*.res $(NAME).def 
!ifndef PASS0ONLY
		@echo link -OUT:$@
		set LIB=$(LIB16);$(C16_ROOT)\lib
        $(LINK16) $(LFLAGS) @<<$*.lnk
$(OBJS)
$*.dll
$*.map
$(LIBS)
$(NAME).def
<<
        mapsym -o $*.sym $*.map
        $(RC16B) -fe $*.dll $*.res
!endif

clean:
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist obj\i386\*.res del obj\i386\*.res
        -@if exist obj\i386\*.dll del obj\i386\*.dll
        -@if exist *.pch del *.pch
        -@if exist pch.c del pch.c
        -@if exist *.bak del *.bak
