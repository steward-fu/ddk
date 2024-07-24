prog=HLAexVB

build: HLAexVB.dll

buildall: clean HLAexVB.dll

syntax:
	hla -p:tmp -s HLAexVB.hla

clean:
#	del *.dll
	del *.exp
	del *.map
	del *.obj
	del tmp\*.asm
	del tmp\*.inc
	del tmp\*.link

HLAexVB.dll: HLAexVB.obj
	link  @HLAexVB.lin

HLAexVB.obj: HLAexVB.hla
	hla -p:tmp -c HLAexVB.hla
