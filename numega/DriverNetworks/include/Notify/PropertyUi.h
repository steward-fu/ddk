// PropertyUi.h: interface for the CPropertyUi class.
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#if !defined(AFX_PROPERTYUI_H__61BC0DD3_38B1_11D5_B069_00C04F6A5DA9__INCLUDED_)
#define AFX_PROPERTYUI_H__61BC0DD3_38B1_11D5_B069_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CPropertyUi
//
// This class implements the Property Ui functionality required for a Notify Object.
//
class CPropertyUi
{
// Constructors/Destructors
public:
	CPropertyUi(CNetCfgComponent& Protocol);
	virtual ~CPropertyUi();

// Handlers
	virtual HRESULT ApplyRegistryChanges();
	virtual HRESULT QueryPropertyUi(IUnknown* p);
	virtual HRESULT SetContext(IUnknown* p);
	virtual HRESULT MergePropPages(
		DWORD*	pdwDefPages,
		BYTE**	pahpspPrivate,
		UINT*	pcPages,
		HWND	hwndParent,
		LPCWSTR*pszStartPage
		);

	virtual HRESULT ApplyPnpChanges(INetCfgPnpReconfigCallback * pCallback);
	virtual HRESULT CancelChanges(void);

	virtual HRESULT ValidateProperties(HWND hWnd);
	virtual HRESULT ApplyProperties(void);
	virtual HRESULT CancelProperties(void);

// Data Members
protected:
	CNetCfgComponent m_Protocol;

	// Context which the UI is being displayed
    GUID             m_GuidAdapterContext;

};


inline CPropertyUi::CPropertyUi(CNetCfgComponent& Protocol) :
	m_GuidAdapterContext(GUID_NULL),
		m_Protocol(Protocol)
{
}


inline CPropertyUi::~CPropertyUi()
{
}


inline HRESULT CPropertyUi::ApplyRegistryChanges()
{
	return S_OK;
}


inline HRESULT CPropertyUi::QueryPropertyUi(IUnknown* p)
{
	return S_OK;
}


inline HRESULT CPropertyUi::SetContext(IUnknown* p)
{
    ATLTRACE(_T("CNotifyObject::SetContext\n"));

    if (p) // set the new context
    {
		// Assuming LAN context
		m_GuidAdapterContext = GUID_NULL;

        p->AddRef();

        INetLanConnectionUiInfo* pInfo;
        HRESULT hr = p->QueryInterface(
							IID_INetLanConnectionUiInfo,
							reinterpret_cast<PVOID *>(&pInfo)
							);

        if (S_OK == hr)
        {
            hr = pInfo->GetDeviceGuid(&m_GuidAdapterContext);
            pInfo->Release();
        }

		p->Release();
    }

	return S_OK;
}


inline HRESULT CPropertyUi::MergePropPages(
	DWORD*	pdwDefPages,
	BYTE**	pahpspPrivate,
	UINT*	pcPages,
	HWND	hwndParent,
	LPCWSTR*pszStartPage
	)
{
	return S_OK;
}


inline HRESULT CPropertyUi::ApplyPnpChanges(INetCfgPnpReconfigCallback * pCallback)
{
	return S_OK;
}


inline HRESULT CPropertyUi::CancelChanges()
{
	return S_OK;
}


inline HRESULT CPropertyUi::ValidateProperties(HWND hWnd)
{
	return S_OK;
}


inline HRESULT CPropertyUi::ApplyProperties(void)
{
	return S_OK;
}


inline HRESULT CPropertyUi::CancelProperties(void)
{
	return S_OK;
}

#endif // !defined(AFX_PROPERTYUI_H__61BC0DD3_38B1_11D5_B069_00C04F6A5DA9__INCLUDED_)
