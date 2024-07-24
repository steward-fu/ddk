build: Rectangles.exe

buildall: clean Rectangles.exe winmain.lib

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Rectangles winmain.lib

syntax:
	hla -s Rectangles.hla

run: Rectangles.exe
	Rectangles

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


Rectangles.exe: Rectangles.hla wpa.hhf winmain.hhf winmain.lib
	hla -@@ -p:tmp -w Rectangles winmain.lib



