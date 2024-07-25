// KTransport.h	-	Model classes for DriverNetworks transport
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
//                  driver class framework.
//
// The model classes describe transport entities (such as control channel,
// transport address and connection context) by defining the interfaces
// (method signatures) required by the framework for such entities.
//
// The model classes define the conceptual interfaces (mostly, for documentation
// purposes). In a way, this is similar to the DDK online doc defining conceptual
// interfaces for the handlers and callbacks, such as ProtocolXxx, TdiDispatchXxx, etc.
//
// Defining the model classes allows for a generic approach to driver developement.
//
// KTransportXxx classes define the signatures of methods used by
// KTransportIrpDispatch template to translate and dispatch TDI IRPs
// into the actual implementation methods of the user's protocol driver.
//

#if !defined(_KTransport_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_)
#pragma once
#define _KTransport_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_

//{{AFX_DNW_MODEL
struct /*namespace for browsing only*/ KTransportModel
{
///////////////////////////////////////////////////////////////////////
// KTAddress
//
// KTAddress represents TDI address type used by the transport.
// It is one of the specializations of KTA_ADDRESS template.
//
struct KTAddress : public KTA_ADDRESS<T> {};

///////////////////////////////////////////////////////////////////////
// KTBinding
//
// KTBinding represents a NDIS binding between the transport and an
// underlying NIC adapter.
//
struct KTBinding : public KNdisProtocolBinding {};

///////////////////////////////////////////////////////////////////////
// KTransportEntity (model class)
//
// KTransportEntity is a TDI transport object, which could be either
// control channel, transport address or connection context objects.
// Each TDI entity exposes the 3 methods defines below.
//
struct KTransportEntity
{
	NTSTATUS QueryInformation(KIrp);
	NTSTATUS SetInformation(KIrp);
	NTSTATUS Action(KIrp);
};

///////////////////////////////////////////////////////////////////////
// KTransportAddress (model class)
//
// KTransportAddress models Transport Address Object.
//
// The driver developer model her specific Transport Address class
// after KTransportAddress, i.e. the target class must expose the same
// methods to be compliant with DriverNetworks framework for transport
// drivers.
//
// Any transport driver written in DriverNetworks must implement
// the methods declared below on its transport address class.
// Note that DriverNetworks wizard automatically generates skeleton
// implementations for the required methods.
//
// Instances of KTransportAddress get created as a result of a TDI Client
// issuing ZwCreateFile  (IRP_MJ_CREATE) with EA specifying 'TdiTransportAddress'.
//
struct KTransportAddress : public KTransportEntity
{
	NTSTATUS SendDatagram(KIrp);
	NTSTATUS ReceiveDatagram(KIrp);
	NTSTATUS SetEvent(KIrp);
};

///////////////////////////////////////////////////////////////////////
// KTransportConnection (model class)
//
// KTransportConnection models Connection Endpoint object.
//
// The driver developer model her specific Connection Endpoint class
// after KTransportConnection, i.e. the target class must expose the same
// methods to be compliant with DriverNetworks framework for transport
// drivers.
//
// Any connection-oriented transport driver written in DriverNetworks must
// implement the methods declared below on its connection endpoint class.
// Note that DriverNetworks wizard automatically generates skeleton
// implementations for the required methods.
//
// Instances of KTransportConnection get created as a result of a TDI Client
// issuing ZwCreateFile  (IRP_MJ_CREATE) with EA specifying 'TdiConnectionContext'.
//
struct KTransportConnection : public KTransportEntity
{
	NTSTATUS Send(KIrp);
	NTSTATUS Receive(KIrp);

	NTSTATUS Associate(KIrp, KTransportAddress* Tao);
	NTSTATUS Disassociate(KIrp);

	NTSTATUS Connect(KIrp);
	NTSTATUS Listen(KIrp);
	NTSTATUS Accept(KIrp);
	NTSTATUS Disconnect(KIrp);
};

///////////////////////////////////////////////////////////////////////
// KTransportDevice (model class)
//
// KTransportDevice models a KDevice-derived class, which implements the
// upper edge of the TDI interface. It acts as a sink of the TDI IRP
// requests for the transport. The transport manipulate transport
// addresses of type A and, optionally, connection endpoint contexts of type C.
//
// In DriverNetworks, KTransportDevice classes also implement the
// functionality of the control channel. This suggests that only one
// control channel instance exists per transport device. TDI clients,
// of course, can create multiple file objects to access this single
// instance.
//
// Any transport driver written in DriverNetworks must implement
// the methods declared below on its transport device class.
// Note that DriverNetworks wizard automatically generates skeleton
// implementations for the required methods.
//
// The instance of KTransportDevice (usually, a singleton) is generated
// on DriverEntry() by instantiating an implementation of the
// KTransportIrpDispatch template.
//
struct KTransportDevice : public KDevice, public KTransportEntity
{
// General handlers related to the Control Channel object:
	NTSTATUS Create(KIrp);									// create control channel
	NTSTATUS Close(KIrp);									// close conrol channel
	NTSTATUS CleanUp(KIrp);									// cleanup conrol channel

// Transport Address-related TDI handlers:
	KTransportAddress* CreateTransportAddress(KIrp, PTRANSPORT_ADDRESS);	// create TA
	NTSTATUS CleanUp(KIrp, KTransportAddress*);								// clean up TA
	NTSTATUS Close(KIrp, KTransportAddress*);								// close TA

// Transport Connection-related TDI handlers:
    KTransportConnection* CreateConnectionEndpoint(KIrp, PVOID);	// create connection context
    NTSTATUS CleanUp(KIrp, KTransportConnection*);					// cleanup connection context
    NTSTATUS Close(KIrp, KTransportConnection*);					// close connection context
};


///////////////////////////////////////////////////////////////////////
// KTransportTAFactory (model class)
//
// KTransportTAFactory models a class responsisble for allocating and
// maintaining (a set of) transport addresses used by given transport.
// For instance, for a transport implementing the UDP, this class would
// manage a set of UDP ports available for the clients.
//
// A single instance of KTransportTAFactory is usually built into transport's
// control channel object (KTransportDevice).
//
// Method Register() allocates new trasport address or returns a pointer to
// an existing one. It also maintains the reference count for each addrress.
// This is necessary since transports can maintain more than one transport
// address object (KTransportAddress) per transport address. Register() is usually
// called from KTransportDevice::CreateTransportAddress().
//
// Deregister() decrements the ref count and, perhaps, destroys the address.
// Called from KTransportDevice::Close().
//
// Lookup() checks for existance of an address.
//
struct KTransportTAFactory
{
	const KTAddress* Register(PTA_ADDRESS pTA);
	void Deregister(KTAddress* ta);
	KTAddress* Lookup(PTA_ADDRESS pTA);
	ULONG Usage(const KTAddress* ta);
};

///////////////////////////////////////////////////////////////////////
// KTransportRouter (model class)
//
// KTransportRouter models a container, which associates transport addresses with
// bindings.
// Every non-trivial transport driver conceptually maintains at least 2 lists:
// active address list (local KTA_ADDRESS-s) and active binding list
// (KNdisProtocolBinding-s). In addition, the transport usually needs to
// implement some form of mapping between destination addresses
// and the bindings over which data should be sent to the destination.
// For instance, a TCP/IP driver would have this mapping done in its Routing Table.
//
// KTransportRouter can be considered a list of pairs, or, routes
// <TA_ADDRESS, KNdisProtocolBinding> used for fast lookup of a binding B
// by an address. Each pair is optionally associated with mask/metrics predicate
// class M, which can be used for various predicate operations
// (such as search for the best entry).
//
// User's implementation might also have more methods, such as enumeration, locking, etc.
// basically anything a multiset<> would might have.
//
struct KTransportRouter
{
	struct M {};		// Metrics class

	// Cummulative structure returned on searches
	struct Entry {
		KTBinding*		binding;
		KTAddress		ta;
		M				metrics;		// entry's attributes
	};

	// Adds new entry
	NTSTATUS Add(KTAddress* Dest, KTBinding* Binding, M Metrics);

	// Remomes entries
	NTSTATUS Remove(KTAddress* Dest, KTBinding* Binding, M Metrics);
	NTSTATUS Remove(KTAddress* Dest);
	NTSTATUS Remove(KTBinding* Binding);

	// Finds best match
	const Entry* Find(KTAddress* Dest, M Metrics);

};

///////////////////////////////////////////////////////////
// KTransportArp
//
// KTransportArp models a Address Resolution Provider
// needed by the transport.
// KTransportArp provides mapping between the (destination)
// transport addresses and MAC addresses on the local network.
//
// For each supported medium type M, the transport driver must provide
// a KTransportArp-derived class implementing the Resolve(), etc. methods.
// DriverNetworks suggests a template based naming scheme defined
// as KTransportArpOver<M> for the implementations
//
// In DriverNetworks, a single KNdisProtocolBinding-derived binding class
// is used for all types of underlying medium types. Thus, an instance of
// KTransportArp should be built into KTBinding instance, and a run-time
// linking (ie virtual interface) should be used for ARP providers.
// In real life, each ARP provider could be implemented in a separatly loadable driver.
//
struct KTransportArp  {
	// Resolve address. Returns pointer to MAC address.
	// The size of the address is a constant dependant of M, eg 6 for 802.3

	virtual const PUCHAR Resolve(const KTAddress* Dest) = 0;

	// Populate cache externally

	virtual void Add(const KTAddress* Dest, const PUCHAR MACAddress) = 0;
	virtual void Remove(const KTAddress* Dest, const PUCHAR MACAddress) = 0;
};

// NOTE: template is used as a naming scheme.
template <const NDIS_MEDIUM M>
struct KTransportArpOver : public KTransportArp{};


////////////////////////////////////////////////////////////////////
// KTransportProtocol (model class)
//
// KTransportProtocol is a class-utility, which defines formats and
// constants used by the transport driver's protocol. It is an abstract
// base class that defines formatting methods, BuildXx() and ParseXx(),
// to help manipulate the protocol packets.
//
// For each supported medium type M, the transport driver must provide
// a KTransportProtocol-derived class implementing those methods.
// DriverNetworks suggests a template based naming scheme defined
// as KTransportProtocolOver<M> for the implementations
//
//
class KTransportProtocol {
	// Header format
	struct TpduHeader {
		// . . .
	};

	// constants
	//
	// . . .

	// Build Layer 4 (TPDU) header for outgoing TSDU
	virtual NDIS_STATUS
	BuildL4(IN OUT PNDIS_PACKET Packet,			// packet to build for transfer
		    IN KTAddress& Destination,						// destination TA
			IN KTAddress& Source,							// source TA
			IN OUT KNdisBuffer& L4,							// mapped L4 descriptor to fill in
			IN PMDL	 Tsdu,									// user's data
			IN ULONG TsduLen,								// Length of TSDU
			IN KTransportAddress* TAObject=NULL				// optional object ptr to access the state
		  ) = 0;

	// Build Layer 2 header for outgoing TPDU
	virtual NDIS_STATUS
	BuildL2(IN OUT PNDIS_PACKET Packet,			// packet to build for transfer
		    IN PUCHAR Destination,							// destination MAC
			IN PUCHAR Source,								// source MAC
			IN OUT KNdisBuffer& L2, 						// mapped L2 descriptor to fill in
			IN PNDIS_BUFFER L4,								// TPDU
			IN ULONG TpduLen								// Length of TPDU
		  ) = 0;

	// Parse packet received from the network
	virtual NDIS_STATUS
	Parse(IN PNDIS_PACKET Packet,				// packet received
			IN KNdisBufferPool& BufPool,				// pool to allocate descriptors from
			OUT KTAddress& Destination,					// destination TA to fill in
			OUT KTAddress& Source,						// source TA to fill in
			OUT KNdisBuffer& L2,						// ptr to L2 descriptor to fill in (optional)
			OUT KNdisBuffer& L4,						// ptr to L4 descriptor to fill in
			OUT KNdisBuffer& Tsdu						// ptr to TSDU descriptor to fill in
		  ) = 0;


};

// NOTE: template is used as a naming scheme.
template <const NDIS_MEDIUM M>
class KTransportProtocolOver : public KTransportProtocol {}


}; // KTransportModel


//}}AFX_DNW_MODEL

#endif  //(_KTransport_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_)
