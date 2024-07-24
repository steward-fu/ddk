build: CopyModes.exe

buildall: clean CopyModes.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp CopyModes.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w CopyModes winmain.lib

run: CopyModes.exe
	CopyModes

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


CopyModes.exe: CopyModes.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w CopyModes winmain.lib



