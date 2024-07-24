build: MouseCapture.exe

buildall: clean MouseCapture.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp MouseCapture.hla

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w MouseCapture winmain.lib

run: MouseCapture.exe
	MouseCapture

clean:
	delete tmp
	delete MouseCapture.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.ilk
	delete *.pdb


MouseCapture.exe: MouseCapture.hla wpa.hhf winmain.hhf
	hla -@@ -p:tmp -w MouseCapture winmain.lib



