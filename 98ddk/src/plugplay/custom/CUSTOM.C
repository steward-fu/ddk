/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997  Microsoft Corporation
//
//	Module Name:
//              custom.c
//
//	Abstract:
//
//	Author:
//		William Ingle
//
//	Environment:
//              Windows 95 filter sample 16-bit custom install dll
//
//	Notes:
//
//	Future:
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////


#include <custom.h>


ASSERTFILE;


HINSTANCE   ghInstance = NULL;


RETERR _loadds WINAPI ClassInstall
(
	DI_FUNCTION		diFunction,
	LPDEVICE_INFO	lpdi
)
/////////////////////////////////////////////////////////////////////////////
//
//	ClassInstall
//
//	The device class installer procedure used for the custom device
//
/////////////////////////////////////////////////////////////////////////////
{
    RETERR reError;
    
    TRACE_ENTER(  "ClassInstall", diFunction );
    
    ASSERT( lpdi != NULL );
    
    if ( lpdi == NULL )
    	return ERR_DI_BAD_DEV_INFO;
    	
    reError = CustomInitialize( lpdi );
	
	switch ( diFunction )
	{
	case DIF_SELECTDEVICE:
	case DIF_INSTALLDEVICEFILES:
	case DIF_ASSIGNRESOURCES:
	case DIF_PROPERTIES:
	case DIF_REMOVE:
	case DIF_FIRSTTIMESETUP:
	case DIF_FOUNDDEVICE:
	case DIF_SELECTCLASSDRIVERS:
	case DIF_VALIDATECLASSDRIVERS:
	case DIF_INSTALLCLASSDRIVERS:
	case DIF_CALCDISKSPACE:
	case DIF_DESTROYPRIVATEDATA:
	case DIF_VALIDATEDRIVER:
	case DIF_MOVEDEVICE:
	case DIF_DETECT:
	case DIF_INSTALLWIZARD:
	case DIF_DESTROYWIZARDDATA:
	case DIF_PROPERTYCHANGE:
	case DIF_ENABLECLASS:
	case DIF_DETECTVERIFY:
	default:
		break;

	case DIF_INSTALLDEVICE:
		if ( lpdi->lpSelectedDriver != NULL )
			reError = OK;			
	}
	
    TRACE_LEAVE(  "ClassInstall", reError );
}


RETERR CustomInitialize
( 
	LPDEVICE_INFO	lpdi 
)
/////////////////////////////////////////////////////////////////////////////
//
//	CustomInitialize
//
/////////////////////////////////////////////////////////////////////////////
{
	// Tell Setupx to perfrom default action
	
	return ERR_DI_DO_DEFAULT;
}


int CALLBACK LibMain
(
    HINSTANCE   hinst,              // module instance handle
    WORD        wDataSeg,           // data segment
    WORD        cbHeapSize,         // heap size
    LPSTR       lpszCmdLine         // command line
)
/////////////////////////////////////////////////////////////////////////////
//
//  LibMain     Called by system to initialize the DLL.
//
/////////////////////////////////////////////////////////////////////////////
{
    ghInstance = hinst;
    return 1;
}



BOOL CALLBACK _export _loadds WEP
( 
	BOOL fSystemExit 
)
/////////////////////////////////////////////////////////////////////////////
//
//  WEP     Called by system to unload the DLL.
//
/////////////////////////////////////////////////////////////////////////////
{
    // Nothing to do.

    return TRUE;
}
