// NetCfgComponent.cpp: implementation of the CNetCfgComponent class.
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

#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>

#include "Notify.h"


//////////////////////////////////////////////////////////////////////
// INetCfgComponent-like methods
//////////////////////////////////////////////////////////////////////


WCHAR*	CNetCfgComponent::GetInstanceGuid()
{
	WCHAR* pRet = NULL;
	if (m_pComp) {

		GUID G;
		m_hrLastError = GetInstanceGuid(&G);

		if (S_OK == m_hrLastError) {

			pRet = new WCHAR[GUID_CCH];
			if (pRet) {

				int cchGuid = StringFromGUID2(G,pRet,GUID_CCH);
				// Did StringFromGUID2 work?

				if (GUID_CCH != cchGuid) {
					delete pRet;
					pRet = NULL;
				}
			}
		}
	}

	return pRet;
}

HKEY CNetCfgComponent::OpenParamKey()
{
	HKEY hkey = NULL;
	if (m_pComp) {
		m_hrLastError = m_pComp->OpenParamKey(&hkey);
	}

	return hkey;
}

LPWSTR CNetCfgComponent::GetBindName()
{
	LPWSTR pszwBindName = NULL;
	if (m_pComp) {
		m_hrLastError = m_pComp->GetBindName(&pszwBindName);
	}

	return pszwBindName;
}

LPWSTR CNetCfgComponent::GetFullBindName()
{
	LPWSTR pRet = NULL;

	// get the binding name
	LPWSTR pszwBindName = GetBindName();
	if (pszwBindName) {
		// prepend string to bind name
		LPWSTR pDevice = L"\\Device\\";

		// calculate total length of new string
		ULONG ulLength = (ULONG)wcslen(pDevice)+(ULONG)wcslen(pszwBindName)+1;
		// allocate new string
		pRet = new WCHAR[ulLength];

		if (pRet) {
			// initialize the string
			memset(pRet,0,ulLength*sizeof(WCHAR));
			// copy the device string to the new string
			wcscpy(pRet,pDevice);
			// concatenate the binding name to the new string
			wcscat(pRet,pszwBindName);
		}

		// Free the bind name - all done with it
		CoTaskMemFree(pszwBindName);
	}

	return pRet;
}


INetCfgComponentBindings* CNetCfgComponent::GetBindings()
{
	if (!m_pBindings)
	{
		m_hrLastError = m_pComp->QueryInterface(
			IID_INetCfgComponentBindings,
			reinterpret_cast<PVOID*>(&m_pBindings)
			);

		if (S_OK == m_hrLastError)
		{
			m_pBindings->AddRef();
		}
	}

	return m_pBindings;
}

INetCfgBindingPath* CNetCfgComponent::GetFirstBindPath(DWORD dwFlags)
{
	INetCfgBindingPath* pRet = NULL;
	m_hrLastError = E_POINTER;
	IEnumNetCfgBindingPath* p = EnumBindingPaths(dwFlags);
	if (p)
	{
		m_hrLastError = p->Reset();
		ULONG nNumRetrieved = 0;
		m_hrLastError = p->Next(1,&pRet,&nNumRetrieved);
	}

	return pRet;
}

INetCfgBindingPath* CNetCfgComponent::GetNextBindPath(DWORD dwFlags)
{
	INetCfgBindingPath* pRet = NULL;
	m_hrLastError = E_POINTER;
	IEnumNetCfgBindingPath* p = EnumBindingPaths(dwFlags);
	if (p)
	{
		ULONG nNumRetrieved = 0;
		m_hrLastError = p->Next(1,&pRet,&nNumRetrieved);
	}

	return pRet;
}

IEnumNetCfgBindingPath* CNetCfgComponent::EnumBindingPaths(DWORD dwFlags)
{

	if (!m_pBindPaths)
	{
		// Obtain the INetCfgComponentBindings interface
		INetCfgComponentBindings* pBindings = GetBindings();

		if (pBindings)
		{
			m_hrLastError = pBindings->EnumBindingPaths(
				dwFlags,
				&m_pBindPaths
				);
			if (S_OK == m_hrLastError)
			{
				m_pBindPaths->AddRef();
			}
		}
	}

	return m_pBindPaths;
}

bool CNetCfgComponent::operator == (LPCWSTR pszwId)
{
	bool bRetVal = false;
	LPCWSTR pszwCompId = GetId();
	if (pszwCompId && pszwId)
	{
		// case insensitive compare
		if ( !wcsicmp(
				pszwCompId,
				pszwId
				)
		   )
		{
			// Found a match
			bRetVal = true;
		}
	}

	return bRetVal;
}
