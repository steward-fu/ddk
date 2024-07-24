build: DebugWindow.exe DBGHello.exe

buildall: clean DebugWindow.exe DBGHello.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w DebugWindow
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w DBGHello

syntax:
	hla -s DebugWindow.hla DBGHello.hla

run: DebugWindow.exe
	DebugWindow

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


DebugWindow.obj: DebugWindow.hla wpa.hhf
	hla -p:tmp -w -c DebugWindow

DBGHello.obj: DBGHello.hla wpa.hhf
	hla -p:tmp -w -c DBGHello

DebugWindow.exe: DebugWindow.hla wpa.hhf
	hla -@@ -p:tmp -w DebugWindow

DBGHello.exe: DBGHello.hla wpa.hhf
	hla -@@ -p:tmp -w DBGHello


