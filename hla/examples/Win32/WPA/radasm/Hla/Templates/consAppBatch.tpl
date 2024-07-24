Console App
console
Generic Console App Built With Batch Files
[*BEGINPRO*]
[*BEGINDEF*]
[MakeDef]
Menu=1,1,0,1,1,0,0,0
1=0,OT,build.bat,11
2=0,OT,build.bat,11
3=0,OT,syntax.bat,11
4=0,C,compilerc.bat,
5=0,C,run.bat,5
6=
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
build.bat
hla -p:tmp -w %1
[*ENDTXT*]
[*BEGINTXT*]
compilerc.bat
echo "No Resource Files to Compile!"
pause
[*ENDTXT*]
[*BEGINTXT*]
syntax.bat
hla -p:tmp -s %1
[*ENDTXT*]
[*BEGINTXT*]
run.bat
%1
pause
[*ENDTXT*]
[*ENDPRO*]
