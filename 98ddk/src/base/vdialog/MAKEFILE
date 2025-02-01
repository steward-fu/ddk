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


# sample make file for building vdialog virtual device for Windows

NAME = VDIALOG

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for MASM 6 Assembler

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6 -I$(INC32) $(DDEBUG)
ASMENV = ML
LFLAGS = /VXD /NOD


all : obj\i386\$(NAME).vxd obj\i386\dosacc.exe obj\i386\winacc.exe


obj\i386\vdialog.obj: vdialog.asm
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj vdialog.asm
	set $(ASMENV)=

OBJS =  obj\i386\vdialog.obj 

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

# NOTE: this make file uses 16-bit tools!

# supply the location of a 16-bit linker, compiler, and resource compiler

#LINK16 = link  
#CL16   = cl
#RC16   = rc 

# Standard Windows make file.  The make utility compares the
# creation date of the file to the left of the colon with the file(s)
# to the right of the colon.  If the file(s) on the right are newer
# then the file on the left, Make will execute all of the command lines
# following this line that are indented by at least one tab or space.
# Any valid MS-DOS command line may be used.

# This line allows NMAKE to work as well

# Update the resource if necessary

obj\i386\winacc.res: winacc.rc winacc.h
    @set INCLUDE=$(INC16);$(DDKROOT)\media\inc
    $(RC16) -r -foobj\i386\winacc.res winacc.rc


# Update the object file if necessary

obj\i386\winacc.obj: winacc.c winacc.h
    @set INCLUDE=$(INC16);$(DDKROOT)\media\inc
	@echo cl -Fo$* winacc.c
    $(CL16) -c -AS -Gsw -Oas -W3 -Zpei -Fo$* winacc.c

# Update the executable file if necessary, and if so, add the resource back in.

obj\i386\winacc.exe: obj\i386\winacc.obj winacc.def obj\i386\winacc.res
!ifndef PASS0ONLY
	@echo link -OUT:$@
	@cd obj\i386
    $(LINK16) /CO/NOD winacc,,, $(LIB16)\libw $(C16_ROOT)\lib\slibcew, ..\..\winacc.def
    $(RC16) winacc.res
	@cd ..\..
!endif

# If the .res file is new and the .exe file is not, update the resource.
# Note that the .rc file can be updated without having to either 
# compile or link the file.

#winacc.exe:: winacc.res
#   $(RC16) winacc.res

obj\i386\dosacc.obj: dosacc.asm vdevice.inc
    ml -c -Cx -Foobj\i386\dosacc.obj dosacc.asm

obj\i386\dosacc.exe: obj\i386\dosacc.obj
!ifndef PASS0ONLY
	@echo link -OUT:$@
	@cd obj\i386
    $(LINK16) dosacc.obj;
	@cd ..\..
!endif


clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.exe del obj\i386\*.exe
    -@if exist obj\i386\*.res del obj\i386\*.res
    -@if exist *.pdb del *.pdb
    -@if exist *.bak del *.bak
