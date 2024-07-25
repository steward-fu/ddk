# Make file for Win32/VxD interface APPLICATION

!include $(VTOOLSD)\include\user.mak

!if "$(COMPILER)" == "MS9" || "$(COMPILER)" == "MS5"

# Edit here MSVCPP to the root MSVC++ directory on your system
MSVCPP = %MSVCDIR%

mdapp32.exe:	mdapp32.obj
	set lib=$(MSVCPP)\lib
	$(MSVCPP)\bin\link @<<
kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib 
shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console 
/INCREMENTAL:no /PDB:none /MACHINE:I386 /OUT:mdapp32.exe mdapp32.obj
<<

mdapp32.obj:	mdapp32.c
	set include=$(MSVCPP)\include
	$(MSVCPP)\bin\cl /c /ML /GX /YX /Od /D "WIN32" /D "NDEBUG" /D "_CONSOLE"  mdapp32.c
!endif


!if "$(COMPILER)" == "BC4" || "$(COMPILER)" == "BC5"

BC4=D:\BC4

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
LLATC32_w32ifxdexe =  -Tpe -ap
RLATC32_w32ifxdexe =  -w32
BLATC32_w32ifxdexe = 
CNIEAT_w32ifxdexe = -I$(BC4)\INCLUDE -D
LNIEAT_w32ifxdexe = -x
LEAT_w32ifxdexe = $(LLATC32_w32ifxdexe)
REAT_w32ifxdexe = $(RLATC32_w32ifxdexe)
BEAT_w32ifxdexe = $(BLATC32_w32ifxdexe)

#
# Dependency List
#
Dep_w32ifx = \
   mdapp32.exe

w32ifx : BccW32.cfg $(Dep_w32ifx)
  echo MakeNode 

Dep_w32ifxdexe = \
   mdapp32.obj

mdapp32.exe : $(Dep_w32ifxdexe)
  $(VTOOLSD)\bin\clear
  $(TLINK32) @&&|
 $(IDE_LFLAGS32) $(LEAT_w32ifxdexe) $(LNIEAT_w32ifxdexe) +
$(BC4)\lib\c0x32.obj mdapp32.obj
$<,$*,cw32mt.lib+import32.lib


|

mdapp32.obj :  mdapp32.c
  $(VTOOLSD)\bin\clear
  $(BCC32) -P- -c @&&|
 $(CEAT_w32ifxdexe) $(CNIEAT_w32ifxdexe) -o$@ mdapp32.c
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-R
-v
-vi
-H
-H=w32ifx.csm
-WM
-WC
| $@



!endif
