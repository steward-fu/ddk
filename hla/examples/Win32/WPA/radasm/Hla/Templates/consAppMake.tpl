Console App
console
Generic Console App Built with make.exe
[*BEGINPRO*]
[*BEGINDEF*]
[MakeDef]
Menu=1,1,1,1,1,0,0,0
1=0,OT,make -Dhlafile="$12" build,
2=0,OT,make -Dhlafile="$12" buildall,
3=0,OT,make -Dhlafile="$12" syntax,
4=0,OT,make -Dhlafile="$12" compilerc,
5=0,C,make  -Dhlafile="$12" run,
6=
11=0,OT,make -Dhlafile="$12" -DDEBUG="-aZi -aZf" build,
12=0,OT,make -Dhlafile="$12" -DDEBUG="-aZi -aZf" buildall,
13=0,OT,make -Dhlafile="$12" -DDEBUG="-aZi -aZf" compilerc,
14=0,OT,make -Dhlafile="$12" -DDEBUG="-aZi -aZf" syntax,
15=0,C,make  -Dhlafile="$12" -DDEBUG="-aZi -aZf" run,
16=
[Resource]
[StringTable]
[VerInf]
[*ENDDEF*]
[*BEGINTXT*]
console.hla
program consoleApp;
#include( "stdlib.hhf" )
begin consoleApp;


end consoleApp;
[*ENDTXT*]
[*BEGINTXT*]
makefile
build: $(hlafile).exe

buildall: clean $(hlafile).exe

compilerc:
	echo No Resource Files to Process!

syntax:
	hla -s $(hlafile).hla

run: $(hlafile).exe
	$(hlafile)
	pause

clean:
	delete tmp
	delete *.exe
	delete *.obj
	delete *.link
	delete *.inc
	delete *.asm
	delete *.map


$(hlafile).exe: $(hlafile).hla
	hla $(DEBUG) -p:tmp $(hlafile)



[*ENDTXT*]
[*BEGINTXT*]
delete.bat
del /F /Q %1
if errorlevel 1	echo no files
[*ENDTXT*]
[*ENDPRO*]
