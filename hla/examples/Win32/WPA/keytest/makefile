build: keytest.exe

buildall: clean keytest.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w keytest

syntax:
	hla -s -p:tmp keytest.hla

run: keytest.exe
	keytest

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


keytest.obj: keytest.hla wpa.hhf
	hla -p:tmp -w -c keytest

keytest.exe: keytest.hla wpa.hhf
	hla -@@ -p:tmp -w keytest winmain.lib



