build: MousePts.exe

buildall: clean MousePts.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp MousePts.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w MousePts winmain.lib

run: MousePts.exe
	MousePts

clean:
	delete tmp
	delete MousePts.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.ilk
	delete *.pdb


MousePts.exe: MousePts.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w MousePts winmain.lib



