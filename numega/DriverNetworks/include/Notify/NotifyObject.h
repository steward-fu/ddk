// NotifyObject.h: Definition of the CNotifyObject class
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

#if !defined(AFX_NOTIFYOBJECT_H__0BFDF1C5_D2CA_11D4_B047_00C04F6A5DA9__INCLUDED_)
#define AFX_NOTIFYOBJECT_H__0BFDF1C5_D2CA_11D4_B047_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Forwards
class CNetCfgBindingPath;


/////////////////////////////////////////////////////////////////////////////
// CNotifyObject
//
// This class implements the COM interfaces required for a Notify Object.
// It delegates much of the work to _BindPolicy and _PropertyUi templates.
//
template <class _BindPolicy, class _PropertyUi=CPropertyUi>
class CNotifyObject :
	public CComCoClass<CNotifyObject<_BindPolicy,_PropertyUi>,&CLSID_NotifyObject>,
	public INetCfgComponentControl,			// Required Interface to implement
	public INetCfgComponentSetup,			// Optional Interface
	public INetCfgComponentPropertyUi,		// Optional Interface
	public INetCfgComponentNotifyBinding,	// Optional Interface
	public INetCfgComponentNotifyGlobal,	// Optional Interface
	public CComObjectRoot
{

	typedef CNotifyObject<_BindPolicy,_PropertyUi> Type;

// COM declarations
public:
DECLARE_NOT_AGGREGATABLE(Type)
DECLARE_REGISTRY(CLSID_NotifyObject, \
				 _T("Notify Object v1.0"),_T("NotifyObject Class"), \
				 IDS_NOTIFYOBJECT_DESC, THREADFLAGS_APARTMENT)

BEGIN_COM_MAP(Type)
    COM_INTERFACE_ENTRY(INetCfgComponentControl)
    COM_INTERFACE_ENTRY(INetCfgComponentSetup)
    COM_INTERFACE_ENTRY(INetCfgComponentPropertyUi)
    COM_INTERFACE_ENTRY(INetCfgComponentNotifyBinding)
    COM_INTERFACE_ENTRY(INetCfgComponentNotifyGlobal)
END_COM_MAP()

// Constructors/Destructors
public:
	CNotifyObject();
	virtual ~CNotifyObject();

// Notify Object Methods
public:

//	INetCfgComponentControl implementation
//	This is a required interface for a Notify Object
	STDMETHOD(Initialize)(INetCfgComponent*,INetCfg*,int);
	STDMETHOD(ApplyRegistryChanges)(void);
	STDMETHOD(ApplyPnpChanges)(INetCfgPnpReconfigCallback *);
	STDMETHOD(CancelChanges)(void);

//	INetCfgComponentSetup implementation
//	This is an optional interface for a Notify Object
	STDMETHOD(Install)(ULONG);
	STDMETHOD(Upgrade)(ULONG,ULONG);
	STDMETHOD(ReadAnswerFile)(LPCWSTR,LPCWSTR);
	STDMETHOD(Removing)(void);

//	INetCfgComponentPropertyUi implementation
//	This is an optional interface for a Notify Object
	STDMETHOD(QueryPropertyUi)(IUnknown*);
	STDMETHOD(SetContext)(IUnknown*);
	STDMETHOD(MergePropPages)(DWORD*,BYTE**,UINT*,HWND,LPCWSTR*);
	STDMETHOD(ValidateProperties)(struct HWND__*);
	STDMETHOD(ApplyProperties)(void);
	STDMETHOD(CancelProperties)(void);

//	INetCfgComponentNotifyBinding implementation
//	This is an optional interface for a Notify Object
	STDMETHOD(QueryBindingPath)(DWORD, INetCfgBindingPath*);
	STDMETHOD(NotifyBindingPath)(DWORD, INetCfgBindingPath*);

//	INetCfgComponentNotifyGlobal implementation
//	This is an optional interface for a Notify Object
	STDMETHOD(SysQueryBindingPath)(DWORD, INetCfgBindingPath*);
	STDMETHOD(SysNotifyBindingPath)(DWORD, INetCfgBindingPath*);
	STDMETHOD(GetSupportedNotifications)(DWORD*);
	STDMETHOD(SysNotifyComponent)(DWORD, INetCfgComponent*);

public:

	// Notification state
	typedef enum _Notification {
		eUnknown,
		eRemoving,
		eInstalling,
		ePropertyUI
	} Notification;

// Methods
public:
//  Helper methods
	static HKEY OpenAdapterKey(WCHAR* pszwAdapterInstanceGuid, CNetCfgComponent& Protocol);
	static HKEY OpenAdapterKey(GUID& AdapterInstanceGuid, CNetCfgComponent& Protocol);

	static Notification GetState() { return m_Notification; }

// Data Members
protected:

	// Binding policy object
	_BindPolicy*		m_pBindPolicy;
	// Property Page Ui object
	_PropertyUi*		m_pPropUi;

	// INetCfg wrapper objects
	CNetCfgComponent	m_Protocol;
	CNetCfg				m_NetCfg;

protected:

	// Notification state variable
	static Notification	m_Notification;

	// Flag indicating if installing the component
	BOOL				m_fInstalling;

	friend class _BindPolicy;
	friend class _PropertyUi;
};


//////////////////////////////////////////////////////////////////////
// CNotifyObject Implementations

template <class _BindPolicy, class _PropertyUi>
typename CNotifyObject<_BindPolicy,_PropertyUi>::Notification CNotifyObject<_BindPolicy,_PropertyUi>::m_Notification=eUnknown;

template <class _BindPolicy, class _PropertyUi>
CNotifyObject<_BindPolicy,_PropertyUi>::CNotifyObject() :
	m_pBindPolicy(NULL),
		m_pPropUi(NULL),
			m_fInstalling(FALSE)
{
}


template <class _BindPolicy, class _PropertyUi>
CNotifyObject<_BindPolicy,_PropertyUi>::~CNotifyObject()
{
	delete m_pPropUi;
	delete m_pBindPolicy;
}


// Notify Object Methods

//	INetCfgComponentControl implementation
//	This is a required interface for a Notify Object
template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::Initialize(
	INetCfgComponent* pComponent,
	INetCfg* pNetCfg,
	BOOL fInstalling
	)
{
    ATLTRACE(_T("CNotifyObject::Initialize\n"));

	HRESULT hr = S_OK;
	m_fInstalling = fInstalling;

	// Initialize CNetCfg and CNetCfgComponent's
	// These objects take care of COM stuff
	m_Protocol.Initialize(pComponent);
	m_NetCfg.Initialize(pNetCfg);

	// Allocate _BindPolicy instance
	m_pBindPolicy = new _BindPolicy(m_Protocol, m_NetCfg);
	ATLASSERT(m_pBindPolicy);
	if (!m_pBindPolicy) return S_FALSE;

	// Allocate _PropertyUi instance
	m_pPropUi = new _PropertyUi(m_Protocol);
	ATLASSERT(m_pPropUi);
	if (!m_pPropUi) {
		delete m_pBindPolicy;
		m_pBindPolicy = NULL;
		hr = S_FALSE;
	}

	return hr;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::ApplyRegistryChanges(void)
{
	HRESULT hr = S_OK;

	switch (m_Notification)
	{
	case eUnknown:
	default:
		hr = m_pBindPolicy->OnApplyRegistryChanges()?NETCFG_S_REBOOT:S_OK;
		break;

	case eRemoving:
		break;

	case ePropertyUI:
		hr = m_pPropUi->ApplyRegistryChanges();
		break;

	case eInstalling:
		m_pPropUi->ApplyRegistryChanges();
		hr = m_pBindPolicy->OnApplyRegistryChanges()?NETCFG_S_REBOOT:S_OK;
		break;

	}

	return hr;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::ApplyPnpChanges(INetCfgPnpReconfigCallback* pCallback)
{
   return m_pPropUi->ApplyPnpChanges(pCallback);
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::CancelChanges(void)
{
   return m_pPropUi->CancelChanges();
}


//	INetCfgComponentSetup implementation
//	This is an optional interface for a Notify Object

template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::Install(ULONG)
{
	// Set the state of the object to "installing"
	m_Notification = eInstalling;
	return S_OK;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::Upgrade(ULONG,ULONG)
{
	// Not implemented:  Specialize this method for your use
	return S_OK;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::ReadAnswerFile(LPCWSTR,LPCWSTR)
{
	// Not implemented:  Specialize this method for your use
	return S_OK;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::Removing(void)
{
	// Set the state of the object to "removing"
	m_Notification = eRemoving;
	return S_OK;
}


//	INetCfgComponentPropertyUi implementation
//	This is an optional interface for a Notify Object

template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::QueryPropertyUi(IUnknown* p)
{
	return m_pPropUi->QueryPropertyUi(p);
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::SetContext(IUnknown* p)
{
	return m_pPropUi->SetContext(p);
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::MergePropPages(
	DWORD*	pdwDefPages,
	BYTE**	pahpspPrivate,
	UINT*	pcPages,
	HWND	hwndParent,
	LPCWSTR*pszStartPage
	)
{
	// Set the state of the object to "property UI"
	m_Notification = ePropertyUI;

	return m_pPropUi->MergePropPages(pdwDefPages,pahpspPrivate,pcPages,hwndParent,pszStartPage);
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::ValidateProperties(HWND hWnd)
{
	return m_pPropUi->ValidateProperties(hWnd);
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::ApplyProperties(void)
{
	return m_pPropUi->ApplyProperties();
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::CancelProperties(void)
{
	return m_pPropUi->CancelProperties();
}


//	INetCfgComponentNotifyBinding implementation
//	This is an optional interface for a Notify Object
template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::QueryBindingPath(
	DWORD dwChangeFlag,
	INetCfgBindingPath *pncbpItem
	)
{
    ATLTRACE(_T("CNotifyObject::QueryBindingPath\n"));

	HRESULT hr		= S_OK;
	BOOL fAdd		= dwChangeFlag & NCN_ADD;
	BOOL fEnable	= dwChangeFlag & NCN_ENABLE;
	BOOL fDisable	= dwChangeFlag & NCN_DISABLE;

#if	_DEBUG
	LPWSTR	lpsz;

	hr = pncbpItem->GetPathToken(&lpsz);
	if (hr == S_OK)
	{
		ATLTRACE(_T("  BindingPath:%s\n"),lpsz);
		CoTaskMemFree(lpsz);
	}
#endif

	CNetCfgBindingPath BindPath(pncbpItem);

	if ( (fAdd && fEnable) || fEnable ) {
		// If Add && Enable  or just Enable

		// Handle case where NCN_ADD and NCN_ENABLE bits are set
		if (false == m_pBindPolicy->OnAcceptBindingPathEnable(BindPath)) {
			hr = NETCFG_S_DISABLE_QUERY;
		}
	}

	if (fDisable) {
		if (false == m_pBindPolicy->OnAcceptBindingPathDisable(BindPath)) {
			hr = NETCFG_S_DISABLE_QUERY;
		}
	}

	return hr;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::NotifyBindingPath(
	DWORD dwChangeFlag,
	INetCfgBindingPath *pncbpItem
	)
{
    ATLTRACE(_T("CNotifyObject::NotifyBindingPath dwChangeFlag=%X\n"),dwChangeFlag);

	BOOL fEnable  = dwChangeFlag & NCN_ENABLE;
	BOOL fAdd	  = dwChangeFlag & NCN_ADD;
	BOOL fRemove  = dwChangeFlag & NCN_REMOVE;
	BOOL fDisable = dwChangeFlag & NCN_DISABLE;

	CNetCfgBindingPath BindPath(pncbpItem);

	if ( (fAdd && fEnable) || fEnable ) {
		// If Add && Enable  or just Enable
		// Handle case where NCN_ADD and NCN_ENABLE bits are set
		m_pBindPolicy->OnAddBindingPath(BindPath);
	}

	else if (fRemove) {
		// Handle case where NCN_REMOVE bit is set
		m_pBindPolicy->OnRemoveBindingPath(BindPath);
	}

	return S_OK;
}


//	INetCfgComponentNotifyGlobal implementation
//	This is an optional interface for a Notify Object

template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::SysQueryBindingPath(
	DWORD dwChangeFlag,
	INetCfgBindingPath* pncbpItem
	)
{
    ATLTRACE(_T("CNotifyObject::SysQueryBindingPath\n"));

	HRESULT hr		= S_OK;
	BOOL fAdd		= dwChangeFlag & NCN_ADD;
	BOOL fEnable	= dwChangeFlag & NCN_ENABLE;
	BOOL fDisable	= dwChangeFlag & NCN_DISABLE;

#if	_DEBUG
	LPWSTR	lpsz;

	hr = pncbpItem->GetPathToken(&lpsz);
	if (hr == S_OK)
	{
		ATLTRACE(_T("  BindingPath:%s\n"),lpsz);
		CoTaskMemFree(lpsz);
	}
#endif

	CNetCfgBindingPath BindPath(pncbpItem);

	if ( (fAdd && fEnable) || fEnable ) {
		// If Add && Enable  or just Enable

		// Handle case where NCN_ADD and NCN_ENABLE bits are set
		if (false == m_pBindPolicy->OnAcceptBindingPathEnable(BindPath)) {
			hr = NETCFG_S_DISABLE_QUERY;
		}
	}

	if (fDisable) {
		if (false == m_pBindPolicy->OnAcceptBindingPathDisable(BindPath)) {
			hr = NETCFG_S_DISABLE_QUERY;
		}
	}

	return hr;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::SysNotifyBindingPath(
	DWORD dwChangeFlag,
	INetCfgBindingPath* pncbpItem
	)
{
    ATLTRACE(_T("CNotifyObject::SysNotifyBindingPath dwChangeFlag=%X\n"),dwChangeFlag);

	BOOL fEnable  = dwChangeFlag & NCN_ENABLE;
	BOOL fAdd	  = dwChangeFlag & NCN_ADD;
	BOOL fRemove  = dwChangeFlag & NCN_REMOVE;
	BOOL fDisable = dwChangeFlag & NCN_DISABLE;

	CNetCfgBindingPath BindPath(pncbpItem);

	if ( (fAdd && fEnable) || fEnable ) {
		// If Add && Enable  or just Enable
		// Handle case where NCN_ADD and NCN_ENABLE bits are set
		m_pBindPolicy->OnAddBindingPath(BindPath);
	}

	else if (fRemove) {
		// Handle case where NCN_REMOVE bit is set
		m_pBindPolicy->OnRemoveBindingPath(BindPath);
	}

	return S_OK;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::GetSupportedNotifications(DWORD* pdwNotifications)
{
    ATLTRACE(_T("CNotifyObject::GetSupportedNotifications\n"));

    *pdwNotifications =
		  NCN_ADD
		| NCN_REMOVE
		| NCN_UPDATE
		| NCN_ENABLE
		| NCN_DISABLE
		| NCN_BINDING_PATH
		| NCN_PROPERTYCHANGE
		| NCN_NET
		| NCN_NETTRANS
		| NCN_NETCLIENT
		| NCN_NETSERVICE
		;
	return S_OK;
}


template <class _BindPolicy, class _PropertyUi>
STDMETHODIMP CNotifyObject<_BindPolicy,_PropertyUi>::SysNotifyComponent(DWORD dwChangeFlag, INetCfgComponent *pComponent)
{
    ATLTRACE(_T("CNotifyObject::SysNotifyComponent dwChangeFlag=%X\n"),dwChangeFlag);

#if	_DEBUG
	LPWSTR	lpsz;
	HRESULT	hr;

	hr = pComponent->GetId(&lpsz);
	if (hr == S_OK)
	{
		ATLTRACE(_T("  Hardware ID:%s "),lpsz);
		CoTaskMemFree(lpsz);
	}

	hr = pComponent->GetBindName(&lpsz);
	if (hr == S_OK)
	{
		ATLTRACE(_T("BindName:%s"),lpsz);
		CoTaskMemFree(lpsz);
	}

	ATLTRACE(_T("\n"));
#endif

	// Not implemented:  Specialize this method for your use
	return S_OK;
}


// Helper methods used by delegated objects

template <class _BindPolicy, class _PropertyUi>
HKEY CNotifyObject<_BindPolicy,_PropertyUi>::OpenAdapterKey(GUID& AdapterInstanceGuid, CNetCfgComponent& Protocol)
{
	HKEY hRet;
	WCHAR wszGuid[GUID_CCH];
	int cchGuid = StringFromGUID2(AdapterInstanceGuid,wszGuid,GUID_CCH);
	// Did StringFromGUID2 work?
	ATLASSERT(GUID_CCH == cchGuid);
	if (GUID_CCH == cchGuid)
	{
		hRet = OpenAdapterKey(wszGuid, Protocol);
	}

	return hRet;
}


template <class _BindPolicy, class _PropertyUi>
HKEY CNotifyObject<_BindPolicy,_PropertyUi>::OpenAdapterKey(WCHAR* pszwAdapterInstanceGuid, CNetCfgComponent& Protocol)
{
	HKEY hRet = NULL;

	// Obtain handle to Protocol Component's Param reg key
	CRegKey RegKeyProt;
	RegKeyProt.Attach(Protocol.OpenParamKey());

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

#endif // !defined(AFX_NOTIFYOBJECT_H__0BFDF1C5_D2CA_11D4_B047_00C04F6A5DA9__INCLUDED_)
