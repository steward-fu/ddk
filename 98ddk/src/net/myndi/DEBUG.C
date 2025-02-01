/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		debug.c
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
#include <stdarg.h>
#include <stdlib.h>
#include <myndi.rcv>

#ifdef _DEBUG

#ifndef _DEBLEVEL
#define _DEBLEVEL DEBUG_LEVEL_RETAIL
#endif

static  DEBUG_LEVEL gDebugLevel = _DEBLEVEL;
static  char        szCaption[ MAX_MBSTRING ];


ASSERTFILE;


char  *MyGetRegSubKey( HNDI hNdi )
/////////////////////////////////////////////////////////////////////////////
//
//	MyGetRegSubKey
//
//	Description:
//		Returns a string with the szRegSubkey of the device referred to by
//		the Ndi Handle.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns a null terminated 'C' string containing the szRegSubkey
//
/////////////////////////////////////////////////////////////////////////////
{
	LPDEVICE_INFO	lpdi;
	RETERR			reError;
	
	static char szRegSubKey[ MAX_DEVNODE_ID_LEN ];
	
	reError = NdiGetDeviceInfo( hNdi, &lpdi );
	
	if ( reError == OK )
	{
		strcpy( szRegSubKey, lpdi->szRegSubkey );
	}
	else
	{
		LoadString( ghInstance, 
    	        	IDS_CLASS_UNKNOWN,
        	    	szRegSubKey, 
            		sizeof( szRegSubKey ) );
	
	}
	
	return szRegSubKey;
}


char *MyGetDeviceClass
( 
	HNDI hNdi
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyGetDeviceClass
//
//	Description:
//		Returns a string with the device class of the device referred to by
//		the Ndi Handle.
//
//	Arguments:
//		hNdi
//			Ndi object handle
//
//	Return:
//		Returns a null terminated 'C' string containing the device class
//
/////////////////////////////////////////////////////////////////////////////
{
	UINT   dwNetClass;
	RETERR reError;

	static char szDeviceClass[ MAX_RES ];
	
	reError = NdiGetClass( hNdi, &dwNetClass );
	
	if ( reError != OK )
		dwNetClass = NDI_CLASS_NET + 4;

	// Get the device class string from the resources
		
	LoadString( ghInstance, 
            	dwNetClass - NDI_CLASS_NET + IDS_CLASS_NET,
            	szDeviceClass, 
            	sizeof( szDeviceClass ) );
	
	return szDeviceClass;
}


RETERR MyDisplayInterfaces
( 
	HNDI	hNdi,
	UINT	uRelation,
	UINT    uDbgLevel
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyDisplayInterfaces
//
//	Description:
//		Displays interfaces on the debug terminal if it exists 
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		uRelation
//			Indicates what type of interface to display.
//		uDbgLevel
//			Display flags
//
//	Return:
//		Returns OK
//
/////////////////////////////////////////////////////////////////////////////
{
	UINT	uIndex = 0;
	char	szNdiText[ MAX_RES ];

	// Display Interface Type Name	
	LoadString( ghInstance, 
	            uRelation-NDI_EDGE_ALL+IDS_EDGE_ALL, 
	            szNdiText, 
	            sizeof( szNdiText ) );

	debugprintf( uDbgLevel, "\t%s Edge Interfaces :", szNdiText );

	while ( NdiGetInterface( hNdi, uRelation, uIndex++, szNdiText, sizeof( szNdiText ) ) == OK )
	{
		debugprintf( uDbgLevel, "\t\t%s", szNdiText );
	}
	            
	return OK;
}


RETERR MyDisplayBinding
( 
	HNDI hNdi, 
	HNDI hNdiBound, 
	UINT uBindType, 
	UINT uDbgLevel 
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyDisplayBinding
//
//	Description:
//		Displays binding information on to the debug terminal if it exists
//
//	Arguments:
//		hNdi
//			Ndi object handle of target
//		hNdiBoundTo
//			Ndi object handle of binding
//		uBindType
//			NDIBIND_UPPER	hNdi is bound to hNdiBoundTo's upper bindings
//			NDIBIND_LOWER	hNdiBoundTo is bound to hNdi's lower bindings
//		uDbgLevel
//			Display flags
//
//	Return:
//		Returns OK if operation is successful. Otherwise it returns the 
//		error encountered from NetDi.
//
/////////////////////////////////////////////////////////////////////////////
{
	char szNdiText[ MAX_RES ], szNdiTextBound[ MAX_RES ], szBindingType[ MAX_RES ];
	
	ASSERT( NdiIsNdi(hNdi) == OK );
	ASSERT( NdiIsNdi(hNdiBound) == OK );
	ASSERT( (uBindType == NDIBIND_UPPER) || (uBindType == NDIBIND_LOWER) );

	NdiGetText( hNdi, szNdiText, sizeof( szNdiText ) );
	NdiGetText( hNdiBound, szNdiTextBound, sizeof( szNdiTextBound ) );
	LoadString( ghInstance, IDS_NDIBIND_BASE+uBindType, szBindingType, sizeof( szBindingType ) );
	
	debugprintf( DEBUG_LEVEL_DEBUG, 
	             "%s bound to %s : %s", 
	             szNdiTextBound, 
	             szNdiText,
	             szBindingType );
	             
	return OK;	          
}	             


RETERR MyDisplayStatus
( 
	HNDI	hNdi,
	UINT    uFlags
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyDisplayStatus
//
//	Description:
//		Displays status information on to the debug terminal if it exists
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		uFlags
//			Display flags
//
//	Return:
//		Returns OK if operation is successful. Otherwise it returns the 
//		error encountered from NetDi.
//
/////////////////////////////////////////////////////////////////////////////
{
	LPMYNDIPROPS	lpMyNdiProps;
	RETERR			reError;
	UINT			uRelation;
	DEBUG_LEVEL		uDbgLevel = uFlags & 0x03;
	HNDI			hNdiTarget, hNdiBoundTo;
	char			szNdiText[ MAX_RES ];
	
	// Display header                            
	reError = NdiGetText( hNdi, szNdiText, sizeof( szNdiText ) );
	if ( reError != OK )
		return reError;

	debugprintf( uDbgLevel, "%s :", szNdiText );
	
	// Display hNdi
	debugprintf( uDbgLevel, "\tNDI Handle         : %lx", hNdi );
	
	// Display Class
	debugprintf( uDbgLevel, "\tNDI Class          : %s", MyGetDeviceClass( hNdi ) );
	
	// Display Setup flags and optionally the Batch Inf handle
	reError = NdiGetProperties( hNdi, &lpMyNdiProps );
	if ( reError != OK )
		return reError;

	debugprintf( uDbgLevel, "\tSetup Flags        : %lx", lpMyNdiProps->suFlags );
	
	if ( lpMyNdiProps->suFlags & NDI_CREATE_BATCHMODE )
		debugprintf( uDbgLevel, "\tBatch Inf Handle   : %lx", lpMyNdiProps->hInfBatch );

	debugprintf( uDbgLevel, "\tProp Memory Handle : %lx", lpMyNdiProps->hMemNdiProps );

	for ( uRelation=NDI_EDGE_UPPER; uRelation<=NDI_EDGE_LOWERRANGE; uRelation++ )
	{
		reError = MyDisplayInterfaces( hNdi, uRelation, uDbgLevel );
		if ( reError != OK )
			return reError;
	}
	
	for ( hNdiTarget=NdiGetFirst(); NdiIsNdi( hNdiTarget )==OK; hNdiTarget=NdiGetNext(hNdiTarget) )
	{
		uRelation = NDIBIND_LOWER_FIRST;
	
		while ( NdiGetBinding( hNdiTarget, &hNdiBoundTo, uRelation ) == OK )
		{
			reError   = MyDisplayBinding( hNdiTarget, hNdiBoundTo, NDIBIND_LOWER, uDbgLevel );
			
			uRelation = NDIBIND_LOWER_NEXT;
		}
	}
	
	return reError;
}


///////////////////////////////////////////////////////////////////////////
//
//  GetCaption
//
///////////////////////////////////////////////////////////////////////////

char *GetCaption( void )
{
	static BOOL fInitialized = FALSE;
	
	if ( !fInitialized )
	{
		if ( !LoadString( ghInstance, IDS_CAPTION, szCaption, sizeof( szCaption ) ) )
			strcpy( szCaption, VER_INTERNALNAME_STR );
	
		fInitialized = TRUE;
	}
	
	return szCaption;
}


///////////////////////////////////////////////////////////////////////////
//
//  debugprintf
//
///////////////////////////////////////////////////////////////////////////

void CDECL debugprintf( DEBUG_LEVEL debugLevel, LPCSTR szFmt, ... )
{
    int     i;
    va_list marker;
    char    szTmpBuf[256];

    // debug message spigot.
    if ( debugLevel > gDebugLevel )
        return;

    // module title
    OutputDebugString( GetCaption() );
    OutputDebugString( ": " );

    // debug text
    va_start( marker, szFmt );
    i = wvsprintf( szTmpBuf, szFmt, marker );
    va_end( marker );
    lstrcpy( szTmpBuf+i, "\n\r" );

    OutputDebugString(szTmpBuf);
    
    return;
}


///////////////////////////////////////////////////////////////////////////
//
//  assertprintf
//
///////////////////////////////////////////////////////////////////////////

int CDECL assertprintf( HWND hwndParent, UINT uStyle, UINT uIdResource, ... )
{
	static char szBuff[ MAX_MBSTRING ];
	static char szFormat[ MAX_RES ];
	
	if ( LoadString( ghInstance, uIdResource, szFormat, sizeof( szFormat ) ) )
		wvsprintf( szBuff, szFormat, (&uIdResource)+1 );
	else
		strcpy( szBuff, "Not enough memory" );

		
	MessageBeep( MB_ICONEXCLAMATION );
		
	return MessageBox(	hwndParent,
					    szBuff,
			    		GetCaption(),
			    		MB_ICONEXCLAMATION|uStyle );
}


///////////////////////////////////////////////////////////////////////////
//
//  debugassert
//
///////////////////////////////////////////////////////////////////////////

void debugassert( LPCSTR lpszFile, int nLine )
{
	static BOOL fInAssert = FALSE;
	
	if ( !fInAssert )
	{
		int nId;
	
		fInAssert = TRUE;
		
		nId = assertprintf( NULL,
		                    MB_ICONEXCLAMATION |
		                    	MB_ABORTRETRYIGNORE |
		                    	MB_DEFBUTTON3 |
		                    	MB_TASKMODAL,
		                    IDS_ERR_ASSERT,
		                    lpszFile,
		                    nLine );
		                    
		switch ( nId )
		{
		case IDABORT:
			// Kill the application right now!
			_asm
			{
				mov		al,	0ffh
				mov		ah, 4ch
				int		21h
			}
			break;
			
		case IDRETRY:
			DEBUG_TRAP;
			break;
		}
		
		fInAssert = FALSE;
	}

	return;
}

#endif // _DEBUG
