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


#include <myndi.h>


ASSERTFILE;  


RETERR PRIVATE MyNdiInstall
( 
	HNDI	hNdi, 
	DWORD	dwFlags
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiInstall
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
#ifdef _DEBUG
#ifdef MAXDEBUG
	TRACE_NDIENTERWMSG( "MyNdiInstall", hNdi );
#else
	TRACE_NDIENTER( "MyNdiInstall", hNdi );
#endif

	debugprintf( DEBUG_LEVEL_DEBUG, "NdiInstallFlags %lx", dwFlags );
#endif

	// Let's send some creation information to the debug monitor
	MyDisplayStatus( hNdi, DEBUG_LEVEL_DEBUG );
	
	// This is the traditional place to make sure that we have the correct
	// version match between your driver and ndis.  This is where vendor
	// specific install work is supposed to go.
	
	MyCopyFiles( hNdi );

	TRACE_NDILEAVE( "MyNdiInstall", OK );
}



RETERR PRIVATE MyNdiRemove
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiRemove
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
#ifdef MAXDEBUG
	TRACE_NDIENTERWMSG( "MyNdiRemove", hNdi );
#else
	TRACE_NDIENTER( "MyNdiRemove", hNdi );
#endif

	TRACE_NDILEAVE( "MyNdiRemove", OK );
}

#undef MAXDEBUG
