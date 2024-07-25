// NetCfgComponent.h: interface for the CNetCfgComponent class.
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

#if !defined(AFX_NETCFGCOMPONENT_H__FC656853_D4FC_11D4_B047_00C04F6A5DA9__INCLUDED_)
#define AFX_NETCFGCOMPONENT_H__FC656853_D4FC_11D4_B047_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef GUID_CCH
#define GUID_CCH 39
#endif

//////////////////////////////////////////////////////////////////////
// CNetCfgComponent
//
// This class wraps the INetCfgComponent interface pointer.  It
// provides some convenience methods and automatic reference
// counting (i.e. AddRef() and Release() from CTOR/DTOR.
//
// The INetCfgBindingPath interface provides methods to obtain
// information about and to control a network component.
//
class CNetCfgComponent {

// Construction/Initialization
public:
	CNetCfgComponent();
	CNetCfgComponent(INetCfgComponent* p);
	CNetCfgComponent(const CNetCfgComponent& C);
	CNetCfgComponent& operator=(const CNetCfgComponent& C);
	virtual ~CNetCfgComponent();
	void Initialize(INetCfgComponent* p);
	bool IsValid();

// Methods
public:
	BOOL		IsPhysicalNIC();
	DWORD		GetCharacteristics();
	// retrieve instance GUID
	HRESULT		GetInstanceGuid(GUID* p);
	WCHAR*		GetInstanceGuid();
	// open registry parameters key
	HKEY		OpenParamKey();
	// get binding 'name'
	LPWSTR		GetBindName();
	// get full binding 'name'
	LPWSTR		GetFullBindName();
	HRESULT		GetLastError();
	// get human readable ID
	LPCWSTR		GetId();
	// get the first binding path containing the component
	INetCfgBindingPath*	GetFirstBindPath(DWORD dwFlags);
	// get the next binding path containing the component
	INetCfgBindingPath* GetNextBindPath(DWORD dwFlags);
	// get a com enumerator of binding paths
	IEnumNetCfgBindingPath*		EnumBindingPaths(DWORD dwFlags);
	// get another interface for bindings
	INetCfgComponentBindings*	GetBindings();

	operator INetCfgComponent*();
	INetCfgComponent* operator->();

	// compare ID of the component with given ID
	bool operator == (LPCWSTR pszwId);
	bool operator != (LPCWSTR pszwId);

// Data Members
protected:

	// cached handle to parameters registry key
	CRegKey		m_ParamRegKey;
	// cached COM return value
	HRESULT		m_hrLastError;
	// cached pointer to the ID
	LPWSTR		m_pszwId;
	// pointer to the underlying interface
	INetCfgComponent*	m_pComp;
	// pointer to bindings collection
	INetCfgComponentBindings*	m_pBindings;
	// pointer to com enumerator of binding paths
	IEnumNetCfgBindingPath*		m_pBindPaths;
};


//////////////////////////////////////////////////////////////////////
// Inline Implementations


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline CNetCfgComponent::CNetCfgComponent()
	: m_pComp(NULL),
		m_hrLastError(S_OK),
			m_pBindings(NULL),
				m_pBindPaths(NULL),
					m_pszwId(NULL)
{
}

inline CNetCfgComponent::CNetCfgComponent(INetCfgComponent* p)
	: m_pComp(NULL),
		m_hrLastError(S_OK),
			m_pBindings(NULL),
				m_pBindPaths(NULL),
					m_pszwId(NULL)
{
	Initialize(p);
}

inline CNetCfgComponent::CNetCfgComponent(const CNetCfgComponent& C)
	: m_pComp(C.m_pComp),
		m_hrLastError(E_POINTER),
			m_pBindings(NULL),
				m_pBindPaths(NULL),
					m_pszwId(NULL)
{
	if (m_pComp) m_pComp->AddRef();
}

inline CNetCfgComponent& CNetCfgComponent::operator=(const CNetCfgComponent& C)
{
	if (this != &C)
	{
		if (m_pComp) m_pComp->Release();
		m_hrLastError = S_OK;
		m_pBindings = NULL;
		m_pBindPaths = NULL;
		m_pComp = C.m_pComp;
		if (m_pComp) m_pComp->AddRef();
	}

	return *this;
}

inline CNetCfgComponent::~CNetCfgComponent()
{
	if (m_pszwId)
		CoTaskMemFree(m_pszwId);

	if (m_pBindPaths) m_pBindPaths->Release();
	if (m_pBindings) m_pBindings->Release();
	if (m_pComp) m_pComp->Release();
	m_pComp = NULL;
}

inline void CNetCfgComponent::Initialize(INetCfgComponent* p)
{
	if (p)
	{
		m_pComp = p;
		m_pComp->AddRef();
	}
}

inline bool CNetCfgComponent::IsValid()
{
	return m_pComp != NULL;
}


//////////////////////////////////////////////////////////////////////
// INetCfgComponent-like methods
//////////////////////////////////////////////////////////////////////

inline BOOL CNetCfgComponent::IsPhysicalNIC()
{
	return GetCharacteristics() & NCF_PHYSICAL;
}

inline DWORD CNetCfgComponent::GetCharacteristics()
{
	DWORD dwRet = 0;
	m_hrLastError = E_POINTER;
	if (m_pComp)
		m_hrLastError = m_pComp->GetCharacteristics(&dwRet);
	return dwRet;
}


inline HRESULT CNetCfgComponent::GetInstanceGuid(GUID* p)
{
	m_hrLastError = E_POINTER;
	if (m_pComp)
		m_hrLastError = m_pComp->GetInstanceGuid(p);
	return m_hrLastError;
}


inline HRESULT CNetCfgComponent::GetLastError()
{
	return m_hrLastError;
}

inline LPCWSTR CNetCfgComponent::GetId()
{
	if (!m_pszwId)
		m_hrLastError = m_pComp->GetId(&m_pszwId);

	return m_pszwId;
}

inline CNetCfgComponent::operator INetCfgComponent*()
{
	return m_pComp;
}

inline INetCfgComponent* CNetCfgComponent::operator->()
{
	return m_pComp;
}

inline bool CNetCfgComponent::operator != (LPCWSTR pszwId)
{
	return ! (*this == pszwId);
}

#endif // !defined(AFX_NETCFGCOMPONENT_H__FC656853_D4FC_11D4_B047_00C04F6A5DA9__INCLUDED_)
