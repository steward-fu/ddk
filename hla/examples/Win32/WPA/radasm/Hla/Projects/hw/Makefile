build: hw.obj

buildall: clean hw.exe

compilerc: hw.res

syntax:
	hla -s hw.hla

clean:
	del /F /Q tmp
	del *.exe
	del *.obj
	del *.res
	del *.link
	del *.asm
	del *.map

hw.res: hw.rc
	rc /v hw.rc

hw.obj: hw.hla
	hla $(debug) -c hw

hw.exe: hw.hla
	hla $(debug) hw
