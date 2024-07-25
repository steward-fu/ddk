// BindingPolicy.cpp: implementation of the CBindingPolicy class.
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


WCHAR* CImBindingPolicy::m_pszwRegValName = L"DisabledBindings";


///////////////////////////////////////////////////////////////////////
// Initialize registry key and list of previously disabled bindings
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	This routine reads the registry to initialize a list of previously
//  disabled bindings so that these bindings can be restored when the
//	IM driver is disabled or uninstalled.
//
void CImBindingPolicy::LoadBindings()
{
	ATLTRACE(_T("CImBindingPolicy::LoadBindings\n"));

	if (!m_Protocol.IsValid())
	{
		ATLTRACE(_T("Invalid Protocol component\n"));
		return;
	}

	HKEY hRegKey = m_Protocol.OpenParamKey();
	CRegKey RegKey;RegKey.Attach(hRegKey);

	DWORD dwSize = 0;
	DWORD dwType; // REG_MULTI_SZ
	// Read the value m_psztRegValName
	LONG lRes = RegQueryValueExW(
					RegKey,
					m_pszwRegValName,
					NULL,
					&dwType,
					NULL,
					&dwSize
					);

	if (lRes == ERROR_SUCCESS) {

		LPBYTE p = new BYTE[dwSize];
		if (p)
		{
			if (ERROR_SUCCESS == RegQueryValueExW(
				RegKey,
				m_pszwRegValName,
				NULL,
				&dwType,
				p,
				&dwSize
				))
			{
				// populate the list of disabled binding paths
				LPWSTR psztBindPathTok = (LPWSTR)p;
				while (*psztBindPathTok)
				{
					wstring wsBindPathTok(psztBindPathTok);
					m_lstBindPaths.insert(m_lstBindPaths.begin(),wsBindPathTok);

					// move the ptr to the next string
					psztBindPathTok += wsBindPathTok.length() + 1;
				}
			}

			delete p;
		}
	}
}


///////////////////////////////////////////////////////////////////////
// Handle query to accept enabling a binding path
//
// Parameters:
// 	BindPath - Binding path to accept or reject
// Returns:
//	true = accept enabling the bind path, false = reject enabling the bind path
// Comments:
// 	Called from CNotifyObject<>::QueryBindingPath
//
bool CImBindingPolicy::OnAcceptBindingPathEnable(CNetCfgBindingPath& BindPath)
{
	ATLTRACE(_T("CImBindingPolicy::OnAcceptBindingPathEnable\n"));

	// Accept (true) or reject (false) the binding path
	bool bRetVal = true;

	// A depth of 2 indicates that the binding is for a protocol
	// directly to the NIC.  ie.  protocol -> NIC
	// Rather than  protocol -> protocol -> NIC
	if (2 == BindPath.GetDepth())
	{
		// Get the first binding interface
		INetCfgBindingInterface* pBindInterface = BindPath.GetFirstInterface();

		// note that this binding path should only really
		// have one interface which would be equal to
		//		Protocol -> Adapter

		if (pBindInterface)
		{
			CNetCfgBindingInterface BindIntf(pBindInterface);

			// obtain the adapter (the lower component in the interface)
			CNetCfgComponent Adapter(BindIntf.GetLowerComponent());
			// obtain the protocol (the upper component in the interface)
			CNetCfgComponent Protocol(BindIntf.GetUpperComponent());

			bool fProtocol = IsProtocol(Protocol);
			bool fAdapter  = IsMiniport(Adapter);

			// give derived class option of handling case of ourProtocol -> realAdapter
			if (fProtocol && !fAdapter)
				bRetVal = OnAcceptAdapter(Adapter);

			// give derived class option of handling case of realProtocol -> ourAdapter
			if (fAdapter && !fProtocol)
				bRetVal = OnAcceptProtocol(Protocol);

			// give derived class option of handling case of realProtocol -> realAdapter
			// note: if realProtocol -> ourAdapter would return true (by calling
			//		OnAcceptProtocol) then check if we want to disable this binding by
			//		calling OnDisableBinding
			// this handles the case where a user installs a new protocol in the system
			if (!fAdapter && !fProtocol)
			{
				if (OnAcceptProtocol(Protocol) && OnDisableBinding(Protocol,Adapter))
				{
					bRetVal = false;
					// must store the fact that we didn't allow this binding so that
					// it can be reenabled later
					LPWSTR pszwTok = BindPath.GetPathToken();
					if (pszwTok)
					{
						wstring wsBindPath(pszwTok);
						m_lstBindPaths.insert(m_lstBindPaths.begin(),wsBindPath);
						CoTaskMemFree(pszwTok);
					}
				}
			}

			// Note: case of ourProtocol -> ourAdapter
			// will always return true by default since always want
			// to have this binding enabled
		}
	}

	return bRetVal;
}


///////////////////////////////////////////////////////////////////////
// Restore previously disabled bindings
//
// Parameters:
// 	Comp - component involved with the binding change
// Returns:
// 	none
// Comments:
// 	none
//
void CImBindingPolicy::EnableBindings(CNetCfgComponent &Comp)
{
	ATLTRACE(_T("CImBindingPolicy::EnableBindings\n"));

	INetCfgBindingPath* pBindPath = Comp.GetFirstBindPath(EBP_ABOVE);

	while (pBindPath)
	{
		CNetCfgBindingPath BindPath(pBindPath);

		// A depth of 2 indicates that the binding is for a protocol
		// directly to the NIC.  ie.  protocol -> NIC
		// Rather than  protocol -> protocol -> NIC
		if (2 == BindPath.GetDepth())
		{
			LPWSTR pszwTok = BindPath.GetPathToken();
			if (pszwTok)
			{
				wstring wsPathToken(pszwTok);

				for (m_i = m_lstBindPaths.begin(); m_i != m_lstBindPaths.end(); ++m_i)
				{
					wstring wsBindPath(*m_i);

					if (wsBindPath == wsPathToken)
					{
						// Need to enable the binding path here.
						if (BindPath.Enable())
						{
							// remove this binding path from the 'disabled' list
							m_lstBindPaths.erase(m_i);
						}
					}
				}

				CoTaskMemFree(pszwTok);
			}
		}

		pBindPath = Comp.GetNextBindPath(EBP_ABOVE);
	}
}


///////////////////////////////////////////////////////////////////////
// Disable bindings for an existing real adapter
//
// Parameters:
// 	ExistAdap - existing Real Adapter that we want to break bindings to
// 	NewAdap - Virtual Adapter that we added
// Returns:
// 	none
// Comments:
// 	This routine searches for all Protocols that our virtual adapter portion
//	of the IM driver is bound to.  Then it disables bindings between same
//	protocols found and real adapter.  OnDisableBinding() is called for each
//	binding path to disable giving the derived class option of NOT disabling
//	specific binding paths.
//
void CImBindingPolicy::DisableBindings(CNetCfgComponent& ExistAdap, CNetCfgComponent& NewAdap)
{
	ATLTRACE(_T("CImBindingPolicy::DisableBindings\n"));

	// break the transport bindings to the Existing Adapter
	// for those transport bindings that were added for our New Adapter (virtual adapter)

	list<CNetCfgComponent> lstPrToBreak;
	list<CNetCfgComponent>::iterator i;
	LPCWSTR pszwId = m_Protocol.GetId();
	ATLASSERT(pszwId);
	if (!pszwId) return;

	INetCfgBindingPath* pBindPath = NewAdap.GetFirstBindPath(EBP_ABOVE);

	while (pBindPath)
	{
		CNetCfgBindingPath BindPath(pBindPath);

		// A depth of 2 indicates that the binding is for a protocol
		// directly to the NIC.  ie.  protocol -> NIC
		// Rather than  protocol -> protocol -> NIC
		if (2 == BindPath.GetDepth())
		{
			CNetCfgComponent Owner(BindPath.GetOwner());
			LPCWSTR pszwOwner = Owner.GetId();
			// Skipping binding path if our protocol is the Owner
			if (pszwOwner && _wcsnicmp(pszwOwner,pszwId,wcslen(pszwId)))
			{
				// Store the owner in the list
				lstPrToBreak.insert(lstPrToBreak.begin(),Owner);
			}
		}

		pBindPath = NewAdap.GetNextBindPath(EBP_ABOVE);
	}

	// using the RealNIC, iterate thru the bindpath depth of 2 looking to match owner to owner
	// and disable this binding
	pBindPath = ExistAdap.GetFirstBindPath(EBP_ABOVE);

	while (pBindPath)
	{
		CNetCfgBindingPath BindPath(pBindPath);

		// A depth of 2 indicates that the binding is for a protocol
		// directly to the NIC.  ie.  protocol -> NIC
		// Rather than  protocol -> protocol -> NIC
		if (BindPath.IsEnabled() && 2 == BindPath.GetDepth())
		{
			CNetCfgComponent Owner(BindPath.GetOwner());
			LPCWSTR pszwOwner = Owner.GetId();

			// Skipping binding path if our protocol is the Owner
			if (pszwOwner && _wcsnicmp(pszwOwner,pszwId,wcslen(pszwId)) )
			{
				// Our protocol is NOT the owner, so we want to break the binding
				// between This owner and the Real NIC
				// look for a match of the owner ID in the list
				for (i = lstPrToBreak.begin(); i != lstPrToBreak.end(); ++i)
				{
					CNetCfgComponent Protocol(*i);
					LPCWSTR pszwPrId = Protocol.GetId();

					// Look for a matching ID
					// if found call virtual method to give user a chance to accept/reject this operation
					if(pszwPrId && !wcscmp(pszwPrId,pszwOwner) && OnDisableBinding(Protocol,ExistAdap) )
					{
						// Disable the binding path
						if (BindPath.Disable())
						{
							// store the change made to the bindings so that
							// it can be reenabled later
							LPWSTR pszwTok = BindPath.GetPathToken();
							if (pszwTok)
							{
								ATLTRACE(_T("Disabled Binding %ws\n"),pszwTok);
								wstring wsBindPath(pszwTok);
								m_lstBindPaths.insert(m_lstBindPaths.begin(),wsBindPath);
								CoTaskMemFree(pszwTok);
							}
						}
					}
				}
			}
		}

		pBindPath = ExistAdap.GetNextBindPath(EBP_ABOVE);
	}
}


///////////////////////////////////////////////////////////////////////
// Save disabled bindings information to registry
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	This routine saves the list of previously disabled bindings to the
//	registry so that original system bindings can be restored when the
//	component is uninstalled
//
void CImBindingPolicy::SaveBindings()
{
	ATLTRACE(_T("CImBindingPolicy::SaveBindings\n"));

	HKEY hk = m_Protocol.OpenParamKey();
	ATLASSERT((HKEY)hk);
	if(!hk) return;

	DWORD dwSize = 0;
	for (m_i = m_lstBindPaths.begin(); m_i != m_lstBindPaths.end(); ++m_i)
	{
		wstring wsToken(*m_i);
		dwSize += (DWORD)wsToken.length() + 1;
	}

	DWORD dwAllocSize = (dwSize + 1)*sizeof(wchar_t);
	LPBYTE p = new BYTE[dwAllocSize];
	if (p)
	{
		memset(p,0,dwAllocSize);

		LPBYTE ptr = p;
		// iterate thru the list of bind paths
		// add each to buffer
		for (m_i = m_lstBindPaths.begin(); m_i != m_lstBindPaths.end(); ++m_i)
		{
			wstring wsBPToken(*m_i);
			memcpy(ptr,wsBPToken.c_str(),wsBPToken.length()*sizeof(wchar_t));
			ptr += (wsBPToken.length() + 1)*sizeof(wchar_t);
		}

		// Set the value m_psztRegValName
		if (ERROR_SUCCESS != RegSetValueExW(
			hk,
			m_pszwRegValName,
			NULL,
			REG_MULTI_SZ,
			p,
			dwAllocSize
			))
		{
			// ERROR!
			ATLASSERT(FALSE);
		}

		delete p;
	}
}


///////////////////////////////////////////////////////////////////////
// Open adapter registry key at the protocol's params registry key
//
// Parameters:
// 	AdapterInstanceGuid	- Guid for an adapter in the system
// Returns:
//	handle to the registry key
// Comments:
// 	none
//
HKEY CBindingPolicy::OpenAdapterKey(GUID& AdapterInstanceGuid)
{
	HKEY hRet;
	WCHAR wszGuid[GUID_CCH];
	int cchGuid = StringFromGUID2(AdapterInstanceGuid,wszGuid,GUID_CCH);
	// Did StringFromGUID2 work?
	ATLASSERT(GUID_CCH == cchGuid);
	if (GUID_CCH == cchGuid)
	{
		hRet = OpenAdapterKey(wszGuid);
	}

	return hRet;
}


///////////////////////////////////////////////////////////////////////
// Open adapter registry key at the protocol's params registry key
//
// Parameters:
// 	pszwAdapterInstanceGuid	- string (Guid) of an adapter in the system
// Returns:
//	handle to the registry key
// Comments:
// 	none
//
HKEY CBindingPolicy::OpenAdapterKey(WCHAR* pszwAdapterInstanceGuid)
{
	HKEY hRet = NULL;

	// Obtain handle to Protocol Component's Param reg key
	CRegKey RegKeyProt;
	RegKeyProt.Attach(m_Protocol.OpenParamKey());

	// create subkey "Adapters"
	CRegKey RegKeyAdap;
	LONG lRet = RegKeyAdap.Create(
					RegKeyProt,	// Handle to parent key
					_T("Adapters")	// Subkey to create
					);

	if ( ERROR_SUCCESS == lRet )
	{
		// create subkey adapter instance Guid
		lRet = RegCreateKeyExW(
					RegKeyAdap,
					pszwAdapterInstanceGuid,
					0,
					REG_NONE,
					REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS,
					NULL,
					&hRet,
					NULL
					);
	}

	return hRet;
}


///////////////////////////////////////////////////////////////////////
// Add an installed virtual adapter's name to the registry at the
// protocol's params registry key
//
// Parameters:
// 	RealMp	- CNetCfgComponent object for a "Real" adapter
//	MyMp	- CNetCfgComponent object for virtual adapter
// Returns:
//	ERROR_SUCCESS if the registry was updated
// Comments:
// 	Adds the full adapter name to the PROTOCOL component's Param key
//
LONG CImBindingPolicy::AddMpNameToReg(CNetCfgComponent& RealMp, CNetCfgComponent& MyMp)
{
    ATLTRACE(_T("CImBindingPolicy::AddMpNameToReg\n"));

	LONG lRet = E_FAIL;

	// Get the full binding name of the virtual adapter
	LPWSTR pAdapterName = MyMp.GetFullBindName();

	if (!pAdapterName) return lRet;

	// Obtain Real NIC's Adapter Name
	WCHAR* pszwGuid = RealMp.GetInstanceGuid();
	if (!pszwGuid)
	{
		delete pAdapterName;
		return lRet;
	}

	lRet = AddMpNameToReg(pszwGuid,pAdapterName);

	// Done with Adapter Name
	delete pAdapterName;

	// Done with the GUID
	delete pszwGuid;

	return lRet;
}


///////////////////////////////////////////////////////////////////////
// Add an installed virtual adapter's name to the registry at the
// protocol's params registry key
//
// Parameters:
// 	RealMp				- CNetCfgComponent object for a "Real" adapter
//	pszwVirtAdapName	- virtual adapter name
// Returns:
//	ERROR_SUCCESS if the registry was updated
// Comments:
// 	Adds the full adapter name to the PROTOCOL component's Param key
//
LONG CImBindingPolicy::AddMpNameToReg(CNetCfgComponent& RealMp, LPCWSTR pszwVirtAdapName)
{
    ATLTRACE(_T("CImBindingPolicy::AddMpNameToReg\n"));

	LONG lRet = E_FAIL;

	// Obtain Real NIC's Adapter Name
	WCHAR* pszwGuid = RealMp.GetInstanceGuid();
	if (!pszwGuid)
	{
		return lRet;
	}

	lRet = AddMpNameToReg(pszwGuid,pszwVirtAdapName);

	// Done with the GUID
	delete pszwGuid;

	return lRet;
}


///////////////////////////////////////////////////////////////////////
// Add an installed virtual adapter's name to the registry at the
// protocol's params registry key
//
// Parameters:
// 	pszwPhysAdapInstGuid	- "Real" adapter's instance GUID
//	pszwVirtAdapName		- virtual adapter name
// Returns:
//	ERROR_SUCCESS if the registry was updated
// Comments:
// 	Adds the full adapter name to the PROTOCOL component's Param key
//
LONG CImBindingPolicy::AddMpNameToReg(LPCWSTR pszwPhysAdapInstGuid, LPCWSTR pszwVirtAdapName)
{
    ATLTRACE(_T("CImBindingPolicy::AddMpNameToReg\n"));

	LONG lRet = E_FAIL;

	// Length of the Adap Name in bytes including NULL
	ULONG nLen = (ULONG)(wcslen(pszwVirtAdapName)+1)*sizeof(WCHAR);

	CRegKey AdapKey;
	AdapKey.Attach(OpenAdapterKey((WCHAR*)pszwPhysAdapInstGuid));

	if ( NULL != (HKEY)AdapKey)
	{
		// write the adapter name string to the registry as
		// the value data for the value name "UpperBindings"

		LPBYTE lpData = NULL;
		DWORD cbData = 0;	// Size in bytes of registry value data
		DWORD Type = 0;		// Type of the data stored in registry value

		// Acquire the size of the registry value data
		// this will include terminating NULL char for REG_MULTI_SZ
		lRet = RegQueryValueEx(
					AdapKey,				// handle to key
					L"UpperBindings",		// value name
					NULL,					// reserved
					&Type,					// type buffer
					NULL,					// data buffer
					&cbData					// size of data buffer
					);

		if (ERROR_SUCCESS == lRet) {

			ATLASSERT (REG_MULTI_SZ == Type);

			// There was an existing value and its length is
			// stored in cbData so now acquire the actual buffer

			// allocate a buffer to hold the data and the adapter name
			// cbData & nLen both include terminating NULLs
			lpData = new BYTE[cbData + nLen];

			if (lpData) {

				lRet = RegQueryValueEx(
							AdapKey,				// handle to key
							L"UpperBindings",		// value name
							NULL,					// reserved
							&Type,					// type buffer
							lpData,					// data buffer
							&cbData					// size of data buffer
							);

				if (ERROR_SUCCESS == lRet) {

					memcpy(lpData+cbData,pszwVirtAdapName,nLen);
				}
			}
			else
			{
				lRet = E_FAIL;
			}
		}
		else
		{
			lpData = (BYTE * const) pszwVirtAdapName;
			lRet = ERROR_SUCCESS;
			cbData = 0;
		}

		if (ERROR_SUCCESS == lRet) {
			lRet = RegSetValueEx(
						AdapKey,
						L"UpperBindings",
						NULL,
						REG_MULTI_SZ,
						lpData,
						cbData + nLen
						);

			if (ERROR_SUCCESS == lRet) {

				// Add this physical adapter instance to the multimap as the key
				// with the virtual adapter as the value
				wstring strVirtAdapName = pszwVirtAdapName;
				wstring strPhysAdapName = pszwPhysAdapInstGuid;

				mmAdapters.insert(make_pair(strPhysAdapName,strVirtAdapName));
			}
		}

		if (cbData) delete lpData;
	}

	return lRet;
}


///////////////////////////////////////////////////////////////////////
// Install a virtual adapter and add its name to the registry under the
// protocol's param registry key
//
// Parameters:
// 	RealMP	- CNetCfgComponent object for an adapter (Real NIC)
// Returns:
//	CNetCfgComponent of the virtual adapter
// Comments:
// 	none
//
CNetCfgComponent CImBindingPolicy::InstallMiniport(CNetCfgComponent& RealMP)
{
    ATLTRACE(_T("CImBindingPolicy::InstallMiniport\n"));

	// Install the Miniport portion of our driver
	INetCfgComponent* p = m_NetCfg.InstallComponent(m_pszwMiniportID);

	CNetCfgComponent MyMP(p);

	if (MyMP.IsValid())
	{
		// Add the miniport name to the protocols params reg key
		AddMpNameToReg(RealMP, MyMP);
	}

	ATLASSERT(MyMP.IsValid());
	if (MyMP.IsValid())
	{

		// Deactivate existing bindings that use the lower component
		// so that all packets go thru our intermediate driver
		DisableBindings(RealMP,MyMP);
	}

	return MyMP;
}


///////////////////////////////////////////////////////////////////////
// Handle notification of system adding a binding path with our protocol
//
// Parameters:
// 	BindPath	- CNetCfgBindingPath reference describing the binding path
// Returns:
// 	None
// Comments:
// 	Called from NotifyBindingPath() and SysNotifyBindingPath.  This routine
//	installs the virtual adapter edge of the IM driver and requests
//	disabling existing bindings.
//
void CImBindingPolicy::OnAddBindingPath(CNetCfgBindingPath& BindPath)
{
    ATLTRACE(_T("CImBindingPolicy::OnAddBindingPath\n"));

	if (2 == BindPath.GetDepth())
	{
		// Get the first binding interface
		INetCfgBindingInterface* pBindInterface = BindPath.GetFirstInterface();

		if (pBindInterface)
		{
			// Note: should only have one binding interface in the binding path
			// protocol -> Adapter
			CNetCfgBindingInterface BindIntf(pBindInterface);

			// Obtain the adapter (the lower component of the binding interface)
			CNetCfgComponent Adapter(BindIntf.GetLowerComponent());
			// Obtain the Protocol (the upper component of the binding interface)
			CNetCfgComponent Protocol(BindIntf.GetUpperComponent());

			// Handle case of ourProtocol -> realNIC
			// Ignore all others
			if (!IsMiniport(Adapter) && IsProtocol(Protocol))
			{
				// Check if we have already seen this physical Adapter Instance
				// If we have, we do not call virtual method OnBindAdapter
				PWSTR pAdapGuid = Adapter.GetInstanceGuid();

				wstring S(pAdapGuid);

				multimap<wstring,wstring>::const_iterator i = mmAdapters.find(S);

				if (i==mmAdapters.end()) {

					// did not found this instance in the multimap
					// Install the virtual miniport(s) of our NDIS intermediate driver
					OnBindAdapter(Adapter);
				}

				delete pAdapGuid;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////
// Handle notification of system removing a binding path with our component
//
// Parameters:
// 	BindPath	- CNetCfgBindingPath reference describing the binding path
// Returns:
// 	None
// Comments:
// 	Called from NotifyBindingPath().  This routine examines the binding path
//  and optionally uninstalls a virtual adapter.
//
void CImBindingPolicy::OnRemoveBindingPath(CNetCfgBindingPath& BindPath)
{
    ATLTRACE(_T("CImBindingPolicy::RemoveBindingPath\n"));

	// Get the first binding interface
	INetCfgBindingInterface* pBindInterface = BindPath.GetFirstInterface();

	while (pBindInterface)
	{
		CNetCfgBindingInterface BindIntf(pBindInterface);

		CNetCfgComponent LowerComp(BindIntf.GetLowerComponent());
		LPCWSTR pszwLowerId = LowerComp.GetId();

		// Determine if our virtual adapter is the lower component
		if ( !_wcsnicmp(
					pszwLowerId,
					m_pszwMiniportID,
					wcslen(m_pszwMiniportID)
					)
		   )
		{
			ATLTRACE(_T("\tRemoving MP instance\n"));
			// DeInstall the miniport portion of our NDIS intermediate driver
			bool fResult = m_NetCfg.DeInstallComponent(LowerComp);
			ATLASSERT(fResult && _T("DeInstallComponent failed for adapter"));

		} else {
			// Restore bindings that were previously broken for this NIC
			EnableBindings(LowerComp);
		}

		pBindInterface = BindPath.GetNextInterface();
	}
}


///////////////////////////////////////////////////////////////////////
// Accept/Reject binding between a protocol from the system and the
//	IM adapter edge
//
// Parameters:
// 	RealMp	- CNetCfgComponent reference representing an existing
//				adapter in the system.
// Returns:
// 	none
// Comments:
// 	none
//
void CImBindingPolicy::OnBindAdapter(CNetCfgComponent& RealMp)
{
	if (RealMp.IsPhysicalNIC()) {
		CNetCfgComponent MyVirtAdap = InstallMiniport(RealMp);
		ATLASSERT(MyVirtAdap.IsValid());
	}
#if _DEBUG
		// Obtain Real NIC's Adapter Name
	WCHAR* pszwGuid = RealMp.GetInstanceGuid();

    ATLTRACE(_T("CImBindingPolicy::OnBindAdapter  Bind To ----> %ws\n"), RealMp.GetId());
    ATLTRACE(_T("	Inst. Guid: %ws\n"), pszwGuid);

	delete pszwGuid;
#endif
}
