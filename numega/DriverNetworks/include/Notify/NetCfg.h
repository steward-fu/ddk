// NetCfg.h: interface for the CNetCfg class.
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

#if !defined(AFX_NETCFG_H__FC656854_D4FC_11D4_B047_00C04F6A5DA9__INCLUDED_)
#define AFX_NETCFG_H__FC656854_D4FC_11D4_B047_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// CNetCfg
//
// This class wraps the INetCfg interface pointer.  It provides some
// convenience methods and automatic reference counting (i.e. AddRef()
// and Release() from CTOR/DTOR
//
// The INetCfg interface provides access to the Network Configuration
// Subsytem to obtain information, alter network configuration, install/
// uninstall network components.
//
class CNetCfg {

public:

// Construction/Destruction - provides automatic AddRef()/Release()
	CNetCfg();
	CNetCfg(INetCfg* p);
	virtual ~CNetCfg();
	void Initialize(INetCfg* p);

// Methods

	INetCfgClassSetup*	QueryNetCfgClass();
	// Find a network component with given hw-id
	INetCfgComponent *	FindComponent(LPCWSTR pszwComponentId);
	// Install network component with given hw-id
	INetCfgComponent*	InstallComponent(LPCWSTR pszwCompId);
	// Uninstall network component with given hw-id
	bool DeInstallComponent(INetCfgComponent* pComp);
	// operator for calling other methods of the interface
	INetCfg* operator->();

// Data Members
protected:

	// cached return code
	HRESULT		m_hrLastError;
	INetCfg*	m_pNetCfg;
};


//////////////////////////////////////////////////////////////////////
// Inline Implementations


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline CNetCfg::CNetCfg() :
	m_pNetCfg(NULL),
		m_hrLastError(S_OK)
{
}

inline CNetCfg::CNetCfg(INetCfg* p)
{
	Initialize(p);
}

inline CNetCfg::~CNetCfg()
{
	if (m_pNetCfg) m_pNetCfg->Release();
}

inline void CNetCfg::Initialize(INetCfg* p)
{
	m_pNetCfg = p;
	if (p) p->AddRef();
}


inline INetCfg* CNetCfg::operator->()
{
	return m_pNetCfg;
}


// obtain INetCfgClassSetup interface to install/uninstall
inline INetCfgClassSetup* CNetCfg::QueryNetCfgClass()
{
	INetCfgClassSetup* pRet = NULL;
	if (m_pNetCfg)
	{
		m_hrLastError = m_pNetCfg->QueryNetCfgClass(
								&GUID_DEVCLASS_NET,
								IID_INetCfgClassSetup,
								(void**)&pRet
								);
	}

	return pRet;
}


// Find a network component with given hw-id
inline INetCfgComponent* CNetCfg::FindComponent(LPCWSTR pszwComponentId)
{
	INetCfgComponent* pRet = NULL;
	m_hrLastError = m_pNetCfg->FindComponent(pszwComponentId,&pRet);
	return pRet;
}


// Install network component with given hw-id
inline INetCfgComponent* CNetCfg::InstallComponent(LPCWSTR pszwCompId)
{
	INetCfgComponent*	pComp = NULL;
    INetCfgClassSetup*  pNcClassSetup = QueryNetCfgClass();

	if (pNcClassSetup)
	{
		// pszwComponentId is the HW-ID to install
        pNcClassSetup->Install(
			pszwCompId,
			NULL,
			NSF_POSTSYSINSTALL,
			0,       // <upgrade-from-build-num>
			NULL,    // answerfile name
			NULL,    // answerfile section name
			&pComp
			);

		// Done with INetCfgClassSetup
		pNcClassSetup->Release();
	}

	return pComp;
}


// Uninstall network component with given hw-id
inline bool CNetCfg::DeInstallComponent(INetCfgComponent *pComp)
{
	HRESULT hr = E_FAIL;

    INetCfgClassSetup* pNcClassSetup = QueryNetCfgClass();

    if (pNcClassSetup)
    {
		LPWSTR pmszwRefs = NULL;

        hr = pNcClassSetup->DeInstall(
								pComp,
								NULL,
								&pmszwRefs
								);
        if (pmszwRefs)
        {
			CoTaskMemFree(pmszwRefs);
        }

        pNcClassSetup->Release();
    }

	return hr == S_OK;
}

#endif // !defined(AFX_NETCFG_H__FC656854_D4FC_11D4_B047_00C04F6A5DA9__INCLUDED_)
