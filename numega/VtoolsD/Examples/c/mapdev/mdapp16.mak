#
# makefile - makefile for mdapp16.exe (a large model 16-bit Windows app)
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
	REM nmake /f mdapp16.mak COMPILER=MS    
	REM	OR
	REM make /f mdapp16.mak COMPILER=BC

!else
all:	assumes mdapp16.exe

!if "$(COMPILER)" == "MS"

assumes:
	REM assuming MSVC is in $(MSC16) . . .


mdapp16.obj:	mdapp16.c 
	set include=$(MSC16)\include
	set CL=/nologo /YX /G2 /Zp1 /W3 /AL /Od /D "_DEBUG" /FR /GA /Zi
	$(MSC16)\bin\cl  /c mdapp16.c

mdapp16.exe:	mdapp16.obj mdapp16.def 
	echo >NUL @<<mdapp16.crf 
mdapp16.obj
mdapp16.exe
mdapp16.map
oldnames libw commdlg shell olecli olesvr llibcew LIBW
mdapp16.def;
<<
	set lib=$(MSC16)\lib
	$(MSC16)\bin\link /map /NOLOGO /NOD /STACK:5120 /ALIGN:16 /ONERROR:NOEXE /CO @mdapp16.crf

!else

!if "$(COMPILER)" == "BC"

assumes:
	REM assuming BC is in $(BC16)

# tlink.exe must be on the path
mdapp16.exe: mdapp16.c
	$(BC16)\bin\bcc -I$(BC16)\include -ml -3 -WE mdapp16.c
!else

!endif

!endif
!endif