all:
	cd win32console
	make
	cd ..
	
	cd disasm
	make
	cd ..
	
	cd HLA.dll
	make -f dll.mak
	cd ..
	
	cd icz
	make
	cd ..
	
	cd adv
	make
	cd ..
	
	cd wpa
	make
	cd ..
	
	cd howl
	make
	cd ..
	
	
	hla -@@ -p:temp astyleall
	hla -@@ -p:temp basic wincons.lib
	hla -@@ -p:temp detab.hla
	hla -@@ -p:temp DiagChar.hla wincons.lib
	hla -@@ -p:temp doInc.hla
	hla -@@ -p:temp entab
	hla -@@ -p:temp fastlc
	hla -@@ -p:temp filter.hla
	hla -@@ -p:temp findfrst
	hla -@@ -p:temp hlacmp
	hla -@@ -p:temp hlacmpx
	hla -@@ -p:temp howlong
	hla -@@ -p:temp initHLA advapi32.lib
	hla -@@ -p:temp M2T
	hla -@@ -p:temp makemake
	hla -@@ -p:temp PatchRadASM
	hla -@@ -p:temp resources
	hla -@@ -p:temp sleep
	hla -@@ -p:temp textto.hla
	hla -@@ -p:temp tstFE
	hla -@@ -p:temp UCRPgm2.hla


clean:
	del /F /Q *.obj
	del /F /Q *.o
	del /F /Q  *.asm
	del /F /Q  *.exe
	del /F /Q  *.link
	del /F /Q  *.inc
	del /F /Q  *.ilk
	del /F /Q  *.pdb
	del /F /Q  finder.dat
	del /F /Q  temp\*.*	
	cd icz
	make clean
	cd ..
	cd win32console
	make clean
	cd ..
	cd adv
	make clean
	cd ..
	cd disasm
	make clean
	cd ..
	cd HLA.dll
	make -f dll.mak clean
	
	cd ..
	cd howl
	make clean
	cd ..
	
	
	
	
	
	



