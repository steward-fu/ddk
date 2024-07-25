# Make file for APC example APPLICATION

!include $(VTOOLSD)\include\user.mak

!if "$(COMPILER)" == "MS9" || "$(COMPILER)" == "MS5"

# Edit here MSVCPP to the root MSVC++ directory on your system
MSVCPP = %MSVCDIR%

apcxapp.exe:	apcxapp.obj
	set lib=$(MSVCPP)\lib
	$(MSVCPP)\bin\link @<<
kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib 
shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console 
/INCREMENTAL:no /PDB:none /MACHINE:I386 /OUT:apcxapp.exe apcxapp.obj
<<

apcxapp.obj:	apcxapp.c
	set include=$(MSVCPP)\include
	$(MSVCPP)\bin\cl /c /ML /GX /YX /Od /D "WIN32" /D "NDEBUG" /D "_CONSOLE"  apcxapp.c
!endif



!if "$(COMPILER)" == "BC4" || "$(COMPILER)" == "BC5"

BC4=C:\BC45

#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = $(BC4)\bin\Implib
BCC32   = $(BC4)\bin\Bcc32 +BccW32.cfg 
TLINK32 = $(BC4)\bin\TLink32
TLIB    = $(BC4)\bin\TLib
BRC32   = $(BC4)\bin\Brc32
TASM32  = $(bc4)\bin\Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS32 =  -L$(BC4)\LIB
LLATC32_apcxxdexe =  -Tpe -ap
RLATC32_apcxxdexe =  -w32
BLATC32_apcxxdexe = 
CNIEAT_apcxxdexe = -I$(BC4)\INCLUDE -D
LNIEAT_apcxxdexe = -x
LEAT_apcxxdexe = $(LLATC32_apcxxdexe)
REAT_apcxxdexe = $(RLATC32_apcxxdexe)
BEAT_apcxxdexe = $(BLATC32_apcxxdexe)

#
# Dependency List
#
Dep_apcxx = \
   apcxapp.exe

apcxx : BccW32.cfg $(Dep_apcxx)
  echo MakeNode 

Dep_apcxxdexe = \
   apcxapp.obj

apcxapp.exe : $(Dep_apcxxdexe)
  $(VTOOLSD)\bin\clear
  $(TLINK32) @&&|
 $(IDE_LFLAGS32) $(LEAT_apcxxdexe) $(LNIEAT_apcxxdexe) +
$(BC4)\lib\c0x32.obj apcxapp.obj
$<,$*,cw32mt.lib+import32.lib


|

apcxapp.obj :  apcxapp.c
  $(VTOOLSD)\bin\clear
  $(BCC32) -P- -c @&&|
 $(CEAT_apcxxdexe) $(CNIEAT_apcxxdexe) -o$@ apcxapp.c
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=apcxx.csm
-WM
-WC
| $@



!endif
