build: Dialog.obj

buildall: clean Dialog.res Dialog.exe

compilerc: Dialog.res

syntax:
	hla -s Dialog.hla

clean:
	del /F /Q tmp
	del *.exe
	del *.obj
	del *.res
	del *.link
	del *.asm
	del *.map

Dialog.res: Dialog.rc
	rc /v Dialog.rc

Dialog.obj: Dialog.hla
	hla $(debug) -w -c Dialog

Dialog.exe: Dialog.hla
	hla $(debug) -w Dialog Dialog.res
