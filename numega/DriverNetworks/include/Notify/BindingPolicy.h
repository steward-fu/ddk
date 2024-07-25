// BindingPolicy.h: interface for the CBindingPolicy class.
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

#if !defined(AFX_BINDINGPOLICY_H__46F6A583_E0CC_11D4_B048_00C04F6A5DA9__INCLUDED_)
#define AFX_BINDINGPOLICY_H__46F6A583_E0CC_11D4_B048_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// CBindingPolicy
//
// Base class for CNotifyObject<> template parameter.  This class
// abstracts the functionality of the notify object related to bindings.
// The class features 2 types of methods: handlers and services. Handlers
// are the targets of callbacks *from* the Network Config Subsystem.
// Services are the requests from the Notify Object *to* the Network
// Config Subsystem to perform certain actions.  The Notify Object will
// receive callbacks from the Network Configuration
// Subsystem for binding events.  These callbacks are routed to this
// class by the CNotifyObject<> class.
//
// The system will call CNotifyObject<>::QueryBindingPath for each
// binding path containing the component when the system wants to
// disable or enable the binding path.  The Notify Object has the
// opportunity to accept or reject the proposed system change.
// CNotifyObject<>::QueryBindingPath routes requests to enable a binding
// path to CBindingPolicy::OnAcceptBindingPathEnable.  And requests to
// disable a binding path go to OnAcceptBindingPathDisable.  By default,
// these routines returns true to accept the change.  The user can
// override these virtual functions to provide custom behavior.
//
// The system will call CNotifyObject<>::NotifyBindingPath for each
// binding path containing the component when the system wants to
// add or remove a binding path.  The Notify Object has the
// opportunity to perform any operations required for this.
// CNotifyObject<>::NotifyBindingPath routes requests to add a binding
// path to CBindingPolicy::OnAddBindingPath.  And requests to
// disable a binding path go to OnRemoveBindingPath.  The user can
// override these virtual functions to perform any requried operations.
//
// m_Protocol is the network component that the Notify Object "owns".
// This data member can be used during processing of binding
// notifications.  Binding paths contain network components.  Method
// IsProtocol() can be used to test if a component is m_Protocol.
// Also, OpenAdapterKey provides convenient service to open
// Protocol/Parameters/Adapters/{adap_inst_guid} registry keys for
// loading/storing per-adapter information.
//
class CBindingPolicy
{
// Construction/Initialization
public:
	// called from CNotifyObject<>::Initialize
	CBindingPolicy(CNetCfgComponent& Protocol, CNetCfg& NetCfg);
	virtual ~CBindingPolicy();

// Methods
public:

//  Handlers for binding notifications - signatures required by CNotifyObject<> template

	// Handler for enabling a binding path - called from CNotifyObject<>::QueryBindingPath
	virtual bool OnAcceptBindingPathEnable(CNetCfgBindingPath& BindPath);
	// Handler for disabling a binding path - called from CNotifyObject<>::QueryBindingPath
	virtual bool OnAcceptBindingPathDisable(CNetCfgBindingPath& BindPath);

	// return true indicates a re-boot is required for the component to read the changes
	virtual bool OnApplyRegistryChanges() { return false; }

	// System is adding a binding path containing our component
	// called from CNotifyObject<>::NotifyBindingPath
	virtual void OnAddBindingPath(CNetCfgBindingPath& BindPath);
	// System is removing a binding path containing our component
	// called from CNotifyObject<>::NotifyBindingPath
	virtual void OnRemoveBindingPath(CNetCfgBindingPath& BindPath);

// Services

	// Open the Protocol reg key Parameters\Adapters\{AdapInstGuid}
	virtual HKEY OpenAdapterKey(GUID& AdapterInstanceGuid);
	virtual HKEY OpenAdapterKey(WCHAR* pszwAdapterInstanceGuid);

	// Test if a network component is our protocol (m_Protocol)
	bool IsProtocol(CNetCfgComponent& Comp);

// Data members
protected:

	// INetCfg wrapper objects
	CNetCfgComponent&	m_Protocol;	 // Interface to the protocol component
	CNetCfg&			m_NetCfg;	 // Interface to the network config subsystem
};


//////////////////////////////////////////////////////////////////////
// CImBindingPolicy
//
// CBindingPolicy-dericed class for CNotifyObject<> template parameter.
// This class  abstracts the functionality of the notify object related
// to bindings for NDIS Intermediate drivers.
// The class features 2 types of methods: handlers and services. Handlers
// are the targets of callbacks *from* the Network Config Subsystem.
// Services are the requests from the Notify Object *to* the Network
// Config Subsystem to perform certain actions.  The Notify Object will
// receive callbacks from the Network Configuration
// Subsystem for binding events.  These callbacks are routed to this
// class by the CNotifyObject<> class.
//
// Notify Objects for Intermediate drivers must install the virtual
// adapters exposed by the intermediate driver, store virtual adapter
// instance names in the registry, and enable/disable any system
// bindings so that the intermedite driver intercepts packets.
//
// The system will call CNotifyObject<>::QueryBindingPath for each
// binding path containing the component when the system wants to
// disable or enable the binding path.  The Notify Object has the
// opportunity to accept or reject the proposed system change.
// CNotifyObject<>::QueryBindingPath routes requests to enable a binding
// path to CBindingPolicy::OnAcceptBindingPathEnable.  And requests to
// disable a binding path go to OnAcceptBindingPathDisable.  These
// routines will call OnAcceptProtocol and OnAcceptAdapter virtual
// methods for each specific protocol and adapter.  Filtered out are
// requests to bind the virtual adapter to our protocol.  By default,
// OnAcceptAdapter and OnAcceptProtocol return true to accept the
// binding.  The user can override these virtual functions to provide
// custom behavior (i.e. only bind to certain adapters or protocols.)
//
// The system will call CNotifyObject<>::NotifyBindingPath for each
// binding path containing the component when the system wants to
// add or remove a binding path.  The Notify Object has the
// opportunity to perform any operations required for this.
// CNotifyObject<>::NotifyBindingPath routes requests to add a binding
// path to CBindingPolicy::OnAddBindingPath.  For each unique adapter,
// OnAddBindingPath calls virtual method OnBindAdapter().  Filtered
// out are requests to bind to our virtual adapter.  OnBindAdapter()'s
// responsiblities are to install virtual adapters using method
// InstallMiniport().  The default implementation installs one
// virtual adapter.  The user can override this method to perform
// any custom behavior.  Alternatively, if the driver wants to re-use
// an existing virtual adapter instance already installed, the instance
// name must still be added to the registry.  The user can call
// method AddMpNameToReg() to do this.
//
// m_pszwMiniportID is hw-id of the miniport portion of the IM driver.
// This data member is used to install virtual adapters.  A call to
// InstallMiniport() method results in the system searching for a .inf
// file containing the hw-id.  Method IsMiniport() can be used during
// binding notifications to test if a component is our virtual adapter.
// InstallMiniport takes care of adding the name to the registry.
//
// InstallMiniport() calls method DisableBindings() to disable existing
// system bindings to the real miniport.  The OnDisableBinding() method
// will be called for each binding for the component.  The default
// behavior is to return true disabling all bindings so that all packets
// go thru the IM driver.  This method could be overriden to only
// disable particular bindings.  The class provides services
// LoadBindings and SaveBindings to persist any binding changes made
// to the system.  This information is stored in
// Protocol/Parameters/DisabledBindings.  When the component is
// uninstalled, the previous system bindings will be restored.
//
// For IM drivers, the default behavior of the class is to bind to all
// physical NICs and disable all existing protocol bindings to the NICs
// so that all packets go thru the IM driver.  Also, one virtual
// adapter is installed per bound physical adapter.  The user can override
// virtual methods to provide customized behavior such as only
// binding to specific adapters or protocols and exposing more than
// one virtual adapter per physical NIC (N to 1 Intermediate MUX) or
// re-using an existing virtual adapter instance for a
// physical NIC (1 to N Intermidate MUX.)
//
class CImBindingPolicy : public CBindingPolicy
{
// Construction/Initialization
public:
	CImBindingPolicy(CNetCfgComponent& Protocol, CNetCfg& NetCfg);
	virtual ~CImBindingPolicy();

// Methods
public:

//  Handlers for binding notifications - signatures required by CNotifyObject<> template

	// Handler for enabling a binding path - called from CNotifyObject<>::QueryBindingPath
	virtual bool OnAcceptBindingPathEnable(CNetCfgBindingPath& BindPath);
	// return true indicates a re-boot is required for the component to read the changes
	virtual bool OnApplyRegistryChanges() { SaveBindings(); return false;}

	// System is adding a binding path containing our component
	// called from CNotifyObject<>::NotifyBindingPath
	virtual void OnAddBindingPath(CNetCfgBindingPath& BindPath);
	// System is removing a binding path containing our component
	// called from CNotifyObject<>::NotifyBindingPath
	virtual void OnRemoveBindingPath(CNetCfgBindingPath& BindPath);

//  Handlers for binding notifications - called from OnAddBindingPath()

	// Accept/Reject binding to a specific Adapter - called from OnAcceptBindingPathEnable
	virtual bool OnAcceptAdapter(CNetCfgComponent& Adapter);
	// Accept/Reject binding to a specific Protocol - called from OnAcceptBindingPathEnable
	virtual bool OnAcceptProtocol(CNetCfgComponent& Protocol);
	// perform any processing required for binding to an adapter
	virtual void OnBindAdapter(CNetCfgComponent& RealMp);

	// Accept/Reject disabling binding protocol -> adapter
	virtual bool OnDisableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter);
	// Accept/Reject disabling binding protocol -> adapter
	virtual bool OnEnableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter);

// Services
protected:

	// Install m_pszwMiniportID virtual adapter
	virtual CNetCfgComponent InstallMiniport(CNetCfgComponent& RealMP);

	// Add a virtual adapter name to the registry at
	// Protocol\Parameters\Adapters\{AdapInstGuid}\"UpperBindings"
	virtual LONG AddMpNameToReg(LPCWSTR pszwPhysAdapInstGuid, LPCWSTR pszwVirtAdapName);
	virtual LONG AddMpNameToReg(CNetCfgComponent& RealMp, LPCWSTR pszwVirtAdapName);
	virtual LONG AddMpNameToReg(CNetCfgComponent& RealMp, CNetCfgComponent& MyMp);

	// load changes made to existing bindings from the registry
	virtual void LoadBindings();
	// Save changes made to existing bindings in the registry
	virtual void SaveBindings();

	// Disable protocol -> exist adapter bindings when NewAdap is bound to the protocol
	// this will call OnDisableBinding to give finer control
	virtual void DisableBindings(CNetCfgComponent& ExistAdap, CNetCfgComponent& NewAdap);
	// re-enable bindings for an existing component
	virtual void EnableBindings(CNetCfgComponent& Comp);

	// Test if a network component is our virtual adapter
	bool IsMiniport(CNetCfgComponent& Comp);

	void SetMiniportID(LPCWSTR pszwID);

// Data Members
protected:

	// list of binding paths that we disabled
	list<wstring>				m_lstBindPaths;
	list<wstring>::iterator		m_i;

	// ID of our virtual adapter (matches .inf file ID)
	LPCWSTR						m_pszwMiniportID;

	// multimap of Physical Adapter names to Virtual Adapter names
	multimap<wstring,wstring>	mmAdapters;

	// Name of registry key to store disabled bindings
	static WCHAR*				m_pszwRegValName;
};




////////////////////////////////////////////////////////////////////
// Inline Implementations

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline CBindingPolicy::CBindingPolicy(
	CNetCfgComponent& Protocol,
	CNetCfg& NetCfg
	) :
		m_Protocol(Protocol),
			m_NetCfg(NetCfg)
{
}


inline CBindingPolicy::~CBindingPolicy()
{
}


///////////////////////////////////////////////////////////////////////
// Handle query to accept disabling a binding path
//
// Parameters:
// 	BindPath - Binding path to accept or reject
// Returns:
//	true = accept disabling the bind path,
//	false = reject disabling the bind path
// Comments:
// 	Accept (true) or reject (false) disabling the binding path
//
inline bool CBindingPolicy::OnAcceptBindingPathDisable(CNetCfgBindingPath &BindPath)
{
	// default behavior is to 'accept' disabling all bindings
	return true;
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
inline bool CBindingPolicy::OnAcceptBindingPathEnable(CNetCfgBindingPath &BindPath)
{
	// Accept (true) or reject (false) the binding path
	return true;
}


///////////////////////////////////////////////////////////////////////
// Handle notification of system adding a binding path with our protocol
//
// Parameters:
// 	BindPath	- CNetCfgBindingPath reference describing the binding path
// Returns:
// 	None
// Comments:
// 	Called from NotifyBindingPath() and SysNotifyBindingPath().
//
inline void CBindingPolicy::OnAddBindingPath(CNetCfgBindingPath& BindPath)
{
}


///////////////////////////////////////////////////////////////////////
// Handle notification of system removing a binding path with our component
//
// Parameters:
// 	BindPath	- CNetCfgBindingPath reference describing the binding path
// Returns:
// 	None
// Comments:
// 	Called from NotifyBindingPath().
//
inline void CBindingPolicy::OnRemoveBindingPath(CNetCfgBindingPath& BindPath)
{
}


inline bool CBindingPolicy::IsProtocol(CNetCfgComponent& Comp)
{
	return Comp == 	m_Protocol.GetId();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline CImBindingPolicy::CImBindingPolicy(
	CNetCfgComponent& Protocol,
	CNetCfg& NetCfg
	) :
		CBindingPolicy(Protocol,NetCfg)
{
	LoadBindings();
}


inline CImBindingPolicy::~CImBindingPolicy()
{
}


inline bool CImBindingPolicy::IsMiniport(CNetCfgComponent& Comp)
{
	return Comp == m_pszwMiniportID;
}


inline void CImBindingPolicy::SetMiniportID(LPCWSTR pszwID)
{
	m_pszwMiniportID = pszwID;
}


///////////////////////////////////////////////////////////////////////
// Accept/Reject disabling binding between a protocol from the system and an
//	adapter from the system (not component(s) controlled by the notify object)
//
// Parameters:
// 	Protocol	- CNetCfgComponent object representing an existing Protocol
//				in the system.
// 	Adapter		- CNetCfgComponent object representing an existing Adapter
//				in the system.
// Returns:
// 	true - Accept disabling the binding, false - reject disabling the binding
// Comments:
// 	none
//
inline bool CImBindingPolicy::OnDisableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter)
{
	return true;
}


///////////////////////////////////////////////////////////////////////
// Accept/Reject enabling binding between a protocol from the system and an
//	adapter from the system (not component(s) controlled by the notify object)
//
// Parameters:
// 	Protocol	- CNetCfgComponent object representing an existing Protocol
//				in the system.
// 	Adapter		- CNetCfgComponent object representing an existing Adapter
//				in the system.
// Returns:
// 	true - Accept enabling the binding, false - reject enabling the binding
// Comments:
// 	none
//
inline bool CImBindingPolicy::OnEnableBinding(CNetCfgComponent& Protocol, CNetCfgComponent& Adapter)
{
	return true;
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
inline bool CImBindingPolicy::OnAcceptAdapter(CNetCfgComponent& Adapter)
{
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
inline bool CImBindingPolicy::OnAcceptProtocol(CNetCfgComponent& Protocol)
{
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

#endif // !defined(AFX_BINDINGPOLICY_H__46F6A583_E0CC_11D4_B048_00C04F6A5DA9__INCLUDED_)
