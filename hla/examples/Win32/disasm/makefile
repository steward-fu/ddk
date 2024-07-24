disasm.exe: disasm.obj dMain.obj dFPU.obj dInstrs.obj dOperands.obj
   	hla -@@ -lDEBUG -x:disasm.exe tmp\disasm.obj tmp\dMain.obj tmp\dFPU.obj tmp\dInstrs.obj tmp\dOperands.obj

disasm.obj: disasm.hla disasm.hhf
	hla -c -p:tmp -obj:tmp disasm.hla

dMain.obj: dMain.hla disasm.hhf
	hla -c -p:tmp -obj:tmp dMain.hla

dFPU.obj: dFPU.hla disasm.hhf
	hla -c -p:tmp -obj:tmp dFPU.hla

dInstrs.obj: dInstrs.hla disasm.hhf
	hla -c -p:tmp -obj:tmp dInstrs.hla

dOperands.obj: dOperands.hla disasm.hhf
	hla -c -p:tmp -obj:tmp dOperands.hla

test: disasm.exe
	disasm >y.txt
	fc base.txt y.txt

clean:
	delete *.asm
	delete *.inc
	delete *.obj
	delete disasm.exe
	delete *.link
	delete *.backup
	delete *.ilk
	delete *.pdb
	delete tmp\*.*
	delete tmp\*.*



	
 