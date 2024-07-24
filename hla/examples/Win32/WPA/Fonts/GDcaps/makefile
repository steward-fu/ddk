
build: GDcaps.exe

buildall: clean GDcaps.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w GDcaps

syntax:
	hla -s GDcaps.hla

run: GDcaps.exe
	GDcaps

clean:
	delete tmp
	delete *.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map


GDcaps.obj: GDcaps.hla wpa.hhf
	hla -p:tmp -w -c GDcaps

GDcaps.exe: GDcaps.hla wpa.hhf
	hla -p:tmp -w GDcaps


