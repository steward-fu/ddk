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


#############################################################################
#                                                                          
#    Makefile for S3V Mini-VDD Port
#
##########################################################################

DEVICE   = S3VSMP
DDB_NAME = S3MINI_DDB
OBJS     = obj\i386\dmsics64.obj obj\i386\kmvt.obj obj\i386\s3mini.obj obj\i386\s3mobile.obj

AFLAGS   = $(AFLAGS) -DSTANDARD_CCALL -DIS_32
AFLAGS   = $(AFLAGS) -coff -DBLD_COFF -nologo -W2 -Zd -c -Cx -DMASM6 -Sg
AFLAGS   = $(AFLAGS) -DVGA -DVGA31 -DMINIVDD=1 -DWIN40COMPAT=1 -DDX5 -I$(INC32)



ASMENV   = ML
LFLAGS   = /VXD /NOD

.asm{obj\i386}.obj:
        set ml=$(AFLAGS)
        ml -Fo$@ $< 

obj\i386\$(DEVICE).vxd: $(OBJS)
!ifndef PASS0ONLY
		@echo link -OUT:$@
        link $(LFLAGS) @<<obj\i386\$(DEVICE).lnk
$(OBJS)
-ignore:4078 -ignore:4039
/OUT:obj\i386\$(DEVICE).vxd
/MAP:obj\i386\$(DEVICE).map
/DEF:$(DEVICE).def
<<
        mapsym -s -o obj\i386\$(DEVICE).sym obj\i386\$(DEVICE).map
!endif


clean:
    -@if exist obj\i386\*.obj del obj\i386\*.obj
    -@if exist obj\i386\*.vxd del obj\i386\*.vxd
    -@if exist obj\i386\*.lib del obj\i386\*.lib
    -@if exist obj\i386\*.map del obj\i386\*.map
    -@if exist obj\i386\*.sym del obj\i386\*.sym
    -@if exist obj\i386\*.exp del obj\i386\*.exp
    -@if exist *.bak del *.bak
