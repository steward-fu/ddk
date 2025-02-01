/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//		bind.c
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
#include <stdlib.h>
#include <string.h>


ASSERTFILE;


#define REGSTR_PATH_NDI                         "Ndi"
#define INFSTR_MAXINSTANCE                      "MaxInstance"

char *szVfilterInterface = "vfilter";



RETERR PRIVATE MyNdiQueryBind
( 
	HNDI hNdi,
	HNDI hNdiToBind,
	UINT uBindType
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiQueryBind
//
//	Description:
//		Handles NDI_QUERY_BIND message sent by NetDi to inform the installer
//		that an upper or lower binding is about to be added by NdiBind.  
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		hNdiToBind
//			Ndi object handle that is to be bound to first Ndi object
//		uBindType
//			NDIBIND_UPPER	Validate that hNdiToBind can be bound to hNdi's upper bindings
//			NDIBIND_LOWER	Validate that hNdiToBind can be bound to hNdi's lower bindings
//
//	Return:
//		Procedure should return OK if the binding is valid. Otherwise is should
//		return NDI_ERROR.  The procedure should probably display a warning message
//		if it invalidates a potential binding.
//
/////////////////////////////////////////////////////////////////////////////
{
	RETERR	reError;
    HNDI    ndiLower;
    HNDI    ndiUpper;
    char    szBuf[LINE_LEN];
    int     iMaxInstance = 1;

	TRACE_ENTER( "MyNdiQueryBind" );

	// The following does exactly what the default NDI_QUERY_BIND handler does for transports.
	// The real problem is to avoid all of those nasty select boxes for binding to multiple
	// existing net cards.  This happens because NetDi does a DiBuildCompatDrvList for the 
	// Next new transport device instance in just the same way that IpDetect does it to install
	// MSTCP.  However our inf file has not been properly installed into the driver database
	// so DiBuildCompatDrvList comes up with bupkiss.  Copying the inf file beforehand in the
	// inf file only makes the situation worse, oh well.
	
    if ( ((reError = NdiIsNdi( hNdi )) == OK) && ((reError = NdiIsNdi( hNdiToBind )) == OK) )
	{
		switch ( uBindType )
		{
		case NDIBIND_UPPER: 
			ndiLower = hNdi;
			ndiUpper = hNdiToBind;
			break;

		default:
			ASSERT( uBindType == NDIBIND_UPPER || uBindType == NDIBIND_LOWER );

		case NDIBIND_LOWER: 
			ndiLower = hNdiToBind;
			ndiUpper = hNdi;
			break;
		}

		// If we have a matching interface and we're not all ready bound
		// to it then we need to check to see if we support multiple
		// lower bindings.  If not we just return the values from NdiIsBound
		// or NdiCompareInterface.  If we do support multiple lower bindings
		// then we check merely to see if this instance all ready has one.
		
		if ( (reError = NdiIsBound( ndiLower, ndiUpper )) != OK )
		{
			if ( (reError = NdiCompareInterface( ndiLower, 
												 NDI_EDGE_UPPERRANGE, 
												 ndiUpper, 
												 NDI_EDGE_LOWERRANGE )) == OK )
			{
				*szBuf = 0;
			
				NdiRegQueryValue( ndiUpper,
								  REGSTR_PATH_NDI,
								  INFSTR_MAXINSTANCE,
								  szBuf,
								  sizeof(szBuf) ); 
			
				if ( *szBuf )
					iMaxInstance = atoi(szBuf);

				if ( iMaxInstance != 1 )
				{
					// return ERR_NDI_INSTANCE_ONCE if the object all ready 
					// has a lower binding.  NdiIsBound will return OK if
					// ndiUpper all ready has a binding.
					
					if ( NdiIsBound( NULL, ndiUpper ) != ERR_NDI_NOTBOUND )
//						reError = ERR_NDI_INSTANCE_ONCE;
					{
#ifdef _DEBUG
						debugprintf( DEBUG_LEVEL_DEBUG, "Here's Johnny!!!" );
#endif
						reError = ERR_NDI_NOTBOUND;
					}

					// Here's my strategy:
					// NetDi basically installs a new instance just like we
					// would however he doesn't add all the nice stuff that
					// we do.  Let's see if we can do it better.
				}
			}
		}
	}

	TRACE_NDILEAVE( "MyNdiQueryBind", reError );
}


RETERR PRIVATE MyNdiBind
( 
	HNDI hNdi,
	HNDI hNdiBound,
	UINT uBindType
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiBind
//
//	Description:
//		Handles NDI_NOTIFY_BIND message sent by NetDi to inform the installer
//		that an upper or lower binding has been added by NdiBind.  
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		hNdiBound
//			Ndi object handle that was bound to Ndi object
//		uBindType
//			NDIBIND_UPPER	Validate that hNdi can be bound to hNdiBound's upper bindings
//			NDIBIND_LOWER	Validate that hNdiBound can be bound to hNdi's lower bindings
//
//	Return:
//		Procedure should return OK.
//
/////////////////////////////////////////////////////////////////////////////
{
#ifdef _DEBUG

	TRACE_NDIENTER( "MyNdiBind", hNdi );
	ASSERT( NdiIsNdi(hNdiBound) == OK );
	
	MyDisplayBinding( hNdi, hNdiBound, uBindType, DEBUG_LEVEL_DEBUG );
	             		
	TRACE_NDILEAVE( "MyNdiBind", OK );

#endif // _DEBUG
	
	return OK;

}



RETERR PRIVATE MyNdiUnbind
( 
	HNDI hNdi,
	HNDI hNdiUnbound,
	UINT uBindType
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiUnbind
//
//	Description:
//		Handles NDI_NOTIFY_UNBIND message sent by NetDi to inform the installer
//		that an upper or lower binding has been removed by NdiBind.  
//
//	Arguments:
//		hNdi
//			Ndi object handle
//		hNdiUnbound
//			Ndi object handle that was unbound to first Ndi object
//		uBindType
//			NDIBIND_UPPER	Validate that hNdiBound can be bound to hNdi's upper bindings
//			NDIBIND_LOWER	Validate that hNdiBound can be bound to hNdi's lower bindings
//
//	Return:
//		Procedure should return OK.
//
/////////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG

	char szNdiText[ MAX_RES ], szNdiTextUnbound[ MAX_RES ], szBindingType[ MAX_RES ];

	TRACE_NDIENTER( "MyNdiUnbind", hNdi );
	ASSERT( NdiIsNdi(hNdiUnbound) == OK );
		
	NdiGetText( hNdi, szNdiText, sizeof( szNdiText ) );
	NdiGetText( hNdiUnbound, szNdiTextUnbound, sizeof( szNdiTextUnbound ) );
	LoadString( ghInstance, IDS_NDIBIND_BASE+uBindType, szBindingType, sizeof( szBindingType ) );
	
	debugprintf( DEBUG_LEVEL_RETAIL, 
	             "%s bound to %s : %s", 
	             szNdiTextUnbound, 
	             szNdiText,
	             szBindingType );
	             		
	TRACE_NDILEAVE( "MyNdiUnbind", OK );

#else   // DEBUG
	
	return OK;
	
#endif  // DEBUG
}
