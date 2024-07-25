
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
//----------------------------------------------------------------------
//
// RamDisk
//
// Gui for ramdisk
//
//----------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <commctrl.h>
#include <tchar.h>
#include <winioctl.h>
#include <process.h>
#include <dbt.h>	// for broadcast
#include "resource.h"  
#include "instdrv.h"
#include "..\common.h"

#define TESTING 0

// Variables/definitions for the driver that performs the actual monitoring.
#define				SYS_FILE		TEXT("RAMDISK.SYS")
#define				SYS_NAME		TEXT("RAMDISK")
static HANDLE		sys_handle		= INVALID_HANDLE_VALUE;

// General buffer for storing temporary strings
static TCHAR		msgbuf[ 257 ];

// Application instance handle
HINSTANCE			hInst;


// Forwards
LONG APIENTRY MainDialog( HWND hDlg, UINT message, UINT wParam, LONG lParam );
BOOL LockVolume( HANDLE hDisk );
BOOL UnlockVolume( HANDLE hDisk );
BOOL DismountVolume( HANDLE hDisk );
void DriverDisconnect(void);


//======================================================================
//
// MISCELLANEOUS SUPPORT ROUTINES
//
//======================================================================


//----------------------------------------------------------------------
//  
// CenterWindow
// 
// Centers the Window on the screen.
//
//----------------------------------------------------------------------
VOID CenterWindow( HWND hDlg )
{
	RECT            aRt;

	// center the dialog box
	GetWindowRect( hDlg, &aRt );
	OffsetRect( &aRt, -aRt.left, -aRt.top );
	MoveWindow( hDlg,
			((GetSystemMetrics( SM_CXSCREEN ) -
				aRt.right ) / 2 + 4) & ~7,
  			(GetSystemMetrics( SM_CYSCREEN ) -
				aRt.bottom) / 2,
			aRt.right, aRt.bottom, 0 );
}


//----------------------------------------------------------------------
//  
// CenterChild
// 
// Centers the Window within the parent. This assumes the child is 
// smaller than the parent.
//
//----------------------------------------------------------------------
VOID CenterChild( HWND Parent, HWND Child )
{
	RECT		cRt, pRt, sRt;
	LONG		left, top;
	LONG		cheight, pheight, cwidth, pwidth;

	GetWindowRect( Parent, &pRt );
	GetWindowRect( Child, &cRt );
	GetWindowRect( GetDesktopWindow(), &sRt );
	
	pwidth = pRt.right - pRt.left;
	cwidth = cRt.right - cRt.left;
	left   = pRt.left + (pwidth - cwidth)/2;
	if( left + cwidth > sRt.right )
		left = sRt.right - cwidth - 5;

	pheight = pRt.bottom - pRt.top;
	cheight = cRt.bottom - cRt.top;
	top     = pRt.top + (pheight - cheight)/2;
	if( top + cheight > sRt.bottom )
		top = sRt.bottom - cheight - 5;

	SetWindowPos( Child, HWND_TOP, left, top, 
					0, 0, SWP_NOSIZE );
}


//----------------------------------------------------------------------
//  
// Abort
//
// Throw a pretty message box and quit.
//
//----------------------------------------------------------------------
void Abort( HWND hWnd, TCHAR *Msg1, TCHAR * Msg2, BOOL Exit )
{
	TCHAR msg[256];

	wsprintf( msg, TEXT("%s\n\n%s"), Msg1, Msg2 );
	MessageBox( hWnd, msg, TEXT("RamDisk"), MB_ICONEXCLAMATION|MB_OK );
	if( Exit ) {
		DriverDisconnect();
		PostQuitMessage( 1 );
	}
}	


//----------------------------------------------------------------------
//  
// AboutDlgProc
//
// Pops up the standard About dialog box.
//
//----------------------------------------------------------------------
BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, UINT wParam,
                       		LONG lParam) {
	switch (message) {
	case WM_INITDIALOG :
		// center the dialog
#if 0
		if( GetParent( hDlg ) )
			CenterChild( GetParent( hDlg ), hDlg );
		else
			CenterWindow( hDlg );
#endif
		return TRUE ;
 
	case WM_COMMAND :
		switch (wParam) {

		case IDOK :
			EndDialog (hDlg, 0) ;
			return TRUE ;
		}
		break; 

	case WM_CLOSE:	
		EndDialog (hDlg, 0);
		return TRUE;
	}
	return FALSE ;
}



//======================================================================
//
// DRIVER ROUTINES
//
//======================================================================

//----------------------------------------------------------------------
//  
// DriverConnect
//
// Load the driver.
//
//----------------------------------------------------------------------
BOOL DriverConnect( void )
{
#if TESTING
	return TRUE;
#else
	TCHAR		Path[256];
	PVOID		lpMsgBuf;

	// open the handle to the device
	GetCurrentDirectory( sizeof Path, Path );
	wsprintf( Path+lstrlen(Path), TEXT("\\%s"), SYS_FILE );
	if ( ! LoadDeviceDriver( SYS_NAME, Path, &sys_handle ) )  {
		wsprintf( msgbuf, TEXT("Error opening %s (%s):"), SYS_NAME, Path );
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, GetLastError(), 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPTSTR) &lpMsgBuf, 0, NULL );
		Abort( NULL, msgbuf, lpMsgBuf, TRUE );
		LocalFree( lpMsgBuf );
		return FALSE;
	}
	return TRUE;
#endif
}

//----------------------------------------------------------------------
//  
// DriverDisconnect
//
// If under NT 4.0, we need to establish system call hooking in order
// to make the changeover.
//
//----------------------------------------------------------------------
void DriverDisconnect()
{
	UnloadDeviceDriver( SYS_NAME );
}

//----------------------------------------------------------------------
//  
// CallDriver
//
// Sends a DevIoControl to the driver along with some data, 
// if necessary. Don't pop up message box if we're just getting stats
// from the driver and the device isn't ready - otherwise we get 
// a message box every second!
//
//----------------------------------------------------------------------
BOOL CallDriver( int Msg, PBYTE inData, int inDataLen, 
				PBYTE outData, int outDataLen )
{
#if TESTING
	return TRUE;
#else
	PVOID		lpMsgBuf;
	int			nb;
	int			error;
	
	if ( ! (error = DeviceIoControl( sys_handle, Msg,
							inData, inDataLen, outData, outDataLen, &nb, NULL )) )
	{
		// if not being called from stats and error isn't device ready, print message
		wsprintf( msgbuf, TEXT("RamDisk driver error:"));
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, GetLastError(), 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPTSTR) &lpMsgBuf, 0, NULL );
		Abort( NULL, msgbuf, lpMsgBuf, FALSE );
		LocalFree( lpMsgBuf );
		return FALSE;
	}
	return TRUE;
#endif
}


//----------------------------------------------------------------------
//
// LockVolume
//
// Locks the floppy for direct writing.
//
//----------------------------------------------------------------------
BOOL LockVolume( HANDLE hDisk )
{
    DWORD ReturnedByteCount;

    return DeviceIoControl(	hDisk,
                FSCTL_LOCK_VOLUME,
                NULL,0, NULL,
                0, &ReturnedByteCount,
                NULL );
}


//----------------------------------------------------------------------
//
// UnlockVolume
//
//
//----------------------------------------------------------------------
BOOL UnlockVolume( HANDLE hDisk )
{
    DWORD ReturnedByteCount;

    return DeviceIoControl( hDisk,
                FSCTL_UNLOCK_VOLUME, NULL,
                0, NULL, 0,&ReturnedByteCount,
                NULL );
}


//----------------------------------------------------------------------
//
// Dismount Volume
//
// Used to disconnect from the mapped client drives
//
//----------------------------------------------------------------------
BOOL DismountVolume( HANDLE hDisk )
{
	DWORD ReturnedByteCount;

	return DeviceIoControl( hDisk, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL,
							0, &ReturnedByteCount, NULL );
}


//----------------------------------------------------------------------
//  
// Broadcast
//
// Tell the system to take another look at the mounted drive letters.
//
//----------------------------------------------------------------------
BOOL Broadcast( DWORD drive, DWORD msg )
{
	DEV_BROADCAST_VOLUME	hdr;
	HMODULE					userlib;
	FARPROC					broadcastmsg;

	userlib = LoadLibrary( TEXT("User32.dll") );
	if( !(broadcastmsg = GetProcAddress( userlib, "BroadcastSystemMessage")))
		return FALSE;

	hdr.dbcv_size		= sizeof hdr; 
	hdr.dbcv_devicetype = DBT_DEVTYP_VOLUME; 
	hdr.dbcv_reserved	= 0;
	hdr.dbcv_unitmask	= 1 << drive; 
	hdr.dbcv_flags		= DBTF_MEDIA; /* or something */

	return (broadcastmsg( BSF_NOHANG|BSF_POSTMESSAGE,
							NULL,
							WM_DEVICECHANGE,
							msg,
							(LPARAM) &hdr ) != FALSE);

}

//----------------------------------------------------------------------
//  
// RamdiskMount
//
// Initiates a new connection to the client
//
//----------------------------------------------------------------------
BOOL RamdiskMount( HWND hDlg, struct CREATE_PARMS * parms )
{
	if ( ! CallDriver( IOCTL_CREATE, (PVOID) parms, sizeof *parms, NULL, 0 ) )
		return FALSE;
	Broadcast( parms->DriveLetter - 'A', DBT_DEVICEARRIVAL );
	return TRUE;
}

//----------------------------------------------------------------------
//  
// RamdiskUnmount
//
// Disconnects from a client.
// 
//----------------------------------------------------------------------
BOOL RamdiskUnmount( HWND hDlg, WCHAR driveletter )
{
	HANDLE			hDrive;

	// Let apps prepare for lock and dismount
	Broadcast( driveletter-'A', DBT_DEVICEREMOVEPENDING );

	// Open handle to drive
	wsprintf( msgbuf, TEXT("\\\\.\\%c:"), driveletter );
	hDrive = CreateFile( msgbuf,
						  GENERIC_READ | GENERIC_WRITE,
						  FILE_SHARE_READ | FILE_SHARE_WRITE,
						  NULL, OPEN_EXISTING, 0, NULL );

	if ( hDrive == INVALID_HANDLE_VALUE )  {
		wsprintf( msgbuf, TEXT("Cannot open drive %c: for unmount (error %d)"), driveletter, GetLastError() );
		MessageBox( hDlg, msgbuf, TEXT("RamDisk"), MB_OK|MB_ICONEXCLAMATION );
		return FALSE;
	}

	while ( ! LockVolume( hDrive ) ) {
		wsprintf( msgbuf, TEXT("Cannot lock drive %c in preperation for unmount.\n")
						  TEXT("Ensure that there are no open files on the drive.\n")
						  TEXT("Error #%d."), driveletter, GetLastError() );
		if ( MessageBox( hDlg, msgbuf, TEXT("RamDisk"), 
					MB_RETRYCANCEL|MB_APPLMODAL|MB_ICONSTOP ) == IDCANCEL )
		{
			CloseHandle( hDrive );
			return FALSE;
		}
	}

	// Dismount volumes
	if ( ! DismountVolume( hDrive ) ) {
		MessageBox( hDlg, "Cannot unmount ram disk", TEXT("RamDisk"), MB_OK|MB_ICONEXCLAMATION );
		UnlockVolume( hDrive );
		CloseHandle( hDrive );
		return FALSE;
	}

	/* Success.  Delete drive letter. */
	CallDriver( IOCTL_REMOVE, NULL, 0, NULL, 0 );

	/* Indicate drives are gone */
	Broadcast( driveletter-'A', DBT_DEVICEREMOVECOMPLETE );

	// Unlock drive
	UnlockVolume( hDrive );
	CloseHandle( hDrive );

	/* Success.  Delete driver device. */
	CallDriver( IOCTL_DELETE, NULL, 0, NULL, 0 );

	return TRUE;
}




//======================================================================
//
// MAIN DIALOG AND WINMAIN
//
//======================================================================


//----------------------------------------------------------------------
//  
// MainDialog
//
// This is the main window. It presents the statistics to the user
// about VCache.
//
//----------------------------------------------------------------------
LONG APIENTRY MainDialog( HWND hDlg, UINT message, UINT wParam,
                       		LONG lParam )
{
	static struct CREATE_PARMS	parms;
	static BOOL					disk_open = FALSE;
	BOOL						okay;

	switch (message) {

	case WM_INITDIALOG :
		SetDlgItemInt( hDlg, IDC_ROOTDIRECTORYENTRIES, 512, FALSE );
		SetDlgItemInt( hDlg, IDC_SECTORSPERCLUSTER, 1, FALSE );;
		SetDlgItemInt( hDlg, IDC_DISKSIZE, 1024, FALSE );
		{
			DWORD	driveset = GetLogicalDrives();
			DWORD	drive;
			for ( drive = 3; drive < 26; ++drive )  {
				if ( (driveset & (1 << drive)) == 0 )  {
					wsprintf( msgbuf, "%c:", 'A'+drive );
					SendMessage( GetDlgItem(hDlg, IDC_DRIVELETTER), CB_ADDSTRING, 0, (LPARAM)msgbuf );
				}
			}
			SendMessage( GetDlgItem(hDlg, IDC_DRIVELETTER), CB_SETCURSEL, 0, 0 );
		}
		return 1;

	case WM_COMMAND :
		switch (LOWORD( wParam )) {

		case IDC_CREATE:
			parms.nRootDirEntries = GetDlgItemInt( hDlg, IDC_ROOTDIRECTORYENTRIES, &okay, FALSE );
			if ( okay )
				parms.nSectorsPerCluster = GetDlgItemInt( hDlg, IDC_SECTORSPERCLUSTER, &okay, FALSE );;
			if ( okay )
				parms.DiskSize = 1024 * GetDlgItemInt( hDlg, IDC_DISKSIZE, &okay, FALSE );
			if ( okay )  {
				LRESULT idx = SendMessage( GetDlgItem(hDlg, IDC_DRIVELETTER), CB_GETCURSEL, 0, 0 );
				if ( idx != CB_ERR )  {
					SendMessage( GetDlgItem(hDlg, IDC_DRIVELETTER), CB_GETLBTEXT, idx, (LPARAM)msgbuf );
					parms.DriveLetter = msgbuf[0];
					if ( parms.DriveLetter < 'A'  ||  parms.DriveLetter > 'Z' )
						okay = FALSE;
				} else {
					okay = FALSE;
				}
			}
			if ( !okay )  {
				MessageBeep( MB_ICONEXCLAMATION );
				break;
			}
			
			if ( RamdiskMount( hDlg, &parms ) )  {
				disk_open = TRUE;
				EnableWindow( GetDlgItem(hDlg, IDC_CREATE), FALSE );
				EnableWindow( GetDlgItem(hDlg, IDC_ROOTDIRECTORYENTRIES), FALSE );
				EnableWindow( GetDlgItem(hDlg, IDC_SECTORSPERCLUSTER), FALSE );
				EnableWindow( GetDlgItem(hDlg, IDC_DISKSIZE), FALSE );
				EnableWindow( GetDlgItem(hDlg, IDC_DRIVELETTER), FALSE );

				EnableWindow( GetDlgItem(hDlg, IDC_REMOVE), TRUE );
			}
			break;

		case IDC_REMOVE:
			if ( RamdiskUnmount( hDlg, parms.DriveLetter ) )  {
				disk_open = FALSE;
				EnableWindow( GetDlgItem(hDlg, IDC_CREATE), TRUE );
				EnableWindow( GetDlgItem(hDlg, IDC_ROOTDIRECTORYENTRIES), TRUE );
				EnableWindow( GetDlgItem(hDlg, IDC_SECTORSPERCLUSTER), TRUE );
				EnableWindow( GetDlgItem(hDlg, IDC_DISKSIZE), TRUE );
				EnableWindow( GetDlgItem(hDlg, IDC_DRIVELETTER), TRUE );

				EnableWindow( GetDlgItem(hDlg, IDC_REMOVE), FALSE );
			}
			break;

		
		case IDC_ABOUT:
			DialogBox( hInst, TEXT("ABOUT"), hDlg, (DLGPROC)AboutDlgProc );
			break;
		
		}
		break; 



	case WM_CLOSE:
		// Don't exit with disk open
		if ( disk_open )  {
			MessageBox( hDlg, "You must dismount the RamDisk before exiting.", 
						"Compuware RamDisk", MB_OK|MB_ICONASTERISK );
			return 0;
		}
		EndDialog (hDlg, 0);
		PostQuitMessage( 0 );
		return 1;
	}

    return 0;
}


//----------------------------------------------------------------------
//  
// WinMain
//
// Registers a class. The reason I did this is because the application
// doesn't get cleaned up properly upon exit if winmain just calls
// dialogbox. This was manifested by all the system icons disappearing
// after the program exited. See Petzold's hexcalc example for the base
// of this.
//
//----------------------------------------------------------------------
int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
        				LPSTR lpCmdLine, int nCmdShow )
{
#if 0
	MSG				msg ;
	WNDCLASSEX		wndclass ;
	HWND			hMainDlg;
#endif

	// do init stuff
	InitCommonControls();
	hInst = hInstance;

	// load the driver
	DriverDisconnect();
	if( ! DriverConnect() ) {
		DriverDisconnect();
		return 1;
	}

#if 0
	// create the main window class
	wndclass.cbSize			= sizeof( WNDCLASSEX );
	wndclass.style          = CS_HREDRAW | CS_VREDRAW ;
 	wndclass.lpfnWndProc    = (WNDPROC) MainDialog ;
	wndclass.cbClsExtra     = 0 ;
	wndclass.cbWndExtra     = DLGWINDOWEXTRA ;
	wndclass.hInstance      = hInstance ;
	wndclass.hIcon          = LoadIcon (hInstance, TEXT("ICON")) ;
	wndclass.hIconSm		= LoadIcon (hInstance, TEXT("ICON"));
	wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground  = (HBRUSH) (COLOR_BTNFACE+1);
	wndclass.lpszMenuName   = NULL ;
	wndclass.lpszClassName  = TEXT("Ramdisk") ;
	RegisterClassEx (&wndclass) ;

 	// create the dialog
	hMainDlg = CreateDialog( hInstance, TEXT("Main"), NULL, (DLGPROC)MainDialog);
	CenterWindow( hMainDlg );
	ShowWindow( hMainDlg, SW_SHOWNORMAL );

	while (GetMessage (&msg, NULL, 0, 0)) {
		if( !IsDialogMessage( hMainDlg, &msg )) {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
		}
	}
	return msg.wParam ;
#else
	return (int)DialogBox( hInstance, TEXT("MAIN"), NULL, (DLGPROC)MainDialog );
#endif
}


