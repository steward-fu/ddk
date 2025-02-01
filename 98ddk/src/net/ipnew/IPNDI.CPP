/******************************************************************************
**
** Module:       		ipdetect.exe                  
** File:         		ipndi.cpp                            
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
// Device Manager class names

char *szClassNetProtocol    = "NetTrans"; 
char *szClassNet            = "Net";
char *szClassNetClient      = "NetClient";
char *szClassNetService		= "NetService";

char *szClassNetInfFileName        = "Net.inf";
char *szClassNetTransInfFileName   = "Nettrans.inf";
char *szClassNetClientInfFileName  = "Netcli.inf";
char *szClassNetServiceInfFileName = "Netservr.inf";

// Registry string constants
char *szRegKeyNdi           = "Ndi";
char *szRegKeyBindings      = "Bindings";
char *szRegValDeviceID      = "DeviceID";
char *szRegValDriverDesc	= "DriverDesc";
char *szRegValCompatibleIDs = REGSTR_VAL_COMPATIBLEIDS;
char *szRegPathNetwork      = "Enum\\Network\\";
char *szRegPathFilter       = "Enum\\Filter\\";
char *szRegPathTemp         = "\\Temp";

char *szVServer				= "VSERVER";

#define DEVICE_ID_MSTCP		0
#define DEVICE_ID_VPACKET	1
#define DEVICE_ID_VFILTER	2
            
// Transport ID strings
char *szDeviceIDString[] = 
{
	"MSTCP",
	"VPACKET",
	"VFILTER",
	0
};

// Client ID strings
char *szClientIDString[] =
{
	"VREDIR",
	0
};

char *szDUPIDString[] = 
{
	"*PNP8387",
	0
};

// other strings
char *szNull                = "";



/////////////////////////////////////////////////////////////////////////////
// DetectNDIDevice
//

RETERR CIpdetectApp::GetDeviceInfo( LPLPDEVICE_INFO lplpdi, const char *szClass, const char *szTargetDeviceID )
{
	LPDEVICE_INFO lpdi = NULL;
	RETERR	      err;
	            
	// Initialize list pointer to NULL	            
	            
	*lplpdi = NULL;
	
	// At this point we want the device list of the specified class.
	// DiGetClassDevs will return a linked list of DEVICE_INFO structs of this class.

	err = DiGetClassDevs( &lpdi, szClass, NULL, 0 );

	if ( (err == OK) && lpdi )
	{
		LPDEVICE_INFO slpdi = lpdi;

		// We will walk the device list looking for the specified device id

		while ( lpdi )
		{
			BOOL fMatch = FALSE;
        	char szDeviceID[MAX_DEVNODE_ID_LEN+1] = "";
        	HKEY hKey, hKeyNdi;
            
        	// We need to get a key handle from the device node
        	// If we get a valid handle we will open the "Ndi" subkey and look for
        	// szTargetDeviceId as the data value of the szRegValDeviceID="DeviceID"
        	// subkey

			err = DiOpenDevRegKey( lpdi, &hKey, DIREG_DRV );
			
			if ( err == OK )
			{
				// Valid key handle for device node so get subkey handle

				if ( SURegOpenKey( hKey, szRegKeyNdi, &hKeyNdi ) == ERROR_SUCCESS )
				{
					DWORD dwSize = sizeof( szDeviceID );
					
					//  Get Data value from "DeviceID" subkey
				
					SURegQueryValueEx( hKeyNdi, 
					                   szRegValDeviceID, 
					                   NULL, 
					                   NULL, 
					                   (LPSTR)szDeviceID, 
					                   &dwSize );
					
					SURegCloseKey( hKeyNdi );
					
					// If the string data value for the "DeviceID" subkey is
					//     szTargetDeviceID, We Found It!
					
					AddVerboseMessage( szClass, szDeviceID, "\r\n" );
        				
					fMatch = !lstrcmpi( szDeviceID, szTargetDeviceID );
					
					// If szTargetDeviceID is NULL then we will accept ANY device
					// of the specified type
			
					if ( fMatch || !strlen(szTargetDeviceID) )
					{
						// Allocate Device Info
						DiCreateDeviceInfo( lplpdi,
						                    lpdi->szDescription,
						                    lpdi->dnDevnode,
						                    lpdi->hRegKey,
						                    lpdi->szRegSubkey,
						                    lpdi->szClassName,
						                    lpdi->hwndParent );
						
						SURegCloseKey( hKey );
						break;
					}
				}
				
				SURegCloseKey( hKey );
			}
			else
			{
				AddVerboseMessage( szClass, "DiOpenDevRegKey returns error opening key NDI", "\r\n" );
			
				if ( slpdi == lpdi )
				{
					//  It is possible that there are NO devices installed at this point
					//  so we will return with no error

					err = OK;
					AddVerboseMessage( szClass, "There are probably no devices installed : err = OK", "\r\n" );
				}
				else
					AddVerboseMessage( szClass, "Registry for this device is invalid", "\r\n" );
			}
			
			lpdi = lpdi->lpNextDi;
		}

		DiDestroyDeviceInfoList( slpdi );
	}
	else if ( err == OK )
	{
		AddVerboseMessage( szClass, "NO Devices of this class installed", "\r\n" );
	}
	else
	{
		AddVerboseMessage( szClass, "DiGetClassDevs returned an error!", "\r\n" );
	}

	return err;
}

/////////////////////////////////////////////////////////////////////////////
// DetectNDIDevice
//

RETERR CIpdetectApp::DetectNDIDevice( BOOL& bRetF, const char *szClass, const char *szTargetDeviceID )
{
	LPDEVICE_INFO lpdi;
	RETERR	      err;
	
	bRetF = FALSE;

	err = GetDeviceInfo( &lpdi, szClass, szTargetDeviceID );
	
	if ( (err == OK) && lpdi )
	{
		// We have a valid device info
					
		AddMessage( lpdi->szClassName, lpdi->szDescription, " Found.\r\n" );

	    bRetF= TRUE;
	
		DiDestroyDeviceInfoList( lpdi );
	}

	return err;
}

/////////////////////////////////////////////////////////////////////////////
// DetectAdapter
//
//

RETERR CIpdetectApp::DetectAdapter( BOOL& bRetF, const char *szTargetDeviceID )
{
	return DetectNDIDevice( bRetF, szClassNet, szTargetDeviceID );
}

/////////////////////////////////////////////////////////////////////////////
// DetectDialUpAdapter
//
//

RETERR CIpdetectApp::DetectDialUpAdapter( BOOL& bRetF )
{
	return DetectNDIDevice( bRetF, szClassNet, szDUPIDString[ 0 ] );
}

/////////////////////////////////////////////////////////////////////////////
// DetectMSTCP
//
//

RETERR CIpdetectApp::DetectMSTCP( BOOL& bRetF )
{
    return DetectNDIDevice( bRetF, szClassNetProtocol, szDeviceIDString[ DEVICE_ID_MSTCP ] );
}

/////////////////////////////////////////////////////////////////////////////
// DetectVPacket
//
//

RETERR CIpdetectApp::DetectVPacket( BOOL& bRetF )
{
    return DetectNDIDevice( bRetF, szClassNetProtocol, szDeviceIDString[ DEVICE_ID_VPACKET ] );
}

/////////////////////////////////////////////////////////////////////////////
// DetectMicrosoftClient
//
//

RETERR CIpdetectApp::DetectMicrosoftClient( BOOL& bRetF )
{
	return DetectNDIDevice( bRetF, szClassNetClient, szClientIDString[ 0 ] );
}


/////////////////////////////////////////////////////////////////////////////
// DetectVServer
//

RETERR CIpdetectApp::DetectVServer( BOOL& bRetF )
{
	return DetectNDIDevice( bRetF, szClassNetService, szVServer );
}


/////////////////////////////////////////////////////////////////////////////
// DetectClientBinding
//
//

RETERR CIpdetectApp::DetectClientBinding( BOOL& bRetF )
{
	LPDEVICE_INFO lpdi;
	RETERR	      err;
	
	bRetF = FALSE;

	err = GetDeviceInfo( &lpdi, szClassNetProtocol, szDeviceIDString[ DEVICE_ID_MSTCP ] );
	
	if ( (err == OK) && lpdi )
	{
		// We have a valid device info
		HKEY  hKey;
		DWORD index = 0;
						
	    // We need to get a key handle from the device node
		// If we get a valid handle we will enumerate the subkeys
		// look for bindings
    	err = DiOpenDevRegKey( lpdi, &hKey, DIREG_DEV );
			
		if ( err == OK )
		{
			HKEY  hKeyBindings;
							
			err = (RETERR)SURegOpenKey( hKey, szRegKeyBindings, &hKeyBindings );
							
			if ( (err == ERROR_SUCCESS) && hKeyBindings )
			{
				DWORD cbValue, cbRegKey, suRet;
				char  szRegName[REGSTR_MAX_VALUE_LENGTH+1];
			    char  szValueName[REGSTR_MAX_VALUE_LENGTH+1];
							
				do
				{
					cbValue  = sizeof( szValueName );
					cbRegKey = sizeof( szRegName );

					suRet = SURegEnumValue( hKeyBindings, 
				    	                    index,
				        		            szRegName,
				                	        &cbRegKey,
				                	        NULL,
				                	        NULL,
				    					    (LPBYTE)szValueName,
				                      	    &cbValue );
								                      
					if ( suRet == ERROR_SUCCESS )
					{
						AddVerboseMessage( szRegKeyBindings, szRegName, "\r\n" );

						bRetF = TRUE;					// WE FOUND A VALID BINDINGS ENTRY!!!
						suRet = ERROR_NO_MORE_ITEMS;    // Let's get out of here
					}
									
					index++;
				}
				while ( suRet == ERROR_SUCCESS );

				SURegCloseKey( hKeyBindings );
			}
							
			if ( (err != OK) || (index == 0) )
				AddVerboseMessage( szRegKeyBindings, "     Error Enumerating Bindings!", "\r\n" );

			SURegCloseKey( hKey );
		}
	}

	return err;
}


/////////////////////////////////////////////////////////////////////////////
// VerifyDeviceClass
//
// This routine relies HEAVILY on properties of the Network Driver Installer,
// NetDI.  It is highly unlikely that any other class installer will work in
// this way.  When the DIF_DETECTVERIFY message is sent to the net class 
// installer, it builds a list of installed devices and mark any missing or 
// corrupted for removall.  When the DIF_REMOVE message is sent to NetDI it 
// will remove any appropriately marked devices in the list.  In this way a 
// program can rid the registry of corrupted network entries.
//

RETERR CIpdetectApp::VerifyDeviceClass( const char *szDeviceClass )
{
	LPDEVICE_INFO lpdi  = NULL;
	RETERR	      err   = OK;
	
	// Allocate a device_info struct

	err = DiCreateDeviceInfo( &lpdi, NULL, 0, NULL, NULL, szDeviceClass, NULL );
	
	if ( err == OK )
	{
		DiCallClassInstaller( DIF_DETECTVERIFY, lpdi );

		DiCallClassInstaller( DIF_REMOVE, lpdi );

		// need to free memory allocated for device list
				
		DiDestroyDeviceInfoList( lpdi );
	}

	return err;		                 
}

/////////////////////////////////////////////////////////////////////////////
// InstallNDIDevice
//
//

RETERR CIpdetectApp::InstallNDIDevice( const char* szClass, 
                                       const char* szDeviceID, 
                                       const char* szDriverPath,
                                       const char* szRegPath
									)
{
	LPDEVICE_INFO lpdi = NULL;
	RETERR	      err  = OK;

	// Allocate a device_info struct

	err = DiCreateDeviceInfo( &lpdi, NULL, 0, NULL, NULL, szClass, NULL );
	
	if ( err == OK )
	{
		HKEY hKeyTmp;
	
		// fill out an LPDEVICE_INFO based on a device ID
		// this is a temporary registry key 
		
		lpdi->hRegKey = HKEY_LOCAL_MACHINE;
		lstrcpy( lpdi->szRegSubkey, szRegPathNetwork );
		lstrcat( lpdi->szRegSubkey, lpdi->szClassName );
		lstrcat( lpdi->szRegSubkey, szRegPathTemp );
		
		err = DiCreateDevRegKey( lpdi, &hKeyTmp, NULL, NULL, DIREG_DEV );
		
		if ( err == OK )
		{
			// set the device Id in the temporary registry key
		
			if ( SURegSetValueEx( hKeyTmp, 
			                      szRegValCompatibleIDs, 
			                      0, 
			                      REG_SZ, 
			                      (unsigned char *) szDeviceID,
			                      lstrlen( szDeviceID ) + 1 ) == ERROR_SUCCESS )
			{
				// call device mgr API to add driver node lists and fill out structure
				// it will use the device ID we stuffed in the registry.
              
            if ( szDriverPath )
            {
               if ( lpdi->atDriverPath = GlobalAddAtom( szDriverPath ) )
                  lpdi->Flags |= DI_ENUMSINGLEINF;
				}

				err = DiBuildCompatDrvList( lpdi );
				
				SURegCloseKey( hKeyTmp );
				
				// need to delete temp key, set handle to null, set subkey name to 
				// null or else net setup thinks this device all ready exists
				
				DiDeleteDevRegKey( lpdi, DIREG_DEV );
				lpdi->hRegKey = NULL;
				lstrcpy( lpdi->szRegSubkey, szNull );

				if ( err || !lpdi->lpCompatDrvList )
				{
					// We couldn't find the device so we ask the
					// user to select one.
				
					err = DiSelectDevice( lpdi );		
				}
				else
				{
					lpdi->lpSelectedDriver = lpdi->lpCompatDrvList;
				}
				
				if ( err == OK )
				{
					if ( szRegPath )
					{
						lpdi->hRegKey = HKEY_LOCAL_MACHINE;
						lstrcpy( lpdi->szRegSubkey, szRegPath );
						
						DiCreateDevRegKey( lpdi, &hKeyTmp, NULL, NULL, DIREG_DEV );
					}

					lpdi->Flags |= DI_NOVCP | DI_NOFILECOPY | DI_QUIETINSTALL;
               err = DiCallClassInstaller( DIF_INSTALLDEVICE, lpdi );
				}
            else
            {
               // need to free memory allocated for device list
               DiDestroyDeviceInfoList( lpdi );
            }
			}
			else
			{
				// We must still delete the temporary reg key
				DiDeleteDevRegKey( lpdi, DIREG_DEV );

            // need to free memory allocated for device list
            DiDestroyDeviceInfoList( lpdi );
			}
		}
      else
      {
         // need to free memory allocated for device list
         DiDestroyDeviceInfoList( lpdi );
      }
	}

	return err;		                 
}

/////////////////////////////////////////////////////////////////////////////
// InstallAdapter
//
//

RETERR CIpdetectApp::InstallAdapter( const char* szDeviceID )
{
//	char szRegPath[ MAX_DEVNODE_ID_LEN+1 ];

	// Check to see if the device class is installed.
		                                                        
	// testing installation of intermediate driver
	
//	strcpy( szRegPath, szRegPathNetwork );
//	lstrcat( lpdi->szRegSubkey, szDeviceID );
//	lstrcat( lpdi->szRegSubkey, "\\0000" );
		                                                        
	DiInstallClass( szClassNetInfFileName, 0 );

	return InstallNDIDevice( szClassNet, szDeviceID );
//	                         "c:\\ddk\\packet\\vxd\\debug\\vpacket.inf",
//	                         szRegPath );
}

/////////////////////////////////////////////////////////////////////////////
// InstallMSTCP
//
//

RETERR CIpdetectApp::InstallMSTCP( void )
{
	// Check to see if the device class is installed.
	
	DiInstallClass( szClassNetTransInfFileName, 0 );
	
	// testing installation of packet driver

	return InstallNDIDevice( szClassNetProtocol, 
	                         szDeviceIDString[ DEVICE_ID_MSTCP ] );
}

/////////////////////////////////////////////////////////////////////////////
// InstallVPacket
//
//

RETERR CIpdetectApp::InstallVPacket( void )
{
	// Check to see if the device class is installed.
	
	DiInstallClass( szClassNetTransInfFileName, 0 );
	
	// testing installation of packet driver

	return InstallNDIDevice( szClassNetProtocol, 
	                         szDeviceIDString[ DEVICE_ID_VPACKET ] );
}


/////////////////////////////////////////////////////////////////////////////
// InstallDialUpAdapter
//
//

RETERR CIpdetectApp::InstallDialUpAdapter( void )
{
	// Check to see if the device class is installed.
		                                                        
	DiInstallClass( szClassNetInfFileName, 0 );

	return InstallNDIDevice( szClassNet, szDUPIDString[ 0 ] );
}


/////////////////////////////////////////////////////////////////////////////
// InstallClient
//
//

RETERR CIpdetectApp::InstallClient( void )
{
	// Check to see if the device class is installed.
		                                                        
	DiInstallClass( szClassNetClientInfFileName, 0 );

	return InstallNDIDevice( szClassNetClient, szClientIDString[ 0 ] );
}


/////////////////////////////////////////////////////////////////////////////
// InstallVServer
//
//

RETERR CIpdetectApp::InstallVServer( void )
{
	// Check to see if the device class is installed.
		                                                        
	DiInstallClass( szClassNetServiceInfFileName, 0 );

	return InstallNDIDevice( szClassNetService, szVServer );
}


/////////////////////////////////////////////////////////////////////////////
// RemoveDevice
//
//

RETERR CIpdetectApp::RemoveDevice( char* szClass, char* szDeviceId )
{
	LPDEVICE_INFO lpdi = NULL;
	RETERR	      err;
	
	err = GetDeviceInfo( &lpdi, szClass, szDeviceId );
	
	if ( (err == OK) && lpdi )
	{
		REMOVEDEVICE_PARAMS removeParams;

		removeParams.cbSize  = sizeof( REMOVEDEVICE_PARAMS );
		removeParams.dwFlags = DI_REMOVEDEVICE_GLOBAL;
		lpdi->lpClassInstallParams = (LPARAM) &removeParams;
   					
   		DiCallClassInstaller( DIF_REMOVE, lpdi );

		DiDestroyDeviceInfoList( lpdi );
	}
	
	return err;
}
 
