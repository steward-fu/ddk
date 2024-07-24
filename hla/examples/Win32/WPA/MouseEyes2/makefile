build: MouseEyes2.exe

buildall: clean MouseEyes2.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp MouseEyes2.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w MouseEyes2 winmain.lib

run: MouseEyes2.exe
	MouseEyes2

clean:
	delete tmp
	delete MouseEyes2.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.ilk
	delete *.pdb


MouseEyes2.exe: MouseEyes2.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w MouseEyes2 winmain.lib



