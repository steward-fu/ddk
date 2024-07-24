Windows App
emptyWin
Generic Win32 GUI App / RadAsm Build
[*BEGINPRO*]
[*BEGINDEF*]
[MakeDef]
Menu=1,1,0,1,1,0,0,0
1=0,OT,build.bat,11
2=0,OT,build.bat,11
3=0,OT,syntax.bat,11
4=0,OT,compilerc.bat,
5=0,C,run.bat,5
6=
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
