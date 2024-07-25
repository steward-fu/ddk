#
# makefile - makefile for pmapp.exe (a large model 16-bit Windows app)
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
# For Microsoft: nmake /f pmapp.mak COMPILER=MS
# For Borland:   make /f pmapp COMPILER=BC

!ifndef COMPILER
all:
	REM To build, enter either
	REM nmake /f pmapp.mak COMPILER=MS    
	REM	OR
	REM make /f pmapp.mak COMPILER=BC

!else
all:	assumes pmapp.exe

!if "$(COMPILER)" == "MS"

assumes:
	REM assuming MSVC is in $(MSC16) . . .


pmapp.obj:	pmapp.c 
	set include=$(MSC16)\include
	set CL=/nologo /YX /G2 /Zp1 /W3 /AL /Od /D "_DEBUG" /FR /GA /Zi
	$(MSC16)\bin\cl  /c pmapp.c

pmapp.exe:	pmapp.obj pmapp.def 
	echo >NUL @<<pmapp.crf 
pmapp.obj
pmapp.exe
pmapp.map
oldnames libw commdlg shell olecli olesvr llibcew LIBW
pmapp.def;
<<
	set lib=$(MSC16)\lib
	$(MSC16)\bin\link /map /NOLOGO /NOD /STACK:5120 /ALIGN:16 /ONERROR:NOEXE /CO @pmapp.crf

!else

!if "$(COMPILER)" == "BC"

assumes:
	REM assuming BC is in $(BC16)

# tlink.exe must be on the path
pmapp.exe: pmapp.c
	$(BC16)\bin\bcc -I$(BC16)\include -ml -3 -WE pmapp.c
!else

!endif

!endif
!endif