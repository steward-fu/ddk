build: Bitmaps.exe

buildall: clean Bitmaps.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp Bitmaps.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Bitmaps winmain.lib

run: Bitmaps.exe
	Bitmaps

clean:
	delete tmp
	delete *.exe
	delete *.obj
	delete *.link
	delete *.ilk
	delete *.pdb
	delete *.inc
	delete *.asm
	delete *.map


Bitmaps.exe: Bitmaps.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w Bitmaps winmain.lib



