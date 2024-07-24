build: kbsysmet.exe

buildall: clean kbsysmet.exe

compilerc:
	echo No Resource Files to Process!

debug: clean
	hla -ddebug -aZi -aZf -lDEBUG -lDEBUGTYPE:COFF -p:tmp -w kbsysmet

syntax:
	hla -s kbsysmet.hla

run: kbsysmet.exe
	kbsysmet

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


kbsysmet.obj: kbsysmet.hla wpa.hhf
	hla -p:tmp -w -c kbsysmet

kbsysmet.exe: kbsysmet.hla wpa.hhf
	hla -@@ -p:tmp -w kbsysmet winmain.lib



