build: Beziers.exe

buildall: clean Beziers.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp Beziers.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Beziers winmain.lib

run: Beziers.exe
	Beziers

clean:
	delete tmp
	delete *.exe
	delete *.obj
	delete *.link
	delete *.ilk
	delete *.pdb
	delete *.inc
	delete *.asm
	delete *.map


Beziers.exe: Beziers.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w Beziers winmain.lib



