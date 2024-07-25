//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
/******************************************************************************
*
*       CommFilt - Comm Port Monitor for Windows NT
*		
*		Copyright (c) 1996 Bryce Cogswell and Mark Russinovich 
*
*		You have the right to take this code and use it in whatever 
*       way you wish.
*
*    	PROGRAM: CommFilt.c
*
*    	PURPOSE: Communicates with the CommFilt driver to display 
*		comm port activity information.
*
******************************************************************************/
#define UNICODE 1
#include <windows.h>    // includes basic windows functionality
#include <windowsx.h>
#include <commctrl.h>   // includes the common control header
#include <stdio.h>
#include <string.h>
#include <winioctl.h>
#include "resource.h"
#include "../common.h"

#include "instdrv.h"

// Variables/definitions for the driver that performs the actual monitoring.
#define				SYS_FILE		TEXT("COMMFILT.SYS")
#define				SYS_NAME		TEXT("CommFilter")
static HANDLE		sys_handle		= INVALID_HANDLE_VALUE;

#define MAX_COMM	4


// The handle for the shared synchronization event.
HANDLE				hEvent;



// Buffer into which driver can copy statistics
RECORD				Stats[ 100 ];

// Application instance handle
HINSTANCE			hInst;

// Misc globals
HWND				hWndList;
BOOLEAN				Capture = TRUE;
BOOLEAN				Autoscroll = TRUE;
BOOLEAN				ModifyData = FALSE;

// General buffer for storing temporary strings
static TCHAR		msgbuf[ 257 ];

// General cursor manipulation
HCURSOR 			hSaveCursor;
HCURSOR 			hHourGlass;

// procs
LRESULT APIENTRY 		MainWndProc( HWND, UINT, UINT, ULONG_PTR );
BOOL APIENTRY 		About( HWND, UINT, UINT, LONG );

//functions
BOOL 				InitApplication( HANDLE );
HWND 				InitInstance( HANDLE, int );
DWORD 				Hook_Comms( HMENU CommMenu, DWORD CurCommSet ); 
HWND 				CreateList( HWND );
BOOL 				UpdateStatistics( HWND hWnd, HWND hWndList, RECORD a[], DWORD nitems, BOOL Clear );
int 				CALLBACK ListCompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );


/******************************************************************************
*
*	FUNCTION:	Abort:
*
*	PURPOSE:	Handles emergency exit conditions.
*
*****************************************************************************/
void Abort( HWND hWnd, TCHAR * Msg )
{
	UnloadDeviceDriver( SYS_NAME );
	MessageBox( hWnd, Msg, TEXT("CommFilt"), MB_OK );
	PostQuitMessage( 1 );
}		


/****************************************************************************
*
*	FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)
*
*	PURPOSE:	calls initialization function, processes message loop
*
****************************************************************************/
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
						LPSTR lpCmdLine, int nCmdShow )
{
	HWND	hWnd;
        
	if ( ! InitApplication( hInstance ) )
		return FALSE;     

	// initializations that apply to a specific instance 
	if ( (hWnd = InitInstance( hInstance, nCmdShow )) == NULL )
		return FALSE;

	for (;;)  { 
		DWORD result ; 

		// wait for message posted or for event to become signaled 
		result = MsgWaitForMultipleObjects( 1, &hEvent, FALSE, INFINITE, QS_ALLINPUT );

		if ( result == WAIT_OBJECT_0 ) {
			// event signaled
			DWORD nbytes;
			if ( ! ReadFile( sys_handle, Stats, sizeof(Stats), &nbytes, NULL ) )  {
				Abort( hWnd, TEXT("read failed") );
				nbytes = 0;
			}
			UpdateStatistics( hWnd, hWndList, Stats, nbytes/sizeof(RECORD), FALSE );
		} else {

			// message arrived 
			MSG	msg; 

			// read all messages 
			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) { 

				// if it's a quit message we're out of here 
				if ( msg.message == WM_QUIT )  {
					Hook_Comms( NULL, 0 );
					return (int)msg.wParam;
				}

				// otherwise dispatch it 
				TranslateMessage( &msg );
				DispatchMessage( &msg ); 
			}
		}
	} 
}

/****************************************************************************
*
*    FUNCTION: InitApplication(HANDLE)
*
*    PURPOSE: Initializes window data and registers window class
*
****************************************************************************/
BOOL InitApplication( HANDLE hInstance )
{
	WNDCLASS  wc;
	
	// Fill in window class structure with parameters that describe the
	// main (statistics) window. 
	wc.style			= 0;                     
	wc.lpfnWndProc		= (WNDPROC)MainWndProc; 
	wc.cbClsExtra		= 0;              
	wc.cbWndExtra		= 0;              
	wc.hInstance		= hInstance;       
	wc.hIcon			= LoadIcon( hInstance, TEXT("APPICON") );
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground	= GetStockObject( LTGRAY_BRUSH ); 
	wc.lpszMenuName		= TEXT("LISTMENU");  
	wc.lpszClassName	= TEXT("CommFiltClass");
	if ( ! RegisterClass( &wc ) )
		return FALSE;

	return TRUE;
}


/****************************************************************************
*
*    FUNCTION:  InitInstance(HANDLE, int)
*
*    PURPOSE:  Saves instance handle and creates main window
*
****************************************************************************/
HWND InitInstance( HANDLE hInstance, int nCmdShow )
{
	HWND hWndMain;

	hInst = hInstance;

	hWndMain = CreateWindow( TEXT("CommFiltClass"), TEXT("NT Comm Port Monitor"), 
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
							NULL, NULL, hInstance, NULL );

	// if window could not be created, return "failure" 
	if ( ! hWndMain )
		return NULL;
	
	// make the window visible; update its client area; and return "success"
	ShowWindow( hWndMain, nCmdShow );
	UpdateWindow( hWndMain ); 
	return hWndMain;      
}


/****************************************************************************
*
*    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)
*
*    PURPOSE:  Processes messages for the statistics window.
*
****************************************************************************/
LRESULT APIENTRY MainWndProc( HWND hWnd, UINT message, UINT wParam, ULONG_PTR lParam) 
{
	static DWORD		CurCommSet = 0;
	static HMENU		CommMenu;
	DWORD				Comm;
	TCHAR				Path[ 256 ];
	TCHAR				name[32];
	DWORD				nb;

	switch ( message ) {

		case WM_CREATE:

			// get hourglass icon ready
			hHourGlass = LoadCursor( NULL, IDC_WAIT );

			// post hourglass icon
			SetCapture(hWnd);
			hSaveCursor = SetCursor(hHourGlass);

			// Create the ListBox within the main window
			hWndList = CreateList( hWnd );
			if ( hWndList == NULL )
				MessageBox( NULL, TEXT("List not created!"), NULL, MB_OK );

		    // open the handle to the device
			GetCurrentDirectory( sizeof Path, Path );
			wsprintf( Path+lstrlen(Path), TEXT("\\%s"), SYS_FILE );
			if ( ! LoadDeviceDriver( SYS_NAME, Path, &sys_handle ) )  {
				wsprintf( msgbuf, TEXT("Opening %s (%s): error %d"), SYS_NAME, Path,
								GetLastError( ) );
				Abort( hWnd, msgbuf );
			}

			// Create the synchronization event
			hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			// Create a pop-up menu item with the comms
			CommMenu = CreateMenu();

			// Set available comms we can monitor
			for ( Comm = 1; Comm <= MAX_COMM; ++Comm )  {
				wsprintf( name, TEXT("COM%d"), Comm );
				InsertMenu( CommMenu, 0xFFFFFFFF, MF_BYPOSITION|MF_STRING,
							IDC_COMM+Comm, name );
				CurCommSet |= 1 << Comm ;
			}
			// Insert into top-level menu
			InsertMenu( GetMenu( hWnd ), 2, MF_BYPOSITION|MF_POPUP|MF_STRING,
						(UINT_PTR)CommMenu, TEXT("&Comms") );

			// Have driver hook the selected comms
			CurCommSet = Hook_Comms( CommMenu, CurCommSet );

			// Start up timer to periodically update screen
			SetTimer( hWnd,	1, 500/*ms*/, NULL );
			
			// Initialization done
			SetCursor( hSaveCursor );
			ReleaseCapture();
			return 0;

		case WM_NOTIFY:
			// Make sure its intended for us
			if ( wParam == ID_LIST )  {
				NM_LISTVIEW	* pNm = (NM_LISTVIEW *)lParam;
				switch ( pNm->hdr.code )  {

			        case LVN_BEGINLABELEDIT:
						// Don't allow editing of information
						return TRUE;
				}
			}
			return 0;

		case WM_COMMAND:

			switch ( LOWORD( wParam ) )	 {

				// stats related commands to send to driver
				case IDM_CLEAR:
					// Update statistics windows
					UpdateStatistics( hWnd, hWndList, NULL, 0, TRUE );
					return 0;

				case IDM_CAPTURE:
					// Read statistics from driver
					Capture = !Capture;
					CheckMenuItem( GetMenu(hWnd), IDM_CAPTURE,
									MF_BYCOMMAND|(Capture?MF_CHECKED:MF_UNCHECKED) ); 
					return 0;

				case IDM_AUTOSCROLL:
					Autoscroll = !Autoscroll;
					CheckMenuItem( GetMenu(hWnd), IDM_AUTOSCROLL,
									MF_BYCOMMAND|(Autoscroll?MF_CHECKED:MF_UNCHECKED) ); 
					return 0;

				case IDM_MODIFY:
					ModifyData = !ModifyData;
					CheckMenuItem( GetMenu(hWnd), IDM_MODIFY,
									MF_BYCOMMAND|(ModifyData?MF_CHECKED:MF_UNCHECKED) ); 
					DeviceIoControl( sys_handle, IOCTL_MODIFY, &ModifyData, sizeof(BOOLEAN), 
									NULL, 0, &nb, NULL );  
					return 0;

				case IDM_EXIT:
					// Close ourself
					SendMessage( hWnd, WM_CLOSE, 0, 0 );
					return 0;

				case IDM_ABOUT:
					// Show the names of the authors
					DialogBox( hInst, TEXT("AboutBox"), hWnd, (DLGPROC)About );
					return 0;

				default:
					Comm = LOWORD( wParam ) - IDC_COMM;
					if ( Comm-1 < MAX_COMM )  {
						// Toggle status
						CurCommSet ^= (1 << Comm);
						// Have driver hook the selected comms
						CurCommSet = Hook_Comms( CommMenu, CurCommSet );
						return 0;
					} else {
						// Default behavior
						return DefWindowProc( hWnd, message, wParam, lParam );
					}
			}
			break;

		case WM_SIZE:
			// Move or resize the List
            MoveWindow( hWndList, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE );
            return 0;

		case WM_CLOSE:
			// get hourglass icon ready
			hHourGlass = LoadCursor( NULL, IDC_WAIT );

			// post hourglass icon, since close can block
			hSaveCursor = SetCursor(hHourGlass);
			CloseHandle( sys_handle );			
			SetCursor( hSaveCursor );

			if ( ! UnloadDeviceDriver( SYS_NAME ) )  {
				wsprintf( msgbuf, TEXT("Error unloading \"%s\""), SYS_NAME );
				MessageBox( hWnd, msgbuf, TEXT("CommFilt"), MB_OK );
			}


			return DefWindowProc( hWnd, message, wParam, lParam );

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			// Default behavior
			return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}


/******************************************************************************
*
*	FUNCTION:	Hook_Comms
*
*	PURPOSE:	Hook the currently selected comms, updating menu checks
*
******************************************************************************/
DWORD Hook_Comms( HMENU CommMenu, DWORD CurCommSet ) 
{
	DWORD	Comm;

	// Update menu items
	for ( Comm = 1; Comm <= MAX_COMM; ++Comm )  {
		// Tell device driver which Comms to monitor
		if ( CurCommSet & (1<<Comm) )  {
			// Hook comm
			struct HOOK_PARMS	parms;
			DWORD				nb;

			parms.Comm = Comm;
			parms.hEvent = hEvent;
			if ( !DeviceIoControl( sys_handle, IOCTL_HOOK, &parms, sizeof(parms), 
									NULL, 0, &nb, NULL) )  
			{
				CurCommSet &= ~(1 << Comm);
			}
		} else {
			// Unhook comm
			DWORD	nb;
			if ( !DeviceIoControl( sys_handle, IOCTL_UNHOOK, 
									&Comm, sizeof Comm, NULL, 0, &nb, NULL) )
			{
				CurCommSet |= (1 << Comm);
			}
		}
	}

	if ( CommMenu )
		for ( Comm = 1; Comm <= MAX_COMM; ++Comm )
			if ( CurCommSet & (1<<Comm) )
				CheckMenuItem( CommMenu, IDC_COMM+Comm, MF_BYCOMMAND|MF_CHECKED );
			else
				CheckMenuItem( CommMenu, IDC_COMM+Comm, MF_BYCOMMAND|MF_UNCHECKED );

		return CurCommSet;
}


/******************************************************************************
*
*	FUNCTION:	UpdateStatistics
*
*	PURPOSE:	Clear the statistics window and refill it with the current 
*				contents of the statistics buffer.  Does not refresh the 
*				buffer from the device driver.
*
******************************************************************************/
BOOL UpdateStatistics( HWND hWnd, HWND hWndList, RECORD * Stats, DWORD StatsLen, BOOL Clear )
{
	static WCHAR * IrpName[] = {
		TEXT("CREATE"),                  
		TEXT("CREATE_NAMED_PIPE"),     
		TEXT("CLOSE"),            
		TEXT("READ"),           
		TEXT("WRITE"),         
		TEXT("QUERY_INFORMATION"),     
		TEXT("SET_INFORMATION"),   
		TEXT("QUERY_EA"),          
		TEXT("SET_EA"),                
		TEXT("FLUSH_BUFFERS"),       
		TEXT("QUERY_VOLUME_INFORMATION"), 
		TEXT("SET_VOLUME_INFORMATION"),   
		TEXT("DIRECTORY_CONTROL"),      
		TEXT("FILE_SYSTEM_CONTROL"), 
		TEXT("DEVICE_CONTROL"),         
		TEXT("INTERNAL_DEVICE_CONTROL"),  
		TEXT("SHUTDOWN"),               
		TEXT("LOCK_CONTROL"),           
		TEXT("CLEANUP"),            
		TEXT("CREATE_MAILSLOT"),        
		TEXT("QUERY_SECURITY"),        
		TEXT("SET_SECURITY"),            
		TEXT("QUERY_POWER"),         
		TEXT("SET_POWER"),          
		TEXT("DEVICE_CHANGE"),           
		TEXT("QUERY_QUOTA"),          
		TEXT("SET_QUOTA"),              
		TEXT("PNP_POWER"),              
	};
	DWORD j;

	// Just return if nothing to do
	if ( !Clear  &&  StatsLen == 0 )
		return TRUE;

	// Start with empty list
	if ( Clear ) 
		ListView_DeleteAllItems( hWndList );


	// Add all List items from Stats[] data
	for ( j = 0; j < StatsLen; ++j )  {
		RECORD    *	Item = Stats + j;
		LV_ITEM		lvI;	// list view item structure
		int			row;
		int			itemcnt = 0;
		WCHAR	*	func;

		// Sequence number if a new item
		wsprintf( msgbuf, TEXT("%d"), Item->sequence );
		lvI.mask		= LVIF_TEXT | LVIF_PARAM;
		lvI.iItem		= 0x7FFFFFFF;
		lvI.iSubItem	= 0;
		lvI.pszText		= msgbuf;
		lvI.cchTextMax	= lstrlen( lvI.pszText ) + 1;
		lvI.lParam		= Item->sequence;
		row = ListView_InsertItem( hWndList, &lvI );
		if ( row == -1 )  {
			wsprintf( msgbuf, TEXT("Error adding item %d to list view"), Item->sequence );
			MessageBox( hWndList, msgbuf, TEXT("CommFilt Error"), MB_OK );
			return FALSE;
		}

		// function
		if ( Item->function < sizeof IrpName/sizeof *IrpName )  {
			func = IrpName[ Item->function ];
		} else {
			func = msgbuf;
			wsprintf( msgbuf, TEXT("funcion %x"), Item->function );
		}
		ListView_SetItemText( hWndList, row, 1, func );

		// status
		wsprintf( msgbuf, TEXT("%x"), Item->status );
		ListView_SetItemText( hWndList, row, 2, msgbuf );

		// information
		wsprintf( msgbuf, TEXT("%x"), Item->information );
		ListView_SetItemText( hWndList, row, 3, msgbuf );
	}

	// Scroll so newly added items are visible
	if ( Autoscroll ) 
		ListView_EnsureVisible( hWndList, ListView_GetItemCount(hWndList)-1, FALSE );
	
	return TRUE;
}


/****************************************************************************
* 
*    FUNCTION: CreateListView(HWND)
*
*    PURPOSE:  Creates the statistics list view window and initializes it
*
****************************************************************************/
HWND CreateList( HWND hWndParent )                                     
{
	HWND		hWndList;    	  	// handle to list view window
	RECT		rc;         	  	// rectangle for setting size of window
	LV_COLUMN	lvC;				// list view column structure
	DWORD		j;
	static struct {
		TCHAR *	Label;	// title of column
		DWORD	Width;	// width of column in pixels
		DWORD	Fmt;
	} column[] = {
		{	TEXT("#"),				35		},
		{	TEXT("Function"),		150		},
		{	TEXT("Status"),			70		},
		{	TEXT("Information"),	70		},
	};

	// Ensure that the common control DLL is loaded.
	InitCommonControls();

	// Get the size and position of the parent window.
	GetClientRect( hWndParent, &rc );

	// Create the list view window
	hWndList = CreateWindowEx( 0L, WC_LISTVIEW, TEXT(""), 
								WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT |
								    WS_EX_CLIENTEDGE,	// styles
								0, 0, rc.right - rc.left, rc.bottom - rc.top,
								hWndParent,	(HMENU)ID_LIST, hInst, NULL );
	if ( hWndList == NULL )
		return NULL;

	// Initialize columns
	lvC.mask	= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.fmt		= LVCFMT_LEFT;	// left-align column

	// Add the columns.
	for ( j = 0; j < sizeof column/sizeof column[0]; ++j )  {
		lvC.iSubItem	= j;
		lvC.cx			= column[j].Width;
	 	lvC.pszText		= column[j].Label;
		if ( ListView_InsertColumn( hWndList, j, &lvC ) == -1 )
			return NULL;
	}

	return hWndList;
}




/****************************************************************************
*
*	FUNCTION:	About
*
*	PURPOSE:	Processes messages for "About" dialog box
*
****************************************************************************/

BOOL APIENTRY About( HWND hDlg, UINT message, UINT wParam, LONG lParam )
{
	switch ( message )  {
	   case WM_INITDIALOG:
		  return TRUE;

	   case WM_COMMAND:              
		  if ( LOWORD( wParam ) == IDOK )	 {
			  EndDialog( hDlg, TRUE );
			  return TRUE;
		  }
		  break;
	}
	return FALSE;   
}
