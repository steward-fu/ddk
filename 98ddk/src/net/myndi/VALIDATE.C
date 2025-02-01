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


#include <myndi.h>


ASSERTFILE;


extern char *szVfilterInterface;


char *szRegKeyBindings 	 	= "Bindings";
char *szVnic				= "VNIC";
char *szNull				= "";


#ifdef _DEBUG

char *szMyNdiValidate		= "MyNdiValidate";
char *szMyNdiSystemChange	= "MyNdiSystemChange";

#endif


RETERR PRIVATE MyNdiValidate
( 
	HNDI hNdi,
	HWND hwndParent
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiValidate
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
	RETERR			reError;
	LPMYNDIPROPS	lpMyNdiProps;
	                             
#ifdef MAXDEBUG	                             
	TRACE_NDIENTERWMSG( szMyNdiValidate, hNdi );
#else	
	TRACE_NDIENTER( szMyNdiValidate, hNdi );
#endif
	
	// Let's send some creation information to the debug monitor
	MyDisplayStatus( hNdi, DEBUG_LEVEL_RETAIL );
	
	// This is where we make sure that we have the correct file version
	reError = MyCheckFileVersion( hNdi );
	
	// Store VnbtInfo
	reError = NdiGetProperties( hNdi, &lpMyNdiProps );
	
	if ( reError == OK && lpMyNdiProps )
		reError = ApplyVnbtInfo( hNdi, lpMyNdiProps );

	TRACE_NDILEAVE( szMyNdiValidate, reError );	
}


RETERR PRIVATE MyNdiSystemChange
( 
	HNDI hNdi, 
	UINT uTypeOfChange 
)
/////////////////////////////////////////////////////////////////////////////
//
//	MyNdiSystemChange
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
#ifdef MAXDEBUG	                             
	TRACE_NDIENTERWMSG( szMyNdiSystemChange, hNdi );
#else	
	TRACE_NDIENTER( szMyNdiSystemChange, hNdi );
#endif
	
#ifdef _DEBUG
	debugprintf( DEBUG_LEVEL_DEBUG, "%s %u", szMyNdiSystemChange, uTypeOfChange );
#endif

	TRACE_NDILEAVE( szMyNdiSystemChange, OK );	
}


#undef MAXDEBUG
