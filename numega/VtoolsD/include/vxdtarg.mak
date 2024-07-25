# vxdtarg.mak
#
# Master makefile for VtoolsD examples
# Copyright (c) 1994 Compuware Corporation

#########################################################################
#		Error checking:
#

!ifndef VTOOLSDISPRESENT
!	error VTOOLSD.MAK must be included before VXDTARG.MAK
!endif

!ifndef DEVICENAME
!	error DEVICENAME must be defined before including VXDTARG.MAK
!endif

!ifndef OBJECTS
!	error OBJECTS must be defined before including VXDTARG.MAK
!endif

!ifndef FRAMEWORK
!	error FRAMEWORK must be defined before including VXDTARG.MAK
!endif

!if "$(FRAMEWORK)" != "C"
!if "$(FRAMEWORK)" != "CPP"
!	error FRAMEWORK must be C or CPP
!endif
!endif


#########################################################################
#
#	The VxD output file is dependent on the object files and the 
#	.DEF file.  The .DEF file is generated automatically by NMAKE.

VERSUFFIX	= 4

! if "$(TARGET)" == "WIN95" || "$(TARGET)" == "WIN98"
VERSUFFIX	= 4
CLSFWSUFFIX	= 4
!endif

! if "$(TARGET)" == "WIN31"
CLSFWSUFFIX	= 3
! endif

! if "$(TARGET)" == "WFW311"
CLSFWSUFFIX	= 3
! endif
#endif

! if $(DEBUG) != 0
DEBUGSUFFIX	= D
! else
DEBUGSUFFIX	=
! endif

! if "$(FRAMEWORK)" == "C"
FWLIB		= $(VTOOLSD)\lib\cf$(LIBCOMP)$(DEBUGSUFFIX).lib
DAALIB		=
NTLIB		=
! endif

! if "$(FRAMEWORK)" == "CPP"
FWLIB		= $(VTOOLSD)\lib\cl$(LIBCOMP)$(CLSFWSUFFIX)$(DEBUGSUFFIX).lib
DAALIB		= $(VTOOLSD)\lib\daa$(LIBCOMP)$(DEBUGSUFFIX).lib
NTLIB		= $(VTOOLSD)\lib\nt$(LIBCOMP)$(DEBUGSUFFIX).lib
! endif

CRTLIB		= $(VTOOLSD)\lib\rt$(LIBCOMP)$(DEBUGSUFFIX).lib

! if "$(TARGET)" == "WIN95"
NDISVER=95
!endif

! if "$(TARGET)" == "WIN98"
NDISVER=98
!endif

! if "$(TARGET)" == "WIN31"  || "$(TARGET)" == "WFW311"
NDISVER=31
! endif

NDISLIB		= $(VTOOLSD)\lib\nd$(LIBCOMP)$(NDISVER)$(DEBUGSUFFIX).lib

# no debug lib for wraps
W0		= $(VTOOLSD)\lib\wr0$(WRAPCOMP).lib
W1		= $(VTOOLSD)\lib\wr1$(WRAPCOMP).lib
W2		= $(VTOOLSD)\lib\wr2$(WRAPCOMP).lib
W3		= $(VTOOLSD)\lib\wr3$(WRAPCOMP).lib

! include $(INCMAKE_2)

################################################################
#
#		Delete derivative files for clean builds
#

clean:
	del $(OBJPATHSLASH)*.ob?
	del $(OBJPATHSLASH)*.exe
	del $(OBJPATHSLASH)*.map
	del $(OBJPATHSLASH)*.sym
	del $(OBJPATHSLASH)$(DEVICENAME).def
	del $(OBJPATHSLASH)*.vxd
	del $(OBJPATHSLASH)*.386
	del $(OBJPATHSLASH)*.exp
	del $(OBJPATHSLASH)*.lib
	del $(OBJPATHSLASH)*.pel
	del $(OBJPATHSLASH)*.rsp
	del $(OBJPATHSLASH)*.res
	del $(OBJPATHSLASH)*.pdb
	del $(OBJPATHSLASH)*.pch
	del $(OBJPATHSLASH)*.sbr
	del $(OBJPATHSLASH)*.bsc
