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


#include <vfilter.h>
#include <string.h>


ASSERTFILE;


char *szVfilterInterface = "vfilter";



BOOL IsFilterBindAllowed
( 
	HNDI hNdi, 
	UINT uiBindType
)
/////////////////////////////////////////////////////////////////////////////
//
//	IsFilterBindAllowed
//
//	Description:
//		Verify that the NDI object is allowed to bind using the vfilter 
//		interface.  We must make sure that hNdi is either a vnic or vfilter
//		object and that the object can bind in the specified direction.
//
//	Arguments:
//		hNdi
//			Ndi object handle

//		uiBindType
//			NDIBIND_UPPER	
//			NDIBIND_LOWER	
//
//	Return:
//		TRUE if the object and relation allows a binding
//		FALSE otherwise
//
/////////////////////////////////////////////////////////////////////////////
{
	char	szNdiText[ MAX_RES ];
	WORD	wNdiNetClass;
	UINT	uiIndex, uiRelation;
	BOOL	bAllowed = FALSE;

	NdiGetText( hNdi, szNdiText, sizeof( szNdiText ) );
	
	debugprintf( DEBUG_LEVEL_DEBUG, "Query Binding from %s", szNdiText );
	                              
	// We want to compare to the target's opposite interface for compatibility
	
	uiRelation = ( uiBindType == NDIBIND_UPPER ) ? NDI_EDGE_UPPER : NDI_EDGE_LOWER;

	for (	uiIndex=0; 
			NdiGetInterface( hNdi, uiRelation, uiIndex, szNdiText, sizeof( szNdiText ) ) == OK;
			uiIndex++ )
	{
		if ( stricmp( szNdiText, szVfilterInterface ) == 0 )
		{
			NdiGetClass( hNdi, &wNdiNetClass );
		
			if ( ( wNdiNetClass == NDI_CLASS_TRANS ) && ( uiBindType == NDIBIND_UPPER ) )
			{
				// We have VNIC ---> VFILTER	NDIBIND_UPPER 
				debugprintf( DEBUG_LEVEL_DEBUG, "VNIC ---> VFILTER\tNDIBIND_UPPER" );
		
				bAllowed = TRUE;
			}
			else if ( ( wNdiNetClass == NDI_CLASS_NET ) && ( uiBindType == NDIBIND_LOWER ) )
			{
				// We have VFILTER ---> VNIC	NDIBIND_LOWER
				debugprintf( DEBUG_LEVEL_DEBUG, "VFILTER ---> VNIC\tNDIBIND_LOWER" );
		
				bAllowed = TRUE;
			}
			      
			// We're done.  We only care about the szVilterInterface
			break;
		}
	}

	return bAllowed;	
}



RETERR PRIVATE FilterNdiQueryBind
( 
	HNDI hNdi,
	HNDI hNdiToBind,
	UINT uBindType
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiQueryBind
//
//	Description:
//		Handles NDI_QUERY_BIND message sent by NetDi to inform the installer
//		that an upper or lower binding is about to be added by NdiBind.  We know
//		that hNdi is the handle of a vnic or vfilter object.  So all we need
//		to do is make sure that the hNdiToBind object is also vnic or vfilter
//		and that the binding direction is correct.
//
//		VNIC ---> VFILTER  NDIBIND_UPPER
//		VFILTER ---> VNIC  NDIBIND_LOWER	OK
//
//		VNIC ---> VFILTER  NDIBIND_LOWER
//		VFILTER ---> VNIC  NDIBIND_UPPER	NOT OK
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

	TRACE_ENTER( "FilterNdiQueryBind" );
	
	if ( (reError = NdiIsNdi( hNdi )) != OK )
	{
		// invalid handle
	}
	else if ( (reError = NdiIsNdi( hNdiToBind )) != OK )
	{
		// invalid handle
	}		
	else if ( IsFilterBindAllowed( hNdiToBind, uBindType )  )
	{
		reError = OK;
	}
	/*
	else
	{
		reError = DefNdiProc( hNdi, NDI_QUERY_BIND, (WPARAM)hNdiToBind, (LPARAM)uBindType );
	}
	*/

	TRACE_NDILEAVE( "FilterNdiQueryBind", reError );
	
	return reError;
}


RETERR PRIVATE FilterNdiBind
( 
	HNDI hNdi,
	HNDI hNdiBound,
	UINT uBindType
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiBind
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
#ifdef DEBUG
	char szNdiText[ MAX_RES ], szNdiTextBound[ MAX_RES ], szBindingType[ MAX_RES ];

	TRACE_NDIENTER( "FilterNdiBind", hNdi );
	ASSERT( NdiIsNdi(hNdiBound) == OK );
		
	NdiGetText( hNdi, szNdiText, sizeof( szNdiText ) );
	NdiGetText( hNdiBound, szNdiTextBound, sizeof( szNdiTextBound ) );
	LoadString( ghInstance, IDS_NDIBIND_BASE+uBindType, szBindingType, sizeof( szBindingType ) );
	
	debugprintf( DEBUG_LEVEL_DEBUG, 
	             "%s bound to %s : %s", 
	             szNdiTextBound, 
	             szNdiText,
	             szBindingType );
	             		
	TRACE_NDILEAVE( "FilterNdiBind", OK );
#endif
	
	return OK;
}



RETERR PRIVATE FilterNdiUnbind
( 
	HNDI hNdi,
	HNDI hNdiUnbound,
	UINT uBindType
)
/////////////////////////////////////////////////////////////////////////////
//
//	FilterNdiUnbind
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

	TRACE_NDIENTER( "FilterNdiUnbind", hNdi );
	ASSERT( NdiIsNdi(hNdiUnbound) == OK );
		
	NdiGetText( hNdi, szNdiText, sizeof( szNdiText ) );
	NdiGetText( hNdiUnbound, szNdiTextUnbound, sizeof( szNdiTextUnbound ) );
	LoadString( ghInstance, IDS_NDIBIND_BASE+uBindType, szBindingType, sizeof( szBindingType ) );
	
	debugprintf( DEBUG_LEVEL_RETAIL, 
	             "%s bound to %s : %s", 
	             szNdiTextUnbound, 
	             szNdiText,
	             szBindingType );
	             		
	TRACE_NDILEAVE( "FilterNdiUnbind", OK );
#endif
	
	return OK;
}
