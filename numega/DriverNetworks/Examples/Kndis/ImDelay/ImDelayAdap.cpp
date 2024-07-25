// ImDelayAdap.cpp: implementation of the ImDelayAdapter class.
//
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

#include "ImDelayAdap.h"

//////////////////////////////////////////////////////////////////////
// Constructor
ImDelayAdapter::ImDelayAdapter() :
	ImDelayBase(),
	m_SendDelayQ ( BOOLEAN(FALSE) ),	// use MiniportReserved[] for LIST_ENTRY
	m_ReceiveDelayQ ( BOOLEAN(TRUE) ),	// use ProtocolReserved[] for LIST_ENTRY
	m_bOn(true)
{
	// TODO: initialize your private data members
	//       Do NOT call any NDIS functions in here. Defer to Initialize()
}

//////////////////////////////////////////////////////////////////////
// Destructor
ImDelayAdapter::~ImDelayAdapter()
{
	// TODO: deallocate all the resources allocated in constructor/Initialize()
	//
	ASSERT(m_SendDelayQ.IsEmpty());
	ASSERT(m_ReceiveDelayQ.IsEmpty());
}

////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnInitialize
//
//	The framework calls this method when a new virtual NIC represented
//	by the ImDelayAdapter is created. At this time the binding to the
//  underlying real NIC is completed already. Most of the glue work has
//  been done by the framework, so this method could just return 
//  NDIS_STATUS_SUCCESS to grant the creation of the filter. Or, it might
//  inspect the underlying medium type (Medium) and/or set-up
//  some private config parameters from the Registry (Config) before
//  returning.
//
// Parameters:
//		Medium	
//			Reference to the KNdisMedium object which has a medium type
//			selected for the underlying binding by the real NIC
//		Config
//			Reference to miniport's KNdisConfig object used to query various
//			configuration parameters
//
// Return
//		NDIS_STATUS_SUCCESS		Proceed with the creation of the filter
//		NDIS_STATUS_XXXXXXX		Error. The framework rolls back and fails the
//								miniport initialization.	
// IRQL: 
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
NDIS_STATUS ImDelayAdapter::OnInitialize
(const KNdisMedium& Medium, KNdisConfig& Config)
{
	TRACE("ImDelayAdapter::OnInitialize(): %s at %ws\n", Medium.Text(), GetBoundDeviceName());
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
	
	m_Delay = Config.Read(KNDIS_STRING_CONST("Delay"), 10);
    

#ifdef IMDELAY_USE_PRIVATE_POOLS

	ULONG PacketPoolSize = Config.Read(KNDIS_STRING_CONST("PacketCount"), 100);
	
	Status = m_TxPool.Initialize(PacketPoolSize);
	if (Status != NDIS_STATUS_SUCCESS)
		return Status;
	
	Status = m_RxPool.Initialize(PacketPoolSize);
	if (Status != NDIS_STATUS_SUCCESS)
		return Status;

#endif //IMDELAY_USE_PRIVATE_POOLS

	// initialize timer
	m_Timer.Initialize(this, KNDIS_MEMBER_CALLBACK(OnTimeout));
	m_Timer.SetPeriodic(m_Delay);

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnHalt
//
// Either virtual NIC or underlying binding is being destroyed by NDIS
// Not much can we do here.
//
VOID ImDelayAdapter::OnHalt()
{
	TRACE("ImDelayAdapter::OnHalt()\n");

	m_Timer.Cancel();

	m_bOn = false;

	while (!m_SendDelayQ.IsEmpty())
		OnSendComplete(m_SendDelayQ.RemoveHead(), NDIS_STATUS_FAILURE);

	while (!m_ReceiveDelayQ.IsEmpty())
		ReturnPacket(m_ReceiveDelayQ.RemoveHead());

}

////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnSend
//
// Intercepts the upper-layer send. Here's where the filtering in
// the outgoing direction takes place. The filter is given a chance
// to examine the original packet submitted by a protocol (Original)
// and copy or modify or just drop it. The framework supplies a fresh
// packet descriptor (Repackaged) good for passing one packet down
// the binding. A filter that needs more packets (e.g. it implements
// some sort of repacketization) has to maintiain an internal packet
// pool and implement the OnSendComplete() handler.
//
// In LBFO case, the filter can choose forwarding the packet over
// an other filter's instance binding from the bundle of adapter instances.
// 
//
// Parameters:
//		Original		-	Original packet descriptor supplied by NDIS
//		Repackaged		-	Framework-provided descriptor for repackaging
// Returns:
//  The return value determines the behaviour of the framework after
//  OnSend() is returned:
//		NDIS_STATUS_SUCCESS		 Requests the framework to forward the
//								 Repackaged packet down the binding
//		NDIS_STATUS_NOT_ACCEPTED Send nothing. Release resources and Return NDIS_STATUS_SUCCESS to NDIS
//		NDIS_STATUS_PENDING		 Do nothing, Recycle nothing. Return NDIS_STATUS_PENDING to NDIS
//		NDIS_STATUS_XXX			 Do nothing. Retutn this status to NDIS.
//
// NOTE: The Repackaged initiallly describes a packet with no chained buffers. 
//		 It is OK to return NDIS_STATUS_SUCCESS and leave the Repackaged empty.
//		 This would mean the packet has been successfully "consumed" by the filter.
//
// KNOWN PROBLEM: Currently, one can't chain user-allocated buffers to the 
//				  repackaged descriptor. I.e. only "in-place" modifications are possible
//				  Use a private packet/buffer pools and ForwardDown() if needed.
//
NDIS_STATUS ImDelayAdapter::OnSend
(const KNdisPacket& Original, KNdisPacket& Repackaged)
{
	UINT totalLength = Original.QueryTotalLength();
	TRACE("ImDelayAdapter::OnSend() %u bytes\n", totalLength);

	// TODO: Filter based on packet's content, e.g.
	//       HEADER* Content = (HEADER*) Original.QueryFirstBuffer();
	//		 . . .
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
	Repackaged.CloneDown(Original);

	if (m_bOn)
	{

#ifdef IMDELAY_USE_PRIVATE_POOLS
		KNdisPacket p = m_TxPool.Allocate();
		if (p.IsValid())
		{
			// create an alias packet descriptor for private packet to pass down the binding
			p.CloneDown(Original);			

			// Important: set flags. The flags are not cloned; they are passed in the
			// flags of Repackaged saved by the Framework
			p.SetFlags (Repackaged.GetFlags()); 

			// Store the orig packet, queue and return
			m_TxPool.GetContext(p)->FrameworkPacket = Repackaged;	
			m_SendDelayQ.InsertTail(p);
			Status = NDIS_STATUS_PENDING;		// holding the packet
		}
		else 
			Status = NDIS_STATUS_RESOURCES;
#else
		m_SendDelayQ.InsertTail(Repackaged);	// holding the packet
		Status = NDIS_STATUS_PENDING;
#endif //IMDELAY_USE_PRIVATE_POOLS

	}

	return Status;
}


///////////////////////////////////////////////////////////////////////
// Send Complete
//
// Parameters:
// 	pPacket              - Packet Forwarded Down gets completed
// 	Status               - Completion status
// Returns:
// 	none
// Comments:
// 	none
VOID ImDelayAdapter::OnSendComplete(PNDIS_PACKET pPacket, NDIS_STATUS Status)
{
	// OnSendComplete will not be called if private packet pool is not implemented because
	// DriverNetworks RePackaged will be recycled in KNdisFilterBinding::SendComplete

#ifdef IMDELAY_USE_PRIVATE_POOLS
		ASSERT (m_TxPool.IsFrom(pPacket));
	
		// Recover the framework packet, copy OOB info block
	
		KNdisPacket Repackaged = m_TxPool.GetContext(pPacket)->FrameworkPacket;
		ASSERT(Repackaged.IsValid());
		KNdisExportStubs::CopySendCompletePerPacketInfo(Repackaged, pPacket);

		// Recycle our own packet
	
		KNdisPacket::Reinitialize(pPacket);
		m_TxPool.Free(pPacket);
	
		// Recycle the Repackaged and complete the send for the upper protocol.
		// We simulate the packet completion on our binding
	
		GetBinding()->SendComplete(Repackaged, Status);	 

#else
		UNREFERENCED_PARAMETER(pPacket); UNREFERENCED_PARAMETER(Status);
		ASSERT(!"Should not be called!");
#endif //IMDELAY_USE_PRIVATE_POOLS
}


////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnReceive
//
// Intercetpts lower-layer receive. Here's where the filtering in
// the incoming direction takes place. The filter is given a chance
// to examine the original packet indicated by the real NIC (Original)
// and copy or modify or just drop it. The framework supplies a fresh
// packet descriptor (Repackaged) good for passing one packet up to the
// protocols. A filter that needs more packets (e.g. it implements
// some sort of repacketization) has to maintiain an internal packet
// pool and implement the OnReturnPacket() handler.
//
// Parameters:
//		Original		-	Original packet descriptor from the underlying NIC
//		Repackaged		-	Framework-provided descriptor for repackaging
// Returns:
//  The return value determines the behaviour of the framework after
//  OnReceive() is returned:
//		NDIS_STATUS_SUCCESS		Requests the framework to forward the
//								Repackaged packet up to the protocols
//		NDIS_STATUS_NOT_ACCEPTED Forward nothing, but hold the packet
//		NDIS_STATUS_XXX			 Drop packet. 
//
// NOTE: The Repackaged initiallly describes a packet with no chained buffers. 
//		 It is OK to return NDIS_STATUS_SUCCESS and leave the Repackaged empty.
//		 This would mean the packet has been successfully "consumed" by the filter.
//
// KNOWN PROBLEM: Currently, one can't chain user-allocated buffers to the 
//				  repackaged descriptor. I.e. only "in-place" modifications are possible
//				  Use a private packet/buffer pools and ForwardUp() when needed.
//
NDIS_STATUS ImDelayAdapter::OnReceive(const KNdisPacket& Original, KNdisPacket& Repackaged)
{
	TRACE("ImDelayAdapter::OnReceive() %u bytes\n", Original.QueryTotalLength());

	// TODO: Filter based on packet's content, e.g.
	//       HEADER* Content = (HEADER*) Original.QueryFirstBuffer();
	//		 . . .
	// This implementation simply points the Repackaged to the buffers of the
	// Original and forwards it over the binding.
	
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
	Repackaged.CloneUp(Original);

	if (m_bOn)
	{

#ifdef IMDELAY_USE_PRIVATE_POOLS
		KNdisPacket p = m_RxPool.Allocate();
		if (p.IsValid())
		{
			p.CloneUp(Original);									// create an alias packet descriptor for private packet to pass down the binding
			m_RxPool.GetContext(p)->FrameworkPacket = Repackaged;	// store the orig packet
			m_ReceiveDelayQ.InsertTail(p);
			Status = NDIS_STATUS_NOT_ACCEPTED;						// meaning "we're holding on the packet"
		}
		else 
			Status = NDIS_STATUS_RESOURCES;
#else
		NDIS_STATUS PacketStatus = Repackaged.STATUS();
		if ( PacketStatus != NDIS_STATUS_RESOURCES )
		{
			m_ReceiveDelayQ.InsertTail(Repackaged);
			Status = NDIS_STATUS_NOT_ACCEPTED;						// meaning "we're holding on the packet"
		}
		else
			Status = NDIS_STATUS_SUCCESS;							// we are not going to delay packets with NDIS_STATUS_RESOURCES
#endif //IMDELAY_USE_PRIVATE_POOLS

	}
	return Status;
}



///////////////////////////////////////////////////////////////////////
// Return Packet
//
// Parameters:
// 	pPacket              - packet forwarded up returns back
// Returns:
// 	none
// Comments:
// 	none
VOID ImDelayAdapter::OnReturnPacket(PNDIS_PACKET pPacket)
{
	// OnReturnPacket will not be called if private packet pool is not implemented because
	// DriverNetworks RePackaged will be recycled in KNdisFilterAdapter::ReturnPacket

#ifdef IMDELAY_USE_PRIVATE_POOLS
		ASSERT (m_RxPool.IsFrom(pPacket));
	
		// our repackaged packet: complete
	
		// Recover the framework packet, copy OOB info block
	
		KNdisPacket Repackaged = m_RxPool.GetContext(pPacket)->FrameworkPacket;
		ASSERT(Repackaged.IsValid());
	
		// Recycle our own packet
	
		KNdisPacket::Reinitialize(pPacket);
		m_RxPool.Free(pPacket);
		
		// return Repackaged to the framework/originating miniport
	
		KNdisFilterAdapter::ReturnPacket(Repackaged);
#else
		UNREFERENCED_PARAMETER(pPacket); 
		ASSERT(!"Should not be called!");
#endif //IMDELAY_USE_PRIVATE_POOLS
	 
}


///////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnReceive (partial)
//
// Parameters:
//	PacketToAccept		 - Non-initialized packet descriptor
// 	HeaderBuffer         - Header pointer
// 	HeaderBufferSize     - Header size being indicated
// 	LookAheadBuffer      - Lookahead buffer pointer
// 	LookaheadBufferSize  - Lookahead size being indicated
// 	PacketSize           - Total packet size (not including the header)
// Returns:
//  The return value determines the behaviour of the framework after
//  OnReceive() is returned:
//		NDIS_STATUS_SUCCESS		Requests the framework to forward the
//								Repackaged packet up to the protocols
//		NDIS_STATUS_NOT_ACCEPTED Forward nothing. Release resources and Return NDIS_STATUS_SUCCESS to NDIS
//		NDIS_STATUS_XXX			 Do nothing. Retutn this status to NDIS.
// Comments:
// 	The framework calls this version of OnReceive() when the underlying real NIC miniport
//	uses partial receive packet indications (e.g., see discussion on KNdisLookahead).
//  Non-bus mastering NIC miniports may use this form of packet indication.
//	If the client is interested in looking into the remainder of the packet, it could
//  assign the supplied empty PacketToAccept to a real packet descriptor allocated from a
//	private packet pool. Then the framework would attempt to TransferData into supplied
//  packet and call the filter's OnTransferComplete() method.
NDIS_STATUS ImDelayAdapter::OnReceive(IN OUT KNdisPartialPacket& PacketToAccept,	
								  IN PVOID HeaderBuffer, IN UINT HeaderBufferSize,
								  IN PVOID LookAheadBuffer, IN UINT LookaheadBufferSize,
								  IN UINT PacketSize)
{  
	TRACE("ImDelayAdapter::OnReceive() Partial %u/%u%/%u bytes\n",
		HeaderBufferSize, LookaheadBufferSize, PacketSize);

	// NOTE: This sample does not process partial packets. Simply passes them up.
	
	UNREFERENCED_PARAMETER(PacketToAccept);
	
	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnQuery
//
// Intercepts protocol's OID queries/replies to the NIC. The method
// is called *after* the underlying miniport returned it's information.
// Usually, we shouldn't care (the framework does the job), 
// so this method is optional.
//
// Parameters:
//	Request				Repackaged request that had been forwarded to 
//						and completed by real NIC.
//	ReturnedStatus		Resulting status code from the NIC
//
// Return:
//	 Usually, ReturnedStatus. Or, override in case some cheating is required.
//
NDIS_STATUS ImDelayAdapter::OnQuery
(KNdisRequest& Request, NDIS_STATUS ReturnedStatus)
{
	TRACE("ImDelayAdapter::OnQuery() %s\n", Request.Text());
	
	return ReturnedStatus;
}

////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnSet
//
// Intercepts protocol's OID set to the NIC. The method
// is called *before* the underlying miniport gets the request.
// Usually, we shouldn't care (the framework does the job), 
// so this method is optional.
//
// Parameters:
//	Request				Repackaged request to be forwarded to the real NIC.
//
// Return:
//	 NDIS_STATUS_SUCCESS		- pass the request down
//	 NDIS_STATUS_NOT_ACCEPTED	- do now forward: complete request with success
//	 NDIS_XXX_					- do now forward: return this error code.
//
NDIS_STATUS ImDelayAdapter::OnSet(KNdisRequest& Request)
{
	TRACE("ImDelayAdapter::OnSet() %s\n", Request.Text());
	
	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnStatusIndication
//
// Intercepts the real NIC status changes. The filter is given a chance
// to examine the associated status buffer (if any) and made changes
// if necessary to both the buffer and the status code indicated to the
// protocols. The content of the buffer is status-value and medium dependent.
// A filter that generates its own status indications (e.g. it implements
// some sort of line simulation) has to maintiain an internal state machine.
//
// Parameters:
//		Status			-	General Status being indicated, e.g. NDIS_STATUS_RESET_START 
//		StatusBuffer	-	Buffer associated with the status, if any
//		StatusBufferSize -	Buffer Size, or 0.
// Return:
//		Status to indicate, usually Status itself.
//
NDIS_STATUS ImDelayAdapter::OnStatusIndication
(NDIS_STATUS Status, IN OUT PVOID* StatusBuffer, UINT* StatusBufferSize)
{
	TRACE("ImDelayAdapter::OnStatusIndication() 0x%X\n", Status);
	
	// TODO: Filter based on whatever.
	//
	// This implementation simply allows everything go up intact. More
	// sophisticated approach would be blocking all the send-s when the
	// underltying device is offline or being reset, etc.
	
	UNREFERENCED_PARAMETER(StatusBuffer);
	UNREFERENCED_PARAMETER(StatusBufferSize);
	
	return Status;
}

////////////////////////////////////////////////////////////
// ImDelayAdapter::OnReconfiguration
//
// Processes network stack configurtion changes.
//
// Parameters:
//	wsSection	-- Registry key relevant to the indicated change
//  Original PNP event
// Return:
//	Status of return. Must be NDIS_STATUS_SUCCESS
// Comments:
//  For LBFO cases, wsSection indicates the new BundleId reconfigured
//	for the miniport, e.g. using the "Advanced" property page.
//
// NOTE: This method doesn't have to be overridden.
//
NDIS_STATUS ImDelayAdapter::OnReconfiguration(PCWSTR wsSection, PNET_PNP_EVENT pEvent)
{
	TRACE("ImDelayAdapter::OnReconfiguration() %ws\n", wsSection);
	
	return KNdisFilterAdapter::OnReconfiguration(wsSection, pEvent);
}


#if defined NDIS51_MINIPORT
///////////////////////////////////////////////////////////////////////
// ImDelayAdapter::OnCancelSendPackets
//
// Parameters:
// 	CancelId             - packet cancel id
// Returns:
// 	none
// Comments:
// 	NDIS calls the filter to cancel pending packets. If the driver queues
//  any packets internally it should walk the list of pending packets and
//  cancel them, and the call the base class OnCancelSendPackets.
//  The default implementation forwards cancel down to the bound miniport.
VOID ImDelayAdapter::OnCancelSendPackets(IN PVOID CancelId)
{
	// Walk the TX list, cancel pending with give id

	KNdisPacketList CancelList;

	m_SendDelayQ.Lock();							// protect the loop
	PNDIS_PACKET p = m_SendDelayQ.HeadNoLock();
	while (p)  {
			PNDIS_PACKET thepacket = p;
			p = m_SendDelayQ.NextNoLock(p);
			if (NDIS_GET_PACKET_CANCEL_ID(thepacket)==CancelId) {
				m_SendDelayQ.RemoveNoLock(thepacket);
				CancelList.InsertTailNoLock(thepacket);
			}
	}
	m_SendDelayQ.Unlock();							// protect the loop

	// CancelList contains the packets (if any) with given ID. Abort them.

	while (!CancelList.IsEmpty()) {	
		KNdisPacket p = CancelList.RemoveHead();
		SendComplete(p, NDIS_STATUS_REQUEST_ABORTED);
	}

	return ImDelayBase::OnCancelSendPackets(CancelId);

}
#endif // NDIS51_MINIPORT




///////////////////////////////////////////////////////////////////////
// Process Timer Tick
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
void ImDelayAdapter::OnTimeout()
{
	// Send and Receive

	if (m_SendDelayQ.IsEmpty() && m_ReceiveDelayQ.IsEmpty())
		return;

	TRACE("ImDelayAdapter: Tx Queue: %u, Rx Queue: %u, Pool Usage Tx/Rx: %u/%u\n", 
			m_SendDelayQ.Count(), m_ReceiveDelayQ.Count(),
#ifdef IMDELAY_USE_PRIVATE_POOLS
				m_TxPool.Usage(), m_RxPool.Usage());
#else
				0,0);
#endif //IMDELAY_USE_PRIVATE_POOLS


	PNDIS_PACKET p = m_SendDelayQ.RemoveHead();
	if (p)
		ForwardDown(p);
	p = m_ReceiveDelayQ.RemoveHead();
	if (p)
		ForwardUp(p);
}


// end of file
 
