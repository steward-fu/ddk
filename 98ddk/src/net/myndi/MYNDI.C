/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//              myndi.c
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


HINSTANCE   ghInstance = NULL;


RETERR _loadds WINAPI MyNdiProc
(
	HNDI	hNdi,
	UINT	uMsg,
	WPARAM	wParam,
	LPARAM	lParam
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiProc
//
/////////////////////////////////////////////////////////////////////////////
{
    RETERR reError = OK;

	switch ( uMsg )
	{
	case NDI_CREATE:
		reError = MyNdiCreate( hNdi, (HINF) wParam, lParam );
        break;
        
	case NDI_DESTROY:
		reError = MyNdiDestroy( hNdi );
		break;

	case NDI_VALIDATE:
		reError = MyNdiValidate( hNdi, (HWND) wParam );
		break;
		
	case NDI_SYSTEMCHANGE:
		reError = MyNdiSystemChange( hNdi, (UINT) wParam );
		break;
	
	case NDI_INSTALL:
		reError = MyNdiInstall( hNdi, (DWORD) lParam );
		break;
		
	case NDI_REMOVE:
		reError = MyNdiRemove( hNdi );
		break;
		
	case NDI_HASPROPPAGES:
		return MyNdiHasPropPages( hNdi );

	case NDI_ADDPROPPAGES:
		return MyNdiAddPropPages( hNdi, (LPNDIADDPROPPAGES) lParam );
		
	case NDI_FIRSTTIMESETUP:
		MyNdiFirstTimeSetup( hNdi );
		break;
		
	case NDI_QUERY_BIND:
		return MyNdiQueryBind( hNdi, (HNDI)wParam, (UINT)LOWORD(lParam) );
		break;

	case NDI_NOTIFY_BIND:
		reError = MyNdiBind( hNdi, (HNDI)wParam, (UINT)LOWORD(lParam) );
		break;
		
	case NDI_NOTIFY_UNBIND:
		reError = MyNdiUnbind( hNdi, (HNDI)wParam, (UINT)LOWORD(lParam) );	
		break;

	case NDI_NDICREATE:
		reError = MyNdiNdiCreate( hNdi );
		break;
		
	case NDI_NDIDESTROY:
		reError = MyNdiNdiDestroy( hNdi );
		break;
		
	case NDI_GETTEXT:
	case NDI_SETTEXT:
	default:
		break;
	}
	
	if ( reError == OK ) 
		reError = DefNdiProc( hNdi, uMsg, wParam, lParam);
		
	return reError;		
}


int CALLBACK LibMain
(
    HINSTANCE   hinst,              // module instance handle
    WORD        wDataSeg,           // data segment
    WORD        cbHeapSize,         // heap size
    LPSTR       lpszCmdLine         // command line
)
/////////////////////////////////////////////////////////////////////////////
//
//  LibMain     Called by system to initialize the DLL.
//
/////////////////////////////////////////////////////////////////////////////
{
    ghInstance = hinst;
    return 1;
}



BOOL CALLBACK _export _loadds WEP
( 
	BOOL fSystemExit 
)
/////////////////////////////////////////////////////////////////////////////
//
//  WEP     Called by system to unload the DLL.
//
/////////////////////////////////////////////////////////////////////////////
{
    // Nothing to do.

    return TRUE;
}
