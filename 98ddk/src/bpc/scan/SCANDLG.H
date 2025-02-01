//
// Copyright (C) 1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Broadcast Architecture DDK.
// For detailed information regarding Broadcast
// Architecture, see the DDK.
//
// scanDlg.h : header file
//

#if !defined(AFX_SCANDLG_H__E3255686_7555_11D1_9301_0060B0575ABD__INCLUDED_)
#define AFX_SCANDLG_H__E3255686_7555_11D1_9301_0060B0575ABD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <ccdecode.h>
#include <nabts.h>
#include <nabtsfec.h>

/////////////////////////////////////////////////////////////////////////////
// CScanDlg dialog

class CScanDlg : public CDialog
{
private:
	CListBox&			ctlListMessage() { return *(CListBox *) GetDlgItem(IDC_LISTMESSAGE); }

	CListBox&			ctlCC() { return *(CListBox *) GetDlgItem(IDC_CC); }
	CButton&			ctlCCOdd() { return *(CButton *) GetDlgItem(IDC_CCODD); }
	CButton&			ctlCCEven() { return *(CButton *) GetDlgItem(IDC_CCEVEN); }
	CButton&			ctlCCClearReq() { return *(CButton *) GetDlgItem(IDC_CCCLEARREQ); }
	CButton&			ctlCCReqAll() { return *(CButton *) GetDlgItem(IDC_CCREQALL); }
	CButton&			ctlCCReqSpecify() { return *(CButton *) GetDlgItem(IDC_CCREQSPECIFY); }
	CEdit&				ctlCCLine() { return *(CEdit *) GetDlgItem(IDC_CCLINE); }
	CSpinButtonCtrl&	ctlCCLineSpin() { return *(CSpinButtonCtrl *) GetDlgItem(IDC_CCLINESPIN); }

	CListBox&			ctlNabts() { return *(CListBox *) GetDlgItem(IDC_NABTS); }
	CButton&			ctlNClearReq() { return *(CButton *) GetDlgItem(IDC_NCLEARREQ); }
	CButton&			ctlNReqAll() { return *(CButton *) GetDlgItem(IDC_NREQALL); }
	CButton&			ctlNReqSpecify() { return *(CButton *) GetDlgItem(IDC_NREQSPECIFY); }
	CEdit&				ctlNLine() { return *(CEdit *) GetDlgItem(IDC_NLINE); }
	CSpinButtonCtrl&	ctlNLineSpin() { return *(CSpinButtonCtrl *) GetDlgItem(IDC_NLINESPIN); }

	CListBox&			ctlNabtsFEC() { return *(CListBox *) GetDlgItem(IDC_NABTSFEC); }
	CButton&			ctlNFClearReq() { return *(CButton *) GetDlgItem(IDC_NFCLEARREQ); }
	CButton&			ctlNFReqAll() { return *(CButton *) GetDlgItem(IDC_NFREQALL); }
	CButton&			ctlNFReq800() { return *(CButton *) GetDlgItem(IDC_NFREQ800); }
	CButton&			ctlNFReqSpecify() { return *(CButton *) GetDlgItem(IDC_NFREQSPECIFY); }
	CEdit&				ctlNFGroup() { return *(CEdit *) GetDlgItem(IDC_NFGROUP); }

	ICCDecode *m_pCCDriver;
	VBICODECFILTERING_SCANLINES m_CCDiscovered;

	INabts *m_pNabtsDriver;
	VBICODECFILTERING_SCANLINES m_NabtsDiscovered;

	INabtsFEC *m_pNabtsFECDriver;
	VBICODECFILTERING_NABTS_SUBSTREAMS m_NabtsFECDiscovered;

	void ScanCC(ICCDecode&);
	void ScanNabts(INabts&);
	void ScanNabtsFEC(INabtsFEC&);

	void RefreshVBIInfo();
	void PrintMessage(LPCSTR);

// Construction
public:
	CScanDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScanDlg)
	enum { IDD = IDD_SCAN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScanDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRefreshAll();
	afx_msg void OnCCReqall();
	afx_msg void OnCCReqspecify();
	afx_msg void OnUpdateCCLine();
	afx_msg void OnNFreq800();
	afx_msg void OnNFreqall();
	afx_msg void OnNFreqspecify();
	afx_msg void OnKillfocusNFgroup();
	afx_msg void OnNreqall();
	afx_msg void OnNreqspecify();
	afx_msg void OnUpdateNline();
	afx_msg void OnRefreshCC();
	afx_msg void OnRefreshNabts();
	afx_msg void OnRefreshNabtsFEC();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANDLG_H__E3255686_7555_11D1_9301_0060B0575ABD__INCLUDED_)
