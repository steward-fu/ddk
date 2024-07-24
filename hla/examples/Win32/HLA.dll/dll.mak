all: dll.dll dllmain.exe
	dllmain
	
dllmain.exe: dllmain.hla
	hla dllmain dll.lib

dll.dll: dll.obj
	link /map dll.obj @dll.link

dll.obj: dll.hla
	hla -@ -c dll.hla 


clean:
	del *.obj
	del *.exe
	del *.ilk
	del *.pdb
	del *.map
	del *.exp
	del *.lib
	