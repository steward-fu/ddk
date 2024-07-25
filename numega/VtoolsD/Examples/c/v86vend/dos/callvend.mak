#
# makefile - makefile for callvend.exe (a real-mode DOS program)
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
# For Microsoft: nmake /f callvend.mak COMPILER=MS
# For Borland:   make /f callvend COMPILER=BC

!ifndef COMPILER
all:
	REM To build, enter either
	REM nmake /f callvend.mak COMPILER=MS    
	REM	OR
	REM make /f callvend.mak COMPILER=BC

!else
all:	assumes callvend.exe

!if "$(COMPILER)" == "MS"

assumes:
	REM assuming MSVC is in $(MSC16) . . .

callvend.obj:	callvend.c 
	set include=$(MSC16)\include
	set CL=/nologo /YX /G2 /Zp1 /W3 /AS /Ox /FR /GA
	$(MSC16)\bin\cl  /c callvend.c

callvend.exe:	callvend.obj 
	echo >NUL @<<callvend.crf 
callvend.obj
callvend.exe
callvend.map
slibce

<<
	set lib=$(MSC16)\lib
	$(MSC16)\bin\link /map /NOLOGO /NOD /STACK:5120 /ONERROR:NOEXE  @callvend.crf

!else

!if "$(COMPILER)" == "BC"

assumes:
	REM assuming BC is in $(BC16)

# TLINK.EXE must be on your path.

callvend.exe: callvend.c
	$(BC16)\bin\bcc -I$(BC16)\include -ms -3 callvend.c
!else

!endif

!endif
!endif


