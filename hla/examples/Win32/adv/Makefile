# make				-- generates AGE (Adventure Game Engine) system
# make fasm			-- generates AGE using FASM rather than MASM.
# make clean		-- Cleans up the temporary files.
#
# Note: by default, this build uses MASM mainly to allow the inclusion
# of debug information in the executable. 

age.exe: 	age.obj ageobjs.obj objlist.obj agesupport.obj actions.obj \
			parser.obj gsinit.obj gsactions.obj gssupport.obj
	link @age.link
	
age.obj: age.hla age.hhf english.hhf roomnouns.hhf gamespecific.hhf 
	hla -@ -aZi -aZf -c age.hla
	
ageobjs.obj: ageobjs.hla age.hhf english.hhf roomnouns.hhf
	hla -@ -aZi -aZf -c ageobjs.hla

objlist.obj: objlist.hla age.hhf english.hhf roomnouns.hhf
	hla -@ -aZi -aZf -c objlist.hla

agesupport.obj: agesupport.hla age.hhf english.hhf roomnouns.hhf
	hla -@ -aZi -aZf -c agesupport.hla
	
actions.obj: actions.hla age.hhf english.hhf roomnouns.hhf gamespecific.hhf
	hla -@ -aZi -aZf -c actions.hla
	
parser.obj: parser.hla age.hhf english.hhf roomnouns.hhf gamespecific.hhf
	hla -@ -aZi -aZf -c parser.hla
	
gsinit.obj: gsinit.hla age.hhf english.hhf roomnouns.hhf gamespecific.hhf
	hla -@ -aZi -aZf -c gsinit.hla

gsactions.obj: gsactions.hla age.hhf english.hhf roomnouns.hhf gamespecific.hhf
	hla -@ -aZi -aZf -c gsactions.hla

gssupport.obj: gssupport.hla age.hhf english.hhf roomnouns.hhf gamespecific.hhf
	hla -@ -aZi -aZf -c gssupport.hla
	





clean:
	if exist *.exe del /F /Q *.exe
	if exist *.obj del /F /Q *.obj
	if exist *.asm del /F /Q *.asm
	if exist *.bak del /F /Q *.bak
	if exist *.inc del /F /Q *.inc
	if exist *.map del /F /Q *.map
	if exist *.ilk del /F /Q *.ilk
	if exist *.pdb del /F /Q *.pdb

cleanall: clean
	if exist *.exe del /F /Q *.exe
	






