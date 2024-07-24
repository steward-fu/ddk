build: MousePts2.exe

buildall: clean MousePts2.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp MousePts2.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w MousePts2 winmain.lib

run: MousePts2.exe
	MousePts2

clean:
	delete tmp
	delete MousePts2.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.ilk
	delete *.pdb


MousePts2.exe: MousePts2.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w MousePts2 winmain.lib



