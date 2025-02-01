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

BUILD_TYPE=cplx
!INCLUDE $(BASEDIR)\inc\master.mk

# Makefile for CPLX - Control Panel extensions for Mouse/Display

NAME     = cplx
CFLAGS   = -W3 -Zl -c -Oxs -G3fyz -DBUILDDLL -DWIN32 -D_X86_ -DIS_32 -bzalign
RCFLAGS  = -DWIN32
L32FLAGS = -merge:.bss=.data -subsystem:windows,4.0 -entry:LibMain@12 
L32FLAGS = $(L32FLAGS) -align:0x1000 -nodefaultlib -merge:.rdata=.text

!IFDEF DEBUG
CFLAGS   = $(CFLAGS) -DDEBLEVEL=1 -DDEBUG
RCFLAGS  = $(RCFLAGS) -DDEBUG
L32FLAGS = $(L32FLAGS) -debug:full -debugtype:cv
!ELSE
CFLAGS   = $(CFLAGS) -DDEBLEVEL=0
!ENDIF

INCLUDE = $(INCLUDE);$(INC32);$(INC16)

.c{obj\i386}.obj:
       set CL=$(CFLAGS)
       cl -Fo$*.obj $<

.rc{obj\i386}.res:
       rc $(RCFLAGS) -r -fo$@ $(@B).rc

obj\i386\cplx.dll: obj\i386\cplx.obj obj\i386\defclsf.obj cplx.def obj\i386\cplx.res
!ifndef PASS0ONLY
		@echo link -OUT:$@
		link @<<obj\i386\$(NAME).lnk
$(L32FLAGS)
-out:obj\i386\$(NAME).dll -map:obj\i386\$(NAME).map -def:$(NAME).def -dll -base:0x410000
obj\i386\cplx.obj obj\i386\defclsf.obj kernel32.lib user32.lib gdi32.lib advapi32.lib 
comctl32.lib ole32.lib shell32.lib obj\i386\$(NAME).res
<<
	mapsym -s -o obj\i386\$(NAME).sym obj\i386\$(NAME).map
!endif


obj\i386\$(NAME).obj: $(NAME).c

obj\i386\defclsf.obj: defclsf.c

obj\i386\$(NAME).res: $(NAME).rc

clean:
	-@if exist obj\i386\*.res del obj\i386\*.res
	-@if exist obj\i386\*.dll del obj\i386\*.dll
	-@if exist obj\i386\*.obj del obj\i386\*.obj
	-@if exist obj\i386\*.exp del obj\i386\*.exp
	-@if exist obj\i386\*.lib del obj\i386\*.lib
	-@if exist obj\i386\*.sym del obj\i386\*.sym
	-@if exist obj\i386\*.map del obj\i386\*.map
	-@if exist *.bak del *.bak
