build: winmain.lib

buildall: clean winmain.lib

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s -p:tmp winmain.hla

run: 
	echo This module is a library, not an executable program

clean:
	delete tmp
	delete *.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map
	delete *.lib
	delete *.ilk
	delete *.pdb


winmain.lib: winmain.obj
	lib /verbose /out:winmain.lib winmain.obj
	copy winmain.lib .. 

winmain.obj: winmain.hla wpa.hhf winmain.hhf
	hla -c -p:tmp winmain.hla


