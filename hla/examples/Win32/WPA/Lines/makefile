build: lines.exe

buildall: clean lines.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s lines.hla

run: lines.exe
	lines

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


lines.exe: lines.hla wpa.hhf
	hla -@@ -p:tmp -w lines



