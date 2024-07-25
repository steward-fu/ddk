// MyPropertyUi.cpp: implementation of the CMyPropertyUi class.
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
#include <Notify.h>
#include "MyPropertyPage.h"
#include "MyPropertyUi.h"
#include "MyBindingPolicy.h"


#include <NotifyObject.h>
typedef CNotifyObject<CMyBindingPolicy,CMyPropertyUi> MyNotifyObject;


//===========================================================================
//
// Construction/Destruction
//
//===========================================================================
CMyPropertyUi::CMyPropertyUi(CNetCfgComponent& Comp) :
	CPropertyUi(Comp)
{
}

CMyPropertyUi::~CMyPropertyUi()
{
}


//===========================================================================
// Modify parameter information in the registry
//
// Parameters:
// 	none
// Returns:
//	Return S_OK if successful, indicating that the changes were applied
//	Return S_FALSE if no changes were made
//	Return NETCFG_S_REBOOT if the applied changes require a system reboot 
// Comments:
// 	Make changes to the registry to apply any changes to the Component's 
//	configuration that the user made.
//===========================================================================
HRESULT CMyPropertyUi::ApplyRegistryChanges(void)
{
    ATLTRACE(_T("CMyPropertyUi::ApplyRegistryChanges\n"));

	return WriteRegistryParameters()?S_OK:S_FALSE;
}


//===========================================================================
// Store in memory changes proposed for the component's configuration state
//
// Parameters:
// 	none
// Returns:
//	Return S_OK if successful 
//	Return S_FALSE if no changes were made
// Comments:
// 	Not supposed to save the properties to the registry yet.
//===========================================================================
HRESULT CMyPropertyUi::ApplyProperties(void)
{
    ATLTRACE(_T("CMyPropertyUi::ApplyProperties\n"));

	HRESULT hr = S_FALSE;

	if (m_wsBundleID != m_MyPage.m_wsBundleID)
	{
		m_fBundleIdChanged = true;
		m_wsBundleID = m_MyPage.m_wsBundleID;
		hr = S_OK;
	}

	if (m_wsParam1 != m_MyPage.m_wsParam1)
	{
		m_fParam1Changed = true;
		m_wsParam1 = m_MyPage.m_wsParam1;
		hr = S_OK;
	}

	return hr;
}


//===========================================================================
// Create property page(s) for displaying in Network Control Panel
//
// Parameters:
// 	pdwDefPages		- Number of default pages to display
//	pahpspPrivate	- Array of handles to custom property pages
//	pcPages			- number entries in the array
//	hwndParent		- handle to parent window
//	pszStartPage	- name of initial property page
// Returns:
//	Return S_OK if successful 
// Comments:
// 	Not supposed to save the properties to the registry yet.
//===========================================================================
HRESULT CMyPropertyUi::MergePropPages(
	DWORD*	pdwDefPages,  
	BYTE**	pahpspPrivate, 																	
	UINT*	pcPages,  																	
	HWND	hwndParent,  																	
	LPCWSTR*pszStartPage
	)
{
    ATLTRACE(_T("CMyPropertyUi::MergePropPages\n"));

    HPROPSHEETPAGE* ahpsp = (HPROPSHEETPAGE*)CoTaskMemAlloc(sizeof(HPROPSHEETPAGE));
	if (!ahpsp) return S_OK;

	// Read parameters from the registry
	ReadRegistryParameters();

	// Create the property page
	*ahpsp = m_MyPage.Create();

	// Intialize OUT parameters
    *pdwDefPages = 0;
    *pcPages = 1;

    *pahpspPrivate = (LPBYTE) ahpsp;

	return S_OK;
}


//===========================================================================
// Read configuration parameters from the registry 
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	The config parameters are read from the Protocol key and the
//  adapter instance subkey based upon the adapter context which
//	we are displayed in the Network Control Panel App.
//===========================================================================
void CMyPropertyUi::ReadRegistryParameters(void)
{
	// The configuration parameters are stored per
	// adapter instance based upon the Adapter context
	CRegKey AdapKey; 
	AdapKey.Attach(MyNotifyObject::OpenAdapterKey(m_GuidAdapterContext,m_Protocol));

	// Temporary buffer to store results read from registry
	ULONG dwSize = MAX_PATH;
	WCHAR Text[MAX_PATH]; Text[0] = '\0';

	// Query for the BundleID
	RegQueryValueExW(
		AdapKey, 
		L"BundleId", 
		NULL, 
		NULL,
		(LPBYTE) Text, 
		&dwSize
		);

	// Initialize data members
	m_wsBundleID = m_MyPage.m_wsBundleID = Text;

	Text[0] = '\0';

	// Query for the Param1 value
	RegQueryValueExW(
		AdapKey, 
		L"PARAM_1", 
		NULL, 
		NULL,
		(LPBYTE) Text, 
		&dwSize
		);

	// Initialize data members
	m_wsParam1 = m_MyPage.m_wsParam1 = Text;
}


//===========================================================================
// Write configuration parameters that have changed to the registry
//
// Parameters:
// 	none
// Returns:
//	Return true if any changes were written to the registry
//	Return false if no changes were made
// Comments:
// 	none
//===========================================================================
bool CMyPropertyUi::WriteRegistryParameters(void)
{
	bool fRet = false;

	// The configuration parameters are stored per
	// adapter instance based upon the Adapter context
	CRegKey AdapKey;
	AdapKey.Attach(MyNotifyObject::OpenAdapterKey(m_GuidAdapterContext,m_Protocol));

	// If the BundleID changed, update the registry
	if (m_fBundleIdChanged)
	{
		RegSetValueEx(
			AdapKey, 
			L"BundleId", 
			NULL, 
			REG_SZ,
			(LPBYTE) m_wsBundleID.c_str(),
			m_wsBundleID.length()*sizeof(WCHAR)
			);

		fRet = true;
	}

	// If the Param1 changed, update the registry
	if (m_fParam1Changed)
	{
		RegSetValueEx(
			AdapKey, 
			L"PARAM_1", 
			NULL, 
			REG_SZ,
			(LPBYTE) m_wsParam1.c_str(),
			m_wsParam1.length()*sizeof(WCHAR)
			);

		fRet = true;
	}

	return fRet;
}
