// MyPropertyPage.cpp: implementation of the CMyPropertyPage class.
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


#include "stdafx.h"
#include <string>
using namespace std;
#include "MyPropertyPage.h"


//=============================================================================
//
// Construction/Destruction
//
//=============================================================================
CMyPropertyPage::CMyPropertyPage()
{
	// initialize PROPSHEETPAGE struct
	memset(&m_psp, 0, sizeof(PROPSHEETPAGE));
	m_psp.dwSize = sizeof(PROPSHEETPAGE);
	m_psp.dwFlags = PSP_USECALLBACK;
	m_psp.hInstance = _Module.GetResourceInstance();
	m_psp.pszTemplate = MAKEINTRESOURCE(IDD);
	m_psp.pfnDlgProc = (DLGPROC)StartDialogProc;
	m_psp.pfnCallback = PropPageCallback;
	m_psp.lParam = (LPARAM)this;

	/*
	// TODO: Add Title
	m_psp.pszTitle = "Title";
	m_psp.dwFlags |= PSP_USETITLE;
	*/
}

CMyPropertyPage::~CMyPropertyPage()
{
}


//=============================================================================
// CMyPropertyPage::Create
//
//
//=============================================================================
HPROPSHEETPAGE CMyPropertyPage::Create()
{
	_Module.AddCreateWndData(&m_thunk.cd,this);

	m_psp.hInstance = _Module.GetModuleInstance();
	m_psp.dwFlags	= PSP_DEFAULT;

	return ::CreatePropertySheetPage(&m_psp);
}


//=============================================================================
// CMyPropertyPage::OnInitDialog
//
//
//=============================================================================
LRESULT CMyPropertyPage::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	// Cache handles to the Edit boxes
	m_EditBundleID.Attach(GetDlgItem(IDC_BundleId));
	m_EditParam1.Attach(GetDlgItem(IDC_PARAM1));

	// Initialize the edit boxes text
	m_EditBundleID.SetWindowText(m_wsBundleID.c_str());
	m_EditParam1.SetWindowText(m_wsParam1.c_str());

	return 1;
}


//=============================================================================
// CMyPropertyPage::OnApply
//
//
//=============================================================================
BOOL CMyPropertyPage::OnApply()
{
	// Read the values from the edit boxes and store in data members

	// Temporary buffer to read the data from the control
	WCHAR Text[MAX_PATH]; Text[0] = '\0';

	// Get the Param1 text
	m_EditParam1.GetWindowText(Text,MAX_PATH);
	m_wsParam1 = Text;

	// Get the BundleID text
	m_EditBundleID.GetWindowText(Text,MAX_PATH);
	m_wsBundleID = Text;

	// Set the modified flag to inform the property sheet that one
	// of its pages has been modified
	SetModified();

	return TRUE;
}


//=============================================================================
// CMyPropertyPage::OnReset
//
//
//=============================================================================
void CMyPropertyPage::OnReset()
{
	// Reset the data members for the controls
	m_wsParam1.erase();
	m_wsBundleID.erase();
}


