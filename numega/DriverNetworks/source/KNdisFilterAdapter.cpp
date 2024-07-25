// KNdisFilterAdapter.cpp: implementation of the KNdisFilterAdapter class.
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

// Constructor
KNdisFilterAdapter::KNdisFilterAdapter() :
	KNdisMiniAdapter(),
		m_pBinding(NULL), m_State(DOWN)
{
}

// Destructor
KNdisFilterAdapter::~KNdisFilterAdapter()
{
}

//////////////////////////////////////////////////////////////////////
// Miniport handlers
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::Initialize
//
// This handler is called as a result of NdisIMInitializeDeviceInstance
// called by KNdisFilterBinding on successful binding creation. The job
// of this handler is to:
//
// 1) Retrieve and store the associated binding instance;
// 2) Set the medium index to what the binding has been assigned to;
// 3) Initialize the packet pools, etc. internal objects
// 4) Set miniport attributes
// 5) TBD: Maintaining a global adapter list? Or KNdisFilterBinding should do that?
//
// Parameters:
//		Medium
//			Ref to KNdisMedium object that we have to set
//		Config
//			Ref KNdisConfig object that we use to query various cfg params
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
NDIS_STATUS KNdisFilterAdapter::Initialize(IN OUT KNdisMedium& Medium,
										   IN KNdisConfig& Config)
{
	m_State = INITIALIZING;

	// Get the associated binding object:
#if (KNDIS_DDK_VERSION >= 5)
	m_pBinding = reinterpret_cast<KNdisFilterBinding*> ( NdisIMGetDeviceContext(m_Handle) );
#else
#pragma message("WARNING: The IM NDIS Framework/Drivers should be built using Win2000 DDK.")
#endif
	if (!m_pBinding)
		KNDIS_RETURN_ERROR (NDIS_STATUS_FAILURE);

	// Set the medium index to whatever the binding was set to
	// special case is NdisMediumWan where the medium index is set to NdisMedium802_3
	if (!Medium.Select((m_pBinding->m_SelectedMedium == NdisMediumWan)?NdisMedium802_3:m_pBinding->m_SelectedMedium))
		KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);

	// Retrieve some initialization parameters from the Registry. Use defaults if none.
	UINT MaxPoolSize = Config.Read(KNDIS_STRING_CONST("MaxPoolSize"), 1024);
	UINT MinPoolSize = Config.Read(KNDIS_STRING_CONST("MinPoolSize"), 255);

	// Initialize internal objects:
	m_TxPool.InitializeEx(MinPoolSize, MaxPoolSize-MinPoolSize, sizeof(TxRESERVED));
	m_RxPool.InitializeEx(MinPoolSize, MaxPoolSize-MinPoolSize);
	if (!m_TxPool.IsValid() || !m_RxPool.IsValid())
		KNDIS_RETURN_ERROR (NDIS_STATUS_RESOURCES);

	//
	// Setting attributes. Overridable from the Registry, but better don't mess around with.
	//
	// This is a quote from DDK/w2k's passthru:
	//
	// "Set the attributes now. The NDIS_ATTRIBUTE_DESERIALIZE is the key. This enables us
	// to make up-calls to NDIS w/o having to call NdisIMSwitchToMiniport/NdisIMQueueCallBack.
	// This also forces us to protect our data using spinlocks where appropriate. Also in this
	// case NDIS does not queue packets on out behalf"
	//
	// The built-in m_Lock can be used for that purpose.
	//

	ULONG AttrFlags = Config.Read(KNDIS_STRING_CONST("AttrFlags"),
								NDIS_ATTRIBUTE_IGNORE_PACKET_TIMEOUT  |
								NDIS_ATTRIBUTE_IGNORE_REQUEST_TIMEOUT |
								NDIS_ATTRIBUTE_INTERMEDIATE_DRIVER
#if KNDIS_DDK_VERSION >= 5
								| NDIS_ATTRIBUTE_DESERIALIZE |
								NDIS_ATTRIBUTE_NO_HALT_ON_SUSPEND
#endif
								);

	SetAttributesEx(NdisInterfaceInternal, AttrFlags);

	// Now, set a member in the friendly KNdisFilterBinding to point to us.
	// This forges the two-way association between the binding and the virtual adapter:
	m_pBinding->m_pAdapter = this;

	// Finally, delegate to the derived class. It has the last chance to fail the entire thing:

	NDIS_STATUS Status = OnInitialize(Medium, Config);
	if (Status == NDIS_STATUS_SUCCESS)
		m_State = UP;
	return Status;
}

////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::Halt
//
// NDIS removes our virtual NIC for whatever reason.
// We should be careful here -- things could get screwey.
//
// The Halt() could be called as a result of NDIS removing either (1) the
// virtual NIC, or (2) our protocol binding. For (2), the Halt() would be a
// result of the explicit NdisIMDeInitializeDeviceInstance called from
// KNdisFilterBinding::Unbind(). For (1), the miniport should take care of
// destroying the binding since it lost its "head" in the form of the virtual NIC.
// Incidentally, KNdisFilterBinding::Unbind() faces a similar problem since the
// KNdisFilterBinding can't be sure whether the associated virtual NIC is still alive.
//
// The friendly couple [KNdisFilterAdapter, KNdisFilterBinding] interacts via the back
// pointers m_pBinding, m_pAdapter set up during initialization.
//
//
VOID KNdisFilterAdapter::Halt(VOID)
{
	// Call the client first
	OnHalt();

	// TBD: Should we care for submitted but not returned packets here?

	if (m_pBinding) {
		m_pBinding->CloseAdapter();		// do it always!
		if (m_State == CLOSING) {		// called due to NdisIMDeInitializeDeviceInstance
										// from KNdisFilterBinding::Close()
		}
		else {							// called by NDIS directly: simulate protocol framework
			m_pBinding->m_pAdapter = NULL; // tell we're dead
			m_pBinding->Close();
		}
		delete m_pBinding;
		m_pBinding = NULL;
	}

	m_State = DOWN;
}

//////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::Reset
//
NDIS_STATUS KNdisFilterAdapter::Reset(OUT PBOOLEAN AddressingReset)
{
	*AddressingReset = FALSE;
	return OnReset();
}

//////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::Send
//
// A bound protocol sends a packet over our filter. We run the packet
// through the client's OnSend() providing a referecne to the original
// packet and a fresh packet descriptor for repackaging. The client might
// copy the original into the repackaged by Repackaged.CloneDown(Original)
// or submit an entire different packet.
//
// In any case, if OnSend() return NDIS_STATUS_SUCCESS the repackaged is
// forewarded down over the real binding.
//
// Parameters:
//		Packet		-		Original packet to be sent
//		Flags		-		Packet flags
//	Returns:
//		NDIS_STATUS_SUCCESS	-	Packet is submitted and done with
//		NDIS_STATUS_PENDING	-	Packet is in flight now
//		NDIS_STATUS_RESOURCES -	Ran out of Tx descriptors
//
NDIS_STATUS KNdisFilterAdapter::Send(IN	PNDIS_PACKET Packet, IN	UINT Flags)
{
	KNdisPacket Original(Packet);
	KNdisPacket Repackaged = m_TxPool.Allocate();

	// If out-of-packets: TBD; consider congestion handlers
	if (!Repackaged.IsValid())
		return NDIS_STATUS_RESOURCES;

	// Flag to ignore loopbacked
#if KNDIS_DDK_VERSION >= 5
	Flags |= NDIS_FLAGS_DONT_LOOPBACK;
#endif
	Repackaged.SetFlags(Flags);

	// remember the original packet and adapter as it might get pended:
	reinterpret_cast<TxRESERVED*> (Repackaged.ProtocolReserved())->OriginalPacket = Packet;
	reinterpret_cast<TxRESERVED*> (Repackaged.ProtocolReserved())->OriginalAdapter = this;

	// Call the subclass
	NDIS_STATUS Status = OnSend(Original, Repackaged);

	if (Status == NDIS_STATUS_SUCCESS) {
		// forward down the binding
		if (Repackaged.QueryBufferCount() > 0)
			Status = ForwardDown(Repackaged);
	}
	else {
		// the filter "consumed" or forwarded the packet over different binding
		if (Status == NDIS_STATUS_NOT_ACCEPTED) {
			Status = NDIS_STATUS_SUCCESS;	// ok, done
		}
	}

	// If done release resources:

	if (Status != NDIS_STATUS_PENDING) {
		KNdisExportStubs::CopySendCompletePerPacketInfo(Packet, Repackaged);
		KNdisPacket::Reinitialize(Repackaged);
		m_TxPool.Free(Repackaged);
	}

	// If pended then we'll recycle the packet in KNdisFilterBinding::SendComplete()

	return Status;
}

//////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::SendPackets
//
// For now: simply split into single packets.
// TBD: Passing arrays of KNdisPacket[]
//
// NOTE: Either Send() or SendPackets() is used by NDIS. By default,
//       Characteristics.h maps it to Send(). The SendPackets() implementation
//		 below doesn't buy much, anyway.
//
VOID KNdisFilterAdapter::SendPackets(IN	PPNDIS_PACKET PacketArray, IN UINT NumberOfPackets)
{
	for (UINT i=0; i<NumberOfPackets; i++)
		Send(PacketArray[i], 0);
}

//////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::ReturnPacket
//
// NDIS returns a packet previusly submitted by ReceiveIndicate().
// If the packet is allocated from RxPool, put it back there. Otherwise
// let the client deal with the packet.
//
// The assumption is that the client might submit non-NIC-originated
// packets to the bound protocol. These packets need to be dealt with
// by the client.
//
// Parameters:
//		Packet	-		Packet (repackaged)
// Returns:
//		None
VOID KNdisFilterAdapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	// restore originating adapter
	KNdisFilterAdapter* Adapter =
		reinterpret_cast<KNdisFilterAdapter::RxRESERVED*> (Packet->MiniportReserved)->OriginalAdapter;
	ASSERT(Adapter->IsValid());

	if (Adapter->m_RxPool.IsFrom(Packet)) { // original packet: return it to the NIC
		PNDIS_PACKET Original = reinterpret_cast<RxRESERVED*>(Packet->MiniportReserved)->OriginalPacket;
		KNdisPacket::Reinitialize(Packet);
		Adapter->m_RxPool.Free(Packet);
		Adapter->m_pBinding->ReturnPacket(Original);
	}
	else {								 // private stand-alone packet
		OnReturnPacket(Packet);			 // let the client deal with
	}
}

//////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::TransferData
//
// NDIS requests to transfer data previously submitted by a partial
// receive ("filtered") indication. We simply forward it to the
// real NIC over the binding.
//
NDIS_STATUS KNdisFilterAdapter::TransferData(
			OUT PNDIS_PACKET			Packet,
			OUT PUINT					BytesTransferred,
			IN  PVOID					FrameLocation, /* MiniportReceiveContext */
			IN	UINT					ByteOffset,
			IN	UINT					BytesToTransfer
			)
{
	return m_pBinding->TransferData(
						FrameLocation,
						ByteOffset,
						BytesToTransfer,
						Packet,
						BytesTransferred);
}

//////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::QueryInformation
//
// "Query" is treated pretty much as receive, i.e. we aquire
// data first from the underlying NIC and then run them through
// the client ("filter").
//
// If the request is pended on the NIC, we complete
// it from KNdisFilterBinding::RequestComplete.
//
// NOTE: PNP is handled by the base class right now. Consider using
// ===== traits to modify this behaviour.
//
NDIS_STATUS KNdisFilterAdapter::QueryInformation(
			IN	NDIS_OID				Oid,
			IN	PVOID					InformationBuffer,
			IN	ULONG					InformationBufferLength,
			OUT PULONG					BytesWritten,
			OUT PULONG					BytesNeeded
			)
{
	ASSERT(!m_Request.bPending);	// our assumption on NDIS serialization

#if KNDIS_PNP_AWARE
	// The DDK states:
	// "An intermediate driver must always return NDIS_STATUS_SUCCESS to a query of
	// OID_PNP_QUERY_POWER. An intermediate driver should never propagate an OID_PNP_QUERY_POWER
	// request to an underlying miniport". Amen.
	//
	if (Oid == OID_PNP_QUERY_POWER)	{
		*BytesWritten = *BytesNeeded = 0;
		return NDIS_STATUS_SUCCESS;
	}
#endif

	// Repackage & submit using built-in request object.
	// Note that we preserve original OUT pointers that NDIS is supposed to page-lock
	// until the request is completed.

	m_Request.bPending = TRUE;
	m_Request.BytesNeeded = BytesNeeded;
	m_Request.BytesMoved = BytesWritten;
	NDIS_STATUS Status = m_Request.Query
							(*m_pBinding, Oid, InformationBuffer, InformationBufferLength);

	if (Status != NDIS_STATUS_PENDING) {

		m_Request.bPending = FALSE;

#if KNDIS_PNP_AWARE
		// The DDK states:
		// "If the underlying NIC is PM-aware, the intermediate driver should return NDIS_STATUS_SUCCESS
		// to a query of OID_PNP_CAPABILITIES. In the NDIS_PM_WAKE_UP_CAPABILITIES structure returned by
		// this OID, the intermediate driver should specify a device power state of NdisDeviceStateUnspecified
		// for each wake-up capability (MinMagicPacketWakeUp, MinPatternWakeUp, MinLinkChangeWakeUp).
		// Such a response indicates that the intermediate driver is PM-aware but does not manage a physical device.
		// If the underlying NIC is not PM-aware, the intermediate driver should return NDIS_STATUS_NOT_SUPPORTED
		// to a query of OID_PNP_CAPABILITIES". Amen.
		if (Oid  == OID_PNP_CAPABILITIES  && Status == NDIS_STATUS_SUCCESS)
			Status = ManglePnpCapabilities((PNDIS_PNP_CAPABILITIES)InformationBuffer);
		else
#endif // KNDIS_PNP_AWARE
		{
			// Run through the client.
			// Note that the request object points to the data returned by the NIC miniport.
			// Potentially, the client might modify whatever is returned to the requesting protocol.

			Status = OnQuery(m_Request, Status);
			ASSERT(Status != NDIS_STATUS_PENDING);
			*BytesWritten = m_Request.DATA.QUERY_INFORMATION.BytesWritten;
			*BytesNeeded = m_Request.DATA.QUERY_INFORMATION.BytesNeeded;
		}
	}
	else {
		// The underlying miniport pended the request.
	}

	return Status;
}

//////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::SetInformation
//
// "Set" is treated pretty much as send-s, i.e. we run the request
// first through the client ("filter"), and then, perhaps, pass it
// down to the underlying NIC.
//
// If the request is pended on the NIC, we complete
// it from KNdisFilterBinding::RequestComplete.
//
NDIS_STATUS KNdisFilterAdapter::SetInformation(
			IN	NDIS_OID				Oid,
			IN	PVOID					InformationBuffer,
			IN	ULONG					InformationBufferLength,
			OUT PULONG					BytesRead,
			OUT PULONG					BytesNeeded
			)
{
	ASSERT(!m_Request.bPending);	// our assumption on NDIS serialization

#if KNDIS_PNP_AWARE
	// The DDK states:
	// "An intermediate driver must always return NDIS_STATUS_SUCCESS to a query of
	// OID_PNP_SET_POWER. An intermediate driver should never propagate an OID_PNP_SET_POWER
	// request to an underlying miniport". Amen.

	if (Oid == OID_PNP_SET_POWER)	{
		if (InformationBufferLength < sizeof(NDIS_DEVICE_POWER_STATE)) {
			*BytesRead=0; *BytesNeeded=sizeof(NDIS_DEVICE_POWER_STATE);
			return NDIS_STATUS_INVALID_LENGTH;
		}
		OnPowerState(*(PNDIS_DEVICE_POWER_STATE)InformationBuffer, TRUE, NULL);
		*BytesRead = sizeof(NDIS_DEVICE_POWER_STATE); *BytesNeeded=0;
		return NDIS_STATUS_SUCCESS;
	}
#endif

	// Repackage using built-in request object:

	m_Request.RequestType = NdisRequestSetInformation;
	m_Request.DATA.SET_INFORMATION.Oid = Oid;
	m_Request.DATA.SET_INFORMATION.InformationBuffer = InformationBuffer;
	m_Request.DATA.SET_INFORMATION.InformationBufferLength = InformationBufferLength;

	// Run through the client first. Note a "special" status code
	// meaning that the OID has been taken care of by the client

	NDIS_STATUS Status = OnSet(m_Request);
	if (Status == NDIS_STATUS_NOT_ACCEPTED) {	// handled internally, happily exit away
			*BytesRead = m_Request.DATA.SET_INFORMATION.BytesRead;
			*BytesNeeded = m_Request.DATA.SET_INFORMATION.BytesNeeded;
			return NDIS_STATUS_SUCCESS;
	}

	if (Status == NDIS_STATUS_SUCCESS) {

		m_Request.BytesNeeded = BytesNeeded;
		m_Request.BytesMoved = BytesRead;
		m_Request.bPending = TRUE;

		Status = m_Request.Post(*m_pBinding);
		if (Status != NDIS_STATUS_PENDING) {
			m_Request.bPending = FALSE;
			*BytesRead = m_Request.DATA.SET_INFORMATION.BytesRead;
			*BytesNeeded = m_Request.DATA.SET_INFORMATION.BytesNeeded;
		}
		else {
			// The underlying miniport pended the request
		}
	}

	return Status;
}

///////////////////////////////////////////////////////////////////////
// Forward packet down the binding
//
// Parameters:
// 	Packet               - packet to forward
//	Originating			 - originating adapter (LBFO cases)
// Returns:
// 	Status
// Comments:
// 	This function forwards the repackaged protocol's packet descriptor to the
//	NIC the filter adapter is bound to.
 NDIS_STATUS KNdisFilterAdapter::ForwardDown(PNDIS_PACKET Packet, KNdisFilterAdapter* Originating)
{
    // save the adapter we're using
	reinterpret_cast<TxRESERVED*> (Packet->ProtocolReserved)->OriginalAdapter =
		(Originating) ? Originating : this;
	return m_pBinding->Send(Packet);
}

///////////////////////////////////////////////////////////////////////
// Indicate packet to the protocol(s)
//
// Parameters:
// 	Packet               - packet to forward
//	Originating			 - originating adapter (LBFO cases)
// Returns:
// 	none
// Comments:
// 	This function indicates the repackaged NIC adapter's packet descriptor
//	to the protocols bound to the filter adapter.
void KNdisFilterAdapter::ForwardUp(PNDIS_PACKET Packet, KNdisFilterAdapter* Originating)
{
    // save the adapter we're using
	reinterpret_cast<RxRESERVED*> (Packet->MiniportReserved)->OriginalAdapter =
		(Originating) ? Originating : this;
	KNdisMiniAdapter::IndicateReceive(Packet);
}


///////////////////////////////////////////////////////////////////////
// Indicate lookahead to the protocol(s)
//
// Parameters:
// 	OriginatingReceiveContext -
// 	HeaderBuffer         -
// 	HeaderBufferSize     -
// 	LookAheadBuffer      -
// 	LookaheadBufferSize  -
// 	PacketSize           -
// 	Originating          -
// Returns:
// 	none
// Comments:
// 	none
void KNdisFilterAdapter::ForwardUp(NDIS_HANDLE OriginatingReceiveContext,
					 IN PVOID HeaderBuffer, IN UINT HeaderBufferSize,
					 IN PVOID LookaheadBuffer, IN UINT LookaheadBufferSize,
					 IN UINT PacketSize,
					 KNdisFilterAdapter* Originating)
{
		if(Originating == NULL)
			Originating = this;

		switch (m_pBinding->m_SelectedMedium) {
		case NdisMedium802_3:
		case NdisMediumWan:
			NdisMEthIndicateReceive(*Originating, OriginatingReceiveContext,
					(PCHAR) HeaderBuffer, HeaderBufferSize,
					LookaheadBuffer, LookaheadBufferSize,
					PacketSize);
			break;

		case NdisMedium802_5:
			NdisMTrIndicateReceive(*Originating, OriginatingReceiveContext,
					(PCHAR) HeaderBuffer, HeaderBufferSize,
					LookaheadBuffer, LookaheadBufferSize,
					PacketSize);
			break;

	    case NdisMediumFddi:
			// Work around C++ incompblty...
			// NdisMFddiIndicateReceive(*pAdapter, context,
			//   Header, HeaderSize, Data, DataSize, PacketSize);
			#if KNDIS_DDK_VERSION >= 5
				(*((PNDIS_MINIPORT_BLOCK)(*Originating))->FddiRxIndicateHandler)(	// NDIS 5 way
			#else
				FddiFilterDprIndicateReceive(									// NDIS 4 way
			#endif
				((PNDIS_MINIPORT_BLOCK)(*Originating))->FddiDB,								\
				OriginatingReceiveContext,
				PCHAR(HeaderBuffer) + 1,
				((((PUCHAR)HeaderBuffer)[0] & 0x40) ? FDDI_LENGTH_OF_LONG_ADDRESS
				: FDDI_LENGTH_OF_SHORT_ADDRESS),
				HeaderBuffer,
				HeaderBufferSize,
				LookaheadBuffer, LookaheadBufferSize,
				PacketSize);
			break;

		default:
			ASSERT(!"Implemented medium");
		} // switch
}

#if KNDIS_PNP_AWARE


///////////////////////////////////////////////////////////////////////////
// KNdisFilterAdapter::ManglePnpCapabilities()
//
// Internal method that modifies PnP caps to what DDK says NDIS wants
// to see; uses current m_Request
//
NDIS_STATUS KNdisFilterAdapter::ManglePnpCapabilities(PNDIS_PNP_CAPABILITIES pCaps)
{
	PNDIS_PM_WAKE_UP_CAPABILITIES pWakeupCaps = &pCaps->WakeUpCapabilities;

	pWakeupCaps->MinMagicPacketWakeUp = NdisDeviceStateUnspecified;
	pWakeupCaps->MinPatternWakeUp = NdisDeviceStateUnspecified;
	pWakeupCaps->MinLinkChangeWakeUp = NdisDeviceStateUnspecified;

	return NDIS_STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// Manage Power State (handler)
//
// Parameters:
// 	NewState                - New Power State value
// 	bRequestNotNotification - The reason for the handler invokation.
//                            TRUE specifies that the handler is called as a result of OID_PNP_SET_POWER
//                            request issued to the virtual NIC representing the filter adapter, and NewState
//                            is the new power state the system requests to set for the virtual NIC.
//                            FALSE specifies that the handler is called as a result of a power event
//                            on the real NIC the filter is bound to.
//  pEvent				 - Original PNP event
// Returns:
// 	none
// Comments:
// 	The framework calls OnPowerState() handler to manage filter's power transitions.
//	The KNdisFilterAdapter::OnPowerState implementation provides a default scheme of the
//	power management (described below). The user does not have to override OnPowerState unless
//	he/she wants to provide a custom power management scheme.
//
// The default power management scheme of the IM driver maintains consistency
// between the "virtual" power state exposed by the IM and the "real" power
// state of the underlying NIC.
//
// NDIS broadcasts power events to all miniports, including the virtual NIC represented by a KNdisFilterAdapter
// and the real NIC miniport it's bound to. To provide the mentioned above consistency
// we have to synchronize KNdisFilterAdapter's power transitions with the ones of
// the real NIC. The real NIC's power transitions get interecepted by the
// KNdisFilterBinding::OnPnpXXX() handlers.
//
// The built-in m_Power object stores the current power state. It also acts as a synchronization
// object (event). The default implementation treats the power transitions as follows.
//
// ON->OFF TRANSITION
//
// Whenever NDIS notifies KNdisFilterAdapter to go "standby", we simply obey and reset the
// m_Power to the given lower-power state. The associated KNdisFilterBinding, in turn, gets
// notified on the real NIC transition-to-be to the stand-by. At this point, we must ensure
// all the packets and requests posted to the NIC are completed, so we wait for it.
//
// OFF->ON TRANSITION
//
// Whenever our KNdisFilterAdapter is commanded to go ON, we must first ensure that the real
// NIC is on, too. So, we wait on m_Power transition. Eventually, the real NIC goes ON, which
// is signaled into our KNdisFilterBinding. The KNdisFilterBinding handler signals the new
// state on m_Power, which releases the waiting miniport thread.
void KNdisFilterAdapter::OnPowerState(NDIS_DEVICE_POWER_STATE NewState,
									  BOOLEAN bRequestNotNotification, PNET_PNP_EVENT/*pEvent*/)
{
	TRACE("KNdisFilterAdapter::OnPowerState(): %s %u => %u\n",
		(bRequestNotNotification)?"OID":"NetEvent", NDIS_DEVICE_POWER_STATE(m_Power), NewState);

	if (bRequestNotNotification) {

		// Processing OID_SET_POWER (NDIS called on our virtual NIC)

		if (m_Power.IsOn() && NewState > NdisDeviceStateD0) {	// ON->OFF
			m_Power.Set(NewState);
		}
		else if (m_Power.IsOff() && NewState == NdisDeviceStateD0) { // OFF->ON
			m_Power.Wait();
		}
		else {	// low-to-low transitions: don't care, simply update the value
			m_Power = NewState;
		}
	} else {

		// Processing NetEvent (NDIS notified on our binding to real NIC)

		// ON->OFF: wait until clear, so the NIC can go down
		if (NewState > NdisDeviceStateD0) {
			while (m_TxPool.Usage() || m_Request.bPending)
				Sleep(2);
			return;
		}

		// OFF->ON: signal the new state
		if (NewState == NdisDeviceStateD0 && m_Power.IsOff())
			m_Power.Set(NewState);
		return;
	}
}


///////////////////////////////////////////////////////////////////////
// Process reconfiguration notification (handler)
//
// Parameters:
// 	Section              - Protocol section relevant to the change
//  pEvent				 - Original PNP event
// Returns:
// 	none
// Comments:
// 	The framework calls this method when a network configuration change has been detected
//	for the underlying real NIC. For example, if a user, through the Network and Dial-up Connections
//	folder, changes the filter driver's setting, NDIS indicates NetEventReconfigure event
//	to the protocol portion of the driver. The control is then passed on to OnReconfiguration.
//	The default implementation merely returns NDIS_STATUS_SUCCESS.
NDIS_STATUS KNdisFilterAdapter::OnReconfiguration(PCWSTR Section, PNET_PNP_EVENT /*pEvent*/)
 {
	UNREFERENCED_PARAMETER(Section);
	return NDIS_STATUS_SUCCESS;
 }

#endif // KNDIS_PNP_AWARE

///////////////////////////////////////////////////////////////////////////
// Default OnXXX() hanlders - stubs
///////////////////////////////////////////////////////////////////////////

// Upper-edge: delegated from KNdisWrapper<Adapter>


///////////////////////////////////////////////////////////////////////
// Initialize filter (handler)
//
// Parameters:
// 	Medium               - Reference to the initialized KNdisMedium object which has a medium type
//	                       selected for the underlying binding by the real NIC
// 	Config               - Reference to miniport's KNdisConfig object used to query various
//                         configuration parameters
// Returns:
// 	NDIS_STATUS_SUCCESS to allow the new binding; an error code to reject it.
// Comments:
//	The framework calls this method when a new virtual NIC represented
//	by the KNdisFilterAdapter-derived class is created. At this time the binding to the
//  underlying real NIC is completed already. Most of the glue work has
//  been done by the framework, so this method could just return
//  NDIS_STATUS_SUCCESS to grant the creation of the filter. Or, it might
//  inspect the underlying medium type (Medium) and/or set-up
//  some private config parameters from the Registry (Config) before
//  returning.
//	The default implementation merely returns NDIS_STATUS_SUCCESS.
NDIS_STATUS KNdisFilterAdapter::OnInitialize(const KNdisMedium& Medium, KNdisConfig& Config)
{ UNREFERENCED_PARAMETERS2(Medium, Config); return NDIS_STATUS_SUCCESS; }

///////////////////////////////////////////////////////////////////////
// Halt filter (handler)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Framework calls this handler when either virtual NIC or underlying
//	binding is being destroyed by NDIS. This is the last call on the KNdisFilterAdapter-derived
//	object. The driver should do all the necessary deinitialization here. Alternatevely,
//	the driver can defer the deinitialization to the destructor.
//	The default implementation does nothing.
VOID KNdisFilterAdapter::OnHalt() {}

///////////////////////////////////////////////////////////////////////
// Process Outgoing Packet (handler)
//
// Parameters:
//	Original		-	Original packet descriptor supplied by NDIS to the virtual NIC
//	Repackaged		-	Framework-provided descriptor, which will be forwarded down onto the real NIC
// Returns:
//  The return code controls the behaviour of the framework after return from the OnSend() handler
//	NDIS_STATUS_SUCCESS
//		Requests the framework to forward the repackaged packet down the binding
//	NDIS_STATUS_NOT_ACCEPTED
//		Indicates that the packet has been consumed internally or successfully forwared over
//		some other binding. Release resources and Return NDIS_STATUS_SUCCESS to NDIS.
//	NDIS_STATUS_PENDING
//		Packet is pended for transmission over another binding. Do nothing, Recycle nothing.
//		Return NDIS_STATUS_PENDING to NDIS.
//	NDIS_STATUS_XXX
//		An error ocurred. Release resources and retutn this error status to NDIS.
// Comments:
// The framework calls OnSend whenever a NdisSend() is issued on the virtual NIC by a protocol.
// Here's where the filtering in the outgoing direction takes place. The filter is given a chance
// to examine the original packet submitted by a protocol (Original) and copy or modify or just
// drop it. The framework supplies a fresh packet descriptor (Repackaged) good for passing one packet down
// the binding to the real NIC. A filter that needs more packets (e.g. it implements
// some sort of repacketization) has to maintiain an internal packet pool for such purposes.
// NOTE the Repackaged initiallly describes a packet with no chained buffers.
// It is OK to return NDIS_STATUS_SUCCESS and leave the Repackaged empty.
// This would mean the packet has been successfully "consumed" (dropped) by the filter.
// In order to pass the original data down to the real NIC, the driver has to map the Repackaged
// packet descriptor to the original buffer chain as
//		Repackaged.CloneDown(Original);
// and return NDIS_STATUS_SUCCESS.
// The default implementation simply forwards the packet down to the real NIC.
NDIS_STATUS KNdisFilterAdapter::OnSend
	(const KNdisPacket& Original, KNdisPacket& Repackaged)
{
	Repackaged.CloneDown(Original);
	return NDIS_STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// Process Indicated Received Packet (handler)
//
// Parameters:
//	Original		-	Original packet descriptor from the underlying NIC
//	Repackaged		-	Framework-provided descriptor, which will be forwardred up to the protocols
// Returns:
// 	NDIS_STATUS_SUCCESS to forward the Repackaged up to the protocols; error code to drop it
// Comments:
// The framework calls OnReceive whenever the underlying miniport NIC indicates a received packet
// using NdisMIndicateReceive. Here's where the filtering in the incoming direction takes place.
// The filter is given a chance to examine the original packet submitted by the NIC miniport
// (Original) and copy or modify or just drop it. The framework supplies a fresh packet
// descriptor (Repackaged) good for passing one packet up to the protocols. A filter that needs
// more packets (e.g. it implements some sort of repacketization) has to maintiain an internal
// packet pool for such purposes.
// NOTE the Repackaged initiallly describes a packet with no chained buffers.
// It is OK to return NDIS_STATUS_SUCCESS and leave the Repackaged empty.
// This would mean the packet has been successfully "consumed" (dropped) by the filter.
// In order to pass the original data up to the protocols, the driver has to map the Repackaged
// packet descriptor to the original buffer chain as
//		Repackaged.CloneUp(Original);
// and return NDIS_STATUS_SUCCESS.
// The default implementation simply forwards the packet up to the protocol(s).
NDIS_STATUS KNdisFilterAdapter::OnReceive
	(const KNdisPacket& Original, KNdisPacket& Repackaged)
{
	Repackaged.CloneUp(Original);
	return NDIS_STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Process reset request (handler)
//
// Parameters:
// 	none
// Returns:
// 	NDIS_STATUS
// Comments:
// 	The framework calls this method when a reset request is issued by a
//  bound protocol to the filter's virtual NIC.
//	The default implementation merely returns NDIS_STATUS_SUCCESS.
NDIS_STATUS KNdisFilterAdapter::OnReset() { return NDIS_STATUS_SUCCESS; }

///////////////////////////////////////////////////////////////////////
// Process packet send completion (handler)
//
// Parameters:
// 	Packet               - packet completed
// 	Status               - status of completion
// Returns:
// 	none
// Comments:
// 	Filter adapters can not only intercept and forward down a higher-layer protocol's
//	packets, but also submit theirown ("private") packets for transmission over
//	the underlying binding (using ForwardDown). The framework takes care of recycling
//	only the intercepted ("real") packets. The privately submitted packets are allocated
//	from private packet pools maintained by the filter adapter. When a private packet
//	submitted for transmission completes (by the real NIC's miniport), the framework
//	calls OnSendComplete() so that the driver could recycle the private packet
//	appropriately. Failure to override OnSendComplete, thus, might result in the
//	packet descriptor leak.
//	The default implementation does nothing.
VOID KNdisFilterAdapter::OnSendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status)
{
	ASSERT(!"Overriden");				// The derived class MUST implement OnSendComplete
	UNREFERENCED_PARAMETERS2(Packet, Status);
}

///////////////////////////////////////////////////////////////////////
// Process return of indicated received packet (handler)
//
// Parameters:
// 	Packet               - packet returned
// Returns:
// 	none
// Comments:
// 	Filter adapters can not only intercept and forward up incoming packets indicated by the real NIC
//	it's bound to, but also indicate theirown ("private") packets to the bound protocol (using ForwardUp).
//	The framework takes care of recycling only the intercepted ("real") packets. The privately indicated
//	packets are allocated from private packet pools maintained by the filter adapter. When a private packet
//	indicated to the bound protocol is retained by the protocol, the protocol later calls NdisReturnPackets
//	to return such packet back to the filter driver. At that time the framework
//	calls OnReturnPacket() so that the driver could recycle the private packet
//	appropriately. Failure to override OnReturnPacket, thus, might result in the
//	packet descriptor leak.
//	The default implementation does nothing.
VOID KNdisFilterAdapter::OnReturnPacket(IN PNDIS_PACKET Packet)
{
	ASSERT(!"Overriden");				// The derived class MUST implement OnReturnPacket
	UNREFERENCED_PARAMETER(Packet);
}


///////////////////////////////////////////////////////////////////////
// Process OID query request (handler)
//
// Parameters:
// 	Req                  - Bound protocol-issued original OID query request packaged as KNdisRequest
// 	ReturnedStatus       - Status returned by the underlying real NIC on this OID
// Returns:
// 	NDIS_STATUS
// Comments:
//	The NDIS filter architecture requires all the OID requests be transparently forwarded by the
//	filter to the real NIC. The frameworks calls OnQuery() after the original OID request (issued by a protocol
//	using NdisRequest) is submitted to and processed by the underlying miniport NIC.
//	Thus, OnQuery() is the way for the filter adapter to "spy" on the OID queries issued by protocols
//	to the miniports. The driver might "override" the results of the query if needed.
//	The default implementation merely returns ReturnedStatus.
NDIS_STATUS KNdisFilterAdapter::OnQuery(KNdisRequest& Req, NDIS_STATUS ReturnedStatus)
 { UNREFERENCED_PARAMETER(Req); return ReturnedStatus; }

///////////////////////////////////////////////////////////////////////
// Process OID set request (handler)
//
// Parameters:
// 	Req                  - Bound protocol-issued original OID set request packaged as KNdisRequest
// Returns:
// 	NDIS_STATUS
// Comments:
//	The NDIS filter architecture requires all the OID requests be transparently forwarded by the
//	filter to the real NIC. The frameworks calls OnSet() before the original OID request (issued by a protocol
//	using NdisRequest) is submitted to the underlying miniport NIC.
//	Thus, OnSet() is the way for the filter adapter to "spy" on the OID set issued by protocols
//	to the miniports. The driver might "override" the parameters of the requets if needed.
//	The default implementation merely returns NDIS_STATUS_SUCCESS.
NDIS_STATUS KNdisFilterAdapter::OnSet(KNdisRequest& Req)
 { UNREFERENCED_PARAMETER(Req); return NDIS_STATUS_SUCCESS; }


// Lower-edge: delegated from KNdisProtocolWrapper<Binding>


///////////////////////////////////////////////////////////////////////
// Process lookahead receive indication (handler)
//
// Parameters:
//  PacketToAccept		 - An empty packet descriptor
// 	HeaderBuffer         - Header pointer
// 	HeaderBufferSize     - Header size being indicated
// 	LookAheadBuffer      - Lookahead buffer pointer
// 	LookaheadBufferSize  - Lookahead size being indicated
// 	PacketSize           - Total packet size (not including the header)
// Returns:
// 	NDIS_STATUS_SUCCESS to allow forwaring the indication up; error code to drop the packet.
// Comments:
// 	The framework calls this version of OnReceive() when the underlying real NIC miniport
//	uses partial receive packet indications (e.g., see discussion on KNdisLookahead).
//  Non-bus mastering NIC miniports may use this form of packet indication.
//	This method, thus, is the way for the filter adapter to inspect the packet received
//	by the real NIC before the framework forwards it up to the bound protocol(s). The driver might "override"
//	the content of the indicated packet or even drop it altogether.
//	If the client is interested in looking into the remainder of the packet, it could
//  assign the supplied empty PacketToAccept to a real packet descriptor allocated from a
//	private packet pool. Then the framework would attempt to TransferData into supplied
//  packet and call the filter's OnTransferComplete() method.
//	The default implementation merely returns NDIS_STATUS_SUCCESS - passes the packet up.
NDIS_STATUS KNdisFilterAdapter::OnReceive(IN OUT KNdisPartialPacket& PacketToAccept,
								  IN PVOID HeaderBuffer, IN UINT HeaderBufferSize,
								  IN PVOID LookAheadBuffer, IN UINT LookaheadBufferSize,
								  IN UINT PacketSize)
{   UNREFERENCED_PARAMETERS3(PacketToAccept, HeaderBuffer, HeaderBufferSize);
	UNREFERENCED_PARAMETERS3(LookAheadBuffer,LookaheadBufferSize,PacketSize);
	return NDIS_STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// Process completion of TransferData to packet (handler)
//
// Parameters:
//  Packet				- Packet descriptor originally passed w/TransferData
// 	Status				- Final Status of TransferData
// 	BytesTransferred	- Number of Bytes Transferred by the NIC
// Returns:
// 	NDIS_STATUS_SUCCESS to allow signalling to bound protocols that
//		transfer data is complete; error code to drop the packet.
// Comments:
// 	The framework calls this version of OnTransferComplete() when the underlying real NIC
//	miniport uses partial receive packet indications (e.g., see discussion on KNdisLookahead).
//  Non-bus mastering NIC miniports may use this form of packet indication.
//	The framework previously called TransferData with the Packet descriptor.
//	This method, thus, is the way for the filter adapter to inspect the packet received
//	by the real NIC before the framework forwards it up to the bound protocol(s). The driver might "override"
//	the content of the indicated packet or even drop it altogether.
//	If the client had previously allocated the packet descriptor from a private packet pool, then
//	the packet descriptor should be returned to the private pool in this routine.  If the bound
//	protocol supplied the packet descriptor, then return  NDIS_STATUS_SUCCESS for normal
//	clean-up.  If any allocations are made in OnReceive (for lookahead receive indication), then
//	OnTransferComplete is the method to cleanup the allocations.
//	The default implementation merely returns NDIS_STATUS_SUCCESS - passes the packet up.
NDIS_STATUS KNdisFilterAdapter::OnTransferComplete(PNDIS_PACKET Packet, NDIS_STATUS Status, UINT BytesTransferred)
{
	UNREFERENCED_PARAMETERS3(Packet, Status, BytesTransferred);
	return NDIS_STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// Process status change indication (handler)
//
// Parameters:
// 	Status               - NDIS_STATUS_XXX value that indicates the general change in status for the underlying NIC
// 	StatusBuffer         - Pointer to Optional status data buffer pointer (can be modified)
// 	StatusBufferSize     - Pointer to Status data buffer size (can be modified)
// Returns:
// 	NDIS_STATUS_XXX to pass up to the protocols
// Comments:
//	The framework calls OnStatusIndication() whenever the underlying real NIC indicates status
//	change via NdisMIndicateStatus and before the indication is passed up to the protocol(s).
//	The filter adapter, thus, is given a chance to examine the associated status buffer and modify
//	- if necessary - both the buffer and the status code indicated to the protocols. The content of
//	the buffer is status-value and medium dependent.
//	A filter that generates its own status indications (e.g. it implements some sort of line simulation)
//	has to maintiain an internal state machine.
//	The default implementation merely returns Status.
NDIS_STATUS KNdisFilterAdapter::OnStatusIndication
	(NDIS_STATUS Status, IN OUT PVOID* StatusBuffer, UINT* StatusBufferSize)
{	UNREFERENCED_PARAMETERS2(StatusBuffer, StatusBufferSize);
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Process status change completion (handler)
//
// Parameters:
// 	none
// Returns:
// 	NDIS_STATUS_SUCCESS
// Comments:
//	The framework calls OnStatusIndicationComplete() whenever the underlying real NIC indicates status
//	change completion via NdisMIndicateStatusComplete and before the indication is passed up to the protocol(s).
//	The default implementation merely returns NDIS_STATUS_SUCCESS. Any other return code prevents
//	the indication from being passed up to the protocols. The drivers should not block the indication
//	unless they simulate status to the protocols and drop the preceding OnStatusIndication(s) on the floor.
NDIS_STATUS KNdisFilterAdapter::OnStatusIndicationComplete()
{
	return NDIS_STATUS_SUCCESS;
}
