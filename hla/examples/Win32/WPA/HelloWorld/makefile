build: HelloWorld.exe

buildall: clean HelloWorld.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w HelloWorld

syntax:
	hla -s HelloWorld.hla

run: HelloWorld.exe
	HelloWorld

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


HelloWorld.obj: HelloWorld.hla wpa.hhf
	hla -p:tmp -w -c HelloWorld

HelloWorld.exe: HelloWorld.hla wpa.hhf
	hla -@@ -p:tmp -w HelloWorld


