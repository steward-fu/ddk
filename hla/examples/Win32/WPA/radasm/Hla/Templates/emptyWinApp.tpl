Windows App
emptyWin
Generic Win32 GUI App / RadAsm Build
[*BEGINPRO*]
[*BEGINDEF*]
[MakeDef]
Files=1,0,0,0,0
Folders=1,0,1
Menu=1,1,0,1,1,0,0,0,0
1=0,OT,hla,11
2=0,OT,hla,11
3=0,OT,hla -s,11
4=0,OT,rc,1
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
run.bat
%1
pause
[*ENDTXT*]
[*ENDPRO*]
