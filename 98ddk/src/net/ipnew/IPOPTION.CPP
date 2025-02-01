/******************************************************************************
**
** Module:       		ipdetect.exe                  
** File:         		ipoption.cpp                            
** Descriptions:		The ndi detection/installation code
** Contains:    		
**
** Copyright (c) 1996, Microsoft Corporation, all rights reserved
**
** This document is provided for informational purposes only and Microsoft 
** Corporation makes no warranties, either expressed or implied, in this document.
** Information in this document may be substantially changed without notice in
** subsequent versions of windows and does not represent a commitment on the 
** part of Microsoft Corporation. 
**
******************************************************************************/
	 
#include "stdafx.h"
#include "ipdetect.h"

#include "mainfrm.h"
#include "ipdetdoc.h"                                                                         

#include "ipdetvw.h"
#include "winerror.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

#include <regstr.h>
#include <ctype.h>

#define ERROR_SUCCESS			0L

#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////////////////
// static data

// Registry string constants
char *szRegPathOptional 	= "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup\\OptionalComponents\\";
char *szRegRnaId			= "RNA";
char *szRegDccId			= "DCC";

/////////////////////////////////////////////////////////////////////////////
// DetectOptionalComponent
//

RETERR CIpdetectApp::DetectOptionalComponent( BOOL& bRetF, const char* szComponent )
{
	HKEY  hKey;
	DWORD err;
	char  szRegPath[ REGSTR_MAX_VALUE_LENGTH];
	
	lstrcpy( szRegPath, szRegPathOptional );
	lstrcat( szRegPath, szComponent );
	
	bRetF = FALSE;
	err   = SURegOpenKey( HKEY_LOCAL_MACHINE, szRegPath, &hKey );
	
	if ( (err == ERROR_SUCCESS) && hKey )
	{
       	char  szKeyValue[ REGSTR_MAX_VALUE_LENGTH + 1 ] = "";
		DWORD dwSize = sizeof( szKeyValue );
		
		err = SURegQueryValueEx( hKey, "Installed", NULL, NULL, (LPSTR)szKeyValue, &dwSize );
		
		SURegCloseKey( hKey );
		
		if ( lstrcmpi( szKeyValue, "0" ) )
		{
			// szKeyValue is something other that "0" so rna is installed!
		
			AddVerboseMessage( szComponent, szKeyValue, " - Detected\r\n" );
			
			bRetF = TRUE;
		}
	}
	                     
	// If you can't open the key then rna needs to be installed.
				
    return (RETERR)err;
}

/////////////////////////////////////////////////////////////////////////////
// DetectDCC
//

RETERR CIpdetectApp::DetectDCC( BOOL& bRetF )
{
	return DetectOptionalComponent( bRetF, szRegDccId );
}


/////////////////////////////////////////////////////////////////////////////
// DetectRNA
//

RETERR CIpdetectApp::DetectRNA( BOOL& bRetF )
{
	return DetectOptionalComponent( bRetF, szRegRnaId );
}
