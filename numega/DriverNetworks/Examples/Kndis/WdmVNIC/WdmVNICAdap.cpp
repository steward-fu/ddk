// WdmVNICAdap.cpp: implementation of the WdmVNICAdapter class.
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
//////////////////////////////////////////////////////////////////////

#include <kndis.h>
#include <kndisvdw.h>		  // DriverWorks
#include <KIrpPool.h>
#include "Characteristics.h"
#include "WdmVNICAdap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WdmVNICAdapter::WdmVNICAdapter() :
	KNdisMiniAdapter(),
		m_HaltFlag(FALSE),
		m_RxIdleEvent(),
		m_RxIrpsOutstanding(),
#if KNDIS_W9X_COMPATIBLE
		m_RxIndicateQueue((ULONG)&((PHW_RFD)0)->m_LE),
#endif // KNDIS_W9X_COMPATIBLE
		m_RxIrpList()
{
	// TODO: initialize your private data members
	//       Do NOT call any NDIS functions in here. Defer to Initialize()

}

WdmVNICAdapter::~WdmVNICAdapter()
{
	// TODO: deallocate all the resources allocated in constructor/Initialize()

}

////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::Initialize
//
// MiniportInitialize is a required function that sets up a NIC (or
// virtual NIC) for network I/O operations, claims all hardware resources
// necessary to the NIC in the registry, and allocates resources the driver
// needs to carry out network I/O operations
//
// Parameters:
//		Medium
//			Reference to KNdisMedium object that we have to set the medium
//			type our NIC supports
//		Config
//			Reference to KNdisConfig object that we use to query various
//			configuration parameters
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// TODO:
// 1) Select supported medium in Medium object
// 2) Read config parameters if any using Config object
// 3) Allocate h/w resources, pools, etc.
// 4) Register with NDIS using SetAttributes()
// 5) Register Shutdown Handler
// 6) Initialize and enable the NIC.
// NOTE: Be ready to process INTs even before the function exits
//
NDIS_STATUS WdmVNICAdapter::Initialize
				(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config)
{
	TRACE("WdmVNICAdapter::Initialize() Entered\n");

	// Select our the Medium:

	if (! Medium.Select(WDMVNIC_MEDIUM_TYPE) )
		KNDIS_RETURN_ERROR (NDIS_STATUS_UNSUPPORTED_MEDIA);

	// Get network address (if any) from the Registry. If specified
	// it will be used instead of burned permanent address:

	if (Config.ReadNetworkAddress(m_CurrentAddress) != NDIS_STATUS_SUCCESS)
		m_CurrentAddress.Invalidate();

	// Read some other config stuff from the Registry, e.g.

	ULONG	uMode;
	Config.Read(KNDIS_STRING_CONST("Mode"), &uMode);

	// Retrieve some initialization parameters from the Registry. Use defaults if none.
	UINT TxIrpPoolSize = Config.Read(KNDIS_STRING_CONST("TxIrpPoolSize"), 8);
	TRACE("WdmVNICAdapter::Initialize() TxIrpPoolSize %d\n", TxIrpPoolSize);

	UINT RxIrpPoolSize = Config.Read(KNDIS_STRING_CONST("RxIrpPoolSize"), 1);
	TRACE("WdmVNICAdapter::Initialize() RxIrpPoolSize %d\n", RxIrpPoolSize);

	UINT BusTimerIntervalMSecs = Config.Read(KNDIS_STRING_CONST("BusTimerIntervalMSecs"), 5000);
	TRACE("WdmVNICAdapter::Initialize() BusTimerIntervalMSecs %d\n", BusTimerIntervalMSecs);

	UINT RxPacketLength = Config.Read(KNDIS_STRING_CONST("RxPacketLength"), 1000);
	TRACE("WdmVNICAdapter::Initialize() RxPacketLength %d\n", RxPacketLength);

	UINT RxRandomize = Config.Read(KNDIS_STRING_CONST("RxRandomize"), (ULONG)0);
	TRACE("WdmVNICAdapter::Initialize() RxRandomize %d\n", RxRandomize);

	UINT MaxRxBurstCount = Config.Read(KNDIS_STRING_CONST("MaxRxBurstCount"), 20);
	TRACE("WdmVNICAdapter::Initialize() MaxRxBurstCount %d\n", MaxRxBurstCount);

    // Now let NDIS know about the BUS the NIC is on. Here's where the NDIS/Adapter-instance
	// handshake takes place. This should happen *before* allocating the h/w resources:
	SetAttributesEx(NdisInterfaceInternal,
		NDIS_ATTRIBUTE_DESERIALIZE
#if NDIS51_MINIPORT
		| NDIS_ATTRIBUTE_SURPRISE_REMOVE_OK // meaning can support surprise removal. See PnpEventNotify()
#endif
		);

	// Get the physical and "TopOfStack" device objects from NDIS for our WDM
	// device.  The "TopOfStack" device object is the device to which we will
	// submit IRPs to talk to the bus driver for our device.  We will use the PDO, along
	// with the "TopOfStack" device object to initialize our lower device object using
	// the DriverWorks class KPnpLowerDevice (or in this case KUsbLowerDevice, which is
	// derived from KPnpLowerDevice).
	GetDeviceProperty(&m_PhysicalDeviceObject, NULL, &m_TopOfStackDeviceObject);

	// Initialize the IRP pools used to service the bus device for Send and Receive
	m_TxIrpPool.Initialize(m_TopOfStackDeviceObject, TxIrpPoolSize);

	// Initialize the context block heap. We'll need one block per irp.  We only
	// need the heap for Tx context blocks, since the Rx context blocks will be
	// allocated in the HW_RFD structures used by the receive area initialized below.
	m_ContextHeap.Initialize(static_cast<USHORT>(TxIrpPoolSize));

#if KNDIS_W9X_COMPATIBLE
	// Initialize a timer that will be used for indicating Rx packets in a Win9x compatible
	// manner.
	m_Timer.Initialize(this, KNDIS_MEMBER_CALLBACK(IndicateCallback));
#endif // KNDIS_W9X_COMPATIBLE

	m_RxBufPool.Initialize(RxIrpPoolSize);

	// This sets up the Receive Frame Area including:
	// Allocating internal software Receive Frame Descriptors
	// Allocating packet and buffer pools
	// Initializing and chaining packets, software RFD, buffers
	// Maintains list of software RFD descriptors
	//
	// This driver's requirement is to further initialize the hardware RFD's
	// see global template specialization DescriptorInitialize(..) which
	// is called for each descriptor
	m_pRxArea = new KNdisSystemReceiveArea< WdmVNICAdapter, HW_RFD >(
		*this,
		RxIrpPoolSize
		);
	m_pRxArea->Initialize();

	// Initialize the bus based lower device object.  Since NDIS owns this device object, we
	// use the form of Initialize() on our KPnpLowerDevice derived class that takes the
	// "TopOfStackDevice" as the device to which calls to the lower device should be directed
	// in lieu of actually attaching to the device stack.
	m_BusDevice.Initialize(m_TopOfStackDeviceObject, m_PhysicalDeviceObject);

	m_BusDevice.SetAdapter(this);

	// Some initialization calls to our bus simulation device.
	m_BusDevice.SetTimerInterval(BusTimerIntervalMSecs);
	m_BusDevice.SetRxPacketLength(RxPacketLength);
	m_BusDevice.SetRxRandomize((RxRandomize != 0));
	m_BusDevice.SetMaxRxBurstCount(MaxRxBurstCount);

	// TODO: Read perm address from the card instead
	m_PermanentAddress = m_CurrentAddress;

	// Set default filter and MAC options. In principle, we should also
	// relay that to our card...
	m_uPacketFilter = NDIS_PACKET_TYPE_DIRECTED |
					  NDIS_PACKET_TYPE_MULTICAST |
					  NDIS_PACKET_TYPE_BROADCAST;

	m_uMacOptions = NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |
				    NDIS_MAC_OPTION_RECEIVE_SERIALIZED  |
					NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
					NDIS_MAC_OPTION_NO_LOOPBACK;

	// Get the Receive IRPs going
	StartReceiveUnit();

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::Halt
//
// MiniportHalt is a required function that deallocates resources when
// the NIC is removed and halts the NIC.
//
// Parameters:
//		None
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE: Miniports has no "unload" for the driver. The Halt() is the last
//       function called into a miniport, so everything should be stopped
//		 and freed right here. The KNDIS framework takes care of destroying
//		 the adapter object itself, so this member function is merery required
//       to undo things done by the Initialize() above - in the inversed order.
//
//		 Alternatevely, the driver writer can move all the deallocations to the
//       destructor: the destructor will be called immediatelly after Halt()
//		 returns.
//
VOID WdmVNICAdapter::Halt(VOID)
{
	TRACE("WdmVNICAdapter::Halt() Entered\n");

	// Set the halt flag
	m_HaltFlag = TRUE;

	// Cancel all of the IRPs in the Rx Irp list.
	PHW_RFD pHwRfd = m_RxIrpList.Head();
	while (pHwRfd)
	{
		IoCancelIrp(pHwRfd->m_pIrp);
		pHwRfd = m_RxIrpList.Next(pHwRfd);
	}

	// Wait for all of the Rx IRPs to be completed
	LARGE_INTEGER DueTime;
	DueTime.QuadPart = -(10000 * 20000);	// 20 seconds
	if ( m_RxIdleEvent.Wait() == FALSE )
	{
		ASSERT(!"All Rx IRPs were not accounted for");
	}

	// Wait for all of the Tx IRPs to be completed
	if ( m_TxIrpPool.Wait(1000) == STATUS_TIMEOUT )	// 20 seconds
	{
		ASSERT(!"All Tx IRPs were not accounted for");
	}

	// Deallocate our receive area.  This will cleanup all of the HW_RFD descriptors
	// and call the DescriptorInvalidate method, which will deallocate the IRP associated with
	// the descriptor in the DescriptorInitialize method.
	TRACE("WdmVNICAdapter::Halt() Deleting Receive Area\n");
	delete m_pRxArea;
	m_pRxArea = NULL;
}

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::Reset
//
// MiniportReset is a required function that issues a hardware reset
// to the NIC and/or resets the driver's software state.
//
// Parameters:
//		AddressingReset
//			Points to a variable that MiniportReset sets to TRUE if the
//			NDIS library should call MiniportSetInformation to restore
//			addressing information to the current values.
// IRQL:
//		DISPATCH_LEVEL
// Return Mode:
//		Asynchronous
NDIS_STATUS WdmVNICAdapter::Reset
			(OUT PBOOLEAN AddressingReset)
{
	TRACE("WdmVNICAdapter::Reset() Entered\n");

	// TODO:	Reset the card

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::Shutdown
//
// MiniportShutdown does nothing more than restore the NIC to its
// initial state (before the miniport's DriverEntry function runs)
//
// Parameters:
//		None
// IRQL:
//		If MiniportShutdown is called due to a user-initiated system shutdown,
//		it runs at IRQL PASSIVE_LEVEL in a system-thread context. If it is called
//		due to an unrecoverable error, MiniportShutdown runs at an arbitrary IRQL
//		and in the context of whatever component raised the error.
// Return Mode:
//		Synchronous
VOID WdmVNICAdapter::Shutdown(VOID)
{
	TRACE("WdmVNICAdapter::Shutdown() Entered\n");

	// TODO:	Reset the card for good
}

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::Send
//
// Transfers a protocol-supplied packet over the network
//
// Parameters:
//		Packet
//			Points to a packet descriptor specifying the data to be transmitted.
//		Flags
//			Specifies the packet flags, if any, set by the protocol.
// IRQL:
//		DISPATCH_LEVEL
// Return Mode:
//		Asynchronous
// NOTE:
//		The return code determines the ownership of the packet and further
//		interaction between NDIS and our driver. Specifically,
//
//      NDIS_STATUS_SUCCESS - Done with.
//      NDIS_STATUS_PENDING - We keep it until further notice.
//      NDIS_STATUS_RESOURCES - We didn't have the resouces *now*, so, NDIS,
//                              please keep it until further notice.
//
// NOTE: Deserialized Miniports should never return NDIS_STATUS_RESOURCES to NDIS,
// and must instead queue it internally.
//
NDIS_STATUS WdmVNICAdapter::Send(
			IN	PNDIS_PACKET			Packet,
			IN	UINT					Flags
			)
{
	TRACE("WdmVNICAdapter::Send() Entered\n");

	if (TxPacket(Packet) == NDIS_STATUS_RESOURCES)
	{
		// We can never return NDIS_STATUS_RESOURCES to NDIS, and must
		// instead queue This packet for the Deserialized Miniport
		m_TxQueue.InsertTail(Packet);
		TRACE("WdmVNICAdapter::Send(): Queueing packet\n");
	}

	// Deserialized drivers always return pending.
	return NDIS_STATUS_PENDING;
}

///////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::TxPacket
//
// Parameters:
// 	PNDIS_PACKET		- Packet to be transmitted
// IRQL:
//		IRQL DISPATCH_LEVEL
// Returns:
// 	NDIS_STATUS
NDIS_STATUS WdmVNICAdapter::TxPacket(
			IN	PNDIS_PACKET			Packet
			)
{
	NDIS_STATUS Status = NDIS_STATUS_PENDING;

	KIrp I = m_TxIrpPool.Allocate();

	if (I.IsNull())
	{
		Status = NDIS_STATUS_RESOURCES;
	}
	else
	{
		// allocate a new context structure
		IRP_CONTEXT* pContext = new (&m_ContextHeap) IRP_CONTEXT (this, Packet);

		// make sure it succeeded
		if ( pContext == NULL )
		{
			m_TxIrpPool.Free(I);
			Status = NDIS_STATUS_RESOURCES;
		}
		else
		{

			// TODO should transfer the packet information to the IRP.  This is highly dependent
			// on how the IRP should look to the lower device.

			I.SetCompletionRoutine(WdmCompletionTxRoutine(this), pContext);
			m_BusDevice.Call(I);
		}
	}

	return Status;
}

///////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::CompletionTxRoutine
//
// Parameters:
// 	pIrp                 - irp being completed
// 	Context              - context passed when the irp had been submitted
// IRQL:
//		IRQL DISPATCH_LEVEL
// Returns:
// 	Usually, STATUS_MORE_PROCESSING_REQUIRED
// NOTE:
// 	Processes asynchronous "send" IRP completions: this is a "HandleInterrupt()"
//  thing for NDIS WDM drivers.So, the logic is similar to HandleInterrupt():
//  If a pended Tx packet gets completed, return the packet to NDIS.
//  The important point is that if the the irps are be recycled back to the pools they
//  came from, STATUS_MORE_PROCESSING_REQUIRED should be returned to the system.
NTSTATUS WdmVNICAdapter::CompletionTxRoutine(PIRP pIrp, WdmVNICAdapter::IRP_CONTEXT* pContext)
{
	TRACE("WdmVNICAdapter::TxPacketComplete() Entered\n");

	// Recycle the irp and the packet

	// Complete the packet we just processed
	SendComplete(pContext->type.m_Packet, NDIS_STATUS_SUCCESS);

	// release all resources associated with this request
	if ( pContext )
	{
		delete pContext;
	}

	// Free the IRP back to our pool
	m_TxIrpPool.Free(pIrp);

	// If any packets are in the queue, dequeue them, send them
	KNdisPacket QueuePacket = m_TxQueue.RemoveHead();
	while (QueuePacket.IsValid())
	{
		TRACE("WdmVNICAdapter::CompletionTxRoutine: Sending queued Tx packet\n");

        // Attempt to put it onto the hardware
        NDIS_STATUS Status = TxPacket(QueuePacket);

        // If there were no resources for this packet, then we'll just have
        // to try and send it later.
        if (Status == NDIS_STATUS_RESOURCES)
        {
            // re-queue the packet
            m_TxQueue.InsertHead(QueuePacket);
			QueuePacket = NULL;
        }
		else
		{
			QueuePacket = m_TxQueue.RemoveHead();
		}
    }

	// return indicates that system is to quit processing IRP completion
	return STATUS_MORE_PROCESSING_REQUIRED;
}

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::ReturnPacket
//
// This method is called when NDIS returns a packet previuosly
// indicated by IndicateReceive() back to the miniport
//
// Parameters:
//		Packet
//			Points to a packet descriptor specifying the packet
// IRQL:
//		DISPATCH_LEVEL
// Return Mode:
//		n/a
// NOTE:
//		The packet returns to the "free list" in the Rx area.
//		Reclaim() does the necessary reset of the chained buffer
//		and OOB data if any.
VOID WdmVNICAdapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	TRACE("WdmVNICAdapter::ReturnPacket() Entered Packet=%p\n", Packet);

	// Allocate a work item off the heap.  This will be used to submit our IRP to the
	// driver managed queue for processing.  This allows the calling thread to
	// continue along merrily.
	PNDIS_WORK_ITEM_CTX pWorkItemCtx = new NDIS_WORK_ITEM_CTX;
	ASSERT(pWorkItemCtx);
	if (pWorkItemCtx) {

		pWorkItemCtx->m_pAdapter = this;
		pWorkItemCtx->m_Packet = Packet;

		NdisInitializeWorkItem(
			&pWorkItemCtx->m_WorkItem,
			OnReturnPacketX,
			pWorkItemCtx
			);


		if (NDIS_STATUS_SUCCESS != NdisScheduleWorkItem(&pWorkItemCtx->m_WorkItem))
			delete pWorkItemCtx;
	}
}

////////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::OnReturnPacket
//
//		Callback handler for the QueueReturnPacketWorkItem
//
// Input
//		pItem		Pointer to our work item
//
// Output
//
// Notes
//

VOID WdmVNICAdapter::OnReturnPacket(PNDIS_WORK_ITEM_CTX pCtx)
{
	ASSERT(pCtx);

	if (pCtx) {

		KNdisPacket Packet(pCtx->m_Packet);

		// Reclaim the packet back into our receive area
		// Note: Can't hold spin lock while calling this method
		m_pRxArea->Reclaim(Packet);

		// Submit the IRP to the bus device
		PHW_RFD pHwRfd = reinterpret_cast<PHW_RFD>(m_pRxArea->GetDescriptor(Packet));
		m_BusDevice.Call(pHwRfd->m_pIrp);

		delete pCtx;
	}
}

VOID WdmVNICAdapter::StartReceiveUnit(void)
{
	PHW_RFD pHwRfd = m_RxIrpList.Head();
	while (pHwRfd)
	{
		// Increment the number of IRPs in play
		++m_RxIrpsOutstanding;

		m_BusDevice.Call(pHwRfd->m_pIrp);
		pHwRfd = m_RxIrpList.Next(pHwRfd);
	}
}

///////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::CompletionRxRoutine
//
// Parameters:
// 	pIrp                 - irp being completed
// 	Context              - context passed when the irp had been submitted
// IRQL:
//		IRQL DISPATCH_LEVEL
// Returns:
// 	Usually, STATUS_MORE_PROCESSING_REQUIRED
// NOTE:
// 	Processes asynchronous "receive" IRP completions: this is a "HandleInterrupt()"
//  thing for NDIS WDM drivers. So, the logic is similar to HandleInterrupt():
//  If a new received packet arrived indicate it to NDIS. Handle status indications, too.
//  The important point is that if the the irps are be recycled back to the pools they
//  came from, STATUS_MORE_PROCESSING_REQUIRED should be returned to the system.
// NOTE: Under Win9X, NDIS packet and status indications must NOT be made from
//       the completion routine context. Instead, the NDIS WDM driver should schedule an
//       NDIS timer callback that would post the indication to NDIS.
NTSTATUS WdmVNICAdapter::CompletionRxRoutine(PIRP pIrp, WdmVNICAdapter::IRP_CONTEXT* pContext)
{
	TRACE("WdmVNICAdapter::RxPacketComplete() Entered\n");

	// Figure from Context/pIrp what has completed: receive or status

	// If the status of the IRP is not STATUS_SUCCESS or the m_HaltFlag is set
	// do not process this IRP.  Instead, decrement the m_RxIrpsOutstanding IRPs
	// and signal the m_RxIdleEvent if the counter reaches zero.
	KIrp I(pIrp);
	if ( I.Status() != STATUS_SUCCESS || m_HaltFlag )
	{
		TRACE("WdmVNICAdapter::CompletionRxRoutine: status = %08x, HaltFlag = %d\n", ULONG(I.Status()), ULONG(m_HaltFlag));
		if(!--m_RxIrpsOutstanding)
		{
			m_RxIdleEvent.Set();
			TRACE("WdmVNICAdapter::CompletionRxRoutine: Setting RxIdleEvent");
		}

		// return indicates that system is to quit processing IRP completion
		return STATUS_MORE_PROCESSING_REQUIRED;
	}

	PHW_RFD pHwRfd = pContext->type.m_pHwRfd;

#if KNDIS_W9X_COMPATIBLE
	// For Win9x, it is not safe to indicate the packet to NDIS from the context of a
	// non-NDIS thread, so we must queue the packet, set an NDIS timer, and indicate the packet
	// from the timer callback
	m_RxIndicateQueue.InsertTail(pHwRfd);
	m_Timer.Set(10);
#else // KNDIS_W9X_COMPATIBLE
	CompleteHwRfd(pHwRfd);
#endif // KNDIS_W9X_COMPATIBLE

	// return indicates that system is to quit processing IRP completion
	return STATUS_MORE_PROCESSING_REQUIRED;
}

#if KNDIS_W9X_COMPATIBLE

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::IndicateCallback
//		Callback routine for the NDIS timer used to indicate packets
//			in a Win9x compliant manner
// Parameters:
//		none
// IRQL:
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		This timer is an NDIS timer, and therefore operates in the context
//			of an NDIS thread.
//
VOID WdmVNICAdapter::IndicateCallback(void)
{
	PHW_RFD pHwRfd = m_RxIndicateQueue.RemoveHead();
	while (pHwRfd)
	{
		CompleteHwRfd(pHwRfd);
        pHwRfd = m_RxIndicateQueue.RemoveHead();
	}
}

#endif // KNDIS_W9X_COMPATIBLE

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::CompleteHwRfd
//		Helper routine to complete the HW_RFD
// Parameters:
//		PHW_RFD	- pointer to the HW_RFD to be completed
// IRQL:
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		This routine is simply common code shared between CompletionRxRoutine
//		and IndicateCallback
//
VOID WdmVNICAdapter::CompleteHwRfd(PHW_RFD pHwRfd)
{

	// Get the current packet based on the HW_RFD pointer stored in the context
	KNdisPacket Packet(m_pRxArea->GetPacket(pHwRfd));
	ASSERT( Packet.IsValid() );

	// Get the current buffer
	KNdisBuffer Buf = Packet.QueryFirstBuffer();
	ASSERT( Buf.IsValid() );

	// Check the Information field to see if the packet is valid.  If so, indicate the packet, otherwise
	// recycle the HW_RFD now.
	//
	// Determining if a completed IRP contains a valid packet is dependent on specifics of
	// the lower device
	KIrp I(pHwRfd->m_pIrp);
	if (I.Information())
	{

		// The simulator saves the buffer length in the information field.  Store
		// this in the HW_RFD
		pHwRfd->m_RfdActualCount = (ULONG)I.Information();

		// Remove the RFD from the free list
		m_pRxArea->Complete(pHwRfd);

		// Get the packet length
		ULONG FrameLength = pHwRfd->m_RfdActualCount;
		ASSERT( FrameLength <= MAXIMUM_ETHERNET_PACKET_SIZE );
		TRACE("WdmVNICAdapter::CompleteHwRfd Received packet length %d\n", FrameLength);

		// Adjust the buffer length for this sw rfd
		Buf.AdjustLength(FrameLength);

		// Indicate the packet to NDIS
		IndicateReceive(Packet);
	}
	else
	{
		// Reclaim the packet back into our receive area
		m_pRxArea->Reclaim(Packet);

		// Submit the IRP to the bus device
		m_BusDevice.Call(pHwRfd->m_pIrp);
	}
}

////////////////////////////////////////////////////////////////////
// WdmVNICAdapter::CheckForHang
//		Optional function that reports the state of the NIC or monitors
//		 the responsiveness of an underlying device driver.
// Parameters:
//		none
// IRQL:
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		By default, the NDIS library calls MiniportCheckForHang
//		approximately every two seconds. If MiniportCheckForHang returns
//		TRUE, NDIS then calls the driver's MiniportReset function.
BOOLEAN WdmVNICAdapter::CheckForHang()
{
	// TODO:	Check the h/w for hang
	return FALSE;
}

#if defined(NDIS51_MINIPORT)
///////////////////////////////////////////////////////////////////////
// Notify on PNP or Power Event (5.1)
//
// Parameters:
// 	PnPEvent             - PNP Event Type
// 	InformationBuffer    - Buffer With Extra info
// 	InformationBufferLength - Size of the buffer
// Returns:
// 	none
// Comments:
// 	See DDK MiniportPnPEventNotify
VOID WdmVNICAdapter::PnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
		    )
{
	switch(PnPEvent) {
	case NdisDevicePnPEventSurpriseRemoved:
		// Surprise Removal
		// "When a miniport driver receives notification of a surprise removal, it should note internally
		// that the device has been removed and cancel any pending IRPs that it sent down to the underlying
		// bus driver. After calling the MiniportPnPEventNotify function to indicate the surprise removal,
		// NDIS calls the miniport's MiniportHalt function. If the miniport driver receives any requests to send
		// packets or query or set OIDs before its MiniportHalt function is called, it should immediately complete
		// such requests with a status value of NDIS_STATUS_NOT_ACCEPTED"
		//
		// @@TODO: Figure what exactly we must do here.
		//         The Halt() method takes care of cancelling the IRPs.
		//		   So, should we do that here now? Or it is OK no still do it on Halt()?
		//		   What's the point of this notification except we know WHY
		//		   the adapter is being Halted...
		//
		TRACE("WdmVNICAdapter::PnPEventNotify: Surprised Removal!\n");
		break;

	case NdisDevicePnPEventPowerProfileChanged:
		//
		// Power change notification.
		// The "AC" notification also triggers when the computer exits the standby mode.
		// TODO: Apply some operational mode adjustments if needed.
		//       E.g., reduce power consumption when switching to battery.
		//
		{
		bool bOnBattery = (*PULONG(InformationBuffer))==NdisPowerProfileBattery;
		TRACE("WdmVNICAdapter::PnPEventNotify: Running on %s\n", (bOnBattery)?"Batterry":"AC");
		}
		break;
	default:
		//
		// Everything else is non-documented in XP DDK
		//
		TRACE("WdmVNICAdapter::PnPEventNotify: Event %u\n", PnPEvent);
		break;
	}
}

///////////////////////////////////////////////////////////////////////
// Cancel Packets Being Sent (5.1)
//
// Parameters:
// 	CancelId             - An identifier specifying the packet or group
//						  of packets whose transmission is being canceled.
// Returns:
// 	none
// Comments:
// 	Walk the list of not-yet-transmitted packets and cancel them
//
VOID WdmVNICAdapter::CancelSendPackets(
			IN PVOID	CancelId
			)
{
	TRACE("WdmVNICAdapter::CancelSendPackets ID=%p\n", CancelId);

	// m_TxQueue contains the list of packets waiting for resources (IRPs, buffers)
	// to become available. Search for packets with the specified ID.

	KNdisPacketList CancelList;

	m_TxQueue.Lock();							// protect the loop
	PNDIS_PACKET p = m_TxQueue.HeadNoLock();
	while (p)  {
			PNDIS_PACKET thepacket = p;
			p = m_TxQueue.NextNoLock(p);
			if (NDIS_GET_PACKET_CANCEL_ID(thepacket)==CancelId) {
				m_TxQueue.RemoveNoLock(thepacket);
				CancelList.InsertTailNoLock(thepacket);
			}
	}
	m_TxQueue.Unlock();							// protect the loop

	// CancelList contains the packets (if any) with given ID. Abort them.

	while (!CancelList.IsEmpty()) {
		KNdisPacket p = CancelList.RemoveHead();
		SendComplete(p, NDIS_STATUS_REQUEST_ABORTED);
	}

};

#endif // NDIS51_MINIPORT

// Descriptor Management

VOID WdmVNICAdapter::DescriptorInitialize(
	PHW_RFD pHwDesc,					//The descriptor to initialize
	PNDIS_PHYSICAL_ADDRESS pPhysAddr	//Pointer to the physical address of the descriptor
	)
{
    TRACE("WdmVNICAdapter::DescriptorInitialize\n");

	// Called from KNdisSharedReceiveArea ctor on initial descriptor chain setup
	// for each hardware Receive Frame Descriptor (FRD)
	// must initialize the hardware Receive Frame Descriptor

	// Allocate an IRP for this descriptor
	KIrp I = KIrp::Allocate(m_TopOfStackDeviceObject->StackSize);

	if (I.IsNull())
	{
		ASSERT("No IRPs left in Rx Pool");
		return;
	}

	pHwDesc->m_pIrp = I;
	pHwDesc->m_IrpContext.Adapter = this;
	pHwDesc->m_IrpContext.type.m_pHwRfd = pHwDesc;		// back pointer to our HW_RFD
	pHwDesc->m_RfdActualCount= 0;
	pHwDesc->m_RfdSize = sizeof(ETH_RX_BUFFER_STRUC);

	// Set the completion routine on the IRP and submit it to the bus device
	I.SetCompletionRoutine(WdmCompletionRxRoutine(this), &pHwDesc->m_IrpContext);

	// Allocate an NDIS_BUFFER describing the RfdBuffer in the HW_RFD structure.  We will attach
	// this MDL to the IRP.
	//
	// Note that the method in which data is attached to the IRP, and the format of that data
	// is dependent on the particular system bus and device properties.  In this case we are
	// simply using the same buffer that the receive area will be using and assuming that the
	// data will be in TCP/IP format, ready for indication by the miniport.
	KNdisBuffer Buf = m_RxBufPool.Allocate(&pHwDesc->m_RfdBuffer ,pHwDesc->m_RfdSize);
	if (!Buf.IsValid())
	{
		ASSERT("Cannot allocate memory for NDIS_BUFFER");
		KIrp::Deallocate(I);
		return;
	}

	I.Mdl() = (PNDIS_BUFFER) Buf;

	// Add the IRP to the RxIrp list
	m_RxIrpList.InsertTail(pHwDesc);

	DumpHwRfd(pHwDesc);
}

VOID WdmVNICAdapter::DescriptorInvalidate(
	PHW_RFD pHwDesc						//The descriptor to inivalidate
	)
{
	// Free the MDL we associated with the IRP
	KIrp I(pHwDesc->m_pIrp);

	KNdisBuffer Buf((PNDIS_BUFFER)I.Mdl());
	if (Buf.IsValid()) {
		m_RxBufPool.Free(Buf);
	}

	I.Mdl() = NULL;

	// Free the IRP
	KIrp::Deallocate(I);
}

VOID WdmVNICAdapter::DescriptorComplete(
	PHW_RFD pHwDesc,					//The descriptor to initialize
	PNDIS_PHYSICAL_ADDRESS pPhysAddr	//Pointer to the physical address of the descriptor
	)
{
    TRACE("WdmVNICAdapter::DescriptorComplete\n");
	DumpHwRfd(pHwDesc);
}


VOID WdmVNICAdapter::DescriptorReclaim(
	PHW_RFD pHwDesc,					//The descriptor to initialize
	PNDIS_PHYSICAL_ADDRESS pPhysAddr	//Pointer to the physical address of the descriptor
	)
{
    TRACE("WdmVNICAdapter::DescriptorReclaim\n");

	// Called from KNdisSystemReceiveArea Reclaim method
	// for each hardware Receive Frame Descriptor (HW_RFD) we
	// must re-initialize

	// We are done with this RFD so re-initialize it.
	pHwDesc->m_RfdActualCount= 0;

	// Reinitialize the IRP, saving the MDL so it can be reassociated with the IRP after
	// the IRP has been reset
	KIrp I(pHwDesc->m_pIrp);
	KMemory mem(I.Mdl());
	I.Reuse();

	// Set the completion routine on the IRP and submit it to the bus device
	I.SetCompletionRoutine(WdmCompletionRxRoutine(this), &pHwDesc->m_IrpContext);
	I.Mdl() = mem;

	DumpHwRfd(pHwDesc);
}

VOID WdmVNICAdapter::DumpHwRfd(PHW_RFD p)
{
//    TRACE("   RFD_STRUC* p =%lx\n", p);
//    TRACE("		p->m_pIrp=%lx\n",		 p->m_pIrp);
}

// end of file
 