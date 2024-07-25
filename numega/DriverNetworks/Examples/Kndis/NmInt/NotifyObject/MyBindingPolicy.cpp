// MyBindingPolicy.cpp: implementation of the CMyBindingPolicy class.
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

class CMyBindingPolicy;
class CMyPropertyUi;

#include <Notify.h>
#include "MyBindingPolicy.h"
#include "MyPropertyPage.h"


//===========================================================================
//
// Construction/Destruction
//
//===========================================================================
CMyBindingPolicy::CMyBindingPolicy(CNetCfgComponent& Protocol, CNetCfg& NetCfg) :
	CImBindingPolicy(Protocol, NetCfg)
{
	SetMiniportID(L"Nm_IntIMmp");
}


CMyBindingPolicy::~CMyBindingPolicy()
{
}


//===========================================================================
// Accept/Reject binding between IM protocol edge and an 
//	existing adapter from the system
//
// Parameters:
// 	Adapter	- CNetCfgComponent object representing an existing adapter 
//				in the system.
// Returns:
// 	true - Accept the binding, false - reject the binding
// Comments:
// 	none
//===========================================================================
bool CMyBindingPolicy::OnAcceptAdapter(CNetCfgComponent& Adapter)
{
	ATLTRACE(_T("CMyBindingPolicy::OnAcceptAdapter %ws %ws\n"),Adapter.GetId(),Adapter.GetInstanceGuid());

//	TODO: optionally only handle particular adapter binding to 
//	the protocol edge of the IM driver.

/*
	LPWSTR pszwId = L"PCI\\VEN_8086&DEV_1229";
	if ( Adapter == pszwId )
	{
		bRetVal = false;
	}
*/
	
	return true;
}


//===========================================================================
// Accept/Reject binding between a protocol from the system and the 
//	IM adapter edge 
//
// Parameters:
// 	Protocol - CNetCfgComponent object representing an existing Protocol 
//				in the system.
// Returns:
// 	true - Accept the binding, false - reject the binding
// Comments:
// 	none
//===========================================================================
bool CMyBindingPolicy::OnAcceptProtocol(CNetCfgComponent& Protocol)
{
	ATLTRACE(_T("CMyBindingPolicy::OnAcceptProtocol %ws\n"),Protocol.GetId());

// TODO: optionally only handle particular protocol binding to our miniport
//	adapter portion of the IM driver.

/*
	LPWSTR pszwId = L"ms_tcpip";
	if ( Protocol == pszwId )
	{
		bRetVal = false;
	}
*/

	return true;
}


//===========================================================================
// Accept/Reject disabling binding between a protocol from the system and an 
//	adapter from the system (not part of the IM driver) 
//
// Parameters:
// 	Protocol - CNetCfgComponent object representing an existing Protocol 
//				in the system.
// 	Adapter	 - CNetCfgComponent object representing an existing Adapter 
//				in the system.
// Returns:
// 	true - Accept disabling the binding, false - reject disabling the binding
// Comments:
// 	none
//===========================================================================
bool CMyBindingPolicy::OnDisableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter)
{
	ATLTRACE(_T("CMyBindingPolicy::OnDisableBinding %ws --> %ws %ws\n"),
		Protocol.GetId(),Adapter.GetId(),Adapter.GetInstanceGuid());
	return true;
}


//===========================================================================
// Accept/Reject enabling binding between a protocol from the system and an 
//	adapter from the system (not part of the IM driver) 
//
// Parameters:
// 	Protocol - CNetCfgComponent object representing an existing Protocol 
//				in the system.
// 	Adapter	 - CNetCfgComponent object representing an existing Adapter 
//				in the system.
// Returns:
// 	true - Accept enabling the binding, false - reject enabling the binding
// Comments:
// 	none
//===========================================================================
bool CMyBindingPolicy::OnEnableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter)
{
	ATLTRACE(_T("CMyBindingPolicy::OnEnableBinding %ws --> %ws %ws\n"),
		Protocol.GetId(),Adapter.GetId(),Adapter.GetInstanceGuid());
	return true;
}


//===========================================================================
// CMyBindingPolicy::OnBindAdapter
//
//	Called by the framework to handle notification of our protocol 
//	binding to a real NIC in the system.
//
// Parameters:
// 	RealMp		- CNetCfgComponent representing a physical NIC
// Returns:
//	Return true if a virtual adapter was installed
//	Return false otherwise
//
// Comments:
// 	Here is where the virtual adapter instance(s) are to be installed.  Use
//	method InstallMiniport() to install an instance of our virtual adapter.
//	This method takes care of installing a virtual adapter instance and 
//	adding the instance GUID to the registry so the driver can retrieve it 
//	during Bind() processing.
//
//	Alternatively, use method AddMpNameToReg() to "re-use" a previously
//	installed virtual adapter instance.  This method takes care of just 
//	adding the instance GUID to the registry.
//
//	During Bind() processing, the intermediate driver will attempt to read 
//	the "UpperBindings" value from the registry key
//		Services\NmInt\Parameters\Adapters\{...RealMP.Instance.GUID...}
//  This value will contain the instance GUID(s) of the virtual adapter(s) 
//	that the Notify Object set. 
//
//===========================================================================
void CMyBindingPolicy::OnBindAdapter(CNetCfgComponent& RealMp)
{
	ATLTRACE(_T("CMyBindingPolicy::OnBindAdapter %ws %ws\n"),RealMp.GetId(),RealMp.GetInstanceGuid());

	// TODO: install virtual adapter(s) for this physical NIC (MUX N to 1 driver)
	// or just add an existing virtual NIC instance to the registry 
	// for this physical NIC (MUX 1 to N driver)

	// Human readable name of the miniport = NOT a GUID!
	LPWSTR pszwName = NULL;
	// use operator -> to access underlying interface pointer since
	// GetDisplayName is not wrapped by the class.  Note: need to free the string
	RealMp->GetDisplayName(&pszwName);
	ATLASSERT(pszwName);

	// have we installed any virtual adapters?
	bool fEmpty = mmAdapters.empty();

	// virtual adapters to install
	CNetCfgComponent MyVirtAdap,MyVirtAdap2;

	wstring s = L"Install Virtual Adapter: "; 
	s += m_pszwMiniportID; s += L"  for\n"; s += pszwName;

	if (IDYES == MessageBox(NULL,s.c_str(),L"Install Virtual Adapter?",MB_YESNO)) {
		MyVirtAdap = InstallMiniport(RealMp);
	}

	s = L"Install a second instance of Virtual Adapter: "; 
	s += m_pszwMiniportID; s += L"  for\n "; s += pszwName;

	if (IDYES == MessageBox(NULL,s.c_str(),L"Install Virtual Adapter?",MB_YESNO)) {
		MyVirtAdap2 = InstallMiniport(RealMp);
	}

	if (!fEmpty) {
		s = L"Add existing Virtual Adapter Instances for ";
		s += pszwName;
		s += RealMp.GetInstanceGuid();

		if (IDYES == MessageBox(NULL,s.c_str(),L"Use Virtual Adapters?",MB_YESNO)) {

			// TODO: add existing virtual NIC instance to the registry for this physical NIC
			// TODO: Select an existing virtual NIC to use

			// lookup an existing virtual NIC instance in the multimap 
			// Note: the base class keeps track of installed virtual adapters per
			// real miniports in data member mmAdapters.

			// Obtain Real NIC's Instance GUID = key in the map
			WCHAR* pszwGuid = RealMp.GetInstanceGuid();
			if (pszwGuid)
			{
				wstring S(pszwGuid);

				// lookup entry in the map based on GUID key
				multimap<wstring,wstring>::const_iterator i = mmAdapters.find(S);

				if (i != mmAdapters.end()) {

					// found an entry in the map for the GUID key, use
					// its value as the virtual adapter instance GUID
					// to write to the registry
					LONG lRet = AddMpNameToReg(RealMp,i->second.c_str());
				} 

				// Done with the GUID
				delete pszwGuid;
			}
		}
	}

	CoTaskMemFree(pszwName);

#if _DEBUG
		// Obtain Real NIC's Adapter Name
	WCHAR* pszwGuid = RealMp.GetInstanceGuid();

    ATLTRACE(_T("CMyBindingPolicy::OnBindAdapter  Bind To --> %ws\n"), RealMp.GetId());
    ATLTRACE(_T("	Instance Guid: %ws\n"), pszwGuid);

	delete pszwGuid;
#endif

}
