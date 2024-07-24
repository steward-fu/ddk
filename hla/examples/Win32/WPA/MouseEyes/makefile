build: MouseEyes.exe

buildall: clean MouseEyes.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp MouseEyes.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w MouseEyes winmain.lib

run: MouseEyes.exe
	MouseEyes

clean:
	delete tmp
	delete MouseEyes.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.ilk
	delete *.pdb


MouseEyes.exe: MouseEyes.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w MouseEyes winmain.lib



