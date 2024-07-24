build: HitTest.exe

buildall: clean HitTest.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp HitTest.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w HitTest winmain.lib

run: HitTest.exe
	HitTest

clean:
	delete tmp
	delete HitTest.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.ilk
	delete *.pdb


HitTest.exe: HitTest.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w HitTest winmain.lib



