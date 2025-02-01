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

#
#   Makefile for VPOSTD.VXD, POSTHOST.DLL, POSTAPP.EXE, and POSTDOS.EXE
#
#   This is a sample demonstrating how to PostMessage from a VxD.
#

NAME = VPOSTD

# supply the tools location information below

#LINK16	= link
#CL16	= cl
#RC16	= rc

#
# for POSTDOS.EXE build
#

!IFDEF DEBUG_POSTDOS

DAS =   ml -c -Zm -Zi
DCC =   $(CL16) -c -Zi
DLN =   $(LINK16) /CO
DZZ =   POSTDOS: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

DAS =   ml -c -Zm 
DCC =   $(CL16) -c -Fo$@
DLN =   $(LINK16)
DZZ =   POSTDOS: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

DSRCS   =   postdos.c
DOBJS   =   obj\i386\postdos.obj

#
# for POSTAPP.EXE build
#

!IFDEF DEBUG_POSTAPP

AAS =   ml -DIS_32 -W2 -c -Cx -Zm -Zi -DMASM6 -DDEBLEVEL=1 -DDEBUG
ACC =   $(CL16) -c -DDEBUG -AM -G2sw -Owd -W3 -Ziped -Fo$*.obj
ALN =   $(LINK16) /NOD/NOE/A:16/CO 
ARC =   $(RC16B) -v -DDEBUG
AZZ =   POSTAPP: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

AAS =   ml -DIS_32 -W2 -c -Cx -Zm -DMASM6 -DDEBLEVEL=0 
ACC =   $(CL16) -c -DRELEASE -AM -G2sw -Ows -W3 -Zpe -Fo$*.obj
ALN =   $(LINK16) /NOD/NOE/A:16 
ARC =   $(RC16B) -v -DRELEASE
AZZ =   POSTAPP: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

AMISC   =   postapp.def     postapp.lnk
AINCS   =   postapp.h
ASRCS   =   postapp.c
AOBJS   =   obj\i386\postapp.obj


#
# for POSTHOST.DLL build
#

!IFDEF DEBUG_POSTHOST

LAS =   ml -c -Zm -Zi
LCC =   $(CL16) -c -DDEBUG -Alnw -G2sw -Owd -W3 -Ziped -Fo$*.obj
LLN =   $(LINK16) /NOD/NOE/A:16/CO 
LRC =   $(RC16B) -v -DDEBUG
LZZ =   POSTHOST: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

LAS =   ml -c -Zm 
LCC =   $(CL16) -c -DRELEASE -Alnw -G2sw -Ows -W3 -Zpe -Fo$*.obj
LLN =   $(LINK16) /NOD/NOE/A:16 
LRC =   $(RC16B) -v -DRELEASE
LZZ =   POSTHOST: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

LMISC   =   posthost.def    posthost.lnk

LINCS   =   posthost.h
LSRCS   =   posthost.c      libentry.asm
LOBJS   =   obj\i386\posthost.obj    obj\i386\libentry.obj

#
# for VPOSTD.VXD build
#

!IFDEF DEBUG_VPOSTD

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6 -DDEBUG -I$(INC32)
ASMENV = ML
LFLAGS = /VXD /NOD
VRC =   rc -v -DDEBUG
VZZ =   VPOSTD: ***** DEBUG ***** DEBUG ***** DEBUG ***** DEBUG *****

!ELSE

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6 -DRELEASE -I$(INC32)
ASMENV = ML
LFLAGS = /VXD /NOD
VRC =   rc -v -DRELEASE
VZZ =   VPOSTD: **** RELEASE *** RELEASE *** RELEASE *** RELEASE ****

!ENDIF

VMISC   =   vpostd.def  obj\i386\vpostd.lnk
VINCS   =   vpostd.inc
VSRCS   =   vpostd.asm
VOBJS   =   obj\i386\vpostd.obj

#
# now the rules
#

all: obj\i386\$(NAME).vxd obj\i386\postapp.exe obj\i386\postdos.exe

# VPOSTD.vxd

obj\i386\$(NAME).vxd: $(NAME).def $(VOBJS)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<obj\i386\$(NAME).lnk
$(LFLAGS) 
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(NAME).vxd
/MAP:obj\i386\$(NAME).map
/DEF:$(NAME).def
$(VOBJS)
<<
		mapsym -s -o obj\i386\$(NAME).sym obj\i386\$(NAME).map
!endif

obj\i386\vpostd.obj: vpostd.asm  vpostd.inc
		set $(ASMENV)=$(AFLAGS)
		$(ASM) -Fo$*.obj vpostd.asm
		set $(ASMENV)=

vpostd.lst: vpostd.asm  vpostd.inc
                $(VAS) $*;

# POSTHOST.DLL


obj\i386\posthost.dll: $(LOBJS) posthost.def
!ifndef PASS0ONLY	
				@echo link -OUT:$@
                $(LLN) @<<posthost.lnk
obj\i386\posthost.obj obj\i386\libentry.obj
obj\i386\posthost.dll
obj\i386\posthost.map
$(C16_ROOT)\lib\mdllcew $(LIB16)\libw
posthost.def;
<<
                $(C16_ROOT)\bin\implib obj\i386\posthost.lib posthost.def
                $(LRC) obj\i386\posthost.dll
                @echo $(LZZ)
                @echo $(LZZ)
                @echo $(LZZ)
!endif

obj\i386\posthost.obj:   posthost.c posthost.h
				@set INCLUDE=$(INC16)
				@echo.
				@echo cl -Fo$@ posthost.c
                $(LCC) posthost.c

obj\i386\libentry.obj:   libentry.asm
                $(LAS) -Fo$*.obj libentry.asm

#
# POSTAPP.EXE
#

obj\i386\postapp.exe :: $(AOBJS) postapp.def obj\i386\postapp.res obj\i386\posthost.dll
!ifndef PASS0ONLY	
				@echo link -OUT:$@
                $(ALN) @<<postapp.lnk
obj\i386\postapp.obj
obj\i386\postapp.exe
obj\i386\postapp.map
obj\i386\posthost.lib $(C16_ROOT)\lib\mlibcew $(LIB16)\libw
postapp.def;
<<
                $(ARC) obj\i386\postapp.res obj\i386\postapp.exe
                @echo $(AZZ)
                @echo $(AZZ)
                @echo $(AZZ)
!endif

obj\i386\postapp.exe ::   obj\i386\postapp.res
!ifndef PASS0ONLY	
                $(ARC) obj\i386\postapp.res obj\i386\postapp.exe
                @echo $(AZZ)
                @echo $(AZZ)
                @echo $(AZZ)
!endif

obj\i386\postapp.obj :   postapp.c postapp.h posthost.h
				@set INCLUDE=$(INC16)
				@echo cl -Fo$@ postapp.c
                $(ACC) postapp.c

obj\i386\postapp.res :   postapp.rc postapp.h postapp.ico
                $(ARC) -i $(INC16) -fo$@ -r postapp.rc


#
# POSTDOS.EXE
#

obj\i386\postdos.exe : $(DOBJS)
!ifndef PASS0ONLY	
				@echo link -OUT:$@
				@set LIB=$(LIB16);$(C16_ROOT)\lib
				cd obj\i386
                $(DLN) postdos;
				cd ..\..
                @echo $(DZZ)
                @echo $(DZZ)
                @echo $(DZZ)
!endif

obj\i386\postdos.obj :	postdos.c
						@set INCLUDE=$(INC16);$(INC32);$(C16_ROOT)\include
						@echo cl -Foobj\i386\postdos.obj postdos.c
                        $(DCC) postdos.c

clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.exe del obj\i386\*.exe
    -@if exist obj\i386\*.dll del obj\i386\*.dll
    -@if exist obj\i386\*.res del obj\i386\*.res
    -@if exist *.bak del *.bak
