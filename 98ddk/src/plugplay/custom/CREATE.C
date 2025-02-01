/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		create.c
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
#include <string.h>
#include <malloc.h>


ASSERTFILE;


RETERR PRIVATE FilterNdiFirstTimeSetup
(
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiFirstTimeSetup
//
//	Description:
//		Responds to the NDI_FIRSTTIMESETUP message sent whenever the system
//		sends the DIF_FIRSTTIMESETUP message to the network class installer.
//		This allows the driver to perform any special upgrade or first time
//		configuration operations.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		dwSetupflags
//			SUF_FIRSTTIME	First boot into Win95
//			SUF_EXPRESS		User Setup via express mode (vs customize)
//			SUF_BATCHINF	Setup using batch file (e.g. MSBATCH.INF) 
//			SUF_CLEAN		Setup was done to a clean directory
//			SUF_INSETUP		You're in Setup
//			SUF_NETSETUP	Doing a net (workstation) setup
//			SUF_NETHDBOOT	Workstation boots from a local hard drive
//			SUF_NETRPLBOOT	Workstation boots via RPL (vs floppy)
//			SUF_SBSCOPYOK	Can copy to LDID_SHARED (SBS)
//
//	Return:
//		The return value is zero (OK) if successful, otherwise it is a nonzero
//		NDI error value
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_NDIENTER( "FilterNdiFirstTimeSetup", hNdi );
	
	TRACE_NDILEAVE( "FilterNdiFirstTimeSetup", OK );
	
	return OK;
}


RETERR PRIVATE FilterNdiCreate
( 
	HNDI   hNdi, 
	HINF   hInfBatch, 
	LPARAM flags
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiCreate
//
//	Description:
//		Responds to the NDI_CREATE message sent when a new NDI object is being 
//		created after all internal structures have been initialized.  We create
//		instance data that will be referenced later.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		hInfBatch
//			if NDI_CREATE_BATCHMODE flag is set the this is a handle that can 
//			be used to process setup batch scripts.
//		flags
//			NDI_CREATE_BATCHMODE	Setup is being run with a setup batch script
//			NDI_CREATE_NODISPLAY	do not display any UI ( Setup is running automated )
//
//	Return:
//		Returns Ok to continue creating object.  If this procedure returns
//		nonzero, the NDI object will be destroyed and NdiCreate will return
//		the same value.
/////////////////////////////////////////////////////////////////////////////
{
	LPFILTERPROPS	lpFilterProps;
	HGLOBAL			hFilterProps;
	
	TRACE_NDIENTER( "FilterNdiCreate", hNdi );
	
	// allocate instance data
	hFilterProps = GlobalAlloc( GPTR, sizeof( FILTERPROPS ) );
	
	if ( hFilterProps )
	{
		lpFilterProps = (LPFILTERPROPS) GlobalLock( hFilterProps );
	    
	    if ( lpFilterProps )
	    {
			// Initialize properties
			memset( lpFilterProps, 0, sizeof( FILTERPROPS ) );
			lpFilterProps->hMem		= hFilterProps;
			lpFilterProps->hInf		= hInfBatch;
			lpFilterProps->suFlags	= flags;
			
			// Obtain Filter Settings here
			lpFilterProps->pOriginalSettings = malloc( sizeof( FILTERPROPS ) );
			
			if ( lpFilterProps->pOriginalSettings )
			{
				// Make a copy of the orginal settings
				// This is used if the operations are cancelled
				memmove( lpFilterProps->pOriginalSettings,
					 	 lpFilterProps,
						 sizeof( FILTERPROPS ) );

				// Set the properties data associated with this NDI object	                                        
				NdiSetProperties( hNdi, lpFilterProps );
				
				// Display Class
				debugprintf( DEBUG_LEVEL_RETAIL, "\tNDI Class          : %s", FilterGetDeviceClass( hNdi ) );
	
				debugprintf( DEBUG_LEVEL_RETAIL, "\tNDI szRegSubKey    : %s", FilterGetRegSubKey( hNdi ) );
	
				TRACE_NDILEAVE( "FilterNdiCreate", OK );
				return OK;
			}
			
			GlobalUnlock( hFilterProps );
		}
		
		GlobalFree( hFilterProps );
	}
	
	TRACE_NDILEAVE( "FilterNdiCreate", ERR_NDI_LOW_MEM );
	return ERR_NDI_LOW_MEM;
}


RETERR PRIVATE FilterNdiDestroy
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiDestroy
//
//	Description:
//		Responds to the NDI_DESTROY message.  We release instance data.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns Ok if it processes this message.
//
/////////////////////////////////////////////////////////////////////////////
{
	LPFILTERPROPS	lpFilterProps;
	RETERR			reError;
	
	TRACE_NDIENTER( "FilterNdiDestroy", hNdi );
	
	reError = NdiGetProperties( hNdi, &lpFilterProps );
	
	if ( reError == OK )
	{
		if ( lpFilterProps )
		{
			if ( lpFilterProps->pOriginalSettings )
				free( lpFilterProps->pOriginalSettings );
		
			GlobalUnlock( lpFilterProps->hMem );
			GlobalFree( lpFilterProps->hMem );
		}
	}

	TRACE_NDILEAVE( "FilterNdiDestroy", reError );
	return reError;
}


RETERR PRIVATE FilterNdiNdiCreate
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiNdiCreate
//
//	Description:
//		Responds to the NDI_NDICREATE message which is sent when NETDI is
//		about to allocate the object.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns OK when it is acceptable to allocate the object
//
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_NDIENTER( "FilterNdiNdiCreate", hNdi );
	
	// Display Class
	debugprintf( DEBUG_LEVEL_RETAIL, "\tNDI Class          : %s", FilterGetDeviceClass( hNdi ) );
	
	// Display szSubRegKey
	debugprintf( DEBUG_LEVEL_RETAIL, "\tNDI szRegSubKey    : %s", FilterGetRegSubKey( hNdi ) );
	
	TRACE_NDILEAVE( "FilterNdiNdiCreate", OK );

	return OK;
}



RETERR PRIVATE FilterNdiNdiDestroy
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiNdiDestroy
//
//	Description:
//		Responds to the NDI_NDIDESTROY message which is sent when NETDI is
//		about to deallocate the object.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Always Returns Ok
//
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_NDIENTER( "FilterNdiNdiDestroy", hNdi );
	
	TRACE_NDILEAVE( "FilterNdiNdiDestroy", OK );

	return OK;
}
