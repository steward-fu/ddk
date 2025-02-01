//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// scanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scan.h"
#include "scanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CScanDlg dialog

CScanDlg::CScanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScanDlg, CDialog)
	//{{AFX_MSG_MAP(CScanDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_REFRESHALL, OnRefreshAll)
	ON_BN_CLICKED(IDC_CCREQALL, OnCCReqall)
	ON_BN_CLICKED(IDC_CCREQSPECIFY, OnCCReqspecify)
	ON_EN_UPDATE(IDC_CCLINE, OnUpdateCCLine)
	ON_BN_CLICKED(IDC_NFREQ800, OnNFreq800)
	ON_BN_CLICKED(IDC_NFREQALL, OnNFreqall)
	ON_BN_CLICKED(IDC_NFREQSPECIFY, OnNFreqspecify)
	ON_EN_KILLFOCUS(IDC_NFGROUP, OnKillfocusNFgroup)
	ON_BN_CLICKED(IDC_NREQALL, OnNreqall)
	ON_BN_CLICKED(IDC_NREQSPECIFY, OnNreqspecify)
	ON_EN_UPDATE(IDC_NLINE, OnUpdateNline)
	ON_BN_CLICKED(IDC_RefreshCC, OnRefreshCC)
	ON_BN_CLICKED(IDC_RefreshNabts, OnRefreshNabts)
	ON_BN_CLICKED(IDC_RefreshNabtsFEC, OnRefreshNabtsFEC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanDlg message handlers

BOOL CScanDlg::OnInitDialog()
{
	const int cxTab = 15;
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
	
	// let's initialize our stuff
	m_CCDiscovered.DwordBitArray[0] = 0;
	ctlCCOdd().SetCheck(1);
	ctlCCEven().SetCheck(1);
	ctlCCClearReq().SetCheck(1);
	ctlCCReqAll().SetCheck(1);
	ctlCCLine().EnableWindow(FALSE);	
	ctlCCLineSpin().EnableWindow(FALSE);
	ctlCCLineSpin().SetRange(10,21);
	ctlCCLineSpin().SetPos(21);
	ctlCC().SetTabStops(cxTab);

	m_NabtsDiscovered.DwordBitArray[0] = 0;
	ctlNClearReq().SetCheck(1);
	ctlNReqAll().SetCheck(1);
	ctlNLine().EnableWindow(FALSE);	
	ctlNLineSpin().EnableWindow(FALSE);
	ctlNLineSpin().SetRange(10,21);
	ctlNLineSpin().SetPos(21);
	ctlNabts().SetTabStops(cxTab);

	for (unsigned int i = 0; i < 128; i++)
		m_NabtsFECDiscovered.SubstreamMask[i] = 0;
	ctlNFReqAll().SetCheck(0);
	ctlNFReq800().SetCheck(1);
	ctlNFReqSpecify().SetCheck(0);
	ctlNFGroup().SetLimitText(3);
	ctlNFGroup().SetWindowText(_T("8F0"));
	ctlNFGroup().EnableWindow(FALSE);
	ctlNFClearReq().SetCheck(1);

	m_pCCDriver = NULL;
	m_pNabtsDriver = NULL;
	m_pNabtsFECDriver = NULL;

	RefreshVBIInfo();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScanDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScanDlg::OnPaint() 
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
HCURSOR CScanDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/*
#define ENTRIES(a)  (sizeof(a)/sizeof(*(a)))
#define CONCURRENT_READS    8
#define READ_BUFFER_SIZE    2
*/
void CScanDlg::ScanCC(ICCDecode& CCDriver)
{
	CString strMsg;
	int	nStatus = 0;
	BOOL bValid;

	bValid = CCDriver.IsValid();
	strMsg.Format("CC: IsValid()=%s.", (bValid ? "True" : "False"));
	PrintMessage(strMsg);
	if (!bValid)
	{
		return;
	}

		if (ctlCCClearReq().GetCheck())
	{
		nStatus = CCDriver.ClearRequestedScanlines();
		strMsg.Format("CC: ClearRequestedScanlines()=%d", nStatus);
		PrintMessage(strMsg);
		if (nStatus != 0)
			return;
	}

	if (ctlCCReqAll().GetCheck())
	{
		for (int nScanline = 10; nScanline < 22; nScanline++)
		{
			nStatus = CCDriver.AddRequestedScanline(nScanline);
			strMsg.Format("CC: AddRequestedScanlines(%d)=%d", nScanline, nStatus);
			PrintMessage(strMsg);
			if (nStatus != 0)
			{
				return;
			}
		}
	}
	else
	{
		int nScanline;
		nScanline = GetDlgItemInt(IDC_CCLINE, NULL, FALSE);
		nStatus = CCDriver.AddRequestedScanline(nScanline);
		strMsg.Format("CC: AddRequestedScanlines(%d)=%d", nScanline, nStatus);
		PrintMessage(strMsg);
		if (nStatus != 0)
		{
			return;
		}
	}

	nStatus = CCDriver.ClearRequestedVideoFields();
	strMsg.Format("CC: ClearRequestedVideoFields()=%d", nStatus);
	PrintMessage(strMsg);
	if (nStatus != 0)
		return;
	int nSubstream;
	if (ctlCCOdd().GetCheck())
	{
		nSubstream = KS_CC_SUBSTREAM_ODD;
		nStatus = CCDriver.AddRequestedVideoField(nSubstream);
		strMsg.Format("CC: AddRequestedVideoField(%d)=%d", nSubstream, nStatus);
		PrintMessage(strMsg);
		if (nStatus != 0)
			return;
	}

	if (ctlCCEven().GetCheck())
	{
		nSubstream = KS_CC_SUBSTREAM_EVEN;
		nStatus = CCDriver.AddRequestedVideoField(nSubstream);
		strMsg.Format("CC: AddRequestedVideoField(%d)=%d", nSubstream, nStatus);
		PrintMessage(strMsg);
		if (nStatus != 0)
			return;
	}

	// look at first 32 scanlines or one DWORD of bits
	m_CCDiscovered.DwordBitArray[0] = 0;
	nStatus = CCDriver.GetDiscoveredScanlines(m_CCDiscovered);
	strMsg.Format("CC: GetDiscoveredScanlines()=%d", nStatus);
	PrintMessage(strMsg);
	if (nStatus)
	{
		strMsg.Format("CC: GetDiscoveredScanlines() returned %04x", m_CCDiscovered.DwordBitArray[0]);
		PrintMessage(strMsg);
	}
}


void CScanDlg::ScanNabts(INabts& NabtsDriver)
{
	CString strMsg;
	int nStatus;
	BOOL bValid;

	bValid = NabtsDriver.IsValid();
	strMsg.Format("NABTS: IsValid()=%s.", (bValid ? "True" : "False"));
	PrintMessage(strMsg);
	if (!bValid)
	{
		return;
	}
	
	if (ctlNClearReq().GetCheck())
	{
		nStatus = NabtsDriver.ClearRequestedScanlines();
		strMsg.Format("NABTS: ClearRequestedScanlines()=%d", nStatus);
		PrintMessage(strMsg);
		if (nStatus != 0)
		{
			return;
		}
	}

	if (ctlNReqAll().GetCheck())
	{
		for (int nScanline = 10; nScanline < 22; nScanline++)
		{
			nStatus = NabtsDriver.AddRequestedScanline(nScanline);
			strMsg.Format("NABTS: AddRequestedScanlines(%d)=%d", nScanline, nStatus);
			PrintMessage(strMsg);
			if (nStatus != 0)
			{
				return;
			}
		}
	}
	else
	{
		int nScanline;
		nScanline = GetDlgItemInt(IDC_NLINE, NULL, FALSE);
		nStatus = NabtsDriver.AddRequestedScanline(nScanline);
		strMsg.Format("NABTS: AddRequestedScanlines(%d)=%d", nScanline, nStatus);
		PrintMessage(strMsg);
		if (nStatus != 0)
		{
			return;
		}
	}

	nStatus = NabtsDriver.GetDiscoveredScanlines(m_NabtsDiscovered);
	strMsg.Format("NABTS: GetDiscoveredScanlines()=%d", nStatus);
	PrintMessage(strMsg);
}


void CScanDlg::ScanNabtsFEC(INabtsFEC& NabtsFECDriver)
{
	CString strMsg;
	int nStatus;
	BOOL bValid;

	bValid = NabtsFECDriver.IsValid();
	strMsg.Format("NABTS/FEC: IsValid()=%s.", (bValid ? "True" : "False"));
	PrintMessage(strMsg);
	if (!bValid)
	{
		return;
	}
	
	if (ctlNFClearReq().GetCheck())
	{
		nStatus = NabtsFECDriver.ClearRequestedGroups();
		strMsg.Format("NABTS/FEC: ClearRequestedGroups()=%d", nStatus);
		PrintMessage(strMsg);
		if (nStatus != 0)
		{
			return;
		}
	}

	unsigned int uGroupID;
	if (ctlNFReqAll().GetCheck())
	{
		PrintMessage("NABTS/FEC: Adding all groups.");
		for (uGroupID = 0; uGroupID < 0x1000; uGroupID++)
		{
			nStatus = NabtsFECDriver.AddRequestedGroup(uGroupID);
			if (nStatus != 0)
			{
				strMsg.Format("NABTS/FEC: AddRequestedGroup(0x%03X)=%d", uGroupID, nStatus);
				PrintMessage(strMsg);
				return;
			}
		}
	}
	else if (ctlNFReq800().GetCheck())
	{
		PrintMessage("NABTS/FEC: Adding groups 0x800-0x8FF.");
		for (unsigned int uGroupID = 0x800; uGroupID < 0x900; uGroupID++)
		{
			nStatus = NabtsFECDriver.AddRequestedGroup(uGroupID);
			if (nStatus != 0)
			{
				strMsg.Format("NABTS/FEC: AddRequestedGroup(0x%03X)=%d", uGroupID, nStatus);
				PrintMessage(strMsg);
				return;
			}
		}
	}
	else if (ctlNFReqSpecify().GetCheck())
	{
		TCHAR cBuf[4];
		ctlNFGroup().GetWindowText(cBuf, 4);
		sscanf(cBuf, "%X", &uGroupID);
		nStatus = NabtsFECDriver.AddRequestedGroup(uGroupID);
		strMsg.Format("NABTS/FEC: AddRequestedGroup(0x%03X)=%d", uGroupID, nStatus);
		PrintMessage(strMsg);
		if (nStatus)
			return;
	}

	nStatus = NabtsFECDriver.GetDiscoveredGroups(m_NabtsFECDiscovered);
	strMsg.Format("NABTS/FEC: GetDiscoveredGroups()=%d", nStatus);
	PrintMessage(strMsg);
}

// Refresh the list box using the info stored in m_Lines[]
void CScanDlg::RefreshVBIInfo()
{
	unsigned int i, j;
	CString strInfo;

	ctlCC().ResetContent();
	for (i = 10; i < 22; i++)
	{
		DWORD bit = 1 << i;

		strInfo.Format("%02u\t%s", i, 
			(bit & m_CCDiscovered.DwordBitArray[0] ? "CC" : "-")
			);
		ctlCC().AddString(strInfo);
	}

	ctlNabts().ResetContent();
	for (i = 10; i < 22; i++)
	{
		DWORD bit = 1 << i;

		strInfo.Format("%02u\t%s", i, 
			(bit & m_NabtsDiscovered.DwordBitArray[0] ? "Nabts" : "-")
			);
		ctlNabts().AddString(strInfo);
	}

	ctlNabtsFEC().ResetContent();
	for (i = 0; i < 128; i++)
	{
		if (m_NabtsFECDiscovered.SubstreamMask[i])
		{
			for (j = 0; j < 32; j++)
			{
				DWORD bit = (1 << j);
				if (bit & m_NabtsFECDiscovered.SubstreamMask[i])
				{
					strInfo.Format("%03X", (i*32)+j);
					ctlNabtsFEC().AddString(strInfo);
				}
			}
		}
	}
}

// print a message in the bottom box
void CScanDlg::PrintMessage(LPCSTR szMsg)
{
	while (ctlListMessage().GetCount() > 50)
	{
		ctlListMessage().DeleteString(0);
	}

	ctlListMessage().AddString(szMsg);
	ctlListMessage().SetTopIndex(ctlListMessage().GetCount() - 1);
}

void CScanDlg::OnRefreshAll() 
{
	m_pCCDriver = new ICCDecode;
	m_pNabtsDriver = new INabts;
	m_pNabtsFECDriver = new INabtsFEC;

	ScanCC(*m_pCCDriver);
	ScanNabts(*m_pNabtsDriver);
	ScanNabtsFEC(*m_pNabtsFECDriver);

	delete m_pCCDriver;
	delete m_pNabtsDriver;
	delete m_pNabtsFECDriver;

	m_pCCDriver = NULL;
	m_pNabtsDriver = NULL;
	m_pNabtsFECDriver = NULL;

	RefreshVBIInfo();	
}


void CScanDlg::OnCCReqall() 
{
	ctlCCLine().EnableWindow(FALSE);	
	ctlCCLineSpin().EnableWindow(FALSE);
}

void CScanDlg::OnCCReqspecify() 
{
	ctlCCLine().EnableWindow(TRUE);	
	ctlCCLineSpin().EnableWindow(TRUE);
}

void CScanDlg::OnUpdateCCLine() 
{
	int nScanline = GetDlgItemInt(IDC_CCLINE, NULL, FALSE);
	if ((nScanline < 10) || (nScanline > 21))
	{
		nScanline = 21;
		ctlCCLineSpin().SetPos(21);
	}
}

void CScanDlg::OnNFreqall() 
{
	ctlNFGroup().EnableWindow(FALSE);
}

void CScanDlg::OnNFreq800() 
{
	ctlNFGroup().EnableWindow(FALSE);
}


void CScanDlg::OnNFreqspecify() 
{
	ctlNFGroup().EnableWindow(TRUE);	
}

void CScanDlg::OnKillfocusNFgroup() 
{
	unsigned int uGroupID = 242;
	TCHAR cBuf[4];
	ctlNFGroup().GetWindowText(cBuf, 4);
	sscanf(cBuf, "%3x", &uGroupID);
	if (uGroupID > 0xFFF)
	{
		uGroupID = 0x242;
	}
	sprintf(cBuf, "%03X", uGroupID);
	ctlNFGroup().SetWindowText(cBuf);	
}

void CScanDlg::OnNreqall() 
{
	ctlNLine().EnableWindow(FALSE);	
	ctlNLineSpin().EnableWindow(FALSE);
}

void CScanDlg::OnNreqspecify() 
{
	ctlNLine().EnableWindow(TRUE);	
	ctlNLineSpin().EnableWindow(TRUE);
}

void CScanDlg::OnUpdateNline() 
{
	int nScanline = GetDlgItemInt(IDC_NLINE, NULL, FALSE);
	if ((nScanline < 10) || (nScanline > 21))
	{
		nScanline = 21;
		ctlNLineSpin().SetPos(21);
	}
}

void CScanDlg::OnRefreshCC() 
{
	m_pCCDriver = new ICCDecode;
	ScanCC(*m_pCCDriver);
	delete m_pCCDriver;
	m_pCCDriver = NULL;
	RefreshVBIInfo();	
}

void CScanDlg::OnRefreshNabts() 
{
	m_pNabtsDriver = new INabts;
	ScanNabts(*m_pNabtsDriver);
	delete m_pNabtsDriver;
	m_pNabtsDriver = NULL;
	RefreshVBIInfo();	
}

void CScanDlg::OnRefreshNabtsFEC() 
{
	m_pNabtsFECDriver = new INabtsFEC;
	ScanNabtsFEC(*m_pNabtsFECDriver);
	delete m_pNabtsFECDriver;
	m_pNabtsFECDriver = NULL;
	RefreshVBIInfo();	
}
