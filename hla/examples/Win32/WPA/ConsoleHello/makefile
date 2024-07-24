build: ConsoleHello.exe

buildall: clean ConsoleHello.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w ConsoleHello

syntax:
	hla -s ConsoleHello.hla

run: ConsoleHello.exe
	ConsoleHello

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


ConsoleHello.obj: ConsoleHello.hla wpa.hhf
	hla -p:tmp -w -c ConsoleHello

ConsoleHello.exe: ConsoleHello.hla wpa.hhf
	hla -@@ -p:tmp -w ConsoleHello

