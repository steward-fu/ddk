/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		install.c
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


RETERR PRIVATE FilterNdiInstall
( 
	HNDI	hNdi, 
	DWORD	dwFlags
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiRemove
//
//	Description:
//		Responds to the NDI_INSTALL message sent when a NDI manager performs
//		modifications for the driver.  The NdiDefProc should also be called
//		to perform other necessary registry updates.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		dwFlags
//			NDIF_ADDED					The driver has been added
//			NDIF_REMOVED				The driver has been removed
//			NDIF_MODIFIED_BINDINGS		The driver's bindings have been changed
//			NDIF_MODIFIED_PROPERTIES	The driver's properties have been changed
//
//	Return:
//		Returns OK if successful.  Otherwise it returns an NDI error value
//
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_NDIENTER( "FilterNdiInstall", hNdi );

	debugprintf( DEBUG_LEVEL_DEBUG, "NdiInstallFlags %lx", dwFlags );

	// Let's send some creation information to the debug monitor
	FilterDisplayStatus( hNdi, DEBUG_LEVEL_DEBUG );

	TRACE_NDILEAVE( "FilterNdiInstall", OK );
	
	return OK;
}



RETERR PRIVATE FilterNdiRemove
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiRemove
//
//	Description:
//		Responds to the NDI_REMOVE message sent when an NDI object is 
//		removed from the network.  The NdiDefProc should also be called
//		to perform other necessary registry updates.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns OK.
//
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_NDIENTER( "FilterNdiRemove", hNdi );

	TRACE_NDILEAVE( "FilterNdiRemove", OK );
	
	return OK;
}
