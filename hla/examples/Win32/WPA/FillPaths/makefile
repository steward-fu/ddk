build: FillPaths.exe

buildall: clean FillPaths.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp FillPaths.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w FillPaths winmain.lib

run: FillPaths.exe
	FillPaths

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


FillPaths.exe: FillPaths.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w FillPaths winmain.lib



