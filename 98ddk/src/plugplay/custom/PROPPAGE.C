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


#include <vfilter.h>  


ASSERTFILE;


RETERR PRIVATE FilterNdiHasPropPages
( 
	HNDI hNdi
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiHasPropPages
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
	TRACE_ENTER( "FilterNdiHasPropPages" );
	
	TRACE_NDILEAVE( "FilterNdiHasPropPages", FALSE );
	
	return FALSE;
}


RETERR PRIVATE FilterNdiAddPropPages
(
	HNDI				hNdi, 
	LPNDIADDPROPPAGES	lpNdiApp
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiAddPropPages
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
	TRACE_ENTER( "FilterNdiAddPropPages" );
	
	TRACE_NDILEAVE( "FilterNdiAddPropPages", OK );
	
	return OK;
}
