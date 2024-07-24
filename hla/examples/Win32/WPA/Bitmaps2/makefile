build: Bitmaps2.exe

buildall: clean Bitmaps2.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp Bitmaps2.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Bitmaps2 winmain.lib

run: Bitmaps2.exe
	Bitmaps2

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


Bitmaps2.exe: Bitmaps2.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w Bitmaps2 winmain.lib



