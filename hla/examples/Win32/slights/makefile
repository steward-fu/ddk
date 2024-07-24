# makefile for s'lights

all:bc.exe bd.exe slights.exe bcf2000.hbbc "nanoKONTROL 1 SLIDER/KNOB.hbbc" 

slights.exe: slights.hla slights.hhf bx.obj slights.hla
	hla -@@ -thread -p:tmp slights.hla bx.obj 
	


bc.exe: bc.hla basic.hhf siteSpecific.hhf
	hla -p:tmp -obj:tmp bc

bd.exe: bd.hla basic.hhf siteSpecific.hhf
	hla -p:tmp -obj:tmp bd

bx.obj: bx.hla basic.hhf siteSpecific.hhf
	hla -c -thread -p:tmp bx
	
bcf2000.hbbc: bcf2000.bas
	bc bcf2000.bas
	move bcf2000.hbbc midiIn/bcf2000.hbbc
	
"nanoKONTROL 1 SLIDER/KNOB.hbbc":nanoKONTROL.bas
	bc nanoKONTROL.bas
	move nanoKONTROL.hbbc "midiIn\nanoKONTROL 1 SLIDER\KNOB.hbbc"

clean:
	del /q /s /f tmp
	del /q /s /f "midiIn\*.hbbc"
	del /q /s /f "midiIn\nanoKONTROL 1 SLIDER"
	del *.exe
	del *.obj
	del *.link
	del *.inc
	del *.asm
	del *.map
	del *.hbbc
