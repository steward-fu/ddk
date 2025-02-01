/******************************************************************************
**
** Module:       		ipdetect.exe                  
** File:         		ipmodem.cpp                            
** Descriptions:		The modem detection/installation code
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

#define Not_VxD
#include <vmm.h>
#include <configmg.h>
#include <regstr.h>
#include <ctype.h>

#define ERROR_SUCCESS			0L

#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////////////////
// Device Manager class names
char *szClass				= "Class";
char *szClassModem		 	= "Modem";

// Registry string constants
char *szRegPathRoot			= "Enum\\Root\\";
char *szAttachedTo			= "AttachedTo";
char *szHardwareID			= "HardwareID";

extern char *szRegValCompatibleIDs;
extern char *szRegPathTemp;
            
// Modem ID strings
char *szModemIDString[] =
{
	"PNPC031",
	"PNPC032",
	0
};
                             
// other strings
char *szCom1				= "COM1";
char *szCom2				= "COM2";

extern char *szNull;


/////////////////////////////////////////////////////////////////////////////
// DetectModemHardware
//

RETERR CIpdetectApp::DetectModemHardware( BOOL& bRetF )
{
	LPDEVICE_INFO lpdi         = NULL;
	LPDEVICE_INFO lpdiDetected = NULL;
	LPDEVICE_INFO slpdi        = NULL;
	RETERR	      err          = OK;
	int			  cNew		   = 0;
	
	bRetF = FALSE;
	
	// This takes a while
	LoadCursor( IDC_WAIT );

	CM_Lock( 0 );
	
	// Our device info struct needs the window parent to dispay its dialog box
	err = DiCreateDeviceInfo( &lpdi, NULL, NULL, NULL, NULL, szClassModem, NULL );
	
	if ( err == OK )
	{
		// Let the class installer to its magic
	
		err = DiCallClassInstaller( DIF_DETECT, lpdi );
		
		// We will destroy the list for now and rebuild a list of new modems
		
		DiDestroyDeviceInfoList( lpdi );
		lpdi = NULL;
    
		if ( err == ERR_DI_USER_CANCEL )
		{
			// No modems detected
		
			CM_Unlock( 0 );
			return ~OK;
		}
		
		// Now we ask config manager for the whole big list enumerated devices

    	err = DiGetClassDevs( &lpdi, NULL, NULL, DIGCF_ALLCLASSES );
    	
    	if ( err == OK )
    	{
    		slpdi = lpdi;
    	
			while ( lpdi )
			{
				LPDEVICE_INFO lpdiNew;
						
				// There weren't any modems in the registery a minute ago
				// so any modem we find is new right ???  --- UNLESS they
				// are the physical cables
		
				if ( !strcmp( lpdi->szClassName, szClassModem ) )
				{
					// We still want to skip the cables
					HKEY  hKey;
			     	char  szRegStr[MAX_DEVNODE_ID_LEN+1] = "";
					DWORD dwSize = sizeof( szRegStr );
					BOOL  fMatch = TRUE;
					int   i;
					
					err = DiOpenDevRegKey( lpdi, &hKey, DIREG_DEV );
					
					if ( err == OK )
					{
						SURegQueryValueEx(	hKey, 
						              		szHardwareID, 
						               		NULL, 
						               		NULL, 
						               		(LPSTR)szRegStr, 
						               		&dwSize );
					               
						for ( i=0; i<2; i++ )
						{
							if ( !lstrcmpi( szRegStr, szModemIDString[i] ) )
							{
								fMatch = FALSE;
								break;
							}
						}
					
						if ( fMatch )
						{
							// WE FOUND A NEW MODEM ???
					
							err = DiCreateDeviceInfo( &lpdiNew, 
						    	  		              lpdi->szDescription,
						        	    		      lpdi->dnDevnode,
						            	        	  lpdi->hRegKey,
						                	    	  lpdi->szRegSubkey,
						                    		  lpdi->szClassName,
					    	                		  lpdi->hwndParent );
					        	
							// if we create a valid info struct then attach it to the
							// head of the list 
					                    
							if ( err == OK )
							{
								CString msg = "Hardware Detection Found ";
						
								msg += lpdiNew->szDescription;
								AddVerboseMessage( szClassModem, msg,"\r\n" );
					
								lpdiNew->lpNextDi = lpdiDetected;
								lpdiDetected      = lpdiNew;
							}
						}					
					}
				}
				
				lpdi = lpdi->lpNextDi;			
			}

			DiDestroyDeviceInfoList( slpdi );
		}
		
		if ( lpdiDetected )
		{
			lpdi = lpdiDetected;
			
			while ( lpdi )
			{
				CString msg = "Installing ";
				
				if ( !lpdi->lpCompatDrvList )
				{
					DiBuildCompatDrvList( lpdi );
					
					lpdi->lpSelectedDriver = lpdi->lpCompatDrvList;
					lstrcpy( lpdi->szDescription, lpdi->lpSelectedDriver->lpszDescription );
				}

				msg += lpdi->lpSelectedDriver->lpszDescription;
				AddVerboseMessage( szClassModem, msg, "\r\n" );
				
				SetFlag( lpdi->Flags, DI_DONOTCALLCONFIGMG );
				
				if ( DiCallClassInstaller( DIF_INSTALLDEVICE, lpdi ) == OK )
				{
					// Success!
					cNew++;
					bRetF = TRUE;
					config.ModemString = lpdi->szDescription;
				}
				else
				{
					AddVerboseMessage( szClassModem, "Installation Failed!", "\r\n" );
				}
					
				lpdi = lpdi->lpNextDi;
			}
			
			if ( cNew )
			{
				// Since we have config manager locked and we set the
				// DI_DONOTCALLCONFIGMG flag we need to reenumerate
				// the modems ourselves synchronously
				
				DEVNODE dnDevnode;
				
				CM_Locate_DevNode( &dnDevnode, NULL, 0 );
				CM_Reenumerate_DevNode( dnDevnode, CM_REENUMERATE_SYNCHRONOUS );
			}
			
			CM_Unlock( 0 );
			
			DiDestroyDeviceInfoList( lpdiDetected );
		}
	}

	LoadCursor( IDC_ARROW );

	return err;
}

/////////////////////////////////////////////////////////////////////////////
// DetectModem
//

RETERR CIpdetectApp::DetectModem( BOOL& bRetF )
{
	LPDEVICE_INFO	lpdi = NULL;
	BOOL 			fMatch = FALSE;
	RETERR	     	err;
	CString			msg;
	
	bRetF = FALSE;

	// At this point we want the device list
	// DiGetClassDevs will return a linked list of DEVICE_INFO structs of this class.

	err = DiGetClassDevs( &lpdi, szClassModem, NULL, 0 );
    
	if ( (err == OK) && lpdi )
	{
		LPDEVICE_INFO slpdi = lpdi;

		// We will walk the device list looking for a modem id
            
		while ( lpdi )
		{
	     	char  szRegStr[MAX_DEVNODE_ID_LEN+1] = "";
    	    HKEY  hDevKey, hDrvKey;
			DWORD dwSize = sizeof( szRegStr );
            
	       	// We need to get a key handle from the device node.
	       	// If we get a valid handle we will look for the HardwareID.
       		// We want to exclude the serial or parallel cables.
       		
       		err = DiOpenDevRegKey( lpdi, &hDrvKey, DIREG_DRV );
       		
       		if ( err != OK )
       		{
				lpdi = lpdi->lpNextDi;       		
       			continue;
       		}

			err = DiOpenDevRegKey( lpdi, &hDevKey, DIREG_DEV );
			
			if ( err == OK )
			{
				int i;
					
				//  Get Data value from szHardwareID subkey
				
				SURegQueryValueEx( hDevKey, 
					               szHardwareID, 
					               NULL, 
					               NULL, 
					               (LPSTR)szRegStr, 
					               &dwSize );
					               
				SURegCloseKey( hDevKey );	         
				
				// We want to weed out the two physical cable modems for
				// consideration.  To do this we assume the modem will
				// work for us and compare the hardware id to the pnp id's
				// of the physical cable types.  If one matches we drop it.
					               
				fMatch = TRUE;

				for ( i=0; i<2; i++ )
				{
					if ( !lstrcmpi( szRegStr, szModemIDString[i] ) )
					{
						fMatch = FALSE;
						break;
					}
				}
					
				if ( fMatch )
				{
					// We found a modem!
					config.ModemString = szRegStr;
					
					//  Get Data value from szHardwareID subkey
					dwSize = sizeof ( szRegStr );
					
					SURegQueryValueEx( hDrvKey, 
						               szAttachedTo, 
				        		       NULL, 
						               NULL, 
						               (LPSTR)szRegStr, 
						               &dwSize );
							               
					config.PortString = szRegStr;
						
					msg = config.ModemString + " found on " + config.PortString;
					AddVerboseMessage( szClassModem, msg, "\r\n" );
	                          
                    // Set return value
				    bRetF = TRUE;
				}
			}
			
			SURegCloseKey( hDrvKey );
			lpdi = lpdi->lpNextDi;
		}					

		DiDestroyDeviceInfoList( slpdi );
	}
	
	return err;
}


/////////////////////////////////////////////////////////////////////////////
// InstallModem
//

RETERR CIpdetectApp::InstallModem( void )
{
	return InstallRootDevice( "Modem",  config.ModemString, config.PortString );
}


/////////////////////////////////////////////////////////////////////////////
// InstallRootDevice
//

RETERR CIpdetectApp::InstallRootDevice
( 
	const char* szClassStr, 
	const char* szDeviceIdStr, 
	const char* szAttachedToStr
)
{
	LPDEVICE_INFO lpdi = NULL;
	RETERR	      err  = OK;
	
	// Allocate a device_info struct

	err = DiCreateDeviceInfo( &lpdi, NULL, 0, NULL, NULL, szClassStr, NULL );
	
	if ( err == OK )
	{
		HKEY hKeyTmp;
	
		// fill out an LPDEVICE_INFO based on a device ID
		// this is a temporary registry key 
		
		lpdi->hRegKey = HKEY_LOCAL_MACHINE;
		lstrcpy( lpdi->szRegSubkey, szRegPathRoot );
		lstrcat( lpdi->szRegSubkey, lpdi->szClassName );
		lstrcat( lpdi->szRegSubkey, szRegPathTemp );
		
		err = DiCreateDevRegKey( lpdi, &hKeyTmp, NULL, NULL, DIREG_DEV );

		if ( err != OK )
			return err;

		// set the device Id in the temporary registry key
		
		if ( SURegSetValueEx( hKeyTmp, 
		                      szRegValCompatibleIDs, 
		                      0, 
		                      REG_SZ, 
		                      (unsigned char *) szDeviceIdStr,
		                      strlen( szDeviceIdStr ) + 1 ) != ERROR_SUCCESS )
		{
			// delete temp key
			DiDeleteDevRegKey( lpdi, DIREG_DEV );
			return -1;
		}

		// call device mgr API to add driver node lists and fill out structure
		// it will use the device ID we stuffed in the registry.
				
		err = DiBuildCompatDrvList( lpdi );
			
		SURegCloseKey( hKeyTmp );

		// need to delete temp key, set handle to null, set subkey name to 
		// null or else setup thinks this device all ready exists

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
			if ( szAttachedToStr )
			{
				// Set the AttachedTo value in the device section of the modem
				// since the driver section has not been added yet.  The class
				// modem class installer will transfer the value.
			
				err = DiCreateDevRegKey( lpdi, &hKeyTmp, NULL, NULL, DIREG_DEV );
		
				SURegSetValueEx( hKeyTmp, 
				                 szAttachedTo, 
			                 	 0,
			                 	 REG_SZ, 
			                     (unsigned char *) szAttachedToStr,
			                     strlen( szAttachedToStr ) + 1 );

				SURegCloseKey( hKeyTmp );
			}

			err = DiCallClassInstaller( DIF_INSTALLDEVICE, lpdi );
		}

		DiDestroyDeviceInfoList( lpdi );
	}

	return err;		                 
}
