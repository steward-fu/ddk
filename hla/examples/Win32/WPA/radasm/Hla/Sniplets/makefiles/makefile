build: <<file>>.exe

buildall: clean <<file>>.exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s <<file>>.hla

clean:
	delete /F /Q tmp
	delete *.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map


<<file>>.obj: <<file>>.hla wpa.hhf
	hla $(debug) -p:tmp -w -c <<file>>

<<file>>.exe: <<file>>.hla wpa.hhf
	hla $(debug) -p:tmp -w <<file>>



