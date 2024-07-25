// NetCfgBindingPath.h: interface for the CNetCfgBindingPath class.
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

#if !defined(AFX_NETCFGBINDINGPATH_H__00C20024_D816_11D4_B047_00C04F6A5DA9__INCLUDED_)
#define AFX_NETCFGBINDINGPATH_H__00C20024_D816_11D4_B047_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// CNetCfgBindingPath
//
// This class wraps the INetCfgBindingPath interface pointer.  It
// provides some convenience methods and automatic reference
// counting (i.e. AddRef() and Release() from CTOR/DTOR.
//
// The INetCfgBindingPath interface provides methods to obtain
// information about a binding path.  A binding path is a set of
// connected binding interfaces.
//
class CNetCfgBindingPath
{
// Construction/Initialization
public:
	CNetCfgBindingPath(INetCfgBindingPath* p);
	CNetCfgBindingPath();
	void Initialize(INetCfgBindingPath* p);
	bool IsValid();
	virtual ~CNetCfgBindingPath();

// Methods
public:
	// retrieve unique ID of binding path
	LPWSTR	GetPathToken();
	ULONG	GetDepth();
	bool	IsEnabled();
	bool	Disable();
	bool	Enable();
	void	DumpBindingPath();
	INetCfgComponent*			 GetOwner();
	INetCfgBindingInterface*	 GetNextInterface();
	INetCfgBindingInterface*	 GetFirstInterface();
	IEnumNetCfgBindingInterface* EnumBindingInterfaces();

// Data members
protected:
	HRESULT						 m_hrLastError;
	INetCfgBindingPath*			 m_pBindPath;
	IEnumNetCfgBindingInterface* m_pIntfEnum;
};


//////////////////////////////////////////////////////////////////////
// Inline Implementations


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline CNetCfgBindingPath::CNetCfgBindingPath()
	: m_pIntfEnum(NULL),
		m_pBindPath(NULL),
			m_hrLastError(S_OK)
{
}

inline CNetCfgBindingPath::~CNetCfgBindingPath()
{
	if (m_pIntfEnum) {
		m_pIntfEnum->Release();
		m_pIntfEnum = NULL;
	}

	if (m_pBindPath) {
		m_pBindPath->Release();
		m_pBindPath = NULL;
	}
}

inline CNetCfgBindingPath::CNetCfgBindingPath(INetCfgBindingPath *p)
	: m_pIntfEnum(NULL),
		m_pBindPath(p),
			m_hrLastError(S_OK)
{
	if (m_pBindPath) {
		m_pBindPath->AddRef();
		EnumBindingInterfaces();
	}
}

inline bool CNetCfgBindingPath::IsValid()
{
	return m_pBindPath != NULL;
}

inline void CNetCfgBindingPath::Initialize(INetCfgBindingPath *p)
{
	m_pBindPath = p;
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		m_pBindPath->AddRef();
		EnumBindingInterfaces();
	}
}

inline bool CNetCfgBindingPath::Enable()
{
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		m_hrLastError = m_pBindPath->Enable(TRUE);
	}

	return m_hrLastError == S_OK;
}

inline bool CNetCfgBindingPath::Disable()
{
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		m_hrLastError = m_pBindPath->Enable(FALSE);
	}

	return m_hrLastError == S_OK;
}

inline bool CNetCfgBindingPath::IsEnabled()
{
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		m_hrLastError = m_pBindPath->IsEnabled();
	}

	return m_hrLastError == S_OK;
}

inline ULONG CNetCfgBindingPath::GetDepth()
{
	ULONG nRet = 0;
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		m_hrLastError = m_pBindPath->GetDepth(&nRet);
	}

	return nRet;
}

inline INetCfgComponent* CNetCfgBindingPath::GetOwner()
{
	INetCfgComponent* pRet = NULL;
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		m_hrLastError = m_pBindPath->GetOwner(&pRet);
	}

	return pRet;
}

inline LPWSTR CNetCfgBindingPath::GetPathToken()
{
	LPWSTR pRet = NULL;
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		m_hrLastError = m_pBindPath->GetPathToken(&pRet);
	}

	return pRet;
}

inline INetCfgBindingInterface* CNetCfgBindingPath::GetFirstInterface()
{
	INetCfgBindingInterface* pRet = NULL;
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		if (m_pIntfEnum) {
			m_hrLastError = m_pIntfEnum->Reset();
			ULONG nNumRetrieved = 0;
			m_hrLastError = m_pIntfEnum->Next(1,&pRet,&nNumRetrieved);
		}
	}

	return pRet;
}

inline INetCfgBindingInterface* CNetCfgBindingPath::GetNextInterface()
{
	INetCfgBindingInterface* pRet = NULL;
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		if (m_pIntfEnum) {
			ULONG nNumRetrieved = 0;
			m_hrLastError = m_pIntfEnum->Next(1,&pRet,&nNumRetrieved);
		}
	}

	return pRet;
}

inline IEnumNetCfgBindingInterface* CNetCfgBindingPath::EnumBindingInterfaces()
{
	m_hrLastError = E_POINTER;
	if (m_pBindPath) {
		if (!m_pIntfEnum) {
			m_hrLastError = m_pBindPath->EnumBindingInterfaces(&m_pIntfEnum);
		}
	}

	return m_pIntfEnum;
}

inline void CNetCfgBindingPath::DumpBindingPath()
{
    ATLTRACE(_T("CNetCfgBindingPath::DumpBindingPath\n"));

	// GetPathToken
	LPWSTR pszwPathToken = GetPathToken();
	if ( pszwPathToken ) {
		ATLTRACE(_T("\tToken: %ws\n"), pszwPathToken);
		CoTaskMemFree(pszwPathToken);
	}

	// GetOwner
	INetCfgComponent * pComponent = GetOwner();
	if ( pComponent ) {
		CNetCfgComponent Comp(pComponent);
		LPCWSTR pszwId = Comp.GetId();
		if (pszwId) {
			ATLTRACE(_T("\tOwner: %ws\n"), pszwId);
		}
	}

	// GetDepth - retrieves number of binding interfaces contained in a binding path
	ULONG nInterfaces = GetDepth();
	ATLTRACE(_T("\tDepth: %x\n"), nInterfaces);

	// Enabled?
	ATLTRACE(_T("\tEnabled: %ws\n"), IsEnabled()?L"YES":L"NO");

	// EnumBindingInterfaces
	INetCfgBindingInterface * pBindInterface = GetFirstInterface();
	while (pBindInterface) {
		CNetCfgBindingInterface BindIntf(pBindInterface);
		BindIntf.DumpBindingInterface();
		pBindInterface = GetNextInterface();
	}
}

#endif // !defined(AFX_NETCFGBINDINGPATH_H__00C20024_D816_11D4_B047_00C04F6A5DA9__INCLUDED_)
