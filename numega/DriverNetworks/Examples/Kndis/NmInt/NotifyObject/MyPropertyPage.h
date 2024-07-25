// MyPropertyPage.h: interface for the CMyPropertyPage class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================


#if !defined(AFX_MYPROPERTYPAGE_H__76D16DB3_F884_11D4_B048_00C04F6A5DA9__INCLUDED_)
#define AFX_MYPROPERTYPAGE_H__76D16DB3_F884_11D4_B048_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <ATLWIN.h>
#include <commctrl.h>


//=========================================================================
// CMyPropertyPage		
//
//=========================================================================
class CMyPropertyPage : public CDialogImplBase  
{
public:
// Constructors/Destructors
	CMyPropertyPage();
	virtual ~CMyPropertyPage();

// Message Map
BEGIN_MSG_MAP(CMyPropertyPage)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
END_MSG_MAP()


// Message Handlers
	HPROPSHEETPAGE Create();
	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&);
	BOOL OnApply();
	void OnReset();
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		NMHDR* pNMHDR = (NMHDR*)lParam;

		// don't handle messages not from the page/sheet itself
		if(pNMHDR->hwndFrom != m_hWnd && pNMHDR->hwndFrom != ::GetParent(m_hWnd)) {
			bHandled = FALSE;
			return 1;
		}

		LRESULT lResult = 0;

		switch(pNMHDR->code) 
		{
		case PSN_APPLY:
			lResult = OnApply() ? PSNRET_NOERROR : PSNRET_INVALID_NOCHANGEPAGE;
			break;
		case PSN_RESET:
			OnReset();
			break;

		default:
			bHandled = FALSE;	// not handled
			break;
		}

		return lResult;
	}

	static UINT CALLBACK PropPageCallback(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
	{
		ATLASSERT(hWnd == NULL);
		if(uMsg == PSPCB_CREATE) {
			CDialogImplBase* pPage = (CDialogImplBase*)ppsp->lParam;
			_Module.AddCreateWndData(&pPage->m_thunk.cd, pPage);
		}

		if (uMsg == PSPCB_RELEASE) {
			CMyPropertyPage* pPage = (CMyPropertyPage*)ppsp->lParam;
			delete pPage;
		}

		return 1;
	}

	void SetModified(BOOL bChanged = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetParent() != NULL);

		if(bChanged)
			::SendMessage(GetParent(), PSM_CHANGED, (WPARAM)m_hWnd, 0L);
		else
			::SendMessage(GetParent(), PSM_UNCHANGED, (WPARAM)m_hWnd, 0L);
	}

// Data Members
public:

	enum { IDD = IDD_SAMPLE_NMINTNOTIFY };

	// Text Values of the controls
	wstring m_wsBundleID;
	wstring m_wsParam1;

protected:

	// Objects which cache the windows handle for the controls
	CWindow m_EditBundleID;
	CWindow m_EditParam1;

	PROPSHEETPAGE m_psp;
};


#endif // !defined(AFX_MYPROPERTYPAGE_H__76D16DB3_F884_11D4_B048_00C04F6A5DA9__INCLUDED_)
