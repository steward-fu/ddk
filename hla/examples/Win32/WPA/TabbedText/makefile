build: TabbedText.exe

buildall: clean TabbedText.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s TabbedText.hla

run: TabbedText.exe
	TabbedText

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w TabbedText

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


TabbedText.obj: TabbedText.hla wpa.hhf
	hla -p:tmp -w -c TabbedText

TabbedText.exe: TabbedText.hla wpa.hhf
	hla -@@ -p:tmp -w TabbedText



