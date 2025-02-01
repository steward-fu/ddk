/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997-1998 Microsoft Corporation
//
//	Module Name:
//              vnbtinfo.c
//
//	Abstract:
//
//	Author:
//		William Ingle
//
//	Environment:
//		Windows 95 filter sample 16-bit netdi install dll
//
//	Notes:
//
//	Future:
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////


#include <myndi.h>
#include <string.h>
#include <regdef.h>
#include <regstr.h>


ASSERTFILE;


#ifdef _DEBUG

char *szMyInitializeVnbtInfo	= "MyInitializeVnbtInfo";
char *szApplyVnbtInfo			= "ApplyVnbtInfo";

// #define MAXDEBUG 1

#endif


char *szUseNbt = "UseNetBIOS";


/////////////////////////////////////////////////////////////////////////////
//
//	MyInitializeVnbtInfo
//
//	Description:
//
//	Arguments:
//
//	Return:
//		
/////////////////////////////////////////////////////////////////////////////

RETERR MyInitializeVnbtInfo( HNDI hNdi, LPMYNDIPROPS lpMyNdiProps )
{
    DWORD	dwValue;
	RETERR	reError;
	
	TRACE_ENTER( szMyInitializeVnbtInfo );

	reError = NdiRegQueryValue( hNdi, 
	                            NULL, 
	                            szUseNbt, 
	                            (LPSTR)&dwValue,
	                            sizeof(dwValue) );
	                            
	if ( reError == OK )
		lpMyNdiProps->vnbtInfo.fVnbtInstall = LOWORD( dwValue );		                            
	
	TRACE_NDILEAVE( szMyInitializeVnbtInfo, OK );
}


/////////////////////////////////////////////////////////////////////////////
//
//  ApplyVnbtInfo
//
//	Description:
//
//	Arguments:
//
//	Return:
//		
/////////////////////////////////////////////////////////////////////////////

RETERR ApplyVnbtInfo( HNDI hNdi, LPMYNDIPROPS lpMyNdiProps )
{
	DWORD	dwValue = lpMyNdiProps->vnbtInfo.fVnbtInstall;
	RETERR	reError;

	TRACE_ENTER( szApplyVnbtInfo );
	
	reError = NdiRegSetValue( hNdi, 
	                          NULL, 
	                          szUseNbt, 
	                          REG_BINARY, 
	                          (LPCSTR)&dwValue,
	                          sizeof(dwValue) );
	
	TRACE_NDILEAVE( szApplyVnbtInfo, OK );
}


/////////////////////////////////////////////////////////////////////////////
//
//	VnbtDlgProc
//
//	Description:
//    Handles events for Vnbt dialog
//
//	Arguments:
//		hDlg
//		uMsg
//		wParam
//		lParam
//
//	Return:
//		
/////////////////////////////////////////////////////////////////////////////

LONG CALLBACK VnbtDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LPVNBTINFO	lpVnbtInfo;

    lpVnbtInfo = (LPVNBTINFO) GetWindowLong( hDlg, DWL_USER );

    switch ( uMsg ) 
    {
    case WM_INITDIALOG:
		lpVnbtInfo = &(((LPMYNDIPROPS) ((LPPROPSHEETPAGE) lParam)->lParam)->vnbtInfo);

    	SetWindowLong(	hDlg, 
    					DWL_USER, (LPARAM)
						lpVnbtInfo );
		
		UiMakeDlgNonBold( hDlg );
		return TRUE;

	case WM_DESTROY:
		UiDeleteNonBoldFont(hDlg);
		break;
        
	case WM_NOTIFY:
		switch ( ((NMHDR *) lParam)->code ) 
		{
		case PSN_KILLACTIVE:
			// Save User's choice if they have one
			lpVnbtInfo->fVnbtInstall = IsDlgButtonChecked( hDlg, IDD_VNBT );
			break;                    

		case PSN_SETACTIVE:
			CheckDlgButton( hDlg, IDD_VNBT, lpVnbtInfo->fVnbtInstall );
			EnableWindow( GetDlgItem( hDlg, IDD_VNBT ), TRUE );
			break;

		default:
			break;
		}
		break;

	case WM_HELP:
/*		WinHelp(	(HWND)((LPHELPINFO) lParam)->hItemHandle, 
					HELP_FILE,
                    HELP_WM_HELP, 
                    (DWORD)(LPSTR) aVnbtHelpIDs );
*/		break;

	case WM_CONTEXTMENU:
/*		WinHelp(	(HWND) wParam, 
					HELP_FILE, 
					HELP_CONTEXTMENU,
					(DWORD)(LPVOID) aVnbtHelpIDs );
*/		break;

	default:
		return FALSE; 
	}
    
    return TRUE;
}


#undef MAXDEBUG
