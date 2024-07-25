# Make file for COMM Client example APPLICATION

!include $(VTOOLSD)\include\user.mak

!if "$(COMPILER)" == "MS9" || "$(COMPILER)" == "MS5"

# Edit here MSVCPP to the root MSVC++ directory on your system
MSVCPP = %MSVCDIR%

commapp.exe:	commapp.obj
	set lib=$(MSVCPP)\lib
	$(MSVCPP)\bin\link @<<
kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib 
shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console 
/INCREMENTAL:no /PDB:none /MACHINE:I386 /OUT:commapp.exe commapp.obj
<<

commapp.obj:	commapp.c
	set include=$(MSVCPP)\include
	$(MSVCPP)\bin\cl /c /ML /GX /YX /Od /D "WIN32" /D "NDEBUG" /D "_CONSOLE"  commapp.c
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
LLATC32_commxxdexe =  -Tpe -ap
RLATC32_commxxdexe =  -w32
BLATC32_commxxdexe = 
CNIEAT_commxxdexe = -I$(BC4)\INCLUDE -D
LNIEAT_commxxdexe = -x
LEAT_commxxdexe = $(LLATC32_commxxdexe)
REAT_commxxdexe = $(RLATC32_commxxdexe)
BEAT_commxxdexe = $(BLATC32_commxxdexe)

#
# Dependency List
#
Dep_commxx = \
   commapp.exe

commxx : BccW32.cfg $(Dep_commxx)
  echo MakeNode 

Dep_commxxdexe = \
   commapp.obj

commapp.exe : $(Dep_commxxdexe)
  $(VTOOLSD)\bin\clear
  $(TLINK32) @&&|
 $(IDE_LFLAGS32) $(LEAT_commxxdexe) $(LNIEAT_commxxdexe) +
$(BC4)\lib\c0x32.obj commapp.obj
$<,$*,cw32mt.lib+import32.lib


|

commapp.obj :  commapp.c
  $(VTOOLSD)\bin\clear
  $(BCC32) -P- -c @&&|
 $(CEAT_commxxdexe) $(CNIEAT_commxxdexe) -o$@ commapp.c
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=commxx.csm
-WM
-WC
| $@



!endif
