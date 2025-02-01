/******************************************************************************
**
** Module:       		ipdetect.exe                  
** File:         		ipdetect.cpp                            
** Descriptions:		Defines the class behaviors for the application     
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
// CIpdetectApp static data members

IPConfig CIpdetectApp::config;


/////////////////////////////////////////////////////////////////////////////
// Device Manager class names
extern char *szClassNet;

// other strings
extern char *szCom1;
extern char *szCom2;


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp

BEGIN_MESSAGE_MAP(CIpdetectApp, CWinApp)
	//{{AFX_MSG_MAP(CIpdetectApp)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_DETECT_TCPIP, OnDetectDevice)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp construction

CIpdetectApp::CIpdetectApp()
{
	pView           	 = NULL;
	
	config.fVerbose		 = FALSE;
	config.fShow		 = FALSE;
	config.fHelp		 = FALSE;
	config.fHelpDone	 = FALSE;

	config.fAdapter		 = FALSE;
	config.fTcpIp		 = TRUE;
	config.fModem		 = FALSE;
	config.fModemDet	 = FALSE;
	config.fDCC			 = FALSE;
   config.fClient     = FALSE;
	config.fDUN			 = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIpdetectApp object

CIpdetectApp NEAR theApp;


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp initialization

BOOL CIpdetectApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.
    
	SetDialogBkColor();        // Set dialog background color to gray
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	
	// Let's start off hidden
	m_nCmdShow = SW_HIDE;

	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.
	POSITION 	pos;

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate( IDR_MAINFRAME,
	                                       RUNTIME_CLASS(CIpdetectDoc),
	                                       RUNTIME_CLASS(CMainFrame),     // main SDI frame window
	                                       RUNTIME_CLASS(CIpdetectView));
	                                  
	AddDocTemplate(pDocTemplate);

	CSingleDocTemplate* pTemplate = (CSingleDocTemplate*)m_templateList.GetHead();

	ASSERT(pTemplate != NULL);
	ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

	// Get Default Strings
	config.FileString.LoadString( IDS_DEFAULTFILENAME );
	config.PortString.LoadString( IDS_DEFAULTPORTNAME );
	config.ModemString.LoadString( IDS_DEFAULTMODEMNAME );
	config.AdapterString.LoadString( IDS_DEFAULTADAPTERNAME );
	
	// Look in the command line for options
	ParseCommandLine();
	
	// Should we be visible ?
	if ( config.fShow )
		m_nCmdShow = -1;
	
	// Create a new (empty) document
	OnFileNew();
	
	pos = pTemplate->GetFirstDocPosition();
	CIpdetectDoc* pDocument = (CIpdetectDoc*)pTemplate->GetNextDoc( pos );

	ASSERT(pDocument != NULL );
	ASSERT(pDocument->IsKindOf(RUNTIME_CLASS(CDocument)));

	pos = pDocument->GetFirstViewPosition();
	pView = (CIpdetectView*)pDocument->GetNextView( pos );

	ASSERT(pView != NULL );
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CView)));
	
    // Make the App Look good                        
	pView->GetParentFrame()->UpdateWindow();
	pView->GetParentFrame()->RedrawWindow();

	// Fill document
	OnDetectDevice();	
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CIpdetectApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp Run
//

int CIpdetectApp::Run()
{
	CString cstrRestartMsg;

	if ( config.fShow )
		return CWinApp::Run();

	// If we are hidden then we will close the application directly
	
	SaveAllModified();
	HideApplication();
	CloseAllDocuments( TRUE );
	                       
	cstrRestartMsg.LoadString( IDS_MAYNEEDTORESTART );	                       
	AfxMessageBox( cstrRestartMsg );
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp ExitInstance
//

int CIpdetectApp::ExitInstance()
{
	CString cstrRestartMsg;

	int retValue = CWinApp::ExitInstance();
	
	cstrRestartMsg.LoadString( IDS_MAYNEEDTORESTART );	                       
	AfxMessageBox( cstrRestartMsg );
	
	return retValue;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp OnFileNew
//

void CIpdetectApp::OnFileNew()
{
	if ( m_templateList.IsEmpty() )
	{
		AfxMessageBox( AFX_IDP_FAILED_TO_CREATE_DOC );
		return;
	}
	
	CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead();
	
	// We want to open a specified file even if we have to create it
	
	CFile			cFile;
	CFileException	cFileException;
	
	if ( cFile.Open( config.FileString,
	                 CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive,
	                 &cFileException ) )
	{
		cFile.Close();
	}
	           
	pTemplate->OpenDocumentFile( config.FileString );
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp OnDetectDevice
//
//	This function is entered when a document is created application 
//	receives a ID_DETECT_TCPIP message.  The fHelpDone flag	is set 
//	when help is requested so that the subsequent entry will actually 
//	perform the other specified command line actions.  The fHelpDone 
//	flag is cleared when actions are performed to toggle between the 
//	help and action code paths when help is specified.
//

void CIpdetectApp::OnDetectDevice()
{
	BeginWaitCursor();

	// Start document with command line text
    cTextStr  = "";
	*this    += m_lpCmdLine;
	cTextStr += "\r\n";                

	if ( config.fHelp && !config.fHelpDone )
	{
		DoHelp();
		
		config.fHelpDone = TRUE;
	}
	else
	{
		// order is everything so keep the device order specified
		// in the do list here
       
		DoModem();  

		DoAdapter();
		DoDialUpAdapter();
		DoMsTcp();
		DoVPacket();

      DoMsClient();
		DoRna();
		DoDCC();

		// this set is used to make sure everything we did was kosher	    
		
//      *this += "Verifying Network Setup";
//      VerifyDeviceClass( szClassNet );
//      *this += "Verification Complete";
		
		config.fHelpDone = FALSE;
	}
	
	EndWaitCursor();
	
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp AddMessage
//
//	Method for adding a string with prefix and postfix to the document.

CIpdetectApp& CIpdetectApp::AddMessage( const char *szPrefix, const char *szMessage, const char *szPostfix )
{
	unsigned int  len = strlen( szPrefix );
	
	len = ( (len > 0) && (len <= MAXPREFIXLENGTH) ) ? ( MAXPREFIXLENGTH - len + 1 ) : 0;
	
	cTextStr += szPrefix;
	
	if ( len > 0 )
	{
		char* pad = new char[len];
		
		memset( pad, ' ', len-1 );
		pad[len-1] = 0;
		
		cTextStr += pad;		
		cTextStr += ": ";
		
		delete pad;
	}
	
	cTextStr += szMessage;
	cTextStr += szPostfix;
	
	if ( pView )
	{
		pView->GetEditCtrl().SetWindowText( cTextStr );
		pView->GetDocument()->SetModifiedFlag();
		pView->GetEditCtrl().UpdateWindow();
		pView->GetEditCtrl().RedrawWindow();
	}

	return *this;	
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp ParseForString( void )
//
//	Parse given buffer for space delimited string
//

CString& CIpdetectApp::ParseForString( CString& csTarget, char *pChar )
{
	csTarget = "";
				
	while ( *++pChar )
	{
		if ( *pChar == ' ' )
			break;
		else
			csTarget += *pChar;
	}
	
	return csTarget;
}                                                                    


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp ParseCommandLine( void )
//
//

void CIpdetectApp::ParseCommandLine( void )
{
	char* pChar  = m_lpCmdLine;
	
	do
	{
		if ( (*pChar == '/') || (*pChar == '-') )
		{
			switch ( *++pChar )
			{
			case 'a':
			case 'A':
				// Now parse for adapter PnP ID
				ParseForString( config.AdapterString, pChar );
				config.fAdapter = TRUE;
				break;
				
			case 'f':
			case 'F':
				// Now parse for log file name
				ParseForString( config.FileString, pChar );
				break;
			
			case 'h':
			case 'H':
			case '?':
				// You have to see the app to see the help text.
				config.fHelp = TRUE;
				config.fShow = TRUE;
				break;
			
			case 'm':
			case 'M':
				// Now we parse for a Unimodem or PnP string
				ParseForString( config.ModemString, pChar );
				
				// Hardware detection is only used when specified
				if ( !strcmp( config.ModemString, "d" ) || 
				     !strcmp( config.ModemString, "D" ) )
				{
					config.fModemDet = TRUE;
				}
				
				config.fModem    = TRUE;
				break;
				
			case 'p':
			case 'P':
				// Now we parse for a port string
                ParseForString( config.PortString, pChar );
                
				// WARNING: This program expects "COM1" or "COM2" ONLY
				if ( strcmp( config.PortString, szCom2 ) )
					config.PortString = szCom1;
				break;
				
			case 'r':
			case 'R':
				// If you want Remote Networking you need Dial Up Adapter
				config.fRNA = TRUE;
				config.fDUN = TRUE;
				break;
				
			case 's':
			case 'S':
				config.fShow = TRUE;
				break;
				
			case 'v':
			case 'V':
				config.fVerbose = TRUE;
				break;
				
			default:
				break;
			}
		}
		else
			pChar++;
	}
	while ( *pChar );
                 
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CIpdetectApp DoHelp
//
//	Add help messages to document

void CIpdetectApp::DoHelp( void )
{
	CString cString;
	UINT	unId 		= IDS_USAGE;
	char 	szUsage[]	= "Usage";
	char 	szEOL[]		= "\r\n";
	
	for ( unId=IDS_USAGE; unId<=IDS_VERBOSE; unId++ )
	{
		cString.LoadString( unId );
		cTextStr += cString;
	}	
	
	if ( pView )
	{
		pView->GetEditCtrl().SetWindowText( cTextStr );
		pView->GetEditCtrl().UpdateWindow();
		pView->GetEditCtrl().RedrawWindow();
		pView->GetDocument()->SetModifiedFlag();
	}

	return;
}
