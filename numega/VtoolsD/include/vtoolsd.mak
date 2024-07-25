# vtoolsd.mak
#
# Master makefile for VtoolsD examples
# Copyright (c) 1994 Compuware Corporation
#
#########################################################################
# NMAKE Symbols and Environment Variables
#
#########################################################################
#
#   The following symbol should be defined in the environment:
#
#     VTOOLSD	Directory where VtoolsD is installed
#
#   Compuware recommends defining VTOOLSD in your autoexec.bat file
#

! ifdef VTOOLSDISPRESENT
!	error VTOOLSD.MAK included twice in makefile
! endif

VTOOLSDISPRESENT = 1

! include $(VTOOLSD)\INCLUDE\USER.MAK

INCLUDEVXD	= $(VTOOLSD)\INCLUDE;$(VTOOLSD)\INCLUDE\DAA;$(VTOOLSD)\SOURCE\DAALIB;$(INCVXD2)
SEGFILE		= $(VTOOLSD)\include\default.seg
SEGALIAS	= $(VTOOLSD)\bin\segalias

.SUFFIXES: .c .cpp .asm .obj 

#########################################################################
#
#		Allow lower case TARGET definitions
#
#########################################################################

! ifndef TARGET
TARGET	=	WIN95
! endif

! if "$(TARGET)" ==  "win98"
TARGET		= WIN98
! endif

! if "$(TARGET)" ==  "WIN41"
TARGET		= WIN98
! endif

! if "$(TARGET)" ==  "win95"
TARGET		= WIN95
! endif

! if "$(TARGET)" ==  "WIN40"
TARGET		= WIN95
! endif

! if "$(TARGET)" ==  "win40"
TARGET		= WIN95
! endif

! if "$(TARGET)" ==  "win31"
TARGET		= WIN31
! endif

! if "$(TARGET)" ==  "wfw311"
TARGET		= WFW311
! endif


#########################################################################
#
#		Make DEBUG, DYNAMIC, and NDIS canonical (0 or 1)
#
#########################################################################

! ifndef DEBUG
DEBUG = 0
! else
!   if "$(DEBUG)" == "no"
DEBUG = NO
!   endif
!   if "$(DEBUG)" == "NO"
DEBUG = 0
!   else
!     if "$(DEBUG)" != "0"
DEBUG = 1
!     endif
!   endif
! endif


! ifndef DYNAMIC
DYNAMIC = 0
! endif

! if "$(DYNAMIC)" != "0"
DYNAMIC = 1
DYNASWITCH	= DYNAMIC
! else
DYNASWITCH	=
! endif

! if $(DYNAMIC) == 1
! if "TARGET"== "WIN31"
!    error Dynamic VxDs not available for Windows 3.1
! endif
! endif

! ifndef NDIS
NDIS = 0
! endif
! if "$(NDIS)" != "0"
NDIS = 1
! endif
! if "$(TARGET)" == "WIN31"
NDIS = 0
! endif

#########################################################################
#
#		If defined, turn SOURCES into OBJECTS
#
#########################################################################

! ifdef SOURCES

! if "$(COMPILER)" == "BCB3" || "$(COMPILER)" == "BC5"
! error Borland compiler does not allow SOURCES macro
! endif

! 	ifdef OBJECTS
!	error OBJECTS cannot be defined if SOURCES also defined
!	endif

# turn "w.asm x.c y.cpp z.cpp" into "w.obj x.c y.cpp z.cpp"
OBJECTS	= $(SOURCES:.ASM=.obj)
OBJECTS	= $(SOURCES:.Asm=.obj)
OBJECTS	= $(SOURCES:.asm=.obj)
# turn prev line into "w.obj x.c y.obj z.obj"
OBJECTS	= $(OBJECTS:.CPP=.obj)	# make sure this is done before .c!
OBJECTS	= $(OBJECTS:.Cpp=.obj)	# make sure this is done before .c!
OBJECTS	= $(OBJECTS:.cpp=.obj)	# make sure this is done before .c!
# turn prev line into "w.obj x.obj y.obj z.obj"
OBJECTS	= $(OBJECTS:.C=.obj)
OBJECTS	= $(OBJECTS:.c=.obj)

! endif # SOURCES

#########################################################################
#
#		If MAKE_DEBUG_RELEASE_DIRS defined include vdbgrel.mak.
#
#  Note: Since Borland MAKE cannot handle macro redefinition, we need to
#		specify a new macro for OBJECTS (so we can manipulate the paths
#		in VDBGREL.MAK.  Depending on MAKE_DEBUG_RELEASE_DIRS, FINALOBJECTS
#		is defined here or in VDBGREL.MAK.
#
#########################################################################

! ifdef MAKE_DEBUG_RELEASE_DIRS
! 	include $(VTOOLSD)\INCLUDE\VDBGREL.MAK
! else
FINALOBJECTS = $(OBJECTS)
! endif

#########################################################################
#
#		Setup default path macros OBJPATH and OUTPATH
#
#########################################################################

! ifdef OBJPATH
OBJPATHSLASH = $(OBJPATH)^\
! endif

! ifndef OUTPATH
! 	ifdef OBJPATH
OUTPATH = $(OBJPATH)
! 	endif
! endif

! ifdef OUTPATH
OUTPATHSLASH = $(OUTPATH)^\
! endif

#########################################################################
#
#		Select compiler
#
#########################################################################

# Microsoft Visual C++ 5.0 or later
#
! if "$(COMPILER)" == "MS5" || "$(COMPILER)" == "MS6"
INCMAKE_1 = $(VTOOLSD)\INCLUDE\MS.MAK
INCMAKE_2 = $(VTOOLSD)\INCLUDE\MSVXD.MAK
LIBCOMP	  = MS
WRAPCOMP  = MS
NDISCOMP  = MS
! endif

# Borland C++ Builder 3 or later
#
! if "$(COMPILER)" == "BCB3"
INCMAKE_1 = $(VTOOLSD)\INCLUDE\BC.MAK
INCMAKE_2 = $(VTOOLSD)\INCLUDE\BCVXD.MAK
LIBCOMP	  = BOR
WRAPCOMP  = BOR
NDISCOMP  = BOR
! endif


! ifndef INCMAKE_1
!	error COMPILER must be defined to MS5, MS6 or BCB3
! endif

! include $(INCMAKE_1)


#########################################################################
#
#		Select target devicetype
#
#########################################################################

!ifndef DEVICETYPE
DEVICETYPE	= VXD
!endif
__DEVICE_TYPE_DEFINED	= 0

#########################################################################

!   if "$(DEVICETYPE)" == "PORTDRIVER"
TARGNAME	= $(OUTPATHSLASH)$(DEVICENAME).PDR
__DEVICE_TYPE_DEFINED	= 1
!   endif
!   if "$(DEVICETYPE)" == "VXD"
!		if "$(TARGET)" == "WIN31" || "$(TARGET)" == "WFW311"
TARGNAME	= $(OUTPATHSLASH)$(DEVICENAME).386
!		else
TARGNAME	= $(OUTPATHSLASH)$(DEVICENAME).VXD
!		endif
__DEVICE_TYPE_DEFINED	= 1
!   endif

!   if "$(DEVICETYPE)" == "LIB"
TARGNAME	= $(OUTPATHSLASH)$(DEVICENAME).LIB
__DEVICE_TYPE_DEFINED	= 1
!   endif

! if "$(__DEVICE_TYPE_DEFINED)" == "0"
!	error DEVICETYPE must be VXD or PORTDRIVER
! endif


#########################################################################
#
#		Select debugger (symbol mapper)
#
#########################################################################


!ifndef MSYM
MSYM = $(VTOOLSD)\bin\notfound MSYM.EXE "NuMega's msym.exe utility"
! endif

!ifndef DBG2MAP
DBG2MAP = $(VTOOLSD)\bin\notfound DBG2MAP.EXE "NuMega's dbg2map.exe utility"
! endif

!ifndef NMSYM
NMSYM = $(VTOOLSD)\bin\notfound NMSYM.EXE "NuMega's nmsym.exe utility"
! endif

!if "$(DEBUGGER)" == "WDEB386"
MAKESYM		= $(VTOOLSD)\bin\mapsym $(MAPNAME)
!endif

!if "$(DEBUGGER)" == "SOFTICE2"
MAKESYM		= $(MSYM) $(MAPNAME)
!endif

!if "$(DEBUGGER)" == "SOFTICE3"
MAKESYM		= $(NMSYM) $(TARGNAME) /translate:source,package,always
!endif


!if "$(DEBUGGER)" == "SOFTICE"
!	error DEBUGGER must be defined to SOFTICE3 (for SoftICE 3.0) or SOFTICE2 (for SoftICE 2.0).
!endif
