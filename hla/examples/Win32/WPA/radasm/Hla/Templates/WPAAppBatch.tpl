Windows App
wpa
Windows Programming in Assembly App / Batch Build
[*BEGINPRO*]
[*BEGINDEF*]
[MakeDef]
Files=1,0,0,0,0
Folders=1,0,1
Menu=1,1,0,1,1,0,0,0,0
1=0,OT,build.bat,11
2=0,OT,build.bat,11
3=0,OT,syntax.bat,11
4=0,CT,compilerc.bat,
5=0,C,run.bat,5
6=
[Resource]
[StringTable]
[VerInf]
[*ENDDEF*]
[*BEGINTXT*]
wpa.hla
unit WPAapp;

#includeonce( "w.hhf" )
#includeonce( "wpa.hhf" )
#includeonce( "winmain.hhf" )
#asm
	includelib winmain.lib
#endasm

?@NoDisplay := true;
?@NoStackAlign := true;


	
readonly

	ClassName	:string := "WPAappClass";			// Window Class Name
	AppCaption	:string := "WPAapp Program";		// Caption for Window

	
	// The dispatch table:
	//
	//	This table is where you add new messages and message handlers
	//	to the program.  Each entry in the table must be a MsgProcPtr_t
	//	record containing two entries: the message value (a constant,
	//	typically one of the w.WM_***** constants found in windows.hhf)
	//	and a pointer to a "MsgProcPtr_t" procedure that will handle the
	//	message.
 
	
	Dispatch	:MsgProcPtr_t; @nostorage;

		MsgProcPtr_t	
			MsgProcPtr_t:[ w.WM_DESTROY, &QuitApplication	],
			
			// Insert new message handler records here.
			
			MsgProcPtr_t:[ 0, NULL ];	// This marks the end of the list.
			

	
/**************************************************************************/
/*          W I N M A I N   S U P P O R T   C O D E				          */
/**************************************************************************/

// initWC - Just for fun, let's set the background color to a dark gray
//			to demonstrate how we use the initWC procedure:

procedure initWC; @noframe;
begin initWC;

	mov( w.COLOR_GRAYTEXT+1, wc.hbrBackground );
	ret();

end initWC;

// appCreateWindow-	the default window creation code is fine, so just
//					call defaultCreateWindow.

procedure appCreateWindow; @noframe;
begin appCreateWindow;

	jmp defaultCreateWindow;

end appCreateWindow;


// appException-
//
// Handle any application exceptions that come along (clean up
// before aborting program):

procedure appException( theException:dword in eax );
begin appException;

	raise( eax );

end appException;

// This is the custom message translation procedure.
// We're not doing any custom translation, so just return EAX=0
// to tell the caller to go ahead and call the default translation
// code.

procedure LocalProcessMsg( var lpmsg:w.MSG ); 
begin LocalProcessMsg;

	xor( eax, eax );

end LocalProcessMsg;

			
	
/**************************************************************************/
/*          A P P L I C A T I O N   S P E C I F I C   C O D E	          */
/**************************************************************************/


// QuitApplication:
//
//	This procedure handles the w.WM_DESTROY message.
//	It tells the application to terminate.  This code sends
//	the appropriate message to the main program's message loop
//	that will cause the application to terminate.
	
procedure QuitApplication( hwnd: dword; wParam:dword; lParam:dword );
begin QuitApplication;

	w.PostQuitMessage( 0 );

end QuitApplication;




end WPAapp;

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
