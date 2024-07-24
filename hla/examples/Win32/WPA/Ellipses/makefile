build: Ellipses.exe

buildall: clean Ellipses.exe winmain.lib

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Ellipses winmain.lib

syntax:
	hla -s Ellipses.hla

run: Ellipses.exe
	Ellipses

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


Ellipses.exe: Ellipses.hla wpa.hhf winmain.hhf winmain.lib
	hla -@@ -p:tmp -w Ellipses winmain.lib



