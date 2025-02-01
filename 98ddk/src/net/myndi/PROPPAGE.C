/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//              proppage.c
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


ASSERTFILE;


#ifdef _DEBUG

char *szMyNdiHasPropPages	= "MyNdiHasPropPages";
char *szMyNdiAddPropPages	= "MyNdiAddPropPages";

#endif


RETERR PRIVATE MyNdiHasPropPages
( 
	HNDI hNdi
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiHasPropPages
//
//	Description:
//		Returns a Boolean value that indicates whether this ndi procedure
//		wants to display custom property pages.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns TRUE if it has property pages of its own, or FALSE if not.
//		
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_ENTER( szMyNdiHasPropPages );
	
	TRACE_NDILEAVE( szMyNdiHasPropPages, TRUE );
}


RETERR PRIVATE MyNdiAddPropPages
(
	HNDI				hNdi, 
	LPNDIADDPROPPAGES	lpNdiApp
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiAddPropPages
//
//	Description:
//		This function fills out a PROPSEETPAGE structure (psp) and passes
//		it to the call back function. Use
//		(*lpNdiApp->lpfnAddNdiPropPage) ( &psp, lpNdiApp->lParam, FALSE );
//		The last Boolean parameter specifies whether you want this page to 
//		the head or tail of the list of pages.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		lpNdiApp
//			Pointer to NDIADPROPPAGES structure for adding a property sheet
//
//	Return:
//		Returns OK if the page(s) were successfully added
//		Otherwise returns a nonzero NDI error value
//		
/////////////////////////////////////////////////////////////////////////////
{
	PROPSHEETPAGE	psp;
	LPMYNDIPROPS	lpMyNdiProps;
	RETERR			reError;
	
	TRACE_ENTER( szMyNdiAddPropPages );
	
	reError = NdiGetProperties( hNdi, &lpMyNdiProps );
	
	if ( reError == OK && lpMyNdiProps )
	{
		psp.dwSize		= sizeof( PROPSHEETPAGE );
		psp.dwFlags		= PSP_DEFAULT | PSP_USETITLE;
		psp.hInstance	= ghInstance;
		psp.lParam		= (LPARAM) lpMyNdiProps;
		
		psp.pszTemplate	= MAKEINTRESOURCE( IDD_VNBT_DIALOG );
		psp.pfnDlgProc	= (DLGPROC) VnbtDlgProc;
		psp.pszTitle	= MAKEINTRESOURCE( IDS_VNBT_TITLE );
		
		// lpfnAddNdiPropPage returns TRUE if the page is added
		reError = !(*lpNdiApp->lpfnAddNdiPropPage)( &psp, lpNdiApp->lParam, FALSE );
	}

	TRACE_NDILEAVE( szMyNdiAddPropPages, reError );
}
