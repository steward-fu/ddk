build: TextAttr.exe

buildall: clean TextAttr.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w TextAttr

syntax:
	hla -s TextAttr.hla

run: TextAttr.exe
	TextAttr

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


TextAttr.obj: TextAttr.hla wpa.hhf
	hla -p:tmp -w -c TextAttr

TextAttr.exe: TextAttr.hla wpa.hhf
	hla -@@ -p:tmp -w TextAttr



