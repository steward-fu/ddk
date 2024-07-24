build: Metafiles.exe

buildall: clean Metafiles.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp Metafiles.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w Metafiles winmain.lib

run: Metafiles.exe
	Metafiles

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


Metafiles.exe: Metafiles.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w Metafiles winmain.lib



