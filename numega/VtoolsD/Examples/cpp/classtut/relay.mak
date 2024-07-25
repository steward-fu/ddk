#
# makefile - makefile for relay.exe (a large model 16-bit Windows app)
# (c) Copyright 1994, Compuware Corporation

#
# How to use this MAKEFILE 
# 1. Choose your compiler: Microsoft or Borland
# For Microsoft: set MSC16 to the root of your MSVC (16-bit) installation
MSC16 = C:\msvc
#
# For Borland: set BC16 to the root of your Borland installation
BC16=D:\bc4
#
# 2. Run the make utility
# For Microsoft: nmake /f relay.mak COMPILER=MS
# For Borland:   make /f relay COMPILER=BC

!ifndef COMPILER
all:
	REM To build, enter either
	REM nmake /f relay.mak COMPILER=MS    
	REM	OR
	REM make /f relay.mak COMPILER=BC

!else
all:	assumes relay.exe

!if "$(COMPILER)" == "MS"

assumes:
	REM assuming MSVC is in $(MSC16) . . .


relay.obj:	relay.c 
	set include=$(MSC16)\include
	set CL=/nologo /YX /G2 /Zp1 /W3 /AL /Od /D "_DEBUG" /FR /GA /Zi
	$(MSC16)\bin\cl  /c relay.c

relay.exe:	relay.obj relay.def 
	echo >NUL @<<relay.crf 
relay.obj
relay.exe
relay.map
oldnames libw commdlg shell olecli olesvr llibcew LIBW
relay.def;
<<
	set lib=$(MSC16)\lib
	$(MSC16)\bin\link /map /NOLOGO /NOD /STACK:5120 /ALIGN:16 /ONERROR:NOEXE /CO @relay.crf

!else

!if "$(COMPILER)" == "BC"

assumes:
	REM assuming BC is in $(BC16)

relay.exe: relay.c
	$(BC16)\bin\bcc -I$(BC16)\include -ml -3 -WE relay.c
!else

!endif

!endif
!endif


