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


#include <myndi.h>
#include <string.h>
#include <malloc.h>


ASSERTFILE;


#ifdef _DEBUG

char *szMyNdiFirstTimeSetup = "MyNdiFirstTimeSetup";
char *szMyNdiCreate			= "MyNdiCreate";
char *szMyNdiDestroy		= "MyNdiDestroy";
char *szMyNdiNdiCreate		= "MyNdiNdiCreate";
char *szMyNdiNdiDestroy		= "MyNdiNdiDestroy";

#endif  // _DEBUG



RETERR PRIVATE MyNdiFirstTimeSetup
(
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiFirstTimeSetup
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
#ifdef MAXDEBUG
	TRACE_NDIENTERWMSG( szMyNdiFirstTimeSetup, hNdi );
#else
	TRACE_NDIENTER( szMyNdiFirstTimeSetup, hNdi );
#endif
	
	TRACE_NDILEAVE( szMyNdiFirstTimeSetup, OK );
	
	return OK;
}


RETERR PRIVATE MyNdiCreate
( 
	HNDI   hNdi, 
	HINF   hInfBatch, 
	LPARAM flags
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiCreate
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
	LPMYNDIPROPS	lpMyNdiProps, lpOriginalSettings;
	HGLOBAL			hMemNdiProps, hMemOriginal;
	RETERR			reError = ERR_NDI_LOW_MEM;

#ifdef MAXDEBUG	
	TRACE_NDIENTERWMSG( szMyNdiCreate, hNdi );
#else
	TRACE_NDIENTER( szMyNdiCreate, hNdi );
#endif
	
	// allocate instance data
	hMemNdiProps = GlobalAlloc( GPTR, sizeof( MYNDIPROPS ) );
	hMemOriginal = GlobalAlloc( GPTR, sizeof( MYNDIPROPS ) );
	
	if ( hMemNdiProps && hMemOriginal )
	{
		lpMyNdiProps		= (LPMYNDIPROPS) GlobalLock( hMemNdiProps );
		lpOriginalSettings	= (LPMYNDIPROPS) GlobalLock( hMemOriginal );
	    
	    if ( lpMyNdiProps && lpOriginalSettings )
	    {
			// Initialize properties
			memset( lpMyNdiProps, 0, sizeof( MYNDIPROPS ) );
			memset( lpOriginalSettings, 0, sizeof( MYNDIPROPS ) );
			
			lpMyNdiProps->hMemNdiProps	= hMemNdiProps;
			lpMyNdiProps->hInfBatch		= hInfBatch;
			lpMyNdiProps->suFlags		= flags;
			
			reError = MyInitializeFileVersionInfo( lpMyNdiProps );
			reError = MyInitializeVnbtInfo( hNdi, lpMyNdiProps );
			
			if ( reError == OK )
			{
				// Make a copy of the orginal settings
				// This is used if the operations are cancelled
				memmove( lpOriginalSettings,
						 lpMyNdiProps,
						 sizeof( MYNDIPROPS ) );
						 
				lpMyNdiProps->lpOriginalSettings	= lpOriginalSettings;
				lpMyNdiProps->hMemOriginal			= hMemOriginal;

				// Set the properties data associated with this NDI object	                                        
				NdiSetProperties( hNdi, lpMyNdiProps );
            
            	// We want to skip releasing the memory until the object is destroyed
				TRACE_NDILEAVE( szMyNdiCreate, reError );
			}
			
			GlobalUnlock( hMemNdiProps );
			GlobalUnlock( hMemOriginal );
		}
		
		GlobalFree( hMemNdiProps );
		GlobalFree( hMemOriginal );
	}
	
	TRACE_NDILEAVE( szMyNdiCreate, reError );
}


RETERR PRIVATE MyNdiDestroy
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiDestroy
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
	LPMYNDIPROPS	lpMyNdiProps;
	RETERR			reError;

#ifdef MAXDEBUG	
	TRACE_NDIENTERWMSG( szMyNdiDestroy, hNdi );
#else
	TRACE_NDIENTER( szMyNdiDestroy, hNdi );
#endif	
	
	reError = NdiGetProperties( hNdi, &lpMyNdiProps );
	
	if ( reError == OK && lpMyNdiProps )
	{
		MyDestroyFileVersionInfo( lpMyNdiProps );
	
		GlobalUnlock( lpMyNdiProps->hMemOriginal );
		GlobalFree( lpMyNdiProps->hMemOriginal );

		GlobalUnlock( lpMyNdiProps->hMemNdiProps );
		GlobalFree( lpMyNdiProps->hMemNdiProps );
	}
	
	TRACE_NDILEAVE( szMyNdiDestroy, reError );
}


RETERR PRIVATE MyNdiNdiCreate
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiNdiCreate
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
	TRACE_NDIENTER( szMyNdiNdiCreate, hNdi );

#ifdef _DEBUG
	// Display Class
	debugprintf( DEBUG_LEVEL_RETAIL, "\tNDI Class          : %s", MyGetDeviceClass( hNdi ) );
	
	// Display szSubRegKey
	debugprintf( DEBUG_LEVEL_RETAIL, "\tNDI szRegSubKey    : %s", MyGetRegSubKey( hNdi ) );
	
#ifdef MAXDEBUG

	{
		char szText[ MAX_MBSTRING ];

		strcpy( szText, szMyNdiNdiCreate );
		strcat( szText, "\t:\r\nClass\t\t: " );
		strcat( szText, MyGetDeviceClass( hNdi ) );
		strcat( szText, "\r\nszRegSubKey\t: " );
		strcat( szText, MyGetRegSubKey( hNdi ) );
	
		MessageBox( NULL, szText, "MyNdi.Dll", MB_OK );
	}
	
#endif

#endif
	
	TRACE_NDILEAVE( szMyNdiNdiCreate, OK );
}


RETERR PRIVATE MyNdiNdiDestroy
( 
	HNDI hNdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiNdiDestroy
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
#ifdef MAXDEBUG	
	TRACE_NDIENTERWMSG( szMyNdiNdiDestroy, hNdi );
#else
	TRACE_NDIENTER( szMyNdiNdiDestroy, hNdi );
#endif	
	
	TRACE_NDILEAVE( szMyNdiNdiDestroy, OK );
}
