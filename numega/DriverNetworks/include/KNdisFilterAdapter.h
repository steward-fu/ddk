// KNdisFilterAdapter.h: interface for the KNdisFilterAdapter class.
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

#if !defined(AFX_KNDISFILTERADAPTER_H__78D864B3_938E_11D3_9002_00C04F7445B7__INCLUDED_)
#define AFX_KNDISFILTERADAPTER_H__78D864B3_938E_11D3_9002_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter
//
// Base abstract class for layered IM miniport filters.
//
// NDIS IM drivers derive from this class to implement packet filter
// functionality. Classes derived from KNdisFilterAdapter find their "adapter" object
// layered between the upper-layer protocols and a lower-layer real NIC.
// The upper edge of the filter driver is referred to as "virtual NIC" since
// the higher layer protocols drivers treat it as a normal NIC adapter they can
// bind to. The lower edge of the filter driver represents an instance of a
// protocol, which, in turn, can bind itself to a "real NIC" miniport.
//
// Filter adapters, thus, get "sandwisched" between a real protocol and a real
// NIC miniport and act as "passthru" 2-way filters intercepting all the packets,
// requests and events flowing up and down between an instance of real protocol
// and a real miniport.  An instance of a KNdisFilterAdapter-derived class represents
// a single association between the real protocol and miniport. The interception is
// performed by the method-handlers acting on the instance of such a class.
//
// KNdisFilterAdapter publicly inherits from KNdisMiniAdapter, and it follows it's
// model of providing 2 types of methods, handlers and services. The handlers,
// however, use different naming conventions: OnXxx(). All the OnXxx() handlers
// are defined virtual in KNdisFilterAdapter. The user implements the filtering
// functionality by overriding (implementing) the appropriate handlers. All the
// handlers are optional; the trivial derivation of KNdisFilterAdapter would act
// as a NOP ("passhtru") filter. In reality, the user might want to override at
// least OnSend() and OnReceive() handlers.
//
// INSTANTIATION AND DESTRUCTION
//
// The instance(s) of a KNdisFilterAdapter-derived class is created by the
// DriverNetWorks framework automatically, so there is never a need to call
// new() for the adapter. The framework takes care of registering both
// miniport and protocol "personalities" of the IM driver with NDIS, instantiating
// an instance of the filter adapter at appropriate time and calling OnInitialize().
// In OnInitialize(), the driver performs custom resource allocation (if any), etc.
//
// The destruction of KNdisFilterAdapter-derived objects is handled by the framework, too.
// Immediately before the destruction the OnHalt() handler gets called. In OnHalt(), the filter adapter
// class driver performs custom resource allocation (if any), etc.
//
// NOTE: The IM framework assumes the deserialized NDIS miniport execution
// environment. Thus, the driver should care to provide necessary protection
// for objects shared between class' handlers. To faclilitate such measures KNdisFilterAdapter
// comes with a built-in general purpose spinlock exposed via public Lock() and Unlock() methods,
// which could be used to implement the protection.
//
// LBFO SUPPORT
//
// KNdisFilterAdapter provides a built-in LBFO support by maintaing an internal instance of
// KNdisBundle object (provided the Registry is configured to do so). The instance of the bundle
// is accessed using GetBundle() method. The bundle comprises instances of KNdisFilterAdapter objects,
// each of which represents a real NIC miniport exposed to the protocols via a filter adapter instance.
// The adapter instances are accessed using GetBundleAdapter() method. Each filter adapter has
// exactly one binding to the real NIC it represents. Normally, the packets coming to the adapter's OnSend()
// method get forwarded over that binding. In a LBFO scenario, however, the driver
// might decide to forward packets over bindings belonging to other adapter instances. The second
// parameter in ForwardDown() and ForwardUp() methods specifies the adapter used for forwarding packets
// down to the network or up to the protocol accordingly. The default, NULL, passes the packet using
// the 'this' adapter. See the "How To Support LBFO" topic in online help for more.
//
// LIMITATION on packet's reserved field use.
//
// Generally, subclasses of KNdisFilterAdapter cannot use the reserved fields in the packets coming
// from the base class pools (besides the one marked 'ClientReserved' in the TxRESERVED).
// If the subclass maintains private packet pools and uses them for passing packets up and down,
// it can use the reserved fields with the following limitation: the first sizeof(PVOID) bytes
// of both ProtocolReserved and MiniportReserved field should be reserved for the 'this' pointer
// of the originating adapter. This layout is assumed by methods ForwardUp() and ForwardDown() that
// should be used for passing the packets up and down the stack.
// For convenience, KNdisFilterAdapter defines a "base" class, PRIVATExRESERVED, which clients could
// privately inherit from to define their "reserved field" structures.
//
// NOTE: KNdisFilterAdapter implementents NDIS 5.0 filter. For NDIS 5.1, use KNdisFilterAdapter51.
//
class KNdisFilterAdapter : public KNdisMiniAdapter
{
	SAFE_DESTRUCTORS;
public:
	KNdisFilterAdapter();
	virtual ~KNdisFilterAdapter();

// Overridables: must be implemented by a custom filter

// Upper-edge: delegated from KNdisWrapper<Adapter>

	virtual NDIS_STATUS OnInitialize(const KNdisMedium& Medium, KNdisConfig& Config);
	virtual VOID OnHalt();
	virtual NDIS_STATUS OnReset();

	virtual VOID OnSendComplete(PNDIS_PACKET, NDIS_STATUS);
	virtual VOID OnReturnPacket(IN PNDIS_PACKET);

	// Intercepting upper-layer OID requests:
	virtual NDIS_STATUS OnQuery(KNdisRequest& Req, NDIS_STATUS ReturnedStatus);
	virtual NDIS_STATUS OnSet(KNdisRequest&);

	virtual NDIS_STATUS OnSend(const KNdisPacket& Original, KNdisPacket& Repackaged);

	// Lower-edge: delegated from KNdisProtocolWrapper<Binding>

	virtual NDIS_STATUS OnReceive(const KNdisPacket& Original, KNdisPacket& Repackaged);
	virtual NDIS_STATUS OnReceive(IN OUT KNdisPartialPacket& PacketToAccept,
								  IN PVOID HeaderBuffer, IN UINT HeaderBufferSize,
								  IN PVOID LookAheadBuffer, IN UINT LookaheadBufferSize,
								  IN UINT PacketSize);

	virtual NDIS_STATUS OnTransferComplete(PNDIS_PACKET Packet, NDIS_STATUS Status, UINT BytesTransferred);

	virtual NDIS_STATUS OnStatusIndication(NDIS_STATUS Status, IN OUT PVOID*, UINT*);
	virtual NDIS_STATUS OnStatusIndicationComplete();

#if KNDIS_PNP_AWARE
	virtual NDIS_STATUS OnReconfiguration(PCWSTR Section, PNET_PNP_EVENT pEvent);
	virtual void OnPowerState(NDIS_DEVICE_POWER_STATE NewState, BOOLEAN bRequestNotNotification, PNET_PNP_EVENT pEvent);
#endif

public:
// Methods-helpers

// Passing data over bindings
	NDIS_STATUS ForwardDown(PNDIS_PACKET Packet, KNdisFilterAdapter* Originating=NULL);
	void ForwardUp(PNDIS_PACKET Packet, KNdisFilterAdapter* Originating=NULL);
	void ForwardUp(NDIS_HANDLE OriginatingReceiveContext,
					 IN PVOID HeaderBuffer, IN UINT HeaderBufferSize,
					 IN PVOID LookAheadBuffer, IN UINT LookaheadBufferSize,
					 IN UINT PacketSize,
					 KNdisFilterAdapter* Originating);
// Lower miniport name
	inline const PWSTR GetBoundDeviceName();

// Bundle access
	inline KNdisBundle* GetBundle();
	inline KNdisFilterAdapter* GetBundleAdapter(ULONG index=0);

// Gen purpose spinlock. Can be used to protect operations of the deserialized miniport
	inline void Lock() { m_Lock.Lock(); }
	inline void Unlock() { m_Lock.Unlock(); }

// Adapter State (mostly to handle weird cleanup conditions)
	enum State {
		DOWN,				// not-started
		INITIALIZING,		// establsihing bindings
		UP,					// up and running (filtering)
		CLOSING				// destroying binding
	};

protected:
// NDIS miniport callback handlers: methods required by KNdisWrapper template

	NDIS_STATUS Initialize(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config);

    VOID Halt(VOID);
    NDIS_STATUS Reset(OUT PBOOLEAN AddressingReset);

    NDIS_STATUS Send(IN	PNDIS_PACKET Packet,IN	UINT Flags);
    VOID SendPackets(IN	PPNDIS_PACKET PacketArray, IN UINT NumberOfPackets);
	VOID ReturnPacket(IN PNDIS_PACKET Packet);
    NDIS_STATUS TransferData(
			OUT PNDIS_PACKET			Packet,
			OUT PUINT					BytesTransferred,
			IN  PVOID					FrameLocation, /* MiniportReceiveContext */
			IN	UINT					ByteOffset,
			IN	UINT					BytesToTransfer
			);

	NDIS_STATUS QueryInformation(
			IN	NDIS_OID				Oid,
			IN	PVOID					InformationBuffer,
			IN	ULONG					InformationBufferLength,
			OUT PULONG					BytesWritten,
			OUT PULONG					BytesNeeded
			);
	NDIS_STATUS SetInformation(
			IN	NDIS_OID				Oid,
			IN	PVOID					InformationBuffer,
			IN	ULONG					InformationBufferLength,
			OUT PULONG					BytesRead,
			OUT PULONG					BytesNeeded
			);

	// The methods below are not required functionally.
	// If desired they could be defined on a derived class.
	//
    // BOOLEAN CheckForHang();
	VOID Shutdown(VOID) {}


// NDIS protocol callback handlers: delegated by KNdisFilterBinding from KNdisProtocolWrapper<>
// Those should be processed internally by "friendly" KNdisFilterBinding in most cases.

// Friends and Family
	friend class KNdisFilterBinding;

// Accessor to system-registered container handle for all instances of this adapter:
	static NDIS_HANDLE GetContainerHandle();

// Retrieving associated protocol binding
	KNdisFilterBinding*	GetBinding() const { return m_pBinding; }

protected:
// Internals
	KNdisFilterBinding*		m_pBinding;		// associated NIC binding being "filtered"
	KNdisSpinLock			m_Lock;			// spin-lock for use by derived classes

#if KNDIS_PNP_AWARE
	KNdisPowerTransition<KNdisFilterAdapter> m_Power; // Power Management State

	// Internal method that modifies PnP caps to what DDK says NDIS wants to see; uses current m_Request
	NDIS_STATUS ManglePnpCapabilities(PNDIS_PNP_CAPABILITIES pCaps);

#endif
	enum State				m_State;		// working state. Consider: spinlock on it

	typedef struct {	// Format for Repackaged @ ProtocolReserved
		KNdisFilterAdapter*	OriginalAdapter;	// for LBFO and Mux cases it might be != this
		PNDIS_PACKET	    OriginalPacket;
	} TxRESERVED;
	typedef struct {	// Format for Repackaged @ MiniportReserved
		KNdisFilterAdapter*	OriginalAdapter;	// for LBFO an Mux cases it might be != this
		PNDIS_PACKET		OriginalPacket;
	} RxRESERVED;
	typedef struct {	// base class for privately allocated client pools
		KNdisFilterAdapter*	OriginalAdapter;
	} PRIVATExRESERVED;

// Repackaging resources. Can be used only by the frameowork
	KNdisPacketPool			m_TxPool;		// pool for repackaged outgoing packets
	KNdisPacketPool			m_RxPool;		// pool for repackaged incoming packets (used by KNdisFilterBinding)

	struct Request : public KNdisRequest    // repackaged OID request sent over the binding
	{
		BOOLEAN				bPending;			// indicates it's pending
		PULONG				BytesNeeded;		// pointer from the original query/set call
		PULONG				BytesMoved;			// pointer from the original query/set call
		inline Request() : bPending(FALSE) {}
	} m_Request;

};

typedef KNdisFilterAdapter	KNdisFilterAdapter50;	// implements NDIS 5.0 API

////////////////////////////////////////////////////////////////////
// Implementations



///////////////////////////////////////////////////////////////////////
// Return device name
//
// Parameters:
// 	none
// Returns:
// 	Device name string (UNICODE), or NULL if the name is not known
// Comments:
// 	none
const PWSTR KNdisFilterAdapter::GetBoundDeviceName()
{
	return (m_pBinding) ? PWSTR (m_pBinding->m_strDeviceName) : NULL;
}



///////////////////////////////////////////////////////////////////////
// Return LBFO bundle object
//
// Parameters:
// 	none
// Returns:
// 	LBFO bundle object or NULL if no bundle were configured for
//  the underlying binding.
// Comments:
// 	none
KNdisBundle* KNdisFilterAdapter::GetBundle()
{
	return (m_pBinding) ? m_pBinding->m_pBundle : NULL;
}


///////////////////////////////////////////////////////////////////////
// Get primary or secondary adapter from the LBFO bundle
//
// Parameters:
// 	index                - Adapter index in the bundle, 0 for primary
// Returns:
// 	Filter adapter object from the bundle. If no bundle has been configured
//  'this' pointer is returned.
// Comments:
// 	The returned pointer can be used to forward packets over the binding
//  other than binding for this adapter.
KNdisFilterAdapter* KNdisFilterAdapter::GetBundleAdapter(ULONG index)
{
	KNdisBundle* pBundle = GetBundle();
	return (pBundle) ? (reinterpret_cast<KNdisFilterAdapter*> ((*pBundle) [index])) : this;
}


inline NDIS_HANDLE KNdisFilterAdapter::GetContainerHandle()
{
	// Assumes the driver uses KNdisIMiniDriver-derived for driver singleton

	return
	static_cast<KNdisIMiniDriver*>(KNdisMiniDriver::DriverInstance())->GetMiniportHandle();
}

#endif // !defined(AFX_KNDISFILTERADAPTER_H__78D864B3_938E_11D3_9002_00C04F7445B7__INCLUDED_)
