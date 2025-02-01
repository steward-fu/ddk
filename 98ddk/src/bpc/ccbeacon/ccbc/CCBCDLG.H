//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// ccbcDlg.h : header file
//

#if !defined(AFX_CCBCDLG_H__DBF0BD06_965C_11D1_9301_0060B0575ABD__INCLUDED_)
#define AFX_CCBCDLG_H__DBF0BD06_965C_11D1_9301_0060B0575ABD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <ccdecode.h>
#include "ccbeacon.h"

/////////////////////////////////////////////////////////////////////////////
// CCcbcDlg dialog

class CCcbcDlg : public CDialog
{
private:
	CButton&	ctlRun() { return *(CButton*) GetDlgItem(IDC_RUN); }
	CButton&	ctlResetStats() { return *(CButton*) GetDlgItem(IDC_RESETSTATS); }

	BOOL		m_bIsRunning;
	BOOL		m_bIsPaused;
	CWinThread	*m_pThread;

	CString		m_strPattern;
	
// Construction
public:
	CCcbcDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCcbcDlg)
	enum { IDD = IDD_CCBC_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCcbcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCcbcDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRun();
	afx_msg void OnResetStats();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCBCDLG_H__DBF0BD06_965C_11D1_9301_0060B0575ABD__INCLUDED_)
