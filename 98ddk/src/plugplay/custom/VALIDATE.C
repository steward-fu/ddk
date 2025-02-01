/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		validate.c
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


extern char *szVfilterInterface;


char *szRegKeyBindings  = "Bindings";
char *szVnic			= "VNIC";
char *szNull			= "";


RETERR PRIVATE FilterNdiValidate
( 
	HNDI hNdi,
	HWND hwndParent
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiValidate
//
//	Description:
//		Responds to the NDI_VALIDATE message.  The installer should check that
//		all properties are correctly set or have valid default values.  If
//		a required parameter is not known and cannot be defaulted, then the
//		installer should prompt the user for the value
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return ERR_NDI_CANCEL.
//
/////////////////////////////////////////////////////////////////////////////
{
	RETERR			reError = OK;
    LPDEVICE_INFO	lpdi;
    HKEY			hKey, hKeyBindings;
    HNDI 			hNdiFind = NULL;
    char 			szNdiText[ MAX_RES ];
    WORD 			wNetClass;
	
	TRACE_NDIENTER( "FilterNdiValidate", hNdi );

	// Let's send some creation information to the debug monitor
	FilterDisplayStatus( hNdi, DEBUG_LEVEL_RETAIL );
	
	NdiGetClass( hNdi, &wNetClass );
	
	if ( wNetClass == NDI_CLASS_TRANS )
	{
		// Let's look for vnic
	
		for ( hNdiFind = NdiGetFirst(); hNdiFind; hNdiFind = NdiGetNext( hNdiFind ) )
		{
		 	NdiGetDeviceId( hNdiFind, szNdiText, sizeof( szNdiText ) );
		 	
			if ( ( hNdi != hNdiFind ) && ( lstrcmp( szNdiText, szVnic ) == 0 ) )
			{
				_asm { int 3 }
			
				// found it!
				debugprintf( DEBUG_LEVEL_RETAIL, "Found %s to bind to VFILTER", szNdiText );
				
				NdiBind( hNdiFind, hNdi );
				break;
			}
		}
	
		if ( hNdiFind )
		{
			// We want to create a binding from vfilter to vnic
			reError  = NdiGetDeviceInfo( hNdi, &lpdi );
			
			if ( reError == OK  && lpdi )
				reError = DiOpenDevRegKey( lpdi, &hKey, DIREG_DEV );
			
			if ( reError == OK )
				reError = SURegCreateKey( hKey, szRegKeyBindings, &hKeyBindings );
			else
				hKey = NULL;
			
			if ( reError == OK )
			{
				lstrcpy( szNdiText, szVnic );
				lstrcat( szNdiText, "\\0000" );
				
				reError = SURegSetValueEx(	hKeyBindings, 
				                            szNdiText,
				                            0,
				                            REG_SZ,
				                            szNull,
				                            sizeof ( szNull ) );
				                        
				if ( reError == OK )
					debugprintf( DEBUG_LEVEL_RETAIL, "Binding %s Created", szNdiText );				                        

				SURegCloseKey( hKeyBindings );
			}
			
			if ( hKey )
				SURegCloseKey( hKey );
		}
	}
	else
	{
		reError = DefNdiProc( hNdi, NDI_VALIDATE, (WPARAM)hwndParent, (LPARAM)0 );
	}

	TRACE_NDILEAVE( "FilterNdiValidate", reError );	
	
	return reError;
}


RETERR PRIVATE FilterNdiSystemChange
( 
	HNDI hNdi, 
	UINT uTypeOfChange 
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiSystemChange
//
//	Description:
//		Responds to the NDI_SYSTEMCHANGE message.  The installer should 
//		validate its settings and/or bindings.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		uTypeOfChange
//			NDISYSTEMCHANGE_ADD		- an object was added to the network
//			NDISYSTEMCHANGE_REMOVE	- an object was removed from the network
//
//	Return:
//		Returns Ok if successful.  If any property is invalid the installer
//		should return an NDI error value.
//
/////////////////////////////////////////////////////////////////////////////
{
	TRACE_NDIENTER( "FilterNdiSystemChange", hNdi );
	
	debugprintf( DEBUG_LEVEL_DEBUG, "NdiSystemChange %u", uTypeOfChange );

	TRACE_NDILEAVE( "FilterNdiSystemChange", OK );	
	
	return OK;
}
