#############################################################################
#
#	Copyright (C) Microsoft Corporation 1996
#	All Rights Reserved.
#
#	Makefile for Sample PCMCIA Socket Services Driver
#
#############################################################################
BUILD_TYPE=plugplay
!INCLUDE $(BASEDIR)\inc\master.mk

INCLUDE = .;$(INCLUDE)

# Makefile for SAMPLE
DEVICE	=SAMPLESS
OBJS	=obj\i386\pcicss.obj obj\i386\pnp.obj obj\i386\ssmain.obj
LIBS	=$(BASEDIR)\lib\i386\free\vxdwraps.clb
DESC	=Sample PCMCIA Socket Services Driver

KEEPFLAG=NOKEEP
CFLAGS	=-DWIN40COMPAT -nologo -Zdp -Gs -c -DIS_32 -bzalign -Zl -Oy -Ogaisb1 -W3 -WX
AFLAGS	=-DWIN40COMPAT -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6 -Sg -coff -DBLD_COFF
LFLAGS	=-nologo -nodefaultlib -pdb:none -align:0x200 -ignore:4039,4078 
HFLAGS	=-fwc -s $(BASEDIR)\inc\win98\basedef.h

!ifdef DEBUG
CFLAGS	=$(CFLAGS) -DDEBUG
AFLAGS	=$(AFLAGS) -DDEBUG
!endif

.SUFFIXES: .asm .c .h .inc .def .lnk
.SUFFIXES: .obj .lst .map
.SUFFIXES: .vxd .exe .com .sym

.c{obj\i386}.obj:
	cl $(CFLAGS) /Fo$* $** 

.c{obj\i386}.lst:
	cl $(CFLAGS) /Fo$* -Fc$@ $** 

.asm{obj\i386}.obj:
	ml $(AFLAGS) /Fo$* $**

.h{obj\i386}.inc:
	h2inc @<<$(@B).h2i
$(HFLAGS) $< -o $**
<<$(KEEPFLAG)

all:	$(DEVICE).vxd

$(DEVICE).vxd: $(OBJS)
!ifndef PASS0ONLY
	@echo link -OUT:$@
	link @<<obj\i386\$(DEVICE).lnk /def:<<$(DEVICE).def
$(LFLAGS) /vxd
/out:obj\i386\$(DEVICE).vxd /map:obj\i386\$(DEVICE).map
$(OBJS) $(LIBS)
/def:$(DEVICE).def
<<$(KEEPFLAG)
VXD $(DEVICE) DYNAMIC
DESCRIPTION '$(DESC)'

SEGMENTS
	_LTEXT	CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	_LDATA	CLASS 'LCODE'   PRELOAD NONDISCARDABLE
	PNP	CLASS 'PNPCODE' PRELOAD NONDISCARDABLE
	_PDATA	CLASS 'PDATA'   NONDISCARDABLE SHARED
!ifdef DEBUG
	_DBOCODE CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
	_DBODATA CLASS 'DBOCODE' PRELOAD NONDISCARDABLE CONFORMING
!endif

EXPORTS
	$(DEVICE)_DDB @1
<<$(KEEPFLAG)
	mapsym -s -o $(DEVICE).sym obj\i386\$(DEVICE).map
!endif

clean:
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist *.bak del *.bak

depend:
	copy makefile makefile.old
	sed "/^# Dependencies follow/,/^# see depend: above/D" makefile.old > makefile
	echo # Dependencies follow >> makefile
	includes -sobj -I$(INCLUDE) *.c >> makefile
	includes -sobj -I$(INCLUDE) *.asm >> makefile
	echo # IF YOU PUT STUFF HERE IT WILL GET BLASTED >> makefile
	echo # see depend: above >> makefile

# DO NOT DELETE THE FOLLOWING LINE
# Dependencies follow 
pcicss.obj pcicss.lst: pcicss.c pcicss.h

pnp.obj pnp.lst: pnp.c pcicss.h

ssmain.obj ssmain.lst: ssmain.asm pcicss.inc

# IF YOU PUT STUFF HERE IT WILL GET BLASTED 
# see depend: above 
