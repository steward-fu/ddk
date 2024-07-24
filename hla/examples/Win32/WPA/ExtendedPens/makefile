build: ExtendedPens.exe

buildall: clean ExtendedPens.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp ExtendedPens.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w ExtendedPens winmain.lib

run: ExtendedPens.exe
	ExtendedPens

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


ExtendedPens.exe: ExtendedPens.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w ExtendedPens winmain.lib



