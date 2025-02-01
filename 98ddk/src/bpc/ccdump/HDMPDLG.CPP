//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// hDmpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hDmp.h"
#include "hDmpDlg.h"
#include <afxmt.h>
#include <ccdecode.h>
//#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEvent			g_eventStopCapture;
CEvent			g_eventThreadDone;
HANDLE			g_hCaptureFile;
BOOL			g_bClearParity;
BOOL			g_bIgnoreExtraNulls;
HANDLE			g_hCom;
DWORD			g_dwBytesCaptured = 0;

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
// CHDmpDlg dialog

CHDmpDlg::CHDmpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHDmpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHDmpDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHDmpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHDmpDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHDmpDlg, CDialog)
	//{{AFX_MSG_MAP(CHDmpDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DUMPSTART, OnDumpStart)
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DUMPSTOP, OnDumpStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHDmpDlg message handlers

BOOL CHDmpDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	m_pThread = NULL;
	ctlDumpFile().SetLimitText(128);
	ctlDumpFile().SetWindowText("C:\\HUBDUMP.CC");
	ctlComSpin().SetRange(1,2);
	ctlComSpin().SetPos(1);

	ctlSrcCom().SetCheck(1);
	ctlSrcIcodec().SetCheck(0);

	SetDlgItemInt(IDC_BYTESCAPTURED, 0, FALSE);
	SetDlgItemInt(IDC_COMEDIT, LOWORD(ctlComSpin().GetPos()), FALSE);

	ctlDumpStart().EnableWindow(TRUE);
	ctlDumpStop().EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHDmpDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHDmpDlg::OnPaint() 
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
HCURSOR CHDmpDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//#define HUBCAP_ONLY
#ifndef HUBCAP_ONLY
// Save the data from the CCDECODE driver
UINT SaveIcodecData(LPVOID pParam)
{
#define CONCURRENT_READS 32
#define CCBUF_SIZE 2
	ICCDecode		Driver;
	static OVERLAPPED	overlapped[CONCURRENT_READS] = {0};
	static BYTE			ccBuf[CONCURRENT_READS][CCBUF_SIZE];
	static HANDLE		event[CONCURRENT_READS];
	unsigned int	nStatus = 0;
	BYTE			ccExpected[CCBUF_SIZE] = {0};
	DWORD			dwBytesRead = 0;
	DWORD			dwBytesWritten;
	int				nNextRead = 0;
	BOOL			bLastWasNull = FALSE;

	if (!Driver.IsValid())
	{
		AfxMessageBox("Driver isn't valid, worker thread exiting.");
		g_eventThreadDone.SetEvent();
		return(GetLastError());
	}
	Driver.ClearRequestedScanlines();
	Driver.AddRequestedScanline(21);
	Driver.ClearRequestedVideoFields();
	Driver.AddRequestedVideoField(KS_CC_SUBSTREAM_ODD);

	g_eventStopCapture.ResetEvent();
	g_eventThreadDone.ResetEvent();

	//
	// fire off concurrent reads
	//
	for( nNextRead = 0; !nStatus && nNextRead < CONCURRENT_READS; nNextRead++ )
	{
		//
		// create events...
		//
		if (!(overlapped[nNextRead].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		{
			AfxMessageBox("CreateEvent() failed!  Please restart the application.");
			g_eventThreadDone.SetEvent();
			return(GetLastError());
		}
		event[nNextRead] = overlapped[nNextRead].hEvent;
		nStatus = Driver.ReadData(ccBuf[nNextRead], CCBUF_SIZE, &dwBytesRead, 
			overlapped + nNextRead);
		if (!nStatus || nStatus == ERROR_IO_PENDING)
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
	while (::WaitForSingleObject(g_eventStopCapture.m_hObject, 0) != (WAIT_OBJECT_0))
	{
		nNextRead = ::WaitForMultipleObjects(CONCURRENT_READS, event, FALSE, 10);
		if ((nNextRead >= WAIT_OBJECT_0) && (nNextRead <= WAIT_OBJECT_0 + (CONCURRENT_READS - 1)))
		{
			nNextRead -= WAIT_OBJECT_0;
			nStatus = Driver.GetOverlappedResult(&overlapped[nNextRead], &dwBytesRead, FALSE);
			if (!nStatus && dwBytesRead)
			{
				unsigned int i;

				if (g_bClearParity)
				{
					// let's strip parity
					for (i = 0; i < dwBytesRead; i++)
					{
						ccBuf[nNextRead][i] &= 0x7F;
					}
				}

				if (g_bIgnoreExtraNulls)
				{
					// if nulls, just ignore; don't event count
					if (ccBuf[nNextRead][0] || ccBuf[nNextRead][1])
					{
						bLastWasNull = FALSE;
					}
					else if (!ccBuf[nNextRead][0] && !ccBuf[nNextRead][1])
					{
						if (!bLastWasNull)
						{
							// flag that zeroes we found, then set our flag
							bLastWasNull = TRUE;
						}
						else
						{
							// ignore the extra zeroes
							Driver.ReadData(ccBuf[nNextRead], CCBUF_SIZE, &dwBytesRead, &overlapped[nNextRead]);
							continue;
						}
					}
				}


				WriteFile(g_hCaptureFile, ccBuf[nNextRead], dwBytesRead, &dwBytesWritten, NULL);
				g_dwBytesCaptured += dwBytesRead;
				SetDlgItemInt((HWND) pParam, IDC_BYTESCAPTURED, g_dwBytesCaptured, FALSE);
				dwBytesRead = 0;

			}
			Driver.ReadData(ccBuf[nNextRead], CCBUF_SIZE, &dwBytesRead, &overlapped[nNextRead]);

		} //if
		else
		{
			Sleep(10);
		}
	} //while

	//
	// close our save file
	//
	CloseHandle(g_hCaptureFile);
	for( nNextRead = 0; !nStatus && nNextRead < CONCURRENT_READS; nNextRead++ )
	{
		 CloseHandle(overlapped[nNextRead].hEvent);
	}
	g_eventThreadDone.SetEvent();

	// 
	// our class lib driver gets closed...
	//
	return(0);
}
#else
UINT SaveIcodecData(LPVOID pParam)
{
	CloseHandle(g_hCaptureFile);
	AfxMessageBox("Icodec library support is missing from this version.");
	g_eventThreadDone.SetEvent();
	return(0);
}
#endif


#define BUFFERSIZE 32

// save the COM port data
UINT SaveHubcapData(LPVOID pParam)
{
	DWORD	dwBytesRead = 0;
	DWORD	dwBytesWritten;
	CHAR	szBuffer[BUFFERSIZE];

	g_eventThreadDone.ResetEvent();
	//
	// loop, grab the data from the COM port
	//
	while (::WaitForSingleObject(g_eventStopCapture.m_hObject, 0) != (WAIT_OBJECT_0))
	{
		ReadFile(g_hCom, &szBuffer, BUFFERSIZE, &dwBytesRead, NULL);
		if (dwBytesRead)
		{
			if (g_bClearParity)
			{
				// let's strip parity
				for (unsigned int i = 0; i < BUFFERSIZE; i++)
				{
					szBuffer[i] &= 0x7F;
				}
			}

			WriteFile(g_hCaptureFile, &szBuffer, dwBytesRead, &dwBytesWritten, NULL);
			g_dwBytesCaptured += dwBytesRead;
			SetDlgItemInt((HWND) pParam, IDC_BYTESCAPTURED, g_dwBytesCaptured, FALSE);
			dwBytesRead = 0;
		}
	}

	//
	// close our COM handles and save file
	//
	CloseHandle(g_hCom);
	CloseHandle(g_hCaptureFile);
	g_eventThreadDone.SetEvent();

	return 0;
}


#define DISABLEBUTTONS \
	ctlDumpFile().EnableWindow(FALSE); \
	ctlFileExists().EnableWindow(FALSE); \
	ctlClearParity().EnableWindow(FALSE); \
	ctlIgnoreExtraNulls().EnableWindow(FALSE); \
	ctlComSpin().EnableWindow(FALSE); \
	ctlSrcIcodec().EnableWindow(FALSE); \
	ctlSrcCom().EnableWindow(FALSE)

#define ENABLEBUTTONS \
	ctlDumpFile().EnableWindow(TRUE); \
	ctlFileExists().EnableWindow(TRUE); \
	ctlClearParity().EnableWindow(TRUE); \
	ctlIgnoreExtraNulls().EnableWindow(TRUE); \
	ctlComSpin().EnableWindow(TRUE); \
	ctlSrcIcodec().EnableWindow(TRUE); \
	ctlSrcCom().EnableWindow(TRUE)

void CHDmpDlg::OnDumpStart() 
{
	COMMTIMEOUTS		CommTimeouts;
	DCB					dcb;

	//
	// set global flag to see if we're clearing parity bit
	//
	g_bClearParity = ctlClearParity().GetCheck();
	g_bIgnoreExtraNulls = ctlIgnoreExtraNulls().GetCheck();

	DISABLEBUTTONS;
	g_dwBytesCaptured = 0;
	//
	// begin capture - open a capture file
	//
	ctlDumpFile().GetWindowText(m_strSaveFileName);
	if (ctlFileExists().GetCheck())
	{
		g_hCaptureFile = CreateFile(m_strSaveFileName,
									GENERIC_READ, 
									0, 
									NULL, 
									OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, 
									0);
		if (g_hCaptureFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(g_hCaptureFile);
			int iRetVal = MessageBox(
				"Warning!  The output file already exists.  Continue?",
				"File Already Exists",
				MB_ICONEXCLAMATION|MB_YESNO);
			if (iRetVal == IDNO)
			{
				ENABLEBUTTONS;
				return;
			}
		}
	}
	g_hCaptureFile = CreateFile(m_strSaveFileName,
								GENERIC_WRITE, 
								0, 
								NULL, 
								CREATE_ALWAYS, 
								FILE_ATTRIBUTE_NORMAL, 
								0);
	if (g_hCaptureFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(
			"The output file could not be opened.",
			"File Open Failed",
			MB_ICONEXCLAMATION|MB_OK);
		ENABLEBUTTONS;
		return;
	}
	g_eventStopCapture.ResetEvent();
	//
	// create a suspended thread
	//
	m_pThread = AfxBeginThread(
							(ctlSrcIcodec().GetCheck() ? 
								SaveIcodecData : 
								SaveHubcapData), 
							GetSafeHwnd(), 
							THREAD_PRIORITY_NORMAL, 
							0, 
							CREATE_SUSPENDED, 
							NULL);
	if (m_pThread == NULL)
	{
		MessageBox(
			"The capture thread could not be created.",
			"AfxBeginThread Failed",
			MB_ICONEXCLAMATION|MB_OK);
		CloseHandle(g_hCaptureFile);
		ENABLEBUTTONS;
		return;
	}
	if (ctlSrcCom().GetCheck())
	{
		//
		//  Set up our COM port
		//
		CString strComPort;
		strComPort.Format("COM%u", LOWORD(ctlComSpin().GetPos()));
		g_hCom = CreateFile((LPCSTR) strComPort, 
							GENERIC_READ, 
							0, 
							NULL, 
							OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL);
		if (g_hCom == INVALID_HANDLE_VALUE)
		{
			MessageBox(
				"The COM port could not be opened for reading.",
				"COM Port Open Failed",
				MB_ICONEXCLAMATION|MB_OK);
			CloseHandle(g_hCaptureFile);
			CloseHandle(g_hCom);
			ENABLEBUTTONS;
			return;
		}
		GetCommState(g_hCom, &dcb);
		dcb.BaudRate = CBR_9600;
		dcb.fBinary = TRUE;
		dcb.fParity = TRUE;
		dcb.fOutxCtsFlow = TRUE;
		dcb.fOutxDsrFlow = TRUE;
		dcb.fOutX = FALSE;
		dcb.fNull = FALSE;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		SetCommState(g_hCom, &dcb);

		CommTimeouts.ReadIntervalTimeout = MAXDWORD;
		CommTimeouts.ReadTotalTimeoutMultiplier = 0;
		CommTimeouts.ReadTotalTimeoutConstant = 0;
		SetCommTimeouts(g_hCom, &CommTimeouts);
	}

	if (m_pThread->ResumeThread() > 1)
	{
		MessageBox(
			"The worker thread seems to be suspended.",
			"Thread Suspended",
			MB_ICONEXCLAMATION|MB_OK);
	}

	ctlDumpStart().EnableWindow(FALSE);
	ctlDumpStop().EnableWindow(TRUE);
}


void CHDmpDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetDlgItemInt(IDC_COMEDIT, LOWORD(ctlComSpin().GetPos()), FALSE);
}

void CHDmpDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	g_eventStopCapture.SetEvent();
	if (m_pThread)
	{
		DWORD dwRetVal = ::WaitForSingleObject(g_eventThreadDone.m_hObject, 5000);
		if (dwRetVal == WAIT_TIMEOUT)
		{
			CloseHandle(g_hCom);
			CloseHandle(g_hCaptureFile);
		}
	}
	m_pThread = NULL;

	ctlDumpFile().EnableWindow(TRUE);
	ctlFileExists().EnableWindow(TRUE);
	ctlClearParity().EnableWindow(TRUE);
	ctlIgnoreExtraNulls().EnableWindow(TRUE);
	ctlComSpin().EnableWindow(TRUE);
	ctlSrcIcodec().EnableWindow(TRUE);
	ctlSrcCom().EnableWindow(TRUE);
}

void CHDmpDlg::OnDumpStop() 
{
	// end capture
	g_eventStopCapture.SetEvent();
	if (m_pThread)
	{
		::WaitForSingleObject(g_eventThreadDone.m_hObject, INFINITE);
	}
	m_pThread = NULL;;

	ctlDumpFile().EnableWindow(TRUE);
	ctlFileExists().EnableWindow(TRUE);
	ctlClearParity().EnableWindow(TRUE);
	ctlIgnoreExtraNulls().EnableWindow(TRUE);
	ctlComSpin().EnableWindow(TRUE);
	ctlSrcIcodec().EnableWindow(TRUE);
	ctlSrcCom().EnableWindow(TRUE);

	ctlDumpStart().EnableWindow(TRUE);
	ctlDumpStop().EnableWindow(FALSE);
}
