#############################################################################
#
#	Makefile for sample socket services device driver
#                                                                          
#############################################################################
BUILD_TYPE=plugplay
!INCLUDE $(BASEDIR)\inc\master.mk

obj\i386\sampless.vxd: obj\i386\sampless.obj
!ifndef PASS0ONLY
	@echo link -OUT:$@
	link /VXD /NOD /DEF:sampless.def /OUT:obj\i386\sampless.vxd /MAP:obj\i386\sampless.map obj\i386\sampless.obj
	mapsym	-s -o obj\i386\SAMPLESS.sym obj\i386\SAMPLESS.map
!endif

obj\i386\sampless.obj: sampless.asm ssdefs.inc sslocal.inc
	ml -coff -DBLD_COFF -I$(INC32) -DIS_32 -W2 -Zd -c -Cx -DMASM6 -Sg -Fo$@ sampless.asm

clean:
        -@if exist obj\i386\*.obj del obj\i386\*.obj
        -@if exist obj\i386\*.exp del obj\i386\*.exp
        -@if exist obj\i386\*.map del obj\i386\*.map
        -@if exist obj\i386\*.sym del obj\i386\*.sym
        -@if exist obj\i386\*.vxd del obj\i386\*.vxd
        -@if exist obj\i386\*.lib del obj\i386\*.lib
        -@if exist *.bak del *.bak
