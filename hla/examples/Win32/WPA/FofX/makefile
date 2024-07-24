build: fofx.exe

buildall: clean fofx.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp fofx.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w fofx

run: fofx.exe
	fofx

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


fofx.exe: fofx.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w fofx



