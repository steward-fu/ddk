build: Clover.exe

buildall: clean Clover.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp Clover.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Clover winmain.lib

run: Clover.exe
	Clover

clean:
	delete tmp
	delete *.ilk
	delete *.pdb
	delete *.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map


Clover.exe: Clover.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w Clover winmain.lib



