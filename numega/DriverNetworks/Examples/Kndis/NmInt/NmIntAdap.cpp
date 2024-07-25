// NmIntAdap.cpp: implementation of the NmIntAdapter class.
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


#include <kndis.h>
#include "NmIntDrv.h"
#include "NmIntAdap.h"
#include "NmIntBind.h"


// maximum number of bindings which can bind to
// i.e. number of underlying NIC's which we talk to
const ULONG NmIntAdapter::m_nMaxBindings = 4;


// count of adapter instances
UCHAR		NmIntAdapter::m_AdapNum	= 0;


//=============================================================================
// NmIntAdapter Constructor
//
//		NmIntAdapter objects are instanced from KNdisWrapper<NmIntAdapter>::Initialize
//		in response to NDIS calling MiniportInitialize
//
//=============================================================================
NmIntAdapter::NmIntAdapter() :
	m_nBindings(0),
	m_PacketFilter(0),
	m_NumMacAddr(0),
	m_State(DOWN)
{
	m_ppBindings = new NmIntBinding*[m_nMaxBindings];
	ASSERT(m_ppBindings);
	if (m_ppBindings) 
		NdisZeroMemory(m_ppBindings, m_nMaxBindings*sizeof(NmIntBinding*));

	m_CurrEthAddress.Invalidate();
	m_PermEthAddress.Invalidate();
	NdisZeroMemory(m_MulticastAddrs,MAX_MULTICAST_ADDRESSES*sizeof(ETHERNET_ADDRESS));

	NmIntAdapter::m_AdapNum++;
}


//=============================================================================
// NmIntAdapter Destructor
//
//		NmIntAdapter objects are destroyed from KNdisWrapper<NmIntAdapter>::Halt
//		in response to NDIS calling MiniportHalt
//
//=============================================================================
NmIntAdapter::~NmIntAdapter() 
{
	delete m_ppBindings;
}


//=============================================================================
// NmIntAdapter::Initialize
//
// This handler is called as a result of NdisIMInitializeDeviceInstance
// called by NmIntBinding on successful binding creation. The job
// of this handler is to:
//
// 1) Retrieve and store the associated binding instance;
// 2) Set the medium index to what the binding has been assigned to;
// 3) Initialize the packet pools, etc. internal objects
// 4) Set miniport attributes
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
//=============================================================================
NDIS_STATUS NmIntAdapter::Initialize(
	IN OUT KNdisMedium& Medium,
	IN KNdisConfig& Config
	)
{
	TRACE("NmIntAdapter::Initialize\n");
	m_State = INITIALIZING;
	NDIS_STATUS Status = NDIS_STATUS_FAILURE;

	// Get the associated adapter instance name:
	NDIS_STRING VirtInst;
	PWSTR pszwInst = reinterpret_cast<PWSTR> ( NdisIMGetDeviceContext(m_Handle) );
	if (!pszwInst)
		KNDIS_RETURN_ERROR (NDIS_STATUS_FAILURE);

	NdisInitUnicodeString(&VirtInst,pszwInst);

	// Cache the instance GUID
	m_wsInstance.Buffer = (PUSHORT) new UCHAR[VirtInst.MaximumLength];
	if (m_wsInstance.Buffer) {
		NdisMoveMemory(m_wsInstance.Buffer,VirtInst.Buffer,VirtInst.MaximumLength);
		m_wsInstance.Length = VirtInst.Length;
		m_wsInstance.MaximumLength = VirtInst.MaximumLength;
	} else {
		return Status;
	}

	// Retrieve some initialization parameters from the Registry. Use defaults if none.
	UINT MaxPoolSize = Config.Read(KNDIS_STRING_CONST("MaxPoolSize"), 1024);
	UINT MinPoolSize = Config.Read(KNDIS_STRING_CONST("MinPoolSize"), 255);

	// TODO: Initialize internal objects:

	m_TxPktPool.InitializeEx(MinPoolSize, MaxPoolSize-MinPoolSize,sizeof(TxRESERVED));
	if (!m_TxPktPool.IsValid()) {
		ASSERT(m_TxPktPool.IsValid());
		KNDIS_RETURN_ERROR (NDIS_STATUS_RESOURCES);
	}

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
								| NDIS_ATTRIBUTE_DESERIALIZE |
								NDIS_ATTRIBUTE_NO_HALT_ON_SUSPEND
								);

	SetAttributesEx(NdisInterfaceInternal, AttrFlags);

	// TODO: Retrieve any parameters from registry using Config object
	// This are created with the .inf file.  BUT the user cannot edit
	// the properties in the Network Control Panel app (NCPA).  The parameters
	// shown in the NCPA are stored under the Protocols section of Registry

	// Call a method which stores this object in a list of internal bindings
	// The driver object manages the iternal bindings by tracking adapters and
	// binding objects.  The driver object will also update the Adapter's array
	// of bindings.  Likewise, for bindings
	Driver()->OnOpenAdapter(this);

	// TODO: Select the medium which this virtual adapter exposes to 
	// protocols.  This does NOT have to match the underlying NIC.

	// This sample just sets the medium index to whatever the underlying
	// NIC (abstracted by the binding object) was set to
	if (!m_ppBindings[0] || !Medium.Select(m_ppBindings[0]->m_SelectedMedium)) {
		ASSERT(m_ppBindings[0]);
		ASSERT(!"Medium.Select");
	}

	m_State = UP;

	return NDIS_STATUS_SUCCESS;
}


//=============================================================================
// NmIntAdapter::Reset
//
//=========================================================================
NDIS_STATUS NmIntAdapter::Reset(OUT PBOOLEAN AddressingReset)
{
	// @@TODO propagate this to bindings?
	*AddressingReset = FALSE;
	return NDIS_STATUS_SUCCESS;
}


//=============================================================================
// NmIntAdapter::Halt
//
//		Either virtual NIC or underlying binding is being destroyed by NDIS
//		Not much can we do here.
//
//=============================================================================
VOID NmIntAdapter::Halt()
{
	TRACE("NmIntAdapter::Halt()\n");
	if (m_State==UP) m_State = HALTING;

	// Remove all references to this adapter that any binding object has.
	Driver()->OnCloseAdapter(this);

	// Wait for any outstanding requests to complete.  
	while(m_Count) {
		Sleep(100);
	}

	m_State = DOWN;

	delete m_wsInstance.Buffer;
}


//=============================================================================
// NmIntAdapter::Send
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
// If we are halting, ndis won't send us any packets.
//
// Parameters:
//		Packet		-		Original packet to be sent
//		Flags		-		Packet flags
//	Returns:
//		NDIS_STATUS_SUCCESS	-	Packet is submitted and done with
//		NDIS_STATUS_PENDING	-	Packet is in flight now
//		NDIS_STATUS_RESOURCES -	Ran out of Tx descriptors
//
//=============================================================================
NDIS_STATUS NmIntAdapter::Send(IN PNDIS_PACKET Packet, IN UINT Flags)
{
	NMINTTRACE(NMINT_LOUD, ("NmIntAdapter::Send() Packet: %p\n", Packet));

	NDIS_STATUS Status = NDIS_STATUS_NOT_ACCEPTED;

	KNdisPacket Original(Packet);

	// TODO: Process based on packet's content, e.g.
	//
	//		KNdisBuffer Buf = P.QueryFirstBuffer();
	//		if (Buf.IsValid()) {
	//			HEADER* Content = (PUCHAR) Buf.Address();
	//			. . .
	//		}
	// You may want to drop the packet or do something different



	// prevent bindings from being destroyed
	IncBindCount();

	// Choose the binding to utilize here
	// Note: Could choose more than one by allocating an additional 
	//		 packet from the m_TxPktPool

	NmIntBinding* pBinding = GetBinding(Original);

	if (pBinding && pBinding->IsUp()) {

		// Allocate a new packet descriptor
		TxPacket Repackaged = m_TxPktPool.Allocate(); 
		if (!Repackaged.IsValid())
			Status = NDIS_STATUS_RESOURCES; 
		else {

			// Save the adapter instance and the original packet in the context
			// of the packet (i.e. the ProtocolReserved fields of pkt)
			Repackaged.GetContext()->OriginalAdapter = this;
			Repackaged.GetContext()->OriginalPacket  = Original;

			// This implementation simply points the Repackaged to the 
			// buffers of the Original.
			Repackaged.CloneDown(Original);

			// Important: set flags. The flags are not cloned; 
			Repackaged.SetFlags (Original.GetFlags()); 

			// increment outstanding request count
			++m_Count;

			// send the packet to the underlying NIC
			Status = pBinding->Send(Repackaged);

			if (Status != NDIS_STATUS_PENDING) {
				SendComplete(Repackaged,Status);
			}
		}
	}

	DecBindCount();

	return Status;
}


//=============================================================================
// NmIntAdapter::SendComplete
//
//=============================================================================
void NmIntAdapter::SendComplete(
	IN PNDIS_PACKET Packet,
	IN NDIS_STATUS Status
	)
{
	TxPacket Repackaged(Packet);
	PNDIS_PACKET Original = Repackaged.GetContext()->OriginalPacket;
	ASSERT(Original);

	// increment the stats counters
	if (NDIS_STATUS_SUCCESS == Status) {
		m_GenStats->xmit_ok++;
	} 
	else {
		m_GenStats->xmit_error++;
	}

	KNdisExportStubs::CopySendCompletePerPacketInfo(Original, Packet);
	// return to the upper-protocol
	NdisMSendComplete(*this, Original, Status);

	// release repackaged resources
	// KNdisPacket::Reinitialize(Packet);
	m_TxPktPool.Free(Packet);
	// Decrement outstanding request count
	--m_Count;
}


//=============================================================================
// NmIntAdapter::ReturnPacket
//
// NDIS returns a packet previusly submitted by IndicateReceive().
//
// Parameters:
//		Packet	-		Packet (repackaged)		
// Returns:
//		None
//
//=============================================================================
VOID NmIntAdapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	NMINTTRACE(NMINT_LOUD, ("NmIntAdapter::ReturnPacket Packet: %p\n", Packet));

	// Restore originating binding object instance and the
	// original packet both stored in miniport reserved fields of packet 

	NmIntBinding::RxPacket Repackaged(Packet);

	NmIntBinding* b = Repackaged.GetContext()->OriginalBinding;
	PNDIS_PACKET Original = Repackaged.GetContext()->OriginalPacket;

	ASSERT(b->IsValid());
	ASSERT(Original);

	// KNdisExportStubs::CopySendCompletePerPacketInfo(Original, Packet);
	b->ReturnPacket(Original); // return original packet to the real NIC miniport

	// release repackaged resources
	// KNdisPacket::Reinitialize(Packet);
	b->m_RxPktPool.Free(Packet);

	//decrement outstanding request count
	--b->m_Count;
}


//=============================================================================
// NmIntAdapter::TransferData
//
// NDIS requests to transfer data previously submitted by a partial
// receive ("filtered") indication. We simply forward it to the
// real NIC over the binding.
//
//=============================================================================
NDIS_STATUS NmIntAdapter::TransferData(
	OUT PNDIS_PACKET			Packet,
	OUT PUINT					BytesTransferred,
	IN  PVOID					MiniportReceiveContext,
	IN	UINT					ByteOffset,
	IN	UINT					BytesToTransfer
	)
{
	NMINTTRACE(NMINT_INFO, ("->NmIntAdapter::TransferData Packet=%p, RCV_CTX=%p, %d, %d\n", Packet, MiniportReceiveContext, ByteOffset, BytesToTransfer));
	NDIS_STATUS Status = NDIS_STATUS_FAILURE ;

	// Allocate a new packet descriptor
	TxPacket Repackaged = m_TxPktPool.Allocate();
	if (!Repackaged.IsValid()) 
		return Status;

	++m_Count;

	// Save the adapter instance and the original packet in the context
	// of the packet (i.e. the ProtocolReserved fields of pkt)
	Repackaged.GetContext()->OriginalAdapter = this;
	Repackaged.GetContext()->OriginalPacket  = Packet;

	// Repackaged.CloneDown(Packet);
	((PNDIS_PACKET)Repackaged)->Private.Head = Packet->Private.Head;
	((PNDIS_PACKET)Repackaged)->Private.Tail = Packet->Private.Tail;

	// Retrieve partial receive context where we store binding instance
	// and original mac receive context
	NmIntBinding::PARTIAL_RCV_CTX* pCtx = 
			reinterpret_cast<NmIntBinding::PARTIAL_RCV_CTX*>(MiniportReceiveContext);
	ASSERT(pCtx);

	NmIntBinding* b = pCtx->OriginalBinding;
	ASSERT(b->IsValid());

	// Request underlying NIC to transfer data
	Status = b->TransferData(
				pCtx->MacRcvCtx,
				ByteOffset,
				BytesToTransfer,
				Repackaged,
				BytesTransferred
				);

	// Don't call b->TransferDataComplete cuz it's already called by binding.

	return Status;
}


//=============================================================================
// Notify of completion of received data transfer (rarely used)
//
// Parameters:
// 	Packet               - packet filled in
// 	Status               - NDIS_STATUS_SUCCESS
// 	BytesTransferred     - Bytes actually transferred
// Returns:
// 	none
// Comments:
// 	none
//=============================================================================
void NmIntAdapter::TransferDataComplete(
	IN PNDIS_PACKET Packet, 
	IN NDIS_STATUS Status, 
	IN UINT BytesTransferred
	) 
{
	NMINTTRACE(NMINT_INFO, ("<-NmIntAdapter::TransferDataComplete Repackaged=%p", Packet));
	TxPacket Repackaged(Packet);
	PNDIS_PACKET Original = Repackaged.GetContext()->OriginalPacket;
	ASSERT(Original);

	// KNdisExportStubs::CopySendCompletePerPacketInfo(Original, Packet);
	NdisMTransferDataComplete(*this, Original, Status, BytesTransferred);
	// release repackaged resources
	// KNdisPacket::Reinitialize(Packet);
	m_TxPktPool.Free(Packet);
	--m_Count;
}


//=============================================================================
// NmIntAdapter::MatchPacketFilter
//
//		Checks the MAC address in the packet vs. the current packet filter
//		Called when a packet is received by the binding.
// 
// Returns:
//		NDIS_STATUS_SUCCESS		 Packet MAC address matches packet filter
//		NDIS_STATUS_NOT_ACCEPTED Packet MAC address doesn't match packet filter
//
//=============================================================================
NDIS_STATUS NmIntAdapter::MatchPacketFilter(KNdisPacket& P)
{
	// Examine the packet data.  Use the MAC addresses and the Packet 
	// Filter to decide whether to forward it up to bound protocols.  
	// return NDIS_STATUS_SUCCESS to accept this packet

	NDIS_STATUS Status = NDIS_STATUS_NOT_ACCEPTED;

	KNdisBuffer Buf = P.QueryFirstBuffer();
	if (!Buf.IsValid())
		return Status;

	PUCHAR pData = (PUCHAR) Buf.Address();
	ASSERT(pData);
	if (pData) 
		Status = MatchPacketFilter(pData);

	return Status;
}


//=============================================================================
// NmIntAdapter::MatchPacketFilter
//
//		Checks the MAC address in the packet vs. the current packet filter
//		Called when a packet is received by the binding.
// 
// Returns:
//		NDIS_STATUS_SUCCESS		 Packet MAC address matches packet filter
//		NDIS_STATUS_NOT_ACCEPTED Packet MAC address doesn't match packet filter
//
//=============================================================================
NDIS_STATUS NmIntAdapter::MatchPacketFilter(PUCHAR pEthHdr)
{
	if (m_PacketFilter & NDIS_PACKET_TYPE_PROMISCUOUS) {
		// the packet filter is set to promiscuous mode
		return NDIS_STATUS_SUCCESS;
	} 

	NDIS_STATUS Status = NDIS_STATUS_NOT_ACCEPTED;
	
	bool fMultiCast			= ETH_IS_MULTICAST(pEthHdr);
	PETHERNET_ADDRESS pAddr = reinterpret_cast<PETHERNET_ADDRESS>(pEthHdr);

	if (!fMultiCast) {
		// Directed packet

		UINT nResult;
		ETH_COMPARE_NETWORK_ADDRESSES((PUCHAR)PVOID(m_CurrEthAddress),(PUCHAR)PVOID(*pAddr),&nResult)
		if (0 == nResult) {
			// our MAC address matches the address in the packet
			Status = NDIS_STATUS_SUCCESS;
		}

	} else {
		// MultiCast or BroadCast Packet

#ifdef GENERATE_MAC_ADDRESS
		return NDIS_STATUS_SUCCESS;
#endif

		bool fBroadCast = ETH_IS_BROADCAST(pEthHdr);
		if (fBroadCast) {
			// Accepting BroadCast Packet?
			if (m_PacketFilter & NDIS_PACKET_TYPE_BROADCAST)
				Status = NDIS_STATUS_SUCCESS;
		} else {

			// Accepting MultiCast Packet?
			if (m_PacketFilter & NDIS_PACKET_TYPE_ALL_MULTICAST) 
				Status = NDIS_STATUS_SUCCESS;

			if (m_PacketFilter & NDIS_PACKET_TYPE_MULTICAST) {
				// Check the multicast addresses for a match
				for (ULONG i=0; i<m_NumMacAddr; i++) {

					UINT nResult;
					ETH_COMPARE_NETWORK_ADDRESSES((PUCHAR)PVOID(m_MulticastAddrs[i]),(PUCHAR)PVOID(*pAddr),&nResult)
					if (0 == nResult) {
						Status = NDIS_STATUS_SUCCESS;
						break;
					}
				}
			}
		}
	}

	return Status;
}


//=============================================================================
// NmIntAdapter::SetNicPacketFilter
//
//		Sets the packet filter on an underlying NIC (via Binding object).
//
//=============================================================================
NDIS_STATUS NmIntAdapter::SetNicPacketFilter(PULONG Filter)
{
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	// prevent bindings from being destroyed while accessing
	// a binding object
	IncBindCount();

	NmIntBinding** p = GetBindings();
	if (p && p[0] && p[0]->IsUp()) {
		// Set the packet filter on the underlying NIC
		Status = p[0]->SetOid(OID_GEN_CURRENT_PACKET_FILTER,Filter);
	} 

	DecBindCount();
	return Status;
}


//=============================================================================
// NmIntAdapter::GetCapabilities
//
//		Handler for OID_PNP_CAPABILITIES.  Delegates to binding object(s) to
//		determine if the underlying NIC is PM-aware.
//
//=============================================================================
NDIS_STATUS NmIntAdapter::GetCapabilities(PNDIS_PNP_CAPABILITIES pCaps)
{
	NDIS_STATUS Status = NDIS_STATUS_NOT_SUPPORTED;

	NmIntBinding** p = GetBindings();
	if (p && p[0]) {
		// Get the Pnp Capabilites of the underlying NIC
		Status = p[0]->GetCapabilities(pCaps);
		
		if (NDIS_STATUS_SUCCESS == Status) {

			PNDIS_PM_WAKE_UP_CAPABILITIES pWakeupCaps = &pCaps->WakeUpCapabilities;
			pWakeupCaps->MinMagicPacketWakeUp	= NdisDeviceStateUnspecified;
			pWakeupCaps->MinPatternWakeUp		= NdisDeviceStateUnspecified;
			pWakeupCaps->MinLinkChangeWakeUp	= NdisDeviceStateUnspecified;
		}
	} else {
		ASSERT(FALSE);
	}

	return Status;
}


//=============================================================================
// NmIntAdapter::AddBinding
//
//		Add an NmIntBinding instance to the array of bindings.  This represents
//		setting up the internal binding between this object & the binding obj.
//
//=============================================================================
bool NmIntAdapter::AddBinding(NmIntBinding* b)
{
	TRACE("NmIntAdapter::AddBinding NmIntBinding*:%p\n", b);

	bool fRet = false;
	NmIntBinding** B = GetBindings();

	// check if already seen this binding
	for (ULONG i=0;i<m_nBindings;i++) {

		if (B[i] == b) return true;
	}

	// check if we have room for this binding
	if (m_nBindings < NmIntAdapter::m_nMaxBindings) {
		B[m_nBindings] = b;

		// For 1 to N MUX, our virtual adapter simulate the first real miniport
		// HAS to have m_PermEthAddress and m_CurrEthAddress when first AddBinding is being called
		if (0 == m_nBindings)
		{
			m_MaxLookAhead	 = b->m_MaxLookAhead;
			m_LinkSpeed		 = b->m_LinkSpeed;
			m_CurrEthAddress = b->m_CurrEthAddress;
			m_PhysicalMedium = b->m_PhysicalMedium;
#ifdef GENERATE_MAC_ADDRESS
			m_CurrEthAddress.m_bytes[5] = m_CurrEthAddress.m_bytes[5] + m_AdapNum;
#endif
			m_PermEthAddress = m_CurrEthAddress;
		}
		
		fRet = true;
		++m_nBindings;
	}

	return fRet;
}


//=============================================================================
// NmIntAdapter::RemoveBinding
//
//		Remove an NmIntBinding instance from the array of bindings.  This is
//		tearing down the internal binding between this object & the binding obj.
//
//=============================================================================
ULONG NmIntAdapter::RemoveBinding(NmIntBinding* b)
{
	TRACE("NmIntAdapter::RemoveBinding NmIntBinding*: %p\n", b);

	bool fRet = true;
	NmIntBinding** B = GetBindings();

	// check if already seen this adapter
	for (ULONG i=0;i<m_nBindings;i++) {

		if (B[i] == b) {
			B[i] = B[m_nBindings-1];
			B[m_nBindings-1] = NULL;
			break;
		}
	}

	if ( i != m_nBindings ) {
		fRet = true;
		// decrement m_nAdapters
		--m_nBindings;
	}

	return m_nBindings;
}


//=============================================================================
// NmIntAdapter::GetBinding
//
//		"Choose" a binding to use to send a packet to an underlying NIC.  
//
//=============================================================================
NmIntBinding* NmIntAdapter::GetBinding(const KNdisPacket& P)
{
	// more than one binding, have to choose
	NmIntBinding* pRet = NULL;

	if (m_nBindings != ULONG(0)) {

		// if there is only one, just return it
		if (m_nBindings == ULONG(1)) {
			pRet = GetBindings()[0];
		} else {

			// TODO: this is where a decision has to be made
			// to select a binding to operate on the packet

			// This sample simply uses "size" of packet
			// when choosing a binding
			if (P.QueryTotalLength()>500) {
				pRet = GetBindings()[1];
			}
			else {
				pRet = GetBindings()[2];
			}
		}
	}

	return pRet;
}


//=============================================================================
// NmIntAdapter::IncBindCount
//
//		Increments outstanding request counter on each binding object to
//		guard against race condition with Unbind handler
//
//=============================================================================
void NmIntAdapter::IncBindCount()
{
	// Acquire driver-wide spin lock
	Driver()->Lock();

	// For each, adapter, increment the outstanding
	// request count to block halt handler from
	// deleting while we are still using it
	NmIntBinding** Binding = GetBindings();
	ASSERT(Binding);

	if (Binding) {
		for (ULONG i=0; i < m_nBindings; i++) {
			ASSERT(Binding[i]);
			++Binding[i]->m_Count;
		}
	}

	// Drop driver-wide spin lock
	Driver()->Unlock();
}


//=============================================================================
// NmIntAdapter::DecBindCount
//
//		Decrements outstanding request counter on each binding object.
//
//=============================================================================
void NmIntAdapter::DecBindCount()
{
	// Acquire driver-wide spin lock
	Driver()->Lock();

	// For each, adapter, increment the outstanding
	// request count to block halt handler from
	// deleting while we are still using it
	NmIntBinding** Binding = GetBindings();
	ASSERT(Binding);

	if (Binding) {
		for (ULONG i=0; i < m_nBindings; i++) {
			ASSERT(Binding[i]);
			--Binding[i]->m_Count;
		}
	}

	// Drop driver-wide spin lock
	Driver()->Unlock();
}


// end of file
 