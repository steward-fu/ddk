// KTransportPnp.h
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

#if !defined(_KTransportPnp_H__05CFE3D4_9EB2_4225_A102_B20564D3DF58__INCLUDED_)
#define _KTransportPnp_H__05CFE3D4_9EB2_4225_A102_B20564D3DF58__INCLUDED_

///////////////////////////////////////////////////////////////////////
// KTransportProvider
//
// A PNP Transport registers itself with TDI to provide notifications
// to the interested clients when the underlying newtork binding(s)
// are on, and the transport is ready to carry on its operations.
//
// An instance of KTransportProvider gets created in DriverEntry.
// Subsequently, the transport calls Ready() when the (usually, first)
// binding gets available and all the required resources have been
// allocated for the transport.
//
class KTransportProvider : public KObject<HANDLE, 'tprv'>
{
public:
	KTransportProvider();
	~KTransportProvider();
	SAFE_DESTRUCTORS

	NTSTATUS Register(PCWSTR TransportName); // Call on DriverEntry
	void Deregister();						  // Call on Unload

	NTSTATUS Ready();
};

///////////////////////////////////////////////////////////////////////
// KTransportPnpObject
//
// KTransportPnpObject is a wrapper for a TDI object representing a
// particular transport-to-NIC binding.
//
// A PNP Transport should register a "device"-like entity for each network
// binding with the TDI layer (tdi.sys). Generally, transports are not
// required to create a distinct device object for each underlying
// NIC they bind to (for instance, the Windows 2000 TCPIP driver doesn't do that).
// Other transport might want to maintain distinct device objects per
// each NIC binding to let the client direct their requests to specific
// network adapters.
//
// Each PNP transport, however, is required to notify the TDI layer about
// the available bindings by registering a TDI "device". Strictly speaking,
// it's not a device in the WDM sense, but just a named entity, which allows TDI
// clients to receive PNP notifications from the bottom of the network stack.
// The underlying system services TdiRegisterDeviceObject and TdiPnPPowerRequest.
//
// KTransportPnpObject is a helper class, which wraps this functionality.
//
// An instance of KTransportPnpObject is usually a data member of a
// KNdisProtocolBinding-derived class. When a new binding gets created
// (in KNdisProtocolBinding::Open() or OpenComplete() handler), the transport
// driver registers the new binding with the TDI by calling Register() method.
//
// Register() has few overloads. The 1st overload takes a generic pointer to a class
// exposing methods DeviceName. Usually it is a KDevice-derived object,
// which the transport has explicitely created to present the binding to TDI
// clients and, perhaps, networking applications.
// The name of the device is used as the name registered with TDI. This overload
// is a preferred form for transports implementing a device-per-binding design.
//
// The 2nd overload is a generic form. For transports not maintaining
// device-per-binding it allows to conveniently generate the name of the
// interface by concatinating transport name (such as "TCPIP") with the
// network device GUID name passed on  KNdisProtocolBinding::Open().
//
// Reference to KTransportPnpObject is used to notify PNP-aware
// TDI clients on network address changes (see KTransportPnpAddress) and
// power events relevant to the binding
//
//
class KTransportPnpObject : public KObject<HANDLE, 'tpnp'>
{
public:
	KTransportPnpObject();
	~KTransportPnpObject();
	SAFE_DESTRUCTORS

// Register Binding

	template <class D>
	NTSTATUS Register(D* Device)
		{ return Register(Device->DeviceName(), NULL); }
	NTSTATUS Register(PCWSTR TranspName, PUNICODE_STRING DeviceGUID=NULL)
		{ KNdisStringConst wStr(TranspName); return Register(PUNICODE_STRING(wStr), DeviceGUID); }
	NTSTATUS Register(PUNICODE_STRING TranspName, PUNICODE_STRING DeviceGUID=NULL);

// Deregister Binding
	void Deregister();

// Notify on Power event on the binding
	NTSTATUS PowerRequest(IN PNET_PNP_EVENT PowerEvent,
				   IN PTDI_PNP_CONTEXT Context1=NULL,
				   IN PTDI_PNP_CONTEXT Context2=NULL,
				   BOOLEAN bWait=TRUE);

// Wait until current power event processed by all registered TDI clients
	NTSTATUS Wait() { return m_Event.Wait(); }

// Get cached name
	KNdisString& Name() { return m_wsName; }
// Conversion to string
	operator PUNICODE_STRING() { return PUNICODE_STRING (m_wsName); }

protected:
	KNdisString		m_wsName;
	KNdisEvent		m_Event;		// wait for async completions

	struct CallbackContext {	// context used for async cases
		KTransportPnpObject*	m_this;
		NTSTATUS*				m_pStatus;
	};

	static VOID CompletionCallback(IN PNET_PNP_EVENT NetEvent, IN NTSTATUS FinalStatus);
	virtual void CompletionHandler (IN PNET_PNP_EVENT NetEvent, IN NTSTATUS FinalStatus);
};


///////////////////////////////////////////////////////////////////////
// KTransportPnpAddress
//
// Local network address registered with TDI.
//
// A PNP Transport is required to register each new network address or
// a new remote connection with the TDI. It allows to notify the interested
// TDI clients about the network address changes (see ClientPnPAddNetAddress).
// The underlyiing system call is TdiRegisterNetAddress.
//
// NOTE: Network addresses are not to be confused with transport address objects!
//
// Instances of KTransportPnpAddress are usually built into the transport's
// KNdisProtocolBinding-derived class. It could be a single instance or a list
// of instances if the transport allows multiple network addresses per binding.
//
// For instance, a TCPIP driver might create a KTransportPnpAddress as a result
// of DHCP transaction completion over a specific binding.
//
class KTransportPnpAddress : public KObject<HANDLE, 'tadr'>
{
public:
	KTransportPnpAddress();
	~KTransportPnpAddress();

	NTSTATUS Register(KTransportPnpObject& TdiDevice,
							PTA_ADDRESS Address, PTDI_PNP_CONTEXT Context=NULL);
	void Deregister();

public:
	LIST_ENTRY		m_ListEntry;		// For linking the addresses if needed
protected:
};

///////////////////////////////////////////////////////////////////////
// KTransportPnpContext
//
// Helper class for maintaining PNP transport contexts passed to TDI
// clients on TdiPnPPowerRequest and TdiRegisterNetAddress notifications.
//
// KTransportPnpContext wrapps TDI_PNP_CONTEXT structure used to pass TDI
// clients some additional info about the indication. Template parameter T
// defines one of TDI_PNP_CONTEXT_TYPE_ values defined in the DDK. Second
// temlate parameter, C, defines the context structure used by the transport.
// Eg the TCPIP driver would use
//
// KTransportPnpContext<TDI_PNP_CONTEXT_TYPE_PDO, PDEVICE_OBJECT> Context(Pdo);
// m_PnpAddress.Register(m_PnpBinding, NewAddress, Context);
//
// when indicating a new transport address on the physical network adapter
// indetified by Pdo to the TDI clients.
//
//
template <USHORT T, typename C>
class KTransportPnpContext
{
public:
	inline KTransportPnpContext()
		{ m_Context.Size = sizeof(C)+sizeof(TDI_PNP_CONTEXT)-1; m_Context.Type = T;}
	inline KTransportPnpContext(C& Context)
		{ m_Context.Size = sizeof(C)+sizeof(TDI_PNP_CONTEXT)-1; m_Context.Type = T;
		  m_Context.Data = Context;
		}

	inline operator PTDI_PNP_CONTEXT() { return (PTDI_PNP_CONTEXT) &m_Context; }

	inline KTransportPnpContext& operator=(C Context)
		{ m_Context.Data = Context; return *this; }

	C& Data() { return m_Context.Data; }

protected:
	struct { USHORT Size; USHORT Type; C Data; } m_Context; // TDI_PNP_CONTEXT
};

/////////////////////////////////////////////////////////////////////
// Inline implementations
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// KTransportProvider

inline KTransportProvider::KTransportProvider() : KObject<HANDLE, 'tprv'>()
{
}
inline KTransportProvider::~KTransportProvider()
{
	Deregister();
}
inline NTSTATUS KTransportProvider::Register(PCWSTR TransportName)
{
	ASSERT(!IsValid());
	KNdisString wsName(TransportName);
	return ::TdiRegisterProvider(wsName, &m_Handle);
}
inline void KTransportProvider::Deregister()
{
	if (IsValid()) {
		::TdiDeregisterProvider(m_Handle);
		Invalidate();
	}
}
inline NTSTATUS KTransportProvider::Ready()
{
	return ::TdiProviderReady(m_Handle);
}

///////////////////////////////////////////////////////////////////////
// KTransportPnpAddress

inline KTransportPnpAddress::KTransportPnpAddress() : KObject<HANDLE, 'tadr'>()
{
}
inline KTransportPnpAddress::~KTransportPnpAddress()
{
	Deregister();
}
inline NTSTATUS KTransportPnpAddress::Register(KTransportPnpObject& TdiDevice,
							PTA_ADDRESS Address, PTDI_PNP_CONTEXT Context)
{
	ASSERT(!IsValid());
	return ::TdiRegisterNetAddress(Address, TdiDevice.Name(), Context, &m_Handle);
}
inline void KTransportPnpAddress::Deregister()
{
	if (IsValid()) {
		::TdiDeregisterNetAddress(m_Handle);
		Invalidate();
	}
}

#endif  //(_KTransportPnp_H__05CFE3D4_9EB2_4225_A102_B20564D3DF58__INCLUDED_)
