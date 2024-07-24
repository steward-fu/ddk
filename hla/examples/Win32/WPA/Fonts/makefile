build: Fonts.exe

buildall: clean Fonts.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Fonts

syntax:
	hla -s Fonts.hla

run: Fonts.exe
	Fonts

clean:
	delete tmp
	delete *.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.ilk
	delete *.pdb


Fonts.obj: Fonts.hla wpa.hhf
	hla -@@ -p:tmp -w -c Fonts

Fonts.exe: Fonts.hla wpa.hhf
	hla -@@ -p:tmp -w Fonts






