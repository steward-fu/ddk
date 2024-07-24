Windows App
emptyWin
Generic Windows App Built with make.exe
[*BEGINPRO*]
[*BEGINDEF*]
[MakeDef]
Menu=1,1,1,1,1,0,0,0
1=0,OT,make -Dhlafile="$12" -DWINAPP="-w" build,
2=0,OT,make -Dhlafile="$12" -DWINAPP="-w" buildall,
3=0,OT,make -Dhlafile="$12" -DWINAPP="-w" syntax,
4=0,OT,make -Dhlafile="$12" -DWINAPP="-w" compilerc,
5=0,C,make  -Dhlafile="$12" -DWINAPP="-w" run,
6=
11=0,OT,make -Dhlafile="$12" -DWINAPP="-w" -DDEBUG="-aZi -aZf" build,
12=0,OT,make -Dhlafile="$12" -DWINAPP="-w" -DDEBUG="-aZi -aZf" buildall,
13=0,OT,make -Dhlafile="$12" -DWINAPP="-w" -DDEBUG="-aZi -aZf" compilerc,
14=0,OT,make -Dhlafile="$12" -DWINAPP="-w" -DDEBUG="-aZi -aZf" syntax,
15=0,C,make  -Dhlafile="$12" -DWINAPP="-w" -DDEBUG="-aZi -aZf" run,
16=
[Resource]
[StringTable]
[VerInf]
[*ENDDEF*]
[*BEGINTXT*]
emptyWin.hla
program GenericWin32App;
#include( "stdlib.hhf" )
#include( "w.hhf" )

	
begin GenericWin32App;		

				
end GenericWin32App;
[*ENDTXT*]
[*BEGINTXT*]
makefile
build: $(hlafile).exe

buildall: clean $(hlafile).exe

compilerc:
	echo No Resource Files to Process!
	
#uncomment this if you have a res file:
#
#compilerc: $(hlafile).res
#
#$(hlafile.res): $(hlafile).rc
#	rc -v $(hlafile).rc

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


# Note: you'll want to put $(hlafile).res at the end of the following
# two lines if you've got a .rc file in this project:

$(hlafile).exe: $(hlafile).hla
	hla $(DEBUG) $(WINAPP) -p:tmp $(hlafile)



[*ENDTXT*]
[*BEGINTXT*]
delete.bat
del /F /Q %1
if errorlevel 1	echo no files
[*ENDTXT*]
[*ENDPRO*]
