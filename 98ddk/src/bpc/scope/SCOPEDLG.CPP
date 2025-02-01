//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// scopeDlg.cpp : implementation file for the dialog 
//

#include "stdafx.h"
#include "scope.h"
#include "scopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CDerivedTestGraph g_Graph;
extern BOOL BuildGraph(BOOL);

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
// CScopeDlg dialog

CScopeDlg::CScopeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScopeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScopeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScopeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScopeDlg, CDialog)
	//{{AFX_MSG_MAP(CScopeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_TVTUNER, OnTVTuner)
	ON_BN_CLICKED(IDC_XBAR, OnCrossbar)
	ON_BN_CLICKED(IDC_GO, OnGo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CHSPIN, OnDeltaposChSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScopeDlg message handlers

BOOL CScopeDlg::OnInitDialog()
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

	ctlDebugString().SetWindowText("Ok.");
	//
	// place video window inside our dialog
	//
	IVideoWindow *pVid = NULL;
	g_Graph.m_IGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&pVid);
	if (pVid)
	{
		pVid->put_Owner((OAHWND) ctlVideoFrame().GetSafeHwnd());
		pVid->put_WindowStyle(WS_CHILD);
		RECT r;
		ctlVideoFrame().GetClientRect(&r);
		pVid->SetWindowPosition(r.left, r.top, r.right, r.bottom);
		pVid->Release();
	}

	ctlChEdit().SetLimitText(4);
	ctlChSpin().SetRange(0, 181);
	SetDlgItemInt(IDC_CHEDIT, GetChannel(),FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScopeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScopeDlg::OnPaint() 
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
		SetDlgItemInt(IDC_CHSTATIC, GetChannel(),FALSE);
		SetDlgItemInt(IDC_CHEDIT, GetChannel(),FALSE);
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScopeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CScopeDlg::OnPlay() 
{
	static BOOL bPressed = FALSE;
	if (!bPressed)
	{
		//SetChannel(GetChannel());
		ctlPlay().SetCheck(1);
		ctlDebugString().SetWindowText("Attempting to run graph...");
		g_Graph.Run();
		ctlDebugString().SetWindowText("Running.");
		bPressed = TRUE;
	}
	else
	{
		ctlPlay().SetCheck(0);
		ctlDebugString().SetWindowText("Stopping...");
		g_Graph.Stop();
		ctlDebugString().SetWindowText("Stopped.");
		bPressed = FALSE;
	}
}


void CScopeDlg::OnTVTuner() 
{
	DisplayPropertyPage("ATI TVTuner");
}


void CScopeDlg::OnCrossbar()
{
	DisplayPropertyPage("Bt829 Crossbar");
}


void CScopeDlg::DisplayPropertyPage(LPCSTR szFilterName)
{
	// display tuner property page
	IBaseFilter *pFilter = g_Graph.FindFilterByName(szFilterName);
	if (pFilter)
	{
		ISpecifyPropertyPages *pProp = NULL;
		pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
		if (pProp)
		{
			CAUUID caGUID;
			pProp->GetPages(&caGUID);
			pProp->Release();

			// display modal dialog box
			OleCreatePropertyFrame(
				this->GetSafeHwnd(),
				0,
				0,
				L"Filter",				// Caption for the dialog box
				1,						// Number of filters
				(IUnknown **)&pFilter,	// Pointer to the filter whose prop pg is displayed
				caGUID.cElems,			// Number of property pages
				caGUID.pElems,			// Pointer to property page CLSIDs
				0,
				0,
				NULL);
			long actual_channel = GetChannel();
			SetDlgItemInt(IDC_CHSTATIC, actual_channel, FALSE);
			SetDlgItemInt(IDC_CHEDIT, actual_channel, FALSE);
		}
		pFilter->Release();
	}
}


BOOL CScopeDlg::SetChannel(long proposed_channel, long* actual_channel)
{
	HRESULT hr = 0;
	IBaseFilter *pFilter = NULL;
	IAMTVTuner *pTuner = NULL;

	pFilter = g_Graph.FindFilterByName("ATI TVTuner");
	if (pFilter)
	{
		hr = pFilter->QueryInterface(IID_IAMTVTuner, (void **)&pTuner);
		if (SUCCEEDED(hr))
		{
			hr = pTuner->put_Channel(proposed_channel, AMTUNER_SUBCHAN_DEFAULT, AMTUNER_SUBCHAN_DEFAULT);
			if (actual_channel)
			{
				long dummy = AMTUNER_SUBCHAN_DEFAULT;
				hr = pTuner->get_Channel(actual_channel, &dummy, &dummy);
			}
			pTuner->Release();
		}
	}
	if (pFilter)
		pFilter->Release();
	return (SUCCEEDED(hr));
}


long CScopeDlg::GetChannel()
{
	HRESULT hr = 0;
	long channel, dummy = AMTUNER_SUBCHAN_DEFAULT;
	IBaseFilter *pFilter = NULL;
	IAMTVTuner *pTuner = NULL;

	pFilter = g_Graph.FindFilterByName("ATI TVTuner");
	if (SUCCEEDED(hr))
	{
		hr = pFilter->QueryInterface(IID_IAMTVTuner, (void **)&pTuner);
		if (SUCCEEDED(hr))
		{
			hr = pTuner->get_Channel(&channel, &dummy, &dummy);
			pTuner->Release();
		}
	}
	if (pFilter)
		pFilter->Release();
	return channel;
}

void CScopeDlg::OnGo() 
{
	long requested_channel, actual_channel;
	SetChannel(requested_channel = GetDlgItemInt(IDC_CHEDIT, NULL, FALSE), &actual_channel);
	if (requested_channel == actual_channel)
	{
		ctlChSpin().SetPos(actual_channel);
	}
	SetDlgItemInt(IDC_CHEDIT, actual_channel,FALSE);
}

void CScopeDlg::OnDeltaposChSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	long iPos = pNMUpDown->iPos, result;
	SetChannel(iPos + pNMUpDown->iDelta, &result);
	*pResult = (iPos == result);
}
