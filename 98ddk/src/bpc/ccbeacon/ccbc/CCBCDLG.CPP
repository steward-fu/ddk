//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// ccbcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ccbc.h"
#include "ccbcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <afxmt.h>	// CEvent

CEvent			g_eventStopCapture;
CEvent			g_eventThreadDone;
ICCDecode		g_Driver;
DWORD			g_dwBytesRead = 0;
DWORD			g_dwErrors = 0;

char			g_szStatus[80];
HWND			g_hDialog;

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

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
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

/////////////////////////////////////////////////////////////////////////////
// CCcbcDlg dialog

CCcbcDlg::CCcbcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCcbcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCcbcDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCcbcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCcbcDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCcbcDlg, CDialog)
	//{{AFX_MSG_MAP(CCcbcDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RUN, OnRun)
	ON_BN_CLICKED(IDC_RESETSTATS, OnResetStats)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCcbcDlg message handlers

BOOL CCcbcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_bIsRunning = m_bIsPaused = FALSE;

	SetDlgItemInt(IDC_BYTESREAD, g_dwBytesRead, FALSE);
	SetDlgItemInt(IDC_ERRORS, g_dwErrors, FALSE);

	//SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCcbcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCcbcDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCcbcDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Return the number of mismatches.  A perfect match returns 0
unsigned int CompareBytes(BYTE *pRef, BYTE *pBuf, unsigned int len)
{
	unsigned int diff = 0, i = 0;
	if (!pRef || !pBuf)
	{
		return len;
	}
	for (i = 0; i < len; i++)
	{
		diff += (*(pRef+i) == *(pBuf+i) ? 0 : 1);
	}
	return 0;
}

#ifdef OLD
// check a buffer  of length PATTERN_LEN to see if it's our pattern
// if it is, update statistics and return TRUE
// if not, return FALSE
BOOL CheckPattern(BYTE *ccBuf)
{
	unsigned int i;
	BOOL bHeaderSynced = FALSE;

	// match first 5 bytes of pattern string
	// (these same 5 bytes occur at the start of each visible line)  
	bHeaderSynced = (memcmp(ccBuf, g_Pattern, 5) == 0);
	if (!bHeaderSynced)
	{
		return FALSE;
	}
	// okay, now we're synced
	// we wanna pull the index to
	// the superpattern (ASCII-encoded in 3 bytes)

	i = (ccBuf[5]-'0')*100 + (ccBuf[6]-'0')*10 + (ccBuf[7]-'0');

	// make sure our index is usable
	if (((i % PATTERN_LEN) != 0) || ((i + PATTERN_LEN) > g_PatternLen))
	{
		g_dwErrors += 3;
		SetDlgItemInt(g_hDialog, IDC_ERRORS, g_dwErrors, FALSE);
		return FALSE;
	}

	// compare the strings
	g_dwErrors += CompareBytes(g_Pattern + i, ccBuf, PATTERN_LEN);
	SetDlgItemInt(g_hDialog, IDC_ERRORS, g_dwErrors, FALSE);
	return TRUE;
}
#endif

// thread to look for Close Caption pattern
UINT CCReadThread(LPVOID pParam)
{
#define CONCURRENT_READS 64
#define CCBUF_SIZE 2
	static OVERLAPPED	overlapped[CONCURRENT_READS] = {0};
	static BYTE			ccBuf[CONCURRENT_READS][CCBUF_SIZE];
	unsigned int	nStatus = 0;
	BYTE			ccExpected[CCBUF_SIZE] = {0};
	DWORD			dwBytesRead = 0;
	int				nNextRead = 0;
	CString			strCrossLink;
	CString			strMasterCrossLink;
	BOOL			bInCrossLink = FALSE;

	BuildCrosslinkString(strMasterCrossLink);

	//
	// share dialog handle
	//
	g_hDialog = (HWND) pParam;

	g_eventStopCapture.ResetEvent();
	g_eventThreadDone.ResetEvent();

	//
	// fire off concurrent reads
	//
	for(nNextRead = 0; !nStatus && nNextRead < CONCURRENT_READS; nNextRead++)
	{
		//
		// create events if we havent created them yet
		//
		if ((!overlapped[nNextRead].hEvent) && !(overlapped[nNextRead].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		{
			AfxMessageBox("CreateEvent() failed!  Please restart the application.");
			g_eventThreadDone.SetEvent();
			return(GetLastError());
		}
		nStatus = g_Driver.ReadData(ccBuf[nNextRead], CCBUF_SIZE, &dwBytesRead, 
			overlapped + nNextRead);
		if ( !nStatus || nStatus == ERROR_IO_PENDING )
		{
			nStatus = 0;
		}
		else
		{
			AfxMessageBox("ReadData() failed!  Please restart the application.");
			g_eventThreadDone.SetEvent();
			return(nStatus);
		}
	}

	//
	// loop, grab data from the driver
	//
	nNextRead = 0;
	while (::WaitForSingleObject(g_eventStopCapture.m_hObject, 0) != (WAIT_OBJECT_0))
	{
		if (::WaitForSingleObject(overlapped[nNextRead].hEvent, 0) == (WAIT_OBJECT_0))
		{
			nStatus = g_Driver.GetOverlappedResult(&overlapped[nNextRead], &dwBytesRead, FALSE);
			if (!nStatus && (dwBytesRead==2))
			{
				unsigned int i;
				// let's strip parity
				for (i = 0; i < dwBytesRead; i++)
				{
					ccBuf[nNextRead][i] &= 0x7F;
				}

				// if nulls or if discontinuity samples, just ignore;
				// don't event count
				if ((!ccBuf[nNextRead][0] && !ccBuf[nNextRead][1]) ||
					((ccBuf[nNextRead][0]==0x7F) && (ccBuf[nNextRead][1]==0x7F)))
				{
					/*
					SetDlgItemInt(g_hDialog, IDC_ERRORS, g_dwErrors, FALSE);
					sprintf(g_szStatus, "ignoring(%02x, %02x)", 
						ccBuf[nNextRead][0], ccBuf[nNextRead][1]);
					SetDlgItemText(g_hDialog, IDC_STATUS, g_szStatus);
					*/
					g_Driver.ReadData(ccBuf[nNextRead], CCBUF_SIZE, &dwBytesRead, &overlapped[nNextRead]);
					nNextRead = (nNextRead+1 < CONCURRENT_READS ? nNextRead+1 : 0);
					continue;
				}

				g_dwBytesRead += dwBytesRead;
				SetDlgItemInt(g_hDialog, IDC_BYTESREAD, g_dwBytesRead, FALSE);
				dwBytesRead = 0;

				// let's examine the data!

				// are we starting a crosslink?
				if (!bInCrossLink && 
					(ccBuf[nNextRead][0] == 0x1c) && ((ccBuf[nNextRead][1] == 0x2a) //_2TR_
					|| (ccBuf[nNextRead][1] == 0x2b))) // _2RTD_
				{
					bInCrossLink = TRUE;
					//SetDlgItemText(g_hDialog, IDC_STATUS, "xlink starting");
					strCrossLink.Empty();
				}
#if 0
				// are we already in a crosslink?
				if (bInCrossLink)
				{
					strCrossLink += ccBuf[nNextRead][0];
					strCrossLink += ccBuf[nNextRead][1];
					// are we at the pattern's end?
					 if (((ccBuf[nNextRead][0] == 0x1c) && (ccBuf[nNextRead][1] == 0x2d))
						 || (strCrossLink.GetLength() > XLINK_LEN))
					 {
						bInCrossLink = FALSE;
						SetDlgItemText(g_hDialog, IDC_STATUS, "xlink ending");
						if (unsigned int i = CompareBytes(
							(unsigned char *) ((LPCTSTR) strMasterCrossLink), 
							(unsigned char *) ((LPCTSTR) strCrossLink), XLINK_LEN))
						{
							g_dwErrors += i;
							SetDlgItemInt(g_hDialog, IDC_ERRORS, g_dwErrors, FALSE);
							sprintf(g_szStatus, "xlink ending, errors(%02u)", i);
							SetDlgItemText(g_hDialog, IDC_STATUS, g_szStatus);
						}
					 }
				}
#else
				// are we already in a crosslink?
				if (bInCrossLink)
				{
					strCrossLink += ccBuf[nNextRead][0];
					strCrossLink += ccBuf[nNextRead][1];
					SetDlgItemText(g_hDialog, IDC_STATUS, strCrossLink.Right(25));
					// are we at the pattern's end?
					 if (((ccBuf[nNextRead][0] == 0x1c) && (ccBuf[nNextRead][1] == 0x2d))
						 || (strCrossLink.GetLength() > 256))
					 {
						bInCrossLink = FALSE;
						//SetDlgItemText(g_hDialog, IDC_STATUS, "xlink ended.");
						strCrossLink.Empty();
					 }
				}
#endif
				// we must be inside our A-Z pattern
				else
				{
					if (ccExpected[0] && ccExpected[1])
					{
						unsigned int i = 0;
						i += (ccBuf[nNextRead][0] == ccExpected[0] ? 0 : 1);
						i += (ccBuf[nNextRead][1] == ccExpected[1] ? 0 : 1);
						if (i)
						{
							g_dwErrors += i;
							SetDlgItemInt(g_hDialog, IDC_ERRORS, g_dwErrors, FALSE);
							sprintf(g_szStatus, "got(%02x, %02x) expected(%02x, %02x)", 
								ccBuf[nNextRead][0], ccBuf[nNextRead][1], ccExpected[0], ccExpected[1]);
							SetDlgItemText(g_hDialog, IDC_STATUS, g_szStatus);
						}
					}

					if (!ccBuf[nNextRead][0] && !ccBuf[nNextRead][1])
					{
						// do nothing
					}
					else if ((ccBuf[nNextRead][0] >= 'A') && (ccBuf[nNextRead][0] <= 'W'))
					{
						ccExpected[0] = ccBuf[nNextRead][0]+2;
						ccExpected[1] = ccExpected[0]+1;
					}
					else if ((ccBuf[nNextRead][0] == 'Y') && (ccBuf[nNextRead][1] == 'Z'))
					{
						ccExpected[0] = 0x14; // _1CR_
						ccExpected[1] = 0x2D;
					}
					else if ((ccBuf[nNextRead][0] == 0x14) && (ccBuf[nNextRead][1] == 0x2D)) // _1CR_
					{
						ccExpected[0] = 0x14; // _1RU4_
						ccExpected[1] = 0x27;
					}
					else if ((ccBuf[nNextRead][0] == 0x14) && (ccBuf[nNextRead][1] == 0x27)) // _1RU4_
					{
						ccExpected[0] = 0x11; // _1WHT_
						ccExpected[1] = 0x20;
					}
					else if ((ccBuf[nNextRead][0] == 0x11) && (ccBuf[nNextRead][1] == 0x20)) // _1WHT_
					{
						ccExpected[0] = 'A';
						ccExpected[1] = 'B';
					}
					else
					{
						// reset expected stuff... we kinda lost sync
						ccExpected[0] = ccExpected[1] = 0;
					}
				}

			} // if dwBytesRead == 2
			g_Driver.ReadData(ccBuf[nNextRead], CCBUF_SIZE, &dwBytesRead, &overlapped[nNextRead]);
			nNextRead = (nNextRead+1 < CONCURRENT_READS ? nNextRead+1 : 0);
		} //if
		else
		{
			Sleep(10);
		}
	} //while

	/*
	//
	// close our handles to the events
	//
	nStatus = 0;
	for (nNextRead = 0; !nStatus && nNextRead < CONCURRENT_READS; nNextRead++)
	{

		if (!CloseHandle(overlapped[nNextRead].hEvent))
		{
			AfxMessageBox("CloseHandle() failed!  Please restart the application.");
		}
	}
	*/
	g_eventThreadDone.SetEvent();
	return(0);
}


void CCcbcDlg::OnRun() 
{
	if (!m_bIsRunning)
	{
		//
		// start getting CC data!
		//
		if (!g_Driver.IsValid())
		{
			AfxMessageBox("Driver is invalid.\n");
		}
		else
		{
			//
			// clear then add scanlines
			//
			if (g_Driver.ClearRequestedScanlines())
			{
				AfxMessageBox("Unable to clear scanlines.\n");
			}
			else
			{
				if (g_Driver.AddRequestedScanline(21))
				{
					AfxMessageBox("Unable to add requested scanline 21.\n");
				}
				else
				{
					if (g_Driver.ClearRequestedVideoFields())
					{
						AfxMessageBox("Unable to clear requested video fields.\n");
					}
					else
					{
						if (g_Driver.AddRequestedVideoField(KS_CC_SUBSTREAM_ODD))
						{
							AfxMessageBox("Unable to clear requested video fields.\n");
						}
						else
						{
							g_eventStopCapture.ResetEvent();
							//
							// create a suspended thread
							//
							m_pThread = AfxBeginThread(CCReadThread, 
													GetSafeHwnd(), 
													THREAD_PRIORITY_ABOVE_NORMAL, 
													0, 
													CREATE_SUSPENDED, 
													NULL);
							if (m_pThread == NULL)
							{
								AfxMessageBox("The capture thread could not be created.");
							}
							else
							{
								if (m_pThread->ResumeThread() > 1)
								{
									AfxMessageBox("The worker thread seems to be suspended.");
								}
								else
								{
									m_bIsRunning = TRUE;

								}
							}

						}
					}
				}
			}
			if (!m_bIsRunning)
			{
				//
				// something went wrong
				//
				ctlRun().SetCheck(FALSE);
			}
		}
	}
	else
	{
		//
		// shut down the capture thread by signalling to it
		//
		g_eventStopCapture.SetEvent();
		if (m_pThread)
		{
			DWORD dwResult = ::WaitForSingleObject(g_eventThreadDone.m_hObject, 5000);
			//
			// did the thread signal that it's shutting down within in 5 seconds?
			// it should have...
			//
			if (dwResult != WAIT_OBJECT_0)
			{
				AfxMessageBox("Worker thread may not have terminated!");
			}
		}
		m_pThread = NULL;
		m_bIsRunning = FALSE;
	}
}




void CCcbcDlg::OnResetStats() 
{
	g_dwBytesRead = 0;
	g_dwErrors = 0;

	SetDlgItemInt(IDC_BYTESREAD, 0, FALSE);
	SetDlgItemInt(IDC_ERRORS, 0, FALSE);

	SetDlgItemText(IDC_STATUS, "");

	ctlResetStats().SetCheck(0);
}
