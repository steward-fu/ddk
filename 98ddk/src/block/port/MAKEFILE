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

BUILD_TYPE=block
!INCLUDE $(BASEDIR)\inc\master.mk

DEVICE = PORT
OBJS   = obj\i386\port.obj obj\i386\portaer.obj obj\i386\portreq.obj obj\i386\portisr.obj

AFLAGS= -DDEBUG_TRACE=1

######### Definitions for the debug level #########

!ifdef DEBUG
DDEBUG =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG =-DDEBLEVEL=0
!endif

########## Definitions for the Assembler ##########

ASM     = ml
AFLAGS  = -coff -DBLD_COFF $(AFLAGS) -DIS_32 -nologo -W3 -Zd -c -Cx -DMASM6
AFLAGS  = $(AFLAGS) -DINITLOG $(DDEBUG) -I$(DDKROOT)\src\block\inc -I$(INC32)
ASMENV  = ML

########## Definitions for linker #############

LFLAGS  = /VXD /NOD

########## Base inference rules ###################

#       MASM 6 only inference rules

.asm{obj\i386}.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

############### VxD device section ####################

obj\i386\$(DEVICE).sym: obj\i386\$(DEVICE).map
	mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map > nul

obj\i386\$(DEVICE).map: obj\i386\$(DEVICE).pdr

obj\i386\$(DEVICE).pdr : $(OBJS)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link @<<obj\i386\$(DEVICE).lnk
$(LFLAGS) 
/OUT:obj\i386\$(DEVICE).pdr
/MAP:obj\i386\$(DEVICE).map
$(OBJS)
<<
!endif

clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.pdr del obj\i386\*.pdr
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist *.bak del *.bak
