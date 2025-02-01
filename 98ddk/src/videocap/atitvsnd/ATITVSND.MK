#############################################################################
#
#       Copyright (C) Microsoft Corporation 1995
#       All Rights Reserved.
#                                                                          
#       Sample makefile for the WDM stream class mini driver
#
##########################################################################

#BSCMAKE         = 1
#BSCTARGETS      = ATITVSND.BSC 
#WINICE_PATH     = c:\Win95ICE

ROOT            = ..\..\..\..\..\..
MINIPORT        = atitvsnd
SRCDIR          = ..
ALTSRCDIR       = ..\..\..\ATIShare
WANT_LATEST     = TRUE
WANT_WDMDDK     = TRUE
DEPENDNAME      = ..\depend.mk
DESCRIPTION     = TVAudio WDM MiniDriver
VERDIRLIST      = maxdebug debug retail
LIBS            = $(ROOT)\wdm10\ddk\lib\i386\stream.lib \
                  $(ROOT)\wdm10\ddk\lib\i386\ksguid.lib
RESNAME         = atitvsnd

NOVXDVERSION    = TRUE
CFLAGS          = -DMSC $(CFLAGS)

!IF "$(VERDIR)" != "debug"
CFLAGS	 = $(CFLAGS) -Oi
!endif

OBJS =	atitvsnd.obj wdmtvsnd.obj tsndprop.obj tsndhdw.obj \
		aticonfg.obj i2script.obj pinmedia.obj registry.obj
 
!include $(ROOT)\dev\master.mk

INCLUDE=$(ROOT)\wdm10\ddk\inc;$(ROOT)\wdm10\capture\inc;$(INCLUDE)
INCLUDE=$(ROOT)\wdm10\capture\mini\ATIShare;$(INCLUDE)

