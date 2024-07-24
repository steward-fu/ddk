build: Bitmaps3.exe

buildall: clean Bitmaps3.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp Bitmaps3.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Bitmaps3 winmain.lib

run: Bitmaps3.exe
	Bitmaps3

clean:
	delete tmp
	delete *.ilk
	delete *.pdb
	delete Bitmaps3.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map


Bitmaps3.exe: Bitmaps3.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w Bitmaps3 winmain.lib



