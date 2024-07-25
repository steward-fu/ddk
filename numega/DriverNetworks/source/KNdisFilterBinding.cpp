// KNdisFilterBinding.cpp: implementation of the KNdisFilterBinding class.
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

#include "kndis.h"
#include "KNdisFilterBinding.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KNdisFilterBinding::KNdisFilterBinding() :
	KNdisProtocolBinding(),
		m_pAdapter(NULL), m_pBundle(NULL)
{
}

KNdisFilterBinding::~KNdisFilterBinding()
{
	if (m_pBundle)
		KNdisBundle::Deregister(NULL, m_pBundle);
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::Open
//
// The first method called on a newly created binding object.
// The framework calls Open() when a new miniport adapter (NIC) has just
// registered itself. The handler should inspect configuration, initialize
// internal objects, fill in medium types we honor and return
// NDIS_STATUS_SUCCESS if the new binding  to the specified adapter should
// be granted. An error return prevents binding, and the binding object
// gets immediately destroyed.
//
// The IM driver (which this class should be part of) must do an extra step
// after the binding is complete: initialize an instance of the associated
// miniport: see OpenComplete().
//
// Parameters:
//		DeviceName	Name of the NIC device being bound
//		Config		Protocol Configuration object (Registry)
//		Medium		Empty Medium object. The handler must fill in
//					the medium type(s) supported.
//		AddressInfo	Optional char string the client can pass to talk to the device.
//
// Return:
//		NDIS_STATUS_SUCCESS		The framework should proceed with binding creation
//		NDIS_STATUS_XXXXXXX		Initialization error, do not bind.
//
// TODO: Find a way to specialize medium types we honor: Registry? Hard-coded for now.
//
// DDK Says: "The reserved keyword "UpperBindings" contains a list of device names for the
//		 virtual adapters that this particular binding is to expose"
//
//
NDIS_STATUS KNdisFilterBinding::Open(IN PNDIS_STRING DeviceName,
				 IN KNdisConfig& Config,
				 OUT KNdisMedium& Medium,
				 OUT PSTRING* /*AddressInfo*/)
{
	KNDIS_TRACE("KNdisFilterBinding::Open %ws\n", DeviceName->Buffer);

	// Cache the device name
	m_strDeviceName = DeviceName;

	// TODO: Customize the mediums (from the Registry?)
	static NDIS_MEDIUM SupportedMedium[] = { NdisMedium802_3, NdisMedium802_5, NdisMediumFddi, NdisMediumWan };

	// Set medium types in the output object:

	Medium = SupportedMedium;

	// Cache the device string for use in OpenComplete()
	// TODO: rearrange that more nicely...
	//
	if (Config.Read(KNDIS_STRING_CONST("UpperBindings"), &m_wsDriverInstance)
		!= NDIS_STATUS_SUCCESS)	{
			ASSERT(!"UpperBindings");
			return NDIS_STATUS_FAILURE;
	}

	// Create a LBFO bundle if one has been configured in the Registry.
	// This would create a bundle (if any) nor yet populated with adapters:

	m_pBundle = KNdisBundle::Register(NULL, Config);

	return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::OpenComplete
//
// The framework signals the binding to the adapter has been completed
// Now we ask NDIS to initialize our miniport part.
//
// TBD: Context passing from Proto to MP in DDK 4/NT 4. Use a global? Ugly...
//
void KNdisFilterBinding::OpenComplete()
{
	KNDIS_TRACE("KNdisFilterBinding::OpenComplete() %s, %s\n",
		(m_CompleteStatus==NDIS_STATUS_SUCCESS) ? "OK":"FAILED", KNdisMedium::Text(m_SelectedMedium));

	if (m_CompleteStatus==NDIS_STATUS_SUCCESS)
	{
		NdisIMInitializeDeviceInstanceEx(
			KNdisFilterAdapter::GetContainerHandle(),
  			&m_wsDriverInstance,
  			this // IMPORTANT: used by KNdisFilterAdapter on Initialize()
			 );
	}

	// TODO: return status!
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::Close
//
// The binding goes away. The last call before the object is destroyed
// by the framework. See comments in KNdisFilterAdapter::Halt() on sequence
// of events during binding/adapter removal.
//
BOOLEAN KNdisFilterBinding::Close()
{
	KNDIS_TRACE("KNdisFilterBinding::Close\n");

	// if the associated adapter still exists, i.e. its Halt() hasn't been called,
	// then our protocol is getting removed first. Thus, we have to force the removal
	// of the adapter by calling NdisIMDeInitializeDeviceInstance().

	if (m_pAdapter) {
		m_pAdapter->m_State = KNdisFilterAdapter::CLOSING;
		NdisIMDeInitializeDeviceInstance(*m_pAdapter);
		return FALSE; // do not destroy us, let miniport do
	}
	else {
		// Adapter has gone already, i.e. we probably got called from its Halt().
	}
	return TRUE; // delete me
}


///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::Receive
//
// Received Packet is being indicated by the NIC.
//
// Our filter allocates a fresh (repackaged) packet descriptorg and calls
// the client's OnReceive() filter. If the client want to pass this packet along
// up, it should do Repackaged.CloneUp(Original) and return success.
//
INT KNdisFilterBinding::Receive(const IN PNDIS_PACKET Packet)
{
	if (m_pAdapter == NULL)
		return 0;

	KNdisPacket Original(Packet);
	KNdisPacket Repackaged = m_pAdapter->m_RxPool.Allocate();

	// If out-of-packets: TBD; consider congestion handlers
	if (!Repackaged.IsValid())
		return 0;

	// Copy status
	NDIS_STATUS PacketStatus = Original.STATUS();
	Repackaged.STATUS(PacketStatus);

	// Save context
	reinterpret_cast<KNdisFilterAdapter::RxRESERVED*>(Repackaged.MiniportReserved())->OriginalPacket = Packet;
	reinterpret_cast<KNdisFilterAdapter::RxRESERVED*>(Repackaged.MiniportReserved())->OriginalAdapter = m_pAdapter;

	// Let the subclass take a look and CloneUp().

	NDIS_STATUS Status = m_pAdapter->OnReceive(Original, Repackaged);

	if (Status == NDIS_STATUS_SUCCESS) {

		m_pAdapter->ForwardUp(Repackaged);

		if (PacketStatus == NDIS_STATUS_RESOURCES) {
			KNdisPacket::Reinitialize(Repackaged);
			m_pAdapter->m_RxPool.Free(Repackaged);
		}
	}
	else if (Status != NDIS_STATUS_NOT_ACCEPTED)
			m_pAdapter->m_RxPool.Free(Repackaged);
		else {
			// subclass has forwarded the packet itself: have to wait for ReturnPacket unless it's done
			// @TODO: Check logic for delays...
			if (PacketStatus == NDIS_STATUS_RESOURCES) {
				KNdisPacket::Reinitialize(Repackaged);
				m_pAdapter->m_RxPool.Free(Repackaged);
			}

			Status = NDIS_STATUS_SUCCESS;
		}

	return  (Status == NDIS_STATUS_SUCCESS) ? 1:0;
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::Receive
//
// Packet header+lookahead is being indicated.
//
// First we let the client examine (and, potentially, in-place-modify) the
// packet's header and lookahead. If the client returns NDIS_STATUS_SUCCESS
// we forward the indication via an appropriate medium filter receive indication
// to the protocols.
// Eventually, the protocol(s) would call NdisTransferData providing a packet
// descriptor. Our KNdisFilterAdapter::TransferData() intercepts it and forwards
// to the real NIC.
//
// Parameters
//	PacketToAccept		Ptr to location where the client has to put a NDIS_PACKET ptr
//						to proceed with data transfer. NULL causes no further data transfer.
//  HeaderBuffer		Header ptr
//	HeaderBufferSize	Header size
//  LookaheadBuffer		Lookahead data ptr
//	LookaheadBufferSize Lookahead data size
//  PacketSize			Packet size
// Returns:
//	NDIS_STATUS_SUCCESS			-	Data is to be read into the supplied packet
//  NDIS_STATUS_NOT_ACCEPTED	-	Not interested in the packet
//
NDIS_STATUS KNdisFilterBinding::Receive(IN OUT KNdisPartialPacket& PacketToAccept,
						IN  PVOID HeaderBuffer,
						IN  OUT UINT HeaderBufferSize,
						IN	PVOID LookaheadBuffer,
						IN	OUT UINT LookaheadBufferSize,
						IN	UINT PacketSize)
{
	ASSERT(m_pAdapter);
	if (m_pAdapter == NULL)
		return NDIS_STATUS_NOT_ACCEPTED;

	KNDIS_TRACE("KNdisFilterBinding::Receive header %X, for packet len = %u\n", HeaderBuffer, HeaderBufferSize);

	// Let the client inspect the header

	NDIS_STATUS Status = m_pAdapter->OnReceive
		(PacketToAccept, HeaderBuffer, HeaderBufferSize, LookaheadBuffer, LookaheadBufferSize, PacketSize);

	// If the client's happy forward to the protocols. Note that must preserve the
	// MacReceiveContext of thereal NIC while doing the indication.

	if (Status == NDIS_STATUS_SUCCESS) {

		m_pAdapter->ForwardUp(PacketToAccept.MacReceiveContext(),
					(PCHAR) HeaderBuffer, HeaderBufferSize,
					LookaheadBuffer, LookaheadBufferSize,
					PacketSize,m_pAdapter);
	}
	else if (Status == NDIS_STATUS_NOT_ACCEPTED) {
		// The subclass took care of the indication. Simply simulate success w/o supplying
		// packet for the TransferData to the KNdisProtocolWrapper::Receive
		Status = NDIS_STATUS_SUCCESS;
	}

	return Status;
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::TransferDataComplete
//
// This handler is called whenever a data transfer is requested from
// the KNdisFilterAdapter::TransferData handler. Simply forward to
// the protocols
//
// Parameters:
//		Packet				Packet filled in with data (if success)
//		Status				Result of transfer
//		BytesTransferred	Bytes transfered.
//
void KNdisFilterBinding::TransferDataComplete(PNDIS_PACKET Packet,
									  NDIS_STATUS Status,
									  UINT BytesTransferred)
{
	KNDIS_TRACE("KNdisFilterBinding::TransferDataComplete into %X: %s, %u bytes\n",
				Packet, (Status)?"OK":"FAILED", BytesTransferred);

	if (NDIS_STATUS_SUCCESS == m_pAdapter->OnTransferComplete(Packet, Status, BytesTransferred))
		m_pAdapter->TransferDataComplete(Packet, Status, BytesTransferred);
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::ReceiveComplete
//
// ProtocolReceiveComplete completes postprocessing of one or more preceding
// receive indications from a NIC driver. Simply forward it to the protcols.
//
void KNdisFilterBinding::ReceiveComplete()
{
	KNDIS_TRACE("KNdisFilterBinding::ReceiveComplete\n");

	switch(m_SelectedMedium)
	{
	  case NdisMedium802_3:
	  case NdisMediumWan:
			NdisMEthIndicateReceiveComplete(*m_pAdapter);
			break;

	  case NdisMedium802_5:
			NdisMTrIndicateReceiveComplete(*m_pAdapter);
			break;

	  case NdisMediumFddi:
			NdisMFddiIndicateReceiveComplete(*m_pAdapter);
			break;

	  default:
			ASSERT(!"Implemented medium");
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::SendComplete
//
// Packet previusly submitted to the NIC is being reclaimed.
// The packet might be either (1) a repackaged one originally submitted by
// KNdisFilterAdapter::Send(), or (2) stand-alone packet submitted by
// the KNdisFilterAdapter-derived client.
//
// If the client uses stand-alone packets, they must be allocated from a pool
// other than m_TxPool (which is KNdisFilterAdapter-private, after all).
// If the reclaimed packet belongs to m_TxPool, we return it there and
// indicate NdisMSendComplete() for the associated original packet. Otherwise
// we call client's OnSendComplete() to deal with its privately allocated
// packets.
//
// LFBO: we have to "complete" the packet on the primary adapter even though
// it could've had submitted on the secondary one.
//
void KNdisFilterBinding::SendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status)
{
	// Restore adapter. For LFBO, the adapter might be different from m_pAdapter.
	KNdisFilterAdapter* Adapter =
		reinterpret_cast<KNdisFilterAdapter::TxRESERVED*> (Packet->ProtocolReserved)->OriginalAdapter;

	// make sure the packet has been submitted via ForwardDown()
	ASSERT(Adapter->IsValid());

	if (Adapter->m_TxPool.IsFrom(Packet)) { // our repackaged packet: complete
		PNDIS_PACKET Original =
			reinterpret_cast<KNdisFilterAdapter::TxRESERVED*>(Packet->ProtocolReserved)->OriginalPacket;
		KNdisExportStubs::CopySendCompletePerPacketInfo(Original, Packet);
		KNdisPacket::Reinitialize(Packet);
		Adapter->m_TxPool.Free(Packet);
		Adapter->SendComplete(Original, Status);	 // return to the upper-protocol
	}
	else {											 // private stand-alone packet
		Adapter->OnSendComplete(Packet, Status);	 // let the client deal with
	}
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::StatusIndication
//
// General Status indication: run through the client and forward over to the
// protocols. The client is given a chance to modify the indicated status and buffer
// or drop the indication alltogether.
//
// Emperically, during boot the miniport might indicate status before the
// filter's adapter has been created. Need to check for that.
//
void KNdisFilterBinding::StatusIndication(IN NDIS_STATUS GeneralStatus,
						  IN PVOID StatusBuffer,
						  IN UINT StatusBufferSize)
{
	KNDIS_TRACE("KNdisFilterBinding::StatusIndication\n");

	if (m_pAdapter) {
		NDIS_STATUS FilteredStatus = m_pAdapter->OnStatusIndication(GeneralStatus, &StatusBuffer, &StatusBufferSize);
		if (FilteredStatus != 0)
			m_pAdapter->IndicateStatus(FilteredStatus, StatusBuffer, StatusBufferSize);
	}
}

///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::StatusComplete
//
// A call to ProtocolStatusComplete indicates that the protocol can resume normal
// operations on the binding after taking into account the preceding status change(s)
// indicated to its Protocol(Co)Status function.
//
void KNdisFilterBinding::StatusComplete()
{
	KNDIS_TRACE("KNdisFilterBinding::StatusComplete\n");
	ASSERT(m_pAdapter);
	if (m_pAdapter == NULL)
		return;
	if (m_pAdapter->OnStatusIndicationComplete() == NDIS_STATUS_SUCCESS)
		m_pAdapter->IndicateStatusComplete();
}


///////////////////////////////////////////////////////////////////////////////
// KNdisFilterBinding::RequestComplete
//
// A pended request submitted to the NIC by KNdisFilterAdapter completes.
//
// We allow an IM issue stand-alone (non-intercepted) requests to the underlying NIC.
// We require, however, that such requests be synchronous, i.e. they use the built-in
// m_CompletionEvent. Class KNdisRequestSync provides for that.
//
void KNdisFilterBinding::RequestComplete(PNDIS_REQUEST NdisRequest, NDIS_STATUS Status)
{
	KNDIS_TRACE("KNdisFilterBinding::RequestComplete\n");
	ASSERT(m_pAdapter);
	if (m_pAdapter == NULL)
		return;

	if (&m_pAdapter->m_Request == NdisRequest) {
		// KNdisFilterAdapter-originated: complete it to the upper protocol
		KNdisFilterAdapter::Request& req = m_pAdapter->m_Request;

		switch (req.RequestType) {
		  case NdisRequestQueryInformation:	// run through the client first
		  		Status = m_pAdapter->OnQuery(req, Status);
				ASSERT(Status != NDIS_STATUS_PENDING);
				*req.BytesMoved = req.DATA.QUERY_INFORMATION.BytesWritten;
				*req.BytesNeeded = req.DATA.QUERY_INFORMATION.BytesNeeded;
				req.bPending = FALSE;
				m_pAdapter->QueryComplete(Status);
			  break;

		  case NdisRequestSetInformation: // simply complete it
				*req.BytesMoved = req.DATA.QUERY_INFORMATION.BytesWritten;
				*req.BytesNeeded = req.DATA.QUERY_INFORMATION.BytesNeeded;
				req.bPending = FALSE;
				m_pAdapter->SetComplete(Status);
			  break;

		  default:
			  ASSERT(!"RequestType");
		};
	}
	else {
		// Private req: release the waiting thread
		KNdisProtocolBinding::SignalCompletion(Status);
	}
}


//////////////////////////////////////////////////////////////////////////////////
// PnP Events.
//
#if KNDIS_PNP_AWARE
NDIS_STATUS KNdisFilterBinding::OnPnpSetPower(NDIS_DEVICE_POWER_STATE PowerState,PNET_PNP_EVENT pEvent)
{
	// Implemements the default power policy.
	// Delegate to KNdisFilterAdapter::OnPowerState

	if (m_pAdapter)
		m_pAdapter->OnPowerState(PowerState, FALSE, pEvent);

	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS KNdisFilterBinding::OnPnpReconfigure(PCWSTR wszProtocolSection, PNET_PNP_EVENT pEvent)
{
	// Delegate to the adapter. In LBFO case, it could check the Registry
	// for the updated bundle, etc.
	return (m_pAdapter) ? m_pAdapter->OnReconfiguration(wszProtocolSection, pEvent) : NDIS_STATUS_SUCCESS;
}

NDIS_STATUS KNdisFilterBinding::OnPnpQueryPower(NDIS_DEVICE_POWER_STATE /*PowerState*/,PNET_PNP_EVENT)
{
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS KNdisFilterBinding::OnPnpQueryRemoveDevice(PNET_PNP_EVENT)
{
	return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS KNdisFilterBinding::OnPnpCancelRemoveDevice(PNET_PNP_EVENT)
{
	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS KNdisFilterBinding::OnPnpCapabilityChange(ULONG /*uWakeUpCapFlag*/, PNET_PNP_EVENT)
{
	return NDIS_STATUS_SUCCESS;
}

/*static*/
NDIS_STATUS KNdisFilterBinding::OnPnpReconfigureGlobal(PCWSTR /*wszProtocolSection*/, PNET_PNP_EVENT)
{
	// This is an undoc, which supposedly re-binds stuff. TBD.
	NdisReEnumerateProtocolBindings(KNdisFilterBinding::GetContainerHandle());
	return NDIS_STATUS_SUCCESS;
}

/*static*/
NDIS_STATUS KNdisFilterBinding::OnPnpBindListChange(PCWSTR /*wszNewBindList*/, PNET_PNP_EVENT)
{
	return NDIS_STATUS_SUCCESS;
}

/*static*/
NDIS_STATUS KNdisFilterBinding::OnPnpBindListComplete(PNET_PNP_EVENT)
{
	return NDIS_STATUS_SUCCESS;
}
#endif // KNDIS_PNP_AWARE
