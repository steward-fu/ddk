// NetCfgBindingInterface.h: interface for the CNetCfgBindingInterface class.
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

#if !defined(AFX_NETCFGBINDINGINTERFACE_H__00C20021_D816_11D4_B047_00C04F6A5DA9__INCLUDED_)
#define AFX_NETCFGBINDINGINTERFACE_H__00C20021_D816_11D4_B047_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// CNetCfgBindingInterface
//
// This class wraps the INetCfgBindingInterface interface pointer.  It
// provides some convenience methods and automatic reference
// counting (i.e. AddRef() and Release() from CTOR/DTOR.
//
// The INetCfgBindingInterface interface provides methods to obtain
// information about a binding interface.  A binding interface is
// a binding between an upper and a lower component.
//
class CNetCfgBindingInterface
{
public:
// CTOR/DTOR
	CNetCfgBindingInterface(INetCfgBindingInterface* p);
	CNetCfgBindingInterface();
	void Initialize(INetCfgBindingInterface* p);
	virtual ~CNetCfgBindingInterface();
	bool IsValid();

// INetCfgBindingInterface methods
	INetCfgComponent* GetLowerComponent();
	INetCfgComponent* GetUpperComponent();
	LPWSTR GetName();

// Helper methods
	void DumpBindIntfComp(INetCfgComponent* pComp);
	void DumpBindingInterface();
	HRESULT GetLastError();

protected:
// Data members
	INetCfgBindingInterface* m_pBindIntf;
	HRESULT	m_hrLastError;
};


//////////////////////////////////////////////////////////////////////
// Inline Implementations


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline CNetCfgBindingInterface::CNetCfgBindingInterface()
	: m_pBindIntf(NULL),
		m_hrLastError(S_OK)
{
}

inline CNetCfgBindingInterface::~CNetCfgBindingInterface()
{
	if (m_pBindIntf) {
		m_pBindIntf->Release();
		m_pBindIntf = NULL;
	}
}

inline CNetCfgBindingInterface::CNetCfgBindingInterface(INetCfgBindingInterface *p)
	: m_hrLastError(S_OK)
{
	Initialize(p);
}

inline void CNetCfgBindingInterface::Initialize(INetCfgBindingInterface *p)
{
	m_pBindIntf = p;
	if (m_pBindIntf)
		m_pBindIntf->AddRef();
}

inline bool CNetCfgBindingInterface::IsValid()
{
	return m_pBindIntf != NULL;
}

// Obtain the name of the binding interface.
// remember to call CoTaskMemFree to release the memory
inline LPWSTR CNetCfgBindingInterface::GetName()
{
	LPWSTR pRet = NULL;
	if (m_pBindIntf)
	{
		m_hrLastError = m_pBindIntf->GetName(&pRet);
	}
	return pRet;
}

// obtain the upper component interface
inline INetCfgComponent* CNetCfgBindingInterface::GetUpperComponent()
{
	INetCfgComponent* pRet = NULL;
	if (m_pBindIntf) {
		m_hrLastError = m_pBindIntf->GetUpperComponent(&pRet);
	}
	return pRet;
}

// obtain the lower component interface
inline INetCfgComponent* CNetCfgBindingInterface::GetLowerComponent()
{
	INetCfgComponent* pRet = NULL;
	if (m_pBindIntf) {
		m_hrLastError = m_pBindIntf->GetLowerComponent(&pRet);
	}
	return pRet;
}

inline HRESULT CNetCfgBindingInterface::GetLastError()
{
	return m_hrLastError;
}

inline void CNetCfgBindingInterface::DumpBindingInterface()
{
    ATLTRACE(_T("CNetCfgBindingInterface::DumpBindingInterface\n"));

	LPWSTR pszwName = GetName();
	if (pszwName) {
		ATLTRACE(_T("\tBindIntf: %ws \t"),pszwName);
		CoTaskMemFree(pszwName);
		pszwName = NULL;
	}

	DumpBindIntfComp(GetUpperComponent());
	ATLTRACE(_T(" -> "));
	DumpBindIntfComp(GetLowerComponent());
	ATLTRACE(_T("\n"));
}

inline void CNetCfgBindingInterface::DumpBindIntfComp(INetCfgComponent* pComp)
{
	if (pComp) {
		CNetCfgComponent Comp(pComp);
		LPCWSTR pszwName = Comp.GetId();
		if (pszwName) {
			ATLTRACE(_T("%ws"),pszwName);
		}
	}
}

#endif // !defined(AFX_NETCFGBINDINGINTERFACE_H__00C20021_D816_11D4_B047_00C04F6A5DA9__INCLUDED_)
