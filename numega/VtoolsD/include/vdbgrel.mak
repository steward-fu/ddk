# vdbgrel.mak
#
# Copyright (c) 1998 Compuware Corporation
#
# Makefile to facilitate Debug and Release builds into separate directories
# in the style of MSVC
#

! ifdef OBJPATH
!	error OBJPATH cannot be defined if VDBGREL.MAK included
! endif

! ifndef OBJECTS
!	error OBJECTS must be defined before VDBGREL.MAK included 
! endif # OBJECTS

!MESSAGE --------------------------------------------------------------------- 
!MESSAGE Using VDBGREL.MAK.  Note libraries should be specified with USER_LIB,
!MESSAGE and not included in either SOURCES or OBJECTS macro.
!MESSAGE --------------------------------------------------------------------- 

# The following set of macros expand the SOURCES macro into a list of
# objects augmented with the appropriate directory path prefix for the
# type of build we are performing.  The objects will be built into the
# appropriate directory.
#
# Note: Borland compatiblity forces us to use the increasing macro definitions
#	since text substitution is temporary.


# get rid of any extraneous spaces and tabs
OBJECTS1 = $(OBJECTS: =)
OBJECTS2 = $(OBJECTS1:	=)

# add an extra * at the end of the list
OBJECTS3 = $(OBJECTS2)*

# turn "x.obj y.obj z.obj" into "x.obj*y.obj*z.obj**"
OBJECTS4 = $(OBJECTS3:.OBJ=.obj)
OBJECTS5 = $(OBJECTS4:.Obj=.obj)
OBJECTS6 = $(OBJECTS5:.obj=.obj*)

# for Borland we have some more work to do, since there are more types
# of .ob? files
OBJECTS7 = $(OBJECTS6:.OBL=.obl)
OBJECTS8 = $(OBJECTS7:.Obl=.obl)
OBJECTS9 = $(OBJECTS8:.obl=.obl*)
OBJECTS10 = $(OBJECTS9:.OBI=.obi)
OBJECTS11 = $(OBJECTS10:.Obi=.obi)
OBJECTS12 = $(OBJECTS11:.obi=.obi*)
OBJECTS13 = $(OBJECTS12:.OBS=.obs)
OBJECTS14 = $(OBJECTS13:.Obs=.obs)
OBJECTS15 = $(OBJECTS14:.obs=.obs*)
OBJECTS16 = $(OBJECTS15:.OBD=.obd)
OBJECTS17 = $(OBJECTS16:.Obd=.obd)
OBJECTS18 = $(OBJECTS17:.obd=.obd*)
OBJECTS19 = $(OBJECTS18:.OBP=.obp)
OBJECTS20 = $(OBJECTS19:.Obp=.obp)
OBJECTS21 = $(OBJECTS20:.obp=.obp*)

# search for ** and remove to prevent extraneous OBJPATH at end of list
OBJECTS22 = $(OBJECTS21:**=)

# switch based on DEBUG flag
! if $(DEBUG) == 1

# set OBJPATH for debug build
OBJPATH	= .\Debug

# turn "x.obj*y.obj*z.obj" into ".\Debug\x.obj .\Debug\y.obj .\Debug\z.obj"
FINALOBJECTS = $(OBJPATH)\$(OBJECTS22:*= .\Debug\)

! else

# set OBJPATH for release build
OBJPATH	= .\Release

# turn "x.obj*y.obj*z.obj" into ".\Release\x.obj .\Release\y.obj .\Release\z.obj"
FINALOBJECTS = $(OBJPATH)\$(OBJECTS22:*= .\Release\)

! endif

