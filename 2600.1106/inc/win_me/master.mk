#******************************************************************************
#
# This is the environment file for building all the DDK samples.  If the
# following environment is setup on a machine you can build any sample in
# the DDK.	The documentation for each item is provided below.
#
# Created
#
#	 06/01/96
#
#******************************************************************************

DDKROOT=$(BASEDIR)

# C16_ROOT should point to the root of your 16-Bit tools directory.
!if "$(C16_ROOT)" == ""
C16_ROOT= $(DDKROOT)\bin\win_me
!endif


# This line should point to 16Bit versions of RC.EXE
RC16B=$(BASEDIR)\bin\win_me\bin16\rc.exe


!ifndef C16_ROOT
!error error: C16_ROOT must be set to the root path of the 16 bit build tools 
!endif

!if "$(DDKBUILDENV)" == "checked"
DEBUG=1
!endif

BIN16=$(DDKROOT)\bin\win_me\bin16
RC16=$(DDKROOT)\bin\win_me\bin16\rc
CL16=$(C16_ROOT)\bin16\cl
LINK16=$(C16_ROOT)\bin16\link
INC32=$(DDKROOT)\inc\win_me
INC16=$(DDKROOT)\inc\win_me\inc16
LIB32=$(DDKROOT)\lib\win_me
LIB16=$(DDKROOT)\lib\win_me

!if defined(NOMASMCOMPATIBILITY)
386_ASSEMBLER=ml -c -coff -Cx -nologo -Ii386\ -I. $(INCPATH1) $(386_ASMFLAGS)
!else
386_ASSEMBLER=ml -c -coff -Cx -nologo -Ii386\ -I. $(INCPATH1) -Zm $(386_ASMFLAGS)
!endif
