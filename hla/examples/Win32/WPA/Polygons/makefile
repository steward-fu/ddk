build: Polygons.exe

buildall: clean Polygons.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp Polygons.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Polygons winmain.lib

run: Polygons.exe
	Polygons

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


Polygons.exe: Polygons.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w Polygons winmain.lib



