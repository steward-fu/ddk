/******************************************************************************
**
** Module:       		ipdetect.exe                  
** File:         		ipdo.cpp                            
** Descriptions:		Defines the class behaviors for the application     
** Contains:    		
**
** Copyright (c) 1996,1997, Microsoft Corporation, all rights reserved
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

extern char *VALUE_YES;
extern char *VALUE_NO;

#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////////////////
// Device Manager class names
extern char *szClassNet;
extern char *szClassNetClient;

// other strings
extern char *szCom1;
extern char *szCom2;

char szInstallationSuccess[] =	"     Installation Success!";
char szInstallationFailed[]  =	"     Installation Failed.";


#define STR_TRUE						"True"                                                      
#define REGSTR_PATH_NDI					"Ndi"
#define REGSTR_PATH_NDI_DEFAULT			"Ndi\\Default"
#define REGSTR_VAL_NULL					""                                                      


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoAdapter
//
//	Method for detecting and installing a network adapter

RETERR CIpdetectApp::DoAdapter( void )
{
    BOOL bRetF;

	err = OK;

	if ( config.fAdapter )
	{
		CString cString = "Looking for " + config.AdapterString;
	
    	*this += cString;
    	err = DetectAdapter( bRetF, config.AdapterString );
    	
    	if ( err == OK && !bRetF )
    	{
    		cString = "Installing " + config.AdapterString;
    		*this  += cString;
    		
    		err = InstallAdapter( config.AdapterString );
    		
	    	*this += ( err == OK ) ? szInstallationSuccess : szInstallationFailed;
		}
   	}

	return err;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoMsTcp
//
//	Method for detecting and installing MSTCP

RETERR CIpdetectApp::DoMsTcp( void )
{
    BOOL bRetF;

	err = OK;

	if ( config.fTcpIp )
	{
    	*this += "Looking for MSTCP";
    	err    = DetectMSTCP ( bRetF );

    	if ( err == OK && !bRetF ) 
    	{                        
    		*this += "Installing TCP/IP";

    		err = InstallMSTCP();
    	
	    	*this += ( err == OK ) ? szInstallationSuccess : szInstallationFailed;
		}
  	}

	return err;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoVPacket
//
//	Method for detecting and installing VPacket

RETERR CIpdetectApp::DoVPacket( void )
{
    BOOL bRetF;

	err = OK;

   	*this += "Looking for VPacket";
    err    = DetectVPacket ( bRetF );

    if ( err == OK && !bRetF ) 
    {                        
    	*this += "Installing VPacket";

    	err = InstallVPacket();
    	
	    *this += ( err == OK ) ? szInstallationSuccess : szInstallationFailed;
	}

	return err;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoVNetSup
//
//	Method for detecting and installing VNetSup

RETERR CIpdetectApp::DoVServer( void )
{
    BOOL bRetF;

	err = OK;

   	*this += "Looking for VServer";
   	err    = DetectVServer ( bRetF );

    if ( err == OK && !bRetF ) 
    {                        
    	*this += "Installing VServer";

   		err = InstallVServer();
    	
	    *this += ( err == OK ) ? szInstallationSuccess : szInstallationFailed;
	}

	return err;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoMsClient
//
//	Method for detecting and installing the Microsoft Client

RETERR CIpdetectApp::DoMsClient( void )
{
    BOOL bRetF;

	err = OK;

	if ( config.fClient )
	{
		*this += "Looking for Microsoft Client";
		err = DetectMicrosoftClient( bRetF );
	    		
		if ( err == OK && !bRetF )
		{
			*this += "Installing Microsoft Client";

   			err = InstallClient();
   		
	    	*this += ( err == OK ) ? szInstallationSuccess : szInstallationFailed;
		}
		else
		{
			RemoveDevice( "NETCLIENT", "VREDIR" ); 
		}

/*
		*this += "     Check TCP/IP Installation";
    	err   |= DetectMSTCP ( bRetF );
	
		if ( err == OK )
		{	    		
			*this += "     ***** Now we check for a Client Binding";
			err = DetectClientBinding( bRetF );
           	  
   	  		if ( err == OK && bRetF )
				*this += "     Client Binding OK!";
			else
			{
				// MSTCP is installed but we need to initialize the client binding
			
				*this += "     NO Client Binding detected";
				*this += "BUGBUG Client *NOT* initialized!!!";
			}
		}
		else 
			*this += "     TCP/IP or MS Client Still Not Detected!";
*/			
	}
		
	return err;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoRna
//
//	Method for detecting and installing Remote Network Access

RETERR CIpdetectApp::DoRna( void )
{
    BOOL bRetF;
	UINT hInst, j;

	err = OK;

	if ( config.fRNA )
	{
		*this += "Looking for RNA";
		err = DetectRNA( bRetF );

		if ( err == OK && !bRetF )
		{
			*this += "Installing RNA" ;
    	
	    	hInst = WinExec("RunDll setupx,InstallhInfSection rna 0 rna.inf", SW_SHOWNORMAL );

	    	while  ( j= GetModuleUsage((HINSTANCE) hInst ) )
			    Yield();
		}
		else
			*this += "RNA Found";
	}
	
	return err;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoDialUpAdapter
//
//	Method for detecting and installing Dial Up Adapter

RETERR CIpdetectApp::DoDialUpAdapter( void )
{
    BOOL bRetF;

	err = OK;

	if ( config.fDUN )
	{
		*this += "Looking for Dial Up Adapter";
    	err = DetectDialUpAdapter ( bRetF );
     
    	if ( err == OK && !bRetF )
   		{
			*this += "Installing Dial Up Adapter" ;
	    
	    	err = InstallDialUpAdapter();
	    
	    	*this += ( err == OK ) ? szInstallationSuccess : szInstallationFailed;
		}
		else
			*this += "Dial Up Adapter Found";
	}

	return err;	
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoModem
//
//	Method for detecting and installing a modem

RETERR CIpdetectApp::DoModem( void )
{	
    BOOL bRetF;
	UINT hInst, j;
	
	err = OK;

    if ( config.fModem )
    {
    	CString msg;
    	
    	if ( config.ModemString.IsEmpty() )
    	{
    		// No modem specified on command line so look for an existing one

   			*this += "Looking for a Modem";
   			err = DetectModem( bRetF );
    		
	    	if ( !bRetF && config.fModemDet )
    		{
   				// There is no modem in the registry so let's try
   				// hardware detection.  This routine will install
   				// any modem that is detected so don't try to
   				// it again later.  DetectModemHardware fills the 
   				// ModemString with the modem description.
    				
				*this += "Trying Hardware Detection";    			
   				err = DetectModemHardware( bRetF );
    				                                   
    			// If we detect no modem, use the wizard
   				if ( !bRetF )                                  
   				{
   					*this += "No Modem Detected!";
   					config.ModemString = "";
   				}
    		}
    	}
    	else
    	{
    		// Force installation of specified modem on selected port
    		bRetF = FALSE;
    	}
    	    	
        if ( config.ModemString.IsEmpty() )
 		{
        	// At this point we invoke the Modem Install wizard in
         	// Modem.Cpl either because detection failed or by design
         	
			hInst = WinExec( "RunDll32 shell32,Control_RunDLL modem.cpl,@0,add", SW_SHOWNORMAL );

			// Wait for the Modem Wizard to get done.
			
			while  ( j= GetModuleUsage((HINSTANCE) hInst ) )
				Yield();  
        }
    	else if ( !bRetF )
    	{
    		msg    = "Installing Modem " + config.ModemString + " to " + config.PortString;

			*this += msg;
		
			err = InstallModem();
			
		    *this += ( err == OK ) ? szInstallationSuccess : szInstallationFailed;
    	}
	}
    
    return err;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoDCC
//
//	Method for detecting and installing Direct Cable Connect

RETERR CIpdetectApp::DoDCC( void )
{
    BOOL bRetF;
	UINT hInst, j;

	err = OK;

	if ( config.fDCC )
	{		
		*this += "Looking for Direct Cable Connect";
		err = DetectDCC( bRetF );
	
		if ( err == OK && !bRetF )
		{
			*this += "Installing Direct Cable Connect" ;
    	
	    	hInst = WinExec("RunDll setupx,InstallhInfSection dcc 0 rna.inf", SW_SHOWNORMAL );

	    	while  ( j= GetModuleUsage((HINSTANCE) hInst ) )
		    	Yield();
		}
		else
			*this += "Direct Cable Connect Found";
	}
	
	return err;
}

