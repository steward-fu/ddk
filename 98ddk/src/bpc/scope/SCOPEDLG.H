//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// scopeDlg.h : header file for the dialog class
//

#if !defined(AFX_SCOPEDLG_H__5633DDA8_8900_11D1_9301_0060B0575ABD__INCLUDED_)
#define AFX_SCOPEDLG_H__5633DDA8_8900_11D1_9301_0060B0575ABD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "..\testgrf\testgrf.h" // my test graph
#include "testgrf.h" // my test graph
// {2CBDE200-56EB-11d1-8F82-A033C8B05564}
DEFINE_GUID(CLSID_VBIScope, 
0x2cbde200, 0x56eb, 0x11d1, 0x8f, 0x82, 0xa0, 0x33, 0xc8, 0xb0, 0x55, 0x64);

// my own test graph class derived from CTestGraph
class CDerivedTestGraph : public CTestGraph
{
private:
	void PrintDebugString(LPCSTR s) {}
};

/////////////////////////////////////////////////////////////////////////////
// CScopeDlg dialog

class CScopeDlg : public CDialog
{
private:
	CButton&			ctlOk() { return *(CButton*) GetDlgItem (IDOK); }
	CButton&			ctlCancel() { return *(CButton*) GetDlgItem (IDCANCEL); }
	CEdit&				ctlDebugString() { return *(CEdit*) GetDlgItem (IDC_DEBUGSTRING); }
	CButton&			ctlPlay() { return *(CButton*) GetDlgItem (IDC_PLAY); }
	CFrameWnd&			ctlVideoFrame() { return *(CFrameWnd*) GetDlgItem (IDC_VIDEOFRAME); }
	CEdit&				ctlChStatic() { return *(CEdit*) GetDlgItem (IDC_CHSTATIC); }
	CEdit&				ctlChEdit() { return *(CEdit*) GetDlgItem (IDC_CHEDIT); }
	CSpinButtonCtrl&	ctlChSpin() { return *(CSpinButtonCtrl*) GetDlgItem (IDC_CHSPIN); }
	CButton&			ctlGo() { return *(CButton*) GetDlgItem (IDC_GO); }

	void		DisplayPropertyPage(LPCSTR);
	BOOL		SetChannel(long, long* = NULL);
	long		GetChannel();
	CString		m_strMsg;

// Construction
public:
	CScopeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScopeDlg)
	enum { IDD = IDD_SCOPE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScopeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScopeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPlay();
	afx_msg void OnTVTuner();
	afx_msg void OnCrossbar();
	afx_msg void OnGo();
	afx_msg void OnDeltaposChSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOPEDLG_H__5633DDA8_8900_11D1_9301_0060B0575ABD__INCLUDED_)
