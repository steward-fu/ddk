// I1394Adap.cpp: implementation of the I1394Adapter class.
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

#include "I1394Adap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
I1394Adapter::I1394Adapter()
{
}

I1394Adapter::~I1394Adapter()
{
}

////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// I1394Adapter::Initialize
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
NDIS_STATUS I1394Adapter::Initialize
				(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config)
{
	TRACE("I1394Adapter::Initialize() Entered\n");

	m_bHalt = false;
	m_nOutstandingRecvPackets = 1;
	m_NoOutstandingRecvEvent.Reset();

	NDIS_STATUS status;

	// Select our the Medium:
	if ( !Medium.Select(I1394_MEDIUM_TYPE) )
	{
		KNDIS_RETURN_ERROR(NDIS_STATUS_UNSUPPORTED_MEDIA);
	}

	// Get network address (if any) from the Registry. If specified
	// it will be used instead of burned permanent address:
	if ( Config.ReadNetworkAddress(m_CurrentAddress) != NDIS_STATUS_SUCCESS )
	{
		m_CurrentAddress.Invalidate();
	}

	// Read some other config stuff from the Registry, e.g.
	ULONG uMode;
	Config.Read(KNDIS_STRING_CONST("Mode"), &uMode);

	// Get the maximum Transmit Irp Pool size
	m_TxIrpPoolSize = Config.Read(KNDIS_STRING_CONST("TxIrpPoolSize"), 32);
	TRACE("I1394Adapter::Initialize() TxIrpPoolSize %d\n", m_TxIrpPoolSize);

    // Now let NDIS know about the BUS the NIC is on. Here's where the NDIS/Adapter-instance
	// handshake takes place. This should happen *before* allocating the h/w resources:
	SetAttributesEx(NdisInterfaceInternal, NDIS_ATTRIBUTE_DESERIALIZE);

	// NDIS WDM Specific
	//
 	// Get the physical and "TopOfStack" device objects from NDIS for our WDM
	// device.  The "TopOfStack" device object is the device to which we will
	// submit IRPs to talk to the bus driver for our device.  We will use the PDO, along
	// with the "TopOfStack" device object to initialize our lower device object using
	// a DriverWorks's KPnpLowerDevice-derived class (such as KUsbLowerDevice).
	GetDeviceProperty(&m_PhysicalDeviceObject, NULL, &m_TopOfStackDeviceObject);

	// Initialize the IRP pools used to service the bus device for Send
	// NOTE: the wizard generates hard-coded values for the pool sizes. You
	// might want to use Config to read those values from the Registry, or use
	// some other way of allocating and handling IRPs, e.g. not using the pools at all.
	m_TxIrpPool.Initialize(m_TopOfStackDeviceObject, m_TxIrpPoolSize);

	// Initialize the context block heap. We'll need one block per irp:
	m_ContextHeap.Initialize((USHORT)m_TxIrpPoolSize);

	// Initialize the lower device object.  Since NDIS owns this device object, we
	// use the form of Initialize() that takes the "TopOfStackDevice" as the device to which
	// calls to the lower device should be directed in lieu of actually attaching to the
	// device stack.
	m_1394BusDevice.Initialize(m_TopOfStackDeviceObject, m_PhysicalDeviceObject);
	K1394AsyncClient::Initialize(&m_1394BusDevice);
	m_GenerationCount.Initialize(&m_1394BusDevice);

	// We will generate permanent address from the Config ROM GUID
	LARGE_INTEGER guid = m_1394BusDevice.HostUniqueID();
	m_PermanentAddress.Invalidate();
	NdisMoveMemory(m_PermanentAddress.m_bytes + 1, &guid, 5);

	// If there is no registry override use permanent address as the current one
	if ( !m_CurrentAddress.IsValid() )
	{
		m_CurrentAddress = m_PermanentAddress;
	}

	// Set default filter and MAC options.
	m_uPacketFilter = NDIS_PACKET_TYPE_DIRECTED |
					  NDIS_PACKET_TYPE_MULTICAST |
					  NDIS_PACKET_TYPE_BROADCAST;

	m_uMacOptions = NDIS_MAC_OPTION_RECEIVE_SERIALIZED  |
					NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
					NDIS_MAC_OPTION_NO_LOOPBACK;

	// Determine the maximum frame size that the hardware can support
	ULONG CapabilityFlags = 0xffff;
	status = m_1394BusDevice.HostCapabilities(&CapabilityFlags, &m_MaxAsyncReadSizeHW, &m_MaxAsyncWriteSizeHW);

	// Limit the size of a frame
	m_MaxAsyncWriteSize = min(min(m_MaxAsyncWriteSizeHW, m_MaxAsyncReadSizeHW), RCB_BUFFER_SIZE);

	if( !NT_SUCCESS(status) )
	{
		KNDIS_RETURN_ERROR(STATUS_INSUFFICIENT_RESOURCES);
	}

	// This sets up the Receive Frame Area including:
	// Allocating internal software Receive Frame Descriptors
	// Allocating packet and buffer pools
	// Initializing and chaining packets, software RFD, buffers
	// Maintains list of software RFD descriptors
	//
	// This driver's requirement is to further initialize the RFD's
	// see global template specialization DescriptorInitialize(..) which
	// is called for each descriptor
	m_pRxArea = new KNdisSystemReceiveArea<I1394Adapter, WDM_RFD>(*this, m_TxIrpPoolSize);
	if ( !m_pRxArea )
	{
		KNDIS_RETURN_ERROR(STATUS_INSUFFICIENT_RESOURCES);
	}
	m_pRxArea->Initialize();

	// Get the Link Speed
	ULONG ulLinkSpeed;
	status = m_1394BusDevice.GetSpeedBetweenDevices(1,
													&m_TopOfStackDeviceObject,
													USE_LOCAL_NODE,
													&ulLinkSpeed);
	if( NT_SUCCESS(status) )
	{
		switch ( ulLinkSpeed )
		{
		case SPEED_FLAGS_100:
			m_uLinkSpeed = 100000000;
			break;
		case SPEED_FLAGS_200:
			m_uLinkSpeed = 200000000;
			break;
		case SPEED_FLAGS_400:
			m_uLinkSpeed = 400000000;
			break;
		default:
			m_uLinkSpeed = I1394_MAX_LINKSPEED;
			break;
		}
	}
	else
	{
		m_uLinkSpeed = I1394_MAX_LINKSPEED;
	}

	// Allocate the address range
	// For now use a default size of m_MaxAsyncWriteRequest bytes for the
	// length of the buffer. Use a hard coded address of 0x100000000.
	m_HardCodedOffset.Off_High = 0x1;
	m_HardCodedOffset.Off_Low = 0x0;

	status =  K1394AsyncClient::Allocate(
									m_AddressFifo,
									m_MaxAsyncWriteSize,
									m_HardCodedOffset,
									ACCESS_FLAGS_TYPE_WRITE,
									FALSE
									);

	if ( !NT_SUCCESS(status) )
	{
		KNDIS_RETURN_ERROR(STATUS_INSUFFICIENT_RESOURCES);
	}

#if BINARY_COMPATIBLE
	m_PacketIndicate.Initialize(this);
#endif

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// I1394Adapter::Halt
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
//
VOID I1394Adapter::Halt(VOID)
{
	TRACE("I1394Adapter::Halt() Entered\n");

	m_bHalt = true;

	IndicateStatus(NDIS_STATUS_MEDIA_DISCONNECT);

	// Cancel all pended Tx Irps
	if ( m_TxIrpPool.Wait(1000) == STATUS_TIMEOUT )
	{
		ASSERT(!"All Tx Irps were not accounted for.\n");
	}

	m_RecvLock.Lock();
	m_nOutstandingRecvPackets--;
	ULONG nOutstandingRecvPackets = m_nOutstandingRecvPackets;
	m_RecvLock.Unlock();

	if ( nOutstandingRecvPackets )
	{
		while ( true )
		{
			if ( m_NoOutstandingRecvEvent.Wait(1000) == STATUS_TIMEOUT )
			{
				TRACE("I1394Adapter still waiting to reclaim all Recv Packets\n");
			}
			else
			{
				break;
			}
		}
	}

	// Destroy the 1394 backing store elements
	while (PADDRESS_FIFO pFifoElement = m_AddressFifo.Pop() )
	{
	}

	K1394AsyncClient::Free(m_AddressFifo);
	m_GenerationCount.Deinitialize();

	// destroy the receice area
	delete m_pRxArea;
	m_pRxArea = NULL;
}

////////////////////////////////////////////////////////////////////
// I1394Adapter::Reset
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
NDIS_STATUS I1394Adapter::Reset(
	OUT PBOOLEAN AddressingReset
	)
{
	TRACE("I1394Adapter::Reset() Entered\n");
	// TODO:	Reset the card
	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// I1394Adapter::Shutdown
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
VOID I1394Adapter::Shutdown(VOID)
{
	TRACE("I1394Adapter::Shutdown() Entered\n");
	// TODO:	Reset the card for good
}


////////////////////////////////////////////////////////////////////
// I1394Adapter::Send
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
//      NDIS_STATUS_RESOURCES - We didn't have the resouces *now*, so tell NDIS
//                              please hold it on until further notice.
//
NDIS_STATUS I1394Adapter::Send(
			IN	PNDIS_PACKET	Packet,
			IN	UINT			Flags
			)
{
	TRACE("I1394Adapter::Send() Entered\n");

	if ( !m_bHalt )
	{
		NDIS_STATUS status;

		// Attempt to transmit the packet
		status  = Transmit(Packet);

		// If the system was unable to transmit the packet
		// queue it for later attempts
		if ( status == NDIS_STATUS_RESOURCES )
		{
			m_TxQueue.InsertTail(Packet);
		}
	}

	return NDIS_STATUS_PENDING;
}

////////////////////////////////////////////////////////////////////
// I1394Adapter::ReturnPacket
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
//
//		Reclaim() does the necessary reset of the chained buffer
//		and OOB data if any.
VOID I1394Adapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	TRACE("I1394Adapter::ReturnPacket() Entered Packet=%p\n, Packet");

	m_RecvLock.LockDpr();

	ASSERT( m_nOutstandingRecvPackets > 0 );

	m_nOutstandingRecvPackets--;
	// Give the packet descriptor back to the receive area
	m_pRxArea->Reclaim(Packet);

	ULONG nOutstandingRecvPackets = m_nOutstandingRecvPackets;

	m_RecvLock.UnlockDpr();

	if ( !nOutstandingRecvPackets )
	{
		m_NoOutstandingRecvEvent.Set();
	}
}

////////////////////////////////////////////////////////////////////
// I1394Adapter::CheckForHang
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
BOOLEAN I1394Adapter::CheckForHang()
{
	// TODO:	Check the h/w for hang
	return FALSE;
}

///////////////////////////////////////////////////////////////////////
// I1394Adapter::CompletionTxRoutine
//
// Parameters:
// 	pIrp                 - irp being completed
// 	Context              - context passed when the irp had been submitted
// IRQL:
//		IRQL DISPATCH_LEVEL
// Returns:
// 	Usually, STATUS_MORE_PROCESSING_REQUIRED
// Comments:
//
NTSTATUS I1394Adapter::CompletionTxRoutine(PIRP pIrp, I1394Adapter::IRP_CONTEXT* Context)
{

	TRACE("I1394Adapter::CompletionTxRoutine() Entered\n");

	KIrp I(pIrp);

	NTSTATUS status = I.Status();
	TRACE("IRP TX completion status = %08X\n", status);

	if ( NT_SUCCESS(status) )
	{
		// Update the transmit stats
		m_GenStats->xmit_ok++;

		status = NDIS_STATUS_SUCCESS;
	}
	else
	{
		m_GenStats->xmit_error++;

		status = NDIS_STATUS_FAILURE;
	}

	// Complete the packet we just processed
	SendComplete(Context->m_Packet, status);

	// Put the Irp to put back to the Transmit Irp pool
	m_TxIrpPool.Free(pIrp);
	// Destroy the Mdl
	IoFreeMdl(Context->m_pTxMdl);
	// Destroy the Context
	delete Context;

	// If Packets are in the Queue, dequeue them and send them
	KNdisPacket QueuedPacket = m_TxQueue.RemoveHead();
	while ( QueuedPacket.IsValid() )
	{
		TRACE("Sending Queued Tx Packets\n");
		// Reattempt to transmit previously queued packets
		NDIS_STATUS status = Transmit(QueuedPacket);
		if ( status == NDIS_STATUS_RESOURCES )
		{
			// The packet can not be transmited because of a lack of resources
			// so requeue it for later
			m_TxQueue.InsertHead(QueuedPacket);
			break;
		}
		else
		{
			// Get the next queued packet
			QueuedPacket = m_TxQueue.RemoveHead();
		}
	}

	// return indicates that system is to quit processing IRP completion
	return STATUS_MORE_PROCESSING_REQUIRED;
}

///////////////////////////////////////////////////////////////////////
// I1394Adapter::OnWrite
//
// Parameters:
// 	pMdl				- Mdl containing the Rx data
// 	Offset				- Offset into the buffer
//  Length				- Size of data Rx'ed
//  pAddressFifo		- Backing store FIFO pointer
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	None
// Comments:
// 	This routine is called by the AsyncClient to indicate that data was
//  received in the backing stored MDL. The data must be retrived and
//  indicated to NDIS.
//
VOID I1394Adapter::OnWrite(PMDL pMdl, ULONG Offset, ULONG Length, PADDRESS_FIFO pAddressFifo)
{
	TRACE("I1394Adapter::OnWrite Entered\n");
	TRACE("pMdl = %p, Offset = %x, Length = %x, pAddressFifo = %p\n", pMdl, Offset, Length, pAddressFifo);

	KNdisPacket Packet;
	bool bRecv = false;

	m_RecvLock.Lock();

	if ( !m_bHalt )
	{
		// Check to see if the length and offset are in check
		if ( (Length > m_MaxAsyncWriteSize) || (Offset != 0) )
		{
			// Replenish the backing store of Mdl's
			m_AddressFifo.Push(pAddressFifo);
		}
		else
		{
			PWDM_RFD pRfd = (PWDM_RFD)((UCHAR*)pAddressFifo - FIELD_OFFSET(WDM_RFD, m_FifoElement));

			// Complete the associated RFD
			Packet = m_pRxArea->Complete(pRfd);

			ASSERT( Packet.IsValid() );

			// Extract the proper buffer associated to the descriptor
			KNdisBuffer buf = Packet.QueryFirstBuffer();

			buf.AdjustLength(Length);
			Packet.Recalculate();

			// Update the receive stats
			m_GenStats->rcv_ok++;
			m_nOutstandingRecvPackets++;
			bRecv = true;
		}
	}

	m_RecvLock.Unlock();

	if ( bRecv )
	{
#if BINARY_COMPATIBLE
		m_PacketIndicate.ScheduleReceiveIndicate(Packet);
#else
		// Indicate the Packet to NDIS
		IndicateReceive(Packet);
#endif
	}
}

#if BINARY_COMPATIBLE
///////////////////////////////////////////////////////////////////////
// I1394Adapter::ProcessReceiveIndicate
//
// Parameters:
//  Packet			- Packet to indicate
//  Context			- Anything you wish to carry forward that is not
//					  resident in the packet.
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	None
// Comments:
// 	This routine is used to perform the actual indication when the timer
// has expired.
//
VOID I1394Adapter::ProcessReceiveIndicate(PNDIS_PACKET Packet)
{
	// Indicate the Packet to NDIS
	IndicateReceive(Packet);
}
#endif

///////////////////////////////////////////////////////////////////////
// I1394Adapter::Transmit
//
// Parameters:
//  Packet			- Packet supplied by NDIS to transmit
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NDIS_STATUS
// Comments:
// 	This routine compliments the Send() method. It does the necessary
//  checking and build the IRB that must be transmitted. A status of
//  STATUS_INSUFFICIENT_RESOURCES is returned to Send() so that Send
//  knows to queue the packet for transmission when resources are less
//  scarse.
//
NDIS_STATUS I1394Adapter::Transmit(PNDIS_PACKET Packet)
{
	NDIS_STATUS status = NDIS_STATUS_RESOURCES;

	// Get an available IRP from the Tx Irp pool
	KIrp I = m_TxIrpPool.Allocate();
	// Validate the Irp
	if ( !I.IsNull() )
	{
		// Allocate and initialize the context block:
		IRP_CONTEXT* Context = new (&m_ContextHeap)IRP_CONTEXT(this, Packet);
		// Validate the new context
		if( Context )
		{
			// declare for convenience
			KNdisPacket pkt(Packet);
			KNdisBuffer buf = pkt.QueryFirstBuffer();

			// Copy the data to be transmitted to the transfer buffer
			// Also calculate the new size
			ULONG count = 0;
			while ( buf.IsValid() )
			{
				ULONG size = buf.Length();

				// Copy partial packet buffer to Transfer buffer
				NdisMoveMemory(&Context->m_TxBuffer[count], buf.Address(), size);
				count += size;

				buf = buf.GetNext();
			}

			// Create an Mdl to describe the transfer buffer
			Context->m_pTxMdl = IoAllocateMdl(&Context->m_TxBuffer, count, FALSE, FALSE, NULL);

			if ( Context->m_pTxMdl )
			{
				MmBuildMdlForNonPagedPool(Context->m_pTxMdl);

				// Initialize the Irb with the Mdl to be transmitted
				Context->Irb.Initialize(m_GenerationCount, 1, 0, Context->m_pTxMdl);

				// Pass down the IRB to the bus driver
				status = m_1394BusDevice.SubmitIrb	(
											I,
											Context->Irb,
											WdmCompletionTxRoutine(this),
											Context
											);

				// real status will be returned in completion routine
				return NDIS_STATUS_SUCCESS;
			}

			delete Context;
		}

		m_TxIrpPool.Free(I);
	}

	return status;

}

///////////////////////////////////////////////////////////////////////
// I1394Adapter::DescriptorInitialize
//
// Parameters:
//  pWdmDesc			- The descriptor to initialize
//	pPhysAddr			- Physycal Address Pointer
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NONE
// Comments:
//	Called by KNdisSystemReceiveArea to initialize each descriptor.
//	This routine is called for each descriptor.
//
VOID I1394Adapter::DescriptorInitialize(
	PWDM_RFD				pWdmDesc,
	PNDIS_PHYSICAL_ADDRESS	pPhysAddr
	)
{
    TRACE("I1394Adapter::DescriptorInitialize %p\n", pWdmDesc);

	// The device will use the address range to write data to the host.
	// From the host's perspective this will be constitute a read operation
	// on the device.  Since the device can write faster than we may be able
	// to process, we will set up the address range to use ADDRESS_FIFO structures
	// to buffer the data.  Each ADDRESS_FIFO structure must be backed by a
	// buffer and Mdl.  Set that up here and attach them to the address range.

	pWdmDesc->m_FifoElement.FifoMdl = IoAllocateMdl(&pWdmDesc->m_RfdBuffer, m_MaxAsyncWriteSize, FALSE, FALSE, NULL);
	if ( pWdmDesc->m_FifoElement.FifoMdl )
	{
		MmBuildMdlForNonPagedPool(pWdmDesc->m_FifoElement.FifoMdl);
		m_AddressFifo.Push(&pWdmDesc->m_FifoElement);
	}

//	DumpWdmRfd(pWdmDesc);
}

///////////////////////////////////////////////////////////////////////
// I1394Adapter::DescriptorInvalidate
//
// Parameters:
//  pWdmDesc			- The descriptor to invalidate
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NONE
// Comments:
// 	Called by KNdisSystemReceiveArea to undo any initializations done
//	during the Initialize step. The look aside list is flushed.
//	This routine is called for each descriptor.
//
VOID I1394Adapter::DescriptorInvalidate(
	PWDM_RFD pWdmDesc
	)
{
    TRACE("I1394Adapter::DescriptorInvalidate %p\n", pWdmDesc);

	if ( pWdmDesc->m_FifoElement.FifoMdl )
	{
		IoFreeMdl(pWdmDesc->m_FifoElement.FifoMdl);
		pWdmDesc->m_FifoElement.FifoMdl = NULL;
	}
}

///////////////////////////////////////////////////////////////////////
// I1394Adapter::DescriptorComplete
//
// Parameters:
//  pWdmDesc			- The descriptor to complete
//	pPhysAddr			- Physycal Address Pointer
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NONE
// Comments:
//
VOID I1394Adapter::DescriptorComplete(
	PWDM_RFD				pWdmDesc,
	PNDIS_PHYSICAL_ADDRESS	pPhysAddr
	)
{
    TRACE("I1394Adapter::DescriptorComplete %p\n", pWdmDesc);
//	DumpWdmRfd(pWdmDesc);
}


///////////////////////////////////////////////////////////////////////
// I1394Adapter::DescriptorReclaim
//
// Parameters:
//  pWdmDesc			- The descriptor to reclaim
//	pPhysAddr			- Physycal Address Pointer
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NONE
// Comments:
//	The packet descriptor returns to the "free list" in the Rx area.
//	This routine is called for each descriptor.
//
VOID I1394Adapter::DescriptorReclaim(
	PWDM_RFD pWdmDesc,
	PNDIS_PHYSICAL_ADDRESS pPhysAddr
	)
{
    TRACE("I1394Adapter::DescriptorReclaim %p\n", pWdmDesc);

	if ( !m_bHalt )
	{
		// Replenish the backing store of Mdl's
		m_AddressFifo.Push(&pWdmDesc->m_FifoElement);
	}

//	DumpWdmRfd(pWdmDesc);
}

///////////////////////////////////////////////////////////////////////
// I1394Adapter::DumpWdmRfd
//
// Parameters:
//  pWdmDesc			- The descriptor to display
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NONE
// Comments:
//	Display some descriptor data in the softice window.
//
VOID I1394Adapter::DumpWdmRfd(PWDM_RFD p)
{
    TRACE("I1394Adapter::DumpWdmRfd\n");
    TRACE("   RFD_STRUC* p =%p\n", p);
}

// end of file
 