build: typewriter.exe

buildall: clean typewriter.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w typewriter

syntax:
	hla -s -p:tmp typewriter.hla

run: typewriter.exe
	typewriter

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


typewriter.obj: typewriter.hla wpa.hhf
	hla -p:tmp -w -c typewriter

typewriter.exe: typewriter.hla wpa.hhf
	hla -@@ -p:tmp -w typewriter winmain.lib



