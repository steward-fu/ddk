//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// hDmpDlg.h : header file
//

#if !defined(AFX_HDMPDLG_H__5FB09354_70C9_11D1_9301_0060B0575ABD__INCLUDED_)
#define AFX_HDMPDLG_H__5FB09354_70C9_11D1_9301_0060B0575ABD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CHDmpDlg dialog

class CHDmpDlg : public CDialog
{
private:
	CButton&			ctlDumpStart() { return *(CButton*) GetDlgItem (IDC_DUMPSTART); }
	CButton&			ctlDumpStop() { return *(CButton*) GetDlgItem (IDC_DUMPSTOP); }

	CEdit&				ctlDumpFile() { return *(CEdit*) GetDlgItem (IDC_DUMPFILE); }
	CButton&			ctlFileExists() { return *(CButton*) GetDlgItem (IDC_FILEEXISTS); }
	CButton&			ctlClearParity() { return *(CButton*) GetDlgItem (IDC_CLEARPARITY); }
	CButton&			ctlIgnoreExtraNulls() { return *(CButton*) GetDlgItem (IDC_IGNOREEXTRANULLS); }
	CEdit&				ctlBytesCaptured() { return *(CEdit*) GetDlgItem (IDC_BYTESCAPTURED); }

	CButton&			ctlSrcIcodec() { return *(CButton*) GetDlgItem (IDC_SRC_ICODEC); }
	CButton&			ctlSrcCom() { return *(CButton*) GetDlgItem (IDC_SRC_COM); }
	CSpinButtonCtrl&	ctlComSpin() { return *(CSpinButtonCtrl*) GetDlgItem (IDC_COMSPIN); }
	CEdit&				ctlComEdit() { return *(CEdit*) GetDlgItem (IDC_COMEDIT); }

	CString				m_strSaveFileName;
	CWinThread*			m_pThread;


// Construction
public:
	CHDmpDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHDmpDlg)
	enum { IDD = IDD_HDMP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHDmpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHDmpDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDumpStart();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnDumpStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HDMPDLG_H__5FB09354_70C9_11D1_9301_0060B0575ABD__INCLUDED_)
