build: sysmet.exe

buildall: clean sysmet.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w sysmet

syntax:
	hla -s sysmet.hla

run: sysmet.exe
	sysmet

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


sysmet.obj: sysmet.hla wpa.hhf
	hla -p:tmp -w -c sysmet

sysmet.exe: sysmet.hla wpa.hhf
	hla -@@ -p:tmp -w sysmet



