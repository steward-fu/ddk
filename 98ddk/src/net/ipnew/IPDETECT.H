/******************************************************************************
**
** Module:       		ipdetect.exe
** File:         		ipdetect.h
** Descriptions:		main header file for the IPDETECT application
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
**
******************************************************************************/

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"       // main symbols

#ifdef __cplusplus
extern "C" {
#endif

#include "setupx.h"

RETERR  NdiRegQueryValue( LPDEVICE_INFO lpdi, 
                          LPCSTR        lpszSubkey, 
                          LPCSTR        lpszValueName,
                          LPSTR         lpValue,
                          DWORD         cbValue );
RETERR NdiRegSetValue( LPDEVICE_INFO lpdi, 
                       LPCSTR        lpszSubkey, 
                       LPCSTR        lpszValueName,
                       DWORD         dwType,
                       LPSTR         lpValue,
                       DWORD         cbValue );
RETERR NdiRegOpenKey( LPDEVICE_INFO lpdi, LPCSTR lpszSubkey, LPHKEY lphKey );
RETERR NdiRegCreateKey( LPDEVICE_INFO lpdi, LPCSTR lpszSubkey, LPHKEY lphKey );
RETERR NdiRegCloseKey( HKEY hKey );
                          


RETERR  SetReg( HKEY   hKey, 
                LPCSTR lpszSubkey, 
                LPCSTR lpszValueName,
                DWORD  dwType,
                LPBYTE lpbValue,
                DWORD  dwSize );
                
#ifdef __cplusplus
}
#endif


#define	SetFlag(obj, f)		{ (obj) |= (f); }


struct IPConfig
{
	// display flags
	unsigned fVerbose		: 1;
	unsigned fShow			: 1;
	unsigned fHelp			: 1;
	unsigned fHelpDone		: 1;

	// component selection flags
	unsigned fAdapter		: 1;
	unsigned fTcpIp			: 1;
	unsigned fDUN			: 1;
	unsigned fClient		: 1;
	unsigned fRNA			: 1;
	unsigned fModem			: 1;
	unsigned fModemDet		: 1;
	unsigned fDCC			: 1;

    // component selection strings                   
	CString AdapterString;	
	CString	ModemString;
	CString PortString;
	
	// log file name
	CString FileString;
};


#define MAXPREFIXLENGTH		10

/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp:
// See ipdetect.cpp for the implementation of this class
//

class CIpdetectApp : public CWinApp
{
public:
	CIpdetectApp();

// Overrides
	virtual BOOL InitInstance();
	virtual int  Run();
	virtual int  ExitInstance();
	
// Implementation

	//{{AFX_MSG(CIpdetectApp)
	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	afx_msg void OnDetectDevice();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	static IPConfig config;
	
	RETERR     err;
	CEditView* pView;			// For quick access to single document edit control
	CString    cTextStr;
	
	CIpdetectApp& operator += ( const char* szMessage );
	CIpdetectApp& AddMessage( const char* szPrefix, const char* szMessage, const char* szPostfix = "" );
	CIpdetectApp& AddVerboseMessage( const char* szPrefix, const char* szMessage, const char* szPostfix = "" );
	
	// User specified actions
	
	RETERR DoModem( void );
	RETERR DoAdapter( void );
	RETERR DoDialUpAdapter( void );
	RETERR DoMsTcp( void );
	RETERR DoVPacket( void );
	RETERR DoMsClient( void );
	RETERR DoVServer( void );
	RETERR DoRna( void );
	RETERR DoDCC( void );
	
	// Device Specific detection and installation functions

	RETERR DetectMSTCP( BOOL& bRetF );
	RETERR DetectVPacket( BOOL& bRetF );
	RETERR DetectClientBinding( BOOL& bRetF );
	RETERR DetectVServer( BOOL& bRetF);
	RETERR DetectDCC( BOOL& bRetF );
	RETERR DetectDialUpAdapter( BOOL& bRetF );
	RETERR DetectMicrosoftClient( BOOL& bRetF );
	RETERR DetectModem( BOOL& bRetF );
	RETERR DetectModemHardware( BOOL& bRetF );
	RETERR DetectRNA( BOOL& bRetF );
	
	RETERR InstallClient( void );
	RETERR InstallDialUpAdapter( void );
	RETERR InstallModem( void );
	RETERR InstallMSTCP( void );
	RETERR InstallVPacket( void );
	RETERR InstallVServer( void );
	RETERR InstallRootDevice( const char* szClassStr, const char* szModemStr, const char* szPortStr );
	
private:

	// Command Line utilities
	
	void     ParseCommandLine( void );
	void     DoHelp( void );
	CString& ParseForString( CString& csTarget, char *pChar );
	
    // General Network Device detect / install / remove
    
	RETERR  GetDeviceInfo( LPLPDEVICE_INFO lplpdi, const char *szClass, const char *szTargetDeviceID );
	RETERR	DetectNDIDevice( BOOL& bRetF, const char *szClass, const char *szTargetDeviceID );
	RETERR	DetectAdapter( BOOL& bRetF, const char *szTargetDeviceID );
	RETERR	InstallNDIDevice( const char* szClass, const char* szDeviceID, const char* szDevicePath = NULL, const char* szRegPath = NULL );
	RETERR	InstallAdapter( const char* szDeviceID );

	RETERR	DetectOptionalComponent( BOOL& bRetF, const char *szComponent );
	RETERR	VerifyDeviceClass( const char *szDeviceClass );
	RETERR	RemoveDevice( char* szClass, char* szDeviceId );
};


/////////////////////////////////////////////////////////////////////////////
                                                         
                                                         
/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp operater +=
//
//	Inline method for adding a message to the view with a standard prefix and
//	and postfix.

inline CIpdetectApp& CIpdetectApp::operator += ( const char *szMessage )
{
	return AddMessage( "IPDetect", szMessage, "\r\n" );
}

/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp friend operator +
//
//	Inline method for adding a VERBOSE message to the view with a standard prefix 

inline CIpdetectApp& CIpdetectApp::AddVerboseMessage( const char *szPrefix, 
                                                      const char *szMessage, 
                                                      const char *szPostfix )
{
	if ( config.fVerbose )
		return AddMessage( szPrefix, szMessage, szPostfix );
	
	return *this;
}   
