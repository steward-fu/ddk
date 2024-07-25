// KNdisProtocolBinding.h: interface for the KNdisProtocolBinding class.
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

#if !defined(AFX_KNDISPROTOCOLBINDING_H__819D9677_9135_11D3_9001_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPROTOCOLBINDING_H__819D9677_9135_11D3_9001_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <kndis.h>
#include <KNdisPacketsTransfer.h>
#include <KNdisRequest.h>

#pragma warning (disable:4100)	// non-ref formals


//////////////////////////////////////////////////////////////////////
// KNdisProtocolBinding
//
// Base abstract class for NDIS 4 and 5 protocol binding implemented with
// arbitrary handler delegation scheme.
//
// Binding objects derived from KNdisProtocolBinding simply must declare
// the required signatures for handler functions, such as
// Open(), Close(), Status(), etc. They could be either virtual
// or non-virtual, or even inline-s. However, all the mandatory
// handlers MUST be present - a compile error will result otherwise.
//
// This class also defines methods for many NDIS services commonly called
// by NDIS protocol drivers.
//
class KNdisProtocolBinding : public KObject<NDIS_HANDLE, 'knpb'>
{
	SAFE_DESTRUCTORS
public:
	KNdisProtocolBinding();
	virtual ~KNdisProtocolBinding();

// Accessors
	operator PNDIS_STATUS() { return &m_CompleteStatus; }	// last status ptr
	operator PNDIS_MEDIUM() { return &m_SelectedMedium; }	// selected medium

// Synchronization for completion services

	BOOLEAN WaitForCompletion(UINT MSecToWait=0) { return m_CompleteEvent.WaitAndReset(MSecToWait); }
	void SignalCompletion() { m_CompleteEvent.Set(); }
	void SignalCompletion(NDIS_STATUS s) { m_CompleteStatus=s; m_CompleteEvent.Set(); }
	void ResetCompletion() { m_CompleteEvent.Reset(); }

//protected:
// NDIS Services used by protocols (those based on ProtocolBindingHandle)

// Sends a single packet over the bound NIC
	inline NDIS_STATUS Send(PNDIS_PACKET Packet)
#if BINARY_COMPATIBLE || (KNDIS_DDK_VERSION >= 5)
			{ NDIS_STATUS s; NdisSend(&s, m_Handle, Packet); return s;}
#else
	{ ASSERT(!"TODO"); UNREFERENCED_PARAMETER(Packet); return NDIS_STATUS_FAILURE;}
#endif


#if !defined(VTOOLSD)
// Sends an array of packets  over the bound NIC
	inline void Send(KNdisPacketsTransfer* PacketArray)
#if BINARY_COMPATIBLE || (KNDIS_DDK_VERSION >= 5)
			{ NdisSendPackets(m_Handle, *PacketArray, PacketArray->InFlight()); };
#else
	{ ASSERT(!"TODO"); UNREFERENCED_PARAMETER(PacketArray); }
#endif

// Returns NIC-indicated packet back to the miniport:
	static inline void ReturnPacket(IN PNDIS_PACKET p) { NdisReturnPackets(&p, 1); }

// Returns an array of NIC-indicated packets back to the miniport:
	static inline void ReturnPackets(IN PNDIS_PACKET* Array, IN UINT N) { NdisReturnPackets(Array, N); }

#endif // VTOOLSD

// Sends OID requests: better use KNdisRequest
	inline NDIS_STATUS Request(PNDIS_REQUEST preq)
#if BINARY_COMPATIBLE || (KNDIS_DDK_VERSION >= 5)
			{ NDIS_STATUS s; NdisRequest(&s, m_Handle, preq); return s;}
#else
	{ ASSERT(!"TODO"); UNREFERENCED_PARAMETER(preq); return NDIS_STATUS_FAILURE;}
#endif

// Reset the NIC
	inline NDIS_STATUS Reset() { NDIS_STATUS s; NdisReset(&s, m_Handle); return s; }

// Transfers partially indicated packet:
	inline NDIS_STATUS TransferData(IN NDIS_HANDLE MacReceiveContext,
						        IN UINT ByteOffset,
								IN UINT BytesToTransfer,
								IN OUT PNDIS_PACKET Packet,
								OUT PUINT BytesTransferred)
#if BINARY_COMPATIBLE || (KNDIS_DDK_VERSION >= 5)
									{ NDIS_STATUS s;
									  NdisTransferData(&s, m_Handle,
										MacReceiveContext,
										ByteOffset,
										BytesToTransfer,
										Packet,
										BytesTransferred
										); return s; }
#else
	{ ASSERT(!"TODO"); UNREFERENCED_PARAMETERS5(MacReceiveContext,ByteOffset,BytesToTransfer,Packet,BytesTransferred); return NDIS_STATUS_FAILURE;}
#endif


// Closes the adapter.
	void CloseAdapter(bool bWait=true);

// Simulates Unbind() event. Used to forcefully destroy binding 'b', eg on Unload()
	template <class B> static inline NDIS_STATUS Unbind(B* b)
	{ NDIS_STATUS s; KNdisProtocolWrapper<B>::Unbind(&s, PVOID(b), PVOID(b)); return s;}

// Set the "bound" flag. This is used by KNdisProtocolWrapper<> only
// (has to be public due to MSVC C++ limitations)
	void SetBound(bool on) { m_bBound = (on) ? ULONG(1) : ULONG(0); }

// Check if the object is bound. During transitions it potentially can be off
	bool IsBound() { return m_bBound!=ULONG(0); }

// Sleeps (used in Open(), Close(), or PnP handlers())
#ifdef NDIS_MINIPORT_DRIVER
	// IM and MP
	static inline void Sleep(IN ULONG MSecToSleep=10) { NdisMSleep(MSecToSleep); }
#else
	// Protocols
	static inline void Sleep(IN ULONG MSecToSleep=10) { KNdisEvent e; e.Wait(MSecToSleep); }
#endif

	// Overloadable accessor to system-registered container protocol handle.
	// Reqired by KNdisProtocolWrapper template. The default implementation
	// is instantiated by DECLARE_PROTOCOLDRIVER_CLASS macro. Derived
	// classes can overload this method by name scope resolution.
	NDIS_HANDLE GetContainerHandle();

	// Returns a friendly name for the bound adapter.
	inline PCWSTR GetAdapterName();
	// Returns a reference to the friendly name's string
	const KNdisString& GetAdapterNameString() { return m_AdapterName; }

#if (KNDIS_DDK_VERSION > 5)
	// Canceling send packets
	void CancelSendPackets(PVOID id) { NdisCancelSendPackets(m_Handle, id); }
	void CancelSendPackets(PNDIS_PACKET packet)
		{ NdisCancelSendPackets(m_Handle, NDIS_GET_PACKET_CANCEL_ID(packet)); }
	ULONG QueryPendingIOCount()
		{ ULONG c; return (NdisQueryPendingIOCount(m_Handle, &c)==NDIS_STATUS_SUCCESS)?c:0; }
#endif

// Handler Signatures
//////////////////////////////////////////////////////////////

// First call after object creation: new NIC is available
	 NDIS_STATUS Open(		IN PNDIS_STRING DeviceName,
							IN KNdisConfig& Config,
							OUT KNdisMedium& Medium,
							OUT PSTRING* AddressInfo);


// Informs the client the binding has been created
	 void OpenComplete();

// Informs the client the adapter associated with the binding going away
	 BOOLEAN Close();

// Packet is received from the NIC. Should return > 0 if the packet is accepted
	 INT Receive(const IN PNDIS_PACKET Packet) { return 0; };

// Packet header+lookahead is being indicated. Return > 0 if the packet is accepted
	 NDIS_STATUS Receive(IN OUT KNdisPartialPacket& PacketToAccept,
						IN  PVOID HeaderBuffer,
						IN  UINT HeaderBufferSize,
						IN	PVOID LookAheadBuffer,
						IN	UINT LookaheadBufferSize,
						IN	UINT PacketSize) { return 0; };

// Partial receive complete
	 void TransferDataComplete(PNDIS_PACKET Packet,
									  NDIS_STATUS Status,
									  UINT BytesTransferred) {};

// Receive indication burst has completed
	 void ReceiveComplete() {};

// Packet previusly submitted to the NIC si being reclaimed:
	 void SendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status) {};

// General Status indication:
	 void StatusIndication(IN NDIS_STATUS GeneralStatus,
						  IN PVOID StatusBuffer,
						  IN UINT StatusBufferSize) {};

// The underlying miniport's call to NdisMIndicateStatusComplete causes this call
	 void StatusComplete() {} ;

// Reset Complete
	 void ResetComplete(NDIS_STATUS Status) {}

// Request Complete
	 void RequestComplete(PNDIS_REQUEST NdisRequest, NDIS_STATUS Status) {}

// PnP Events.
//		 The default implementation below simulates a legacy protocol, i.e.
//		 it directs NDIS to destroy the binding on lower-power state transitions.
//		 One way to avoid that in a derived class would be using PoRegisterSystemState()
//		 whenever the binding has an active context (a connection).
// NOTE: The PNET_PNP_EVENT parameter is required since for transport drivers we have
//		 to forward the the power event up to the TDI clients. In NDIS filters and
//		 packet drivers it would be rarely used.
//
#if KNDIS_PNP_AWARE
	 NDIS_STATUS OnPnpSetPower(NDIS_DEVICE_POWER_STATE PowerState, PNET_PNP_EVENT)
	{ return (PowerState==NetDeviceStateD0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_NOT_SUPPORTED; }

	 NDIS_STATUS OnPnpQueryPower(NDIS_DEVICE_POWER_STATE /*PowerState*/, PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

	 NDIS_STATUS OnPnpQueryRemoveDevice(PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

	 NDIS_STATUS OnPnpCancelRemoveDevice(PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

	 NDIS_STATUS OnPnpReconfigure(PCWSTR /*wszProtocolSection*/, PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

	 NDIS_STATUS OnPnpCapabilityChange(ULONG /*uWakeUpCapFlag*/, PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

	// NOTE: The following 3 handlers has no associated binding object. Therefore,
	//       they are declared static and can be overriden as static, too, in a derived class.
	//       The KNdisProtocolWrapper template takes care of the proper delegation.

	static NDIS_STATUS OnPnpReconfigureGlobal(PCWSTR /*wszProtocolSection*/, PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

	static NDIS_STATUS OnPnpBindListChange(PCWSTR /*wszNewBindList*/, PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

	static NDIS_STATUS OnPnpBindListComplete(PNET_PNP_EVENT)
		{ return NDIS_STATUS_SUCCESS; }

#endif // KNDIS_PNP_AWARE


protected:
// Common data
	NDIS_STATUS		m_CompleteStatus;	// used for some async completions
	KNdisEvent		m_CompleteEvent;	// used to wait on some async requests
	NDIS_MEDIUM		m_SelectedMedium;	// medium type selected as a result of successful binding

	// Interlocked flag, which is 1 when the binding is active
	KNdisCounter	m_bBound;

private:
	// Cached "friendly" (not GUID!) name. This string's buffer is allocated
	// in a non-standard fashion, so we keep it private.
	KNdisString		m_AdapterName;
};

///////////////////////////////////////////////////////////////////////
// Returns "friendly" name for the bound adapter.
//
// Parameters:
// 	none
// Returns:
// 	Unicode character string with a friendly name like
//	"PCI Ethernet Adapter" rather than a scary GUID string.
//	NULL on error.
// Comments:
// 	The method caches the system-allocated string in the
//  object. The string gets freed only on object destruction.
//  By default, the framework caches the name automatically.
//  This could be disabled by specializing
//	KNdisProtocolTraits<B>::CacheFriendlyAdapterName() trait.
//  Note that the name is not available until OpenComplete()
//  handler has been called.
//  Not implemeted in NDIS 4.
PCWSTR KNdisProtocolBinding::GetAdapterName()
{
#if KNDIS_DDK_VERSION >= 5 && (defined(NDIS50) || defined(NDIS51))
	// Note the non-standard (actually, poor) way of using KNdisString
	if (!m_AdapterName.IsValid())
		NdisQueryAdapterInstanceName(m_AdapterName, m_Handle);
	return (m_AdapterName.IsValid()) ?
		(PCWSTR) PNDIS_STRING(m_AdapterName)->Buffer : NULL;
#else
	return NULL;
#endif
}

#pragma warning (default:4100)	// non-ref formals

#endif // !defined(AFX_KNDISPROTOCOLBINDING_H__819D9677_9135_11D3_9001_00C04F7445B7__INCLUDED_)
