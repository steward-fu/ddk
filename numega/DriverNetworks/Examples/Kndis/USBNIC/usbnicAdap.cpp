// USBNICAdap.cpp: implementation of the USBNICAdapter class.
//====================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2002 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//====================================================================

#include "USBNICAdap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

USBNICAdapter::USBNICAdapter() :
		KNdisMiniAdapter(),
		m_HaltFlag(FALSE),
		m_RxIdleEvent(),
		m_RxIrpsOutstanding(0),
		m_RxIrpList()
{
}

USBNICAdapter::~USBNICAdapter()
{
}

////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// USBNICAdapter::Initialize
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
NDIS_STATUS USBNICAdapter::Initialize
				(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config)
{
	TRACE("USBNICAdapter::Initialize() Entered\n");

	// Select our the Medium:
	if (! Medium.Select(USBNIC_MEDIUM_TYPE) )
		KNDIS_RETURN_ERROR (NDIS_STATUS_UNSUPPORTED_MEDIA);

	// Get network address (if any) from the Registry. If specified
	// it will be used instead of burned permanent address:
	if (Config.ReadNetworkAddress(m_CurrentAddress) != NDIS_STATUS_SUCCESS)
		m_CurrentAddress.Invalidate();

	// Read some other config stuff from the Registry, e.g.
	// This is simply an example of how to do it. The value of uMode is never used
	ULONG	uMode;
	Config.Read(KNDIS_STRING_CONST("Mode"), &uMode);

	// Now let NDIS know about the BUS the NIC is on. Here's where the NDIS/Adapter-instance
	// handshake takes place. This should happen *before* allocating the h/w resources:
	SetAttributesEx(NdisInterfaceInternal,NDIS_ATTRIBUTE_DESERIALIZE);

	// NDIS WDM Specific
	//
	// Get the physical and "TopOfStack" device objects from NDIS for our WDM
	// device.  The "TopOfStack" device object is the device to which we will
	// submit IRPs to talk to the bus driver for our device.  We will use the PDO, along
	// with the "TopOfStack" device object to initialize our lower device object using
	// a DriverWorks's KPnpLowerDevice-derived class (such as KUsbLowerDevice).
	GetDeviceProperty(&m_PhysicalDeviceObject, NULL, &m_TopOfStackDeviceObject);

	// Initialize the IRP pools used to service the bus device for Send and Receive
	// NOTE: the wizard generates hard-coded values for the pool sizes. You
	// might want to use Config to read those values from the Registry, or use
	// some other way of allocating and handling IRPs, e.g. not using the pools at all.
	m_TxIrpPool.Initialize(m_TopOfStackDeviceObject, USBNIC_TX_IRPPOOL_SIZE);

	// Initialize a pool of receive Irps
	m_RxIrpPool.Initialize(m_TopOfStackDeviceObject, USBNIC_RX_IRPPOOL_SIZE);

	// Initialize a pool of control Irps
	m_CntrlIrpPool.Initialize(m_TopOfStackDeviceObject, USBNIC_CNTRL_IRPPOOL_SIZE);

	// Initialize the context block heap. We'll need one block per irp:
	m_ContextHeap.Initialize(USBNIC_TX_IRPPOOL_SIZE + USBNIC_RX_IRPPOOL_SIZE + USBNIC_CNTRL_IRPPOOL_SIZE);

	// Initialize the lower device object.  Since NDIS owns this device object, we
	// use the form of Initialize() that takes the "TopOfStackDevice" as the device to which
	// calls to the lower device should be directed in lieu of actually attaching to the
	// device stack.
	m_BusDevice.Initialize(m_TopOfStackDeviceObject, m_PhysicalDeviceObject);

	// Initialize the interface and pipe object and activate a device configuration.
	// NOTE: This is a SAMPLE code =>
	// TODO: Modify the code to match your device specification.
	m_Interface.Initialize(
		m_BusDevice,	// KUsbLowerDevice instance
		0,		// Interface Number
		1,		// Configuration Value
		0		// Initial Interface Alternate Setting
		);

	m_ReceivePipe.Initialize(m_BusDevice,0x81);
	m_SendPipe.Initialize(m_BusDevice,0x2);
	// The Interrupt pipe is NOT used
	m_InterruptPipe.Initialize(m_BusDevice,0x83,0x08);

	// download the firmware image and tickle the device
	if ( ConfigureAndProbeDevice() == STATUS_SUCCESS )
	{
		m_uMacOptions = NDIS_MAC_OPTION_RECEIVE_SERIALIZED  |
						NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
						NDIS_MAC_OPTION_NO_LOOPBACK;

		// This sets up the Receive Frame Area including:
		// Allocating internal software Receive Frame Descriptors
		// Allocating packet and buffer pools
		// Initializing and chaining packets, software RFD, buffers
		// Maintains list of software RFD descriptors
		//
		// This driver's requirement is to further initialize the RFD's
		// see global template specialization DescriptorInitialize(..) which
		// is called for each descriptor
		m_pRxArea = new KNdisSystemReceiveArea< USBNICAdapter, WDM_RFD >(
			*this,
			USBNIC_RX_IRPPOOL_SIZE
			);
		m_pRxArea->Initialize();

		if (m_pRxArea == NULL)
			return STATUS_UNSUCCESSFUL;
	}
	else
		return STATUS_UNSUCCESSFUL;

#if BINARY_COMPATIBLE
	m_PacketIndicate.Initialize(this);
#endif

	return STATUS_SUCCESS;
}



////////////////////////////////////////////////////////////////////
// USBNICAdapter::Halt
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
//		 returns. Our sample follows this practice.
//
VOID USBNICAdapter::Halt(VOID)
{
	TRACE("USBNICAdapter::Halt() Entered\n");

	m_HaltFlag = TRUE;

	// Cancel all of the IRPs in the Rx IRP list.
	PWDM_RFD pWdmRfd = m_RxIrpList.Head();
	while (pWdmRfd)
	{
		if(pWdmRfd->m_Irp->CancelRoutine)
			IoCancelIrp(pWdmRfd->m_Irp);
		pWdmRfd = m_RxIrpList.Next(pWdmRfd);
	}

	// Wait for all of the Rx IRPs to be completed
	LARGE_INTEGER DueTime;
	DueTime.QuadPart = -(10000 * 20000);	// 20 seconds
	if ( m_RxIdleEvent.Wait() == FALSE )
	{
		ASSERT(!"All Rx IRPs were not accounted for");
	}

	// Wait for all of the Tx IRPs to be completed
	if ( m_TxIrpPool.Wait(1000) == STATUS_TIMEOUT )
	{
		ASSERT(!"All Tx IRPs were not accounted for");
	}

	// Wait for all of the Control IRPs to be completed
	if ( m_CntrlIrpPool.Wait(1000) == STATUS_TIMEOUT )
	{
		ASSERT(!"All Control IRPs were not accounted for");
	}

	m_BusDevice.DeActivateConfiguration();

	// Deallocate our receive area.
	TRACE("USBNICAdapter::Halt() Deleting Receive Area\n");
	delete m_pRxArea;
	m_pRxArea = NULL;

}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::Reset
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
NDIS_STATUS USBNICAdapter::Reset
			(OUT PBOOLEAN AddressingReset)
{
	TRACE("USBNICAdapter::Reset() Entered\n");
	// TODO:	Reset the card
	// This may include sumiting an URB via the control pipe
	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::Shutdown
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
VOID USBNICAdapter::Shutdown(VOID)
{
	TRACE("USBNICAdapter::Shutdown() Entered\n");
	// This may include sumiting an URB via the control pipe
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::CompletionTxRoutine
//
// Parameters:
// 	pIrp                 - irp being completed
// 	Context              - context passed when the irp had been submitted
// IRQL:
//		IRQL DISPATCH_LEVEL
// Returns:
// 	Usually, STATUS_MORE_PROCESSING_REQUIRED
// Comments:
// 	Processes asynchronous "send" IRP completions: this is a "HandleInterrupt()"
//  thing for NDIS WDM drivers.So, the logic is similar to HandleInterrupt():
//  If a pended Tx packet gets completed, return the packet to NDIS.
//  The important point is that if the the irps are be recycled back to the pools they
//  came from, STATUS_MORE_PROCESSING_REQUIRED should be returned to the system.
NTSTATUS USBNICAdapter::CompletionTxRoutine(PIRP pIrp, USBNICAdapter::IRP_CONTEXT* Context)
{
	//TRACE("USBNICAdapter::CompletionTxRoutine() Entered\n");

	NTSTATUS status;
	KIrp I(pIrp);
	status = I.Status();
	if(status)
		TRACE("IRP TX completion status = %08X\n",ULONG(I.Status()));

	if ( status == STATUS_DEVICE_DATA_ERROR )
	{
		// The device may have been disconnected
		// TODO: add code to handle your case
	}

	// Complete the packet we just processed
	SendComplete(Context->type.m_Packet, NDIS_STATUS_SUCCESS);

	// Put the Irp to put back to the Transmit Irp pool
	m_TxIrpPool.Free(I);

	// Destroy the associated Urb
	delete Context->m_pUrb;

	// Destroy the Context
	delete Context;

	if ( status != STATUS_DEVICE_DATA_ERROR )
	{
		// Update the transmit stats
		m_GenStats->xmit_ok++;

		// If Packets are in the Queue, dequeue them and send them
		KNdisPacket QueuedPacket = m_TxQueue.RemoveHead();
		while (QueuedPacket.IsValid())
		{
			TRACE("Sending Queued Tx Packets\n");
			// Retry to transmit previously queued packets
			NDIS_STATUS status = Transmit(QueuedPacket);
			if (status == NDIS_STATUS_RESOURCES)
			{
				// The packet can not be transmited because of a lack of resources
				// so requeue it for later
				m_TxQueue.InsertHead(QueuedPacket);
				QueuedPacket = NULL;
			}
			else
			{
				// Get the next queued packet
				QueuedPacket = m_TxQueue.RemoveHead();
			}
		}
	}

	return STATUS_MORE_PROCESSING_REQUIRED;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::Send
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
NDIS_STATUS USBNICAdapter::Send(
			IN	PNDIS_PACKET			Packet,
			IN	UINT					Flags
			)
{
	//TRACE("USBNICAdapter::Send() Entered\n");
	NDIS_STATUS status = NDIS_STATUS_SUCCESS;
	// declare for convenience
	KNdisPacket pkt(Packet);

	// Attempt to transmit the packet
	status  = Transmit(Packet);
	if (status == NDIS_STATUS_RESOURCES)
	{
		// We can never return NDIS_STATUS_RESOURCES to NDIS, and must
		// instead queue This packet for the Deserialized Miniport
		m_TxQueue.InsertTail(Packet);
		TRACE("!!!Queueing packet\n");
	}

	// Deserialized drivers always return pending.
	return NDIS_STATUS_PENDING;
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::Transmit
//
// Parameters:
//  Packet			- Packet supplied by NDIS to transmit
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NDIS_STATUS
// Comments:
// 	This routine compliments the Send() method. It does the necessary
//  checking and builds the URB that must be transmitted.
//
NDIS_STATUS USBNICAdapter::Transmit(PNDIS_PACKET Packet)
{
	PURB			pUrb;
	NDIS_STATUS		status = NDIS_STATUS_SUCCESS;

	//TRACE("USBNICAdapter::Transmit() Entered\n");

	// declare for convenience
	KNdisPacket pkt(Packet);
	KNdisBuffer buf = pkt.QueryFirstBuffer();

	// Get an available IRP from the Tx Irp pool
	KIrp I = m_TxIrpPool.Allocate();
	// Validate the Irp
	if (I.IsNull())
	{
		// Return failure so that send can queue the packet
		return NDIS_STATUS_RESOURCES;
	}

	// Allocate and initialize the context block:
	IRP_CONTEXT* Context = new (&m_ContextHeap) IRP_CONTEXT (this, Packet);
	// Validate the new context
	if(Context == NULL)
	{
		m_TxIrpPool.Free(I);
		// Return failure so that send can queue the packet
		return NDIS_STATUS_RESOURCES;
	}

	// Copy the data to be transmitted to the transfer buffer
	// Also calculate the new size
	int count = 2;
	int Size = 0;
	while (buf.IsValid())
	{
		Size = buf.Length();
		PUCHAR Buffer = (PUCHAR)buf.Address();

		// Copy partial packet buffer to Transfer buffer
		memcpy(Context->Buffer+count,Buffer,Size);
		count += Size;

		buf = buf.GetNext();
	}
	// Store the packet size in the Frame at the first word
	*((short *)Context->Buffer) = (short)count-2;

	pUrb = m_SendPipe.BuildBulkTransfer(
											Context->Buffer,
											count,
											FALSE,
											NULL,
											TRUE
											);

	if (pUrb == NULL)
	{
		delete Context;
		m_TxIrpPool.Free(I);
		return NDIS_STATUS_RESOURCES;
	}
	else
	{
		Context->m_pUrb = pUrb;
	}

	// Pass down the URB to the bus driver
	status = m_SendPipe.SubmitUrb (
									I,
									pUrb,
									WdmCompletionTxRoutine(this),
									Context
									);

	return status;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::ReturnPacket
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
VOID USBNICAdapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	//TRACE("USBNICAdapter::ReturnPacket() Entered Packet=%p\n", Packet);

	// declare for convenience
	m_pRxArea->Reclaim(Packet);
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::CheckForHang
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
BOOLEAN USBNICAdapter::CheckForHang()
{
	return FALSE;
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::CompletionRxRoutine
//
// Parameters:
// 	pIrp                 - irp being completed
// 	Context              - context passed when the irp had been submitted
// IRQL:
//		IRQL DISPATCH_LEVEL
// Returns:
// 	Usually, STATUS_MORE_PROCESSING_REQUIRED
// Comments:
// 	Processes asynchronous "receive" IRP completions: this is a "HandleInterrupt()"
//  thing for NDIS WDM drivers. So, the logic is similar to HandleInterrupt():
//  If a new received packet arrived indicate it to NDIS. Handle status indications, too.
//  The important point is that if the the irps are be recycled back to the pools they
//  came from, STATUS_MORE_PROCESSING_REQUIRED should be returned to the system.
// NOTE: Under Win9X, NDIS packet and status indications must NOT be made from
//       the completion routine context. Instead, the NDIS WDM driver should use the
//		 KNdisWdm9XIndication class which is used to schedule the indication on
//		 a timer.
NTSTATUS USBNICAdapter::CompletionRxRoutine(PIRP pIrp, IRP_CONTEXT *Context)
{
	PWDM_RFD pContext = (PWDM_RFD)Context;
	KIrp I(pIrp);
	NTSTATUS status = I.Status();
	//TRACE("USBNICAdapter::CompletionRxRoutine() Entered\n");
	if(status)
		TRACE("IRP RX completion status = %08X\n",ULONG(I.Status()));

	if (m_HaltFlag)
	{
		--m_RxIrpsOutstanding;
		TRACE("IRP RX Outstanding Count--= %08X\n",(ULONG)m_RxIrpsOutstanding);
		if (!m_RxIrpsOutstanding)
		{
			TRACE("IRP RX Idle Event being set!");
			m_RxIdleEvent.Set();
		}
		return STATUS_MORE_PROCESSING_REQUIRED;
	}

	if (status != NDIS_STATUS_SUCCESS)
	{
		if ((status == STATUS_DEVICE_DATA_ERROR) ||
			(status == STATUS_CANCELLED) ||
			(status == STATUS_DEVICE_NOT_CONNECTED))
		{
			// This error usually occurs when the device is malfunctioning
			// or it has been disconnected. Halt() cancels all outstanding Irps
			// Note: The OutStanding Rx Irp count must be decrimented because we got
			// here prior to the m_HaltFlag being set
			// TODO: add code to handle you case
			--m_RxIrpsOutstanding;
			TRACE("IRP RX Outstanding Count--= %08X\n",(ULONG)m_RxIrpsOutstanding);
			if (!m_RxIrpsOutstanding)
			{
				TRACE("IRP RX Idle Event being set!");
				m_RxIdleEvent.Set();
			}
			return STATUS_MORE_PROCESSING_REQUIRED;
		}
		else
		{
			//TODO: Handle the case were the Irp did not complete successfully
			//		In this example we simply are resending the URB back to the
			//		bus driver.
			ReSubmitRfd(pContext);
			return STATUS_MORE_PROCESSING_REQUIRED;
		}
	}

	// Retrieve the Packet descriptor that matches this HwRfd
	KNdisPacket Packet(m_pRxArea->GetPacket(pContext));
	ASSERT( Packet.IsValid() );

	// Extract the first available buffer
	KNdisBuffer buf = Packet.QueryFirstBuffer();
	ASSERT( buf.IsValid() );

	// Copy the Rx Packet data to the Packet
	short ActualSize = *((short *)pContext->m_RfdBuffer);
	if(0 == ActualSize)
	{
		ReSubmitRfd(pContext);
		return STATUS_MORE_PROCESSING_REQUIRED;
	}

	// Remove the Packet from the Rfd list
	m_pRxArea->Complete(pContext);

	// Make sure that the Packet to be indicated has the correct length
	buf.AdjustLength(ActualSize);
	Packet.STATUS(NDIS_STATUS_SUCCESS);

#if BINARY_COMPATIBLE
	m_PacketIndicate.ScheduleReceiveIndicate(Packet);
#else
	// Indicate the Packet to NDIS
	IndicateReceive(Packet);
#endif

	// Update the transmit stats
	m_GenStats->rcv_ok++;

	return STATUS_MORE_PROCESSING_REQUIRED;
}

#if BINARY_COMPATIBLE
///////////////////////////////////////////////////////////////////////
// USBNIC::ProcessReceiveIndicate
//
// Parameters:
//  Packet			- Packet to indicate
//  Context			- Anything you wish to carry forward that is not 
//					  resident in the packet.
// IRQL: 
//				= DISPATCH_LEVEL
// Returns:
// 	None
// Comments:
// 	This routine is used to perform the actual indication when the timer
// has expired.
//  
VOID USBNICAdapter::ProcessReceiveIndicate(PNDIS_PACKET Packet)
{
	// Indicate the Packet to NDIS
	IndicateReceive(Packet);
}
#endif

////////////////////////////////////////////////////////////////////
// USBNICAdapter::CompletionCntrlRoutine()
//
//
// Parameters:
//				Irp - IRP to be completed
//				Context - Associated context for the Irp
// IRQL:
//		IRQL DISPATCH_LEVEL
//
// Return Mode:
//
NTSTATUS USBNICAdapter::CompletionCntrlRoutine(KIrp Irp, IRP_CONTEXT* Context)
{
	TRACE("USBNICAdapter::CompletionCntrlRoutineLINK() Entered\n");
	TRACE("IRP Control completion status = %08X\n",ULONG(Irp->IoStatus.Information));

	// Free the associated Irp
	Context->Adapter->m_CntrlIrpPool.Free(Irp);

	// Destroy the associated Urb
	delete Context->m_pUrb;

	// Destroy the Context
	delete Context;

	return STATUS_MORE_PROCESSING_REQUIRED;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::ReadConfiguration()
//
//		When this routine returns success, then it is
//	accepted that the device configuration was successfull.
//
// Parameters:
//				pEthConfig - pointer to a Ethernet_Configuration
//
// IRQL:
//		PASSIVE_LEVEL
//
// Return Mode:
//
NTSTATUS USBNICAdapter::ReadConfiguration(Ethernet_Configuration	*pEthConfig)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = ControlCommand (
								USBNIC_COMMAND_GET_ETHERNET_DESC,
								USBD_TRANSFER_DIRECTION_IN,
								0,
								0,
								(PUCHAR)pEthConfig,
								sizeof(Ethernet_Configuration),
								TIMEOUT);
	return status;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::DownLoadFirmware()
//
//		This routine is used to supply the Firmware Buffers to
//	the device.
//
// Parameters:
//
//			data - data to be downloaded
//			length - length of data buffer
//			interrupt - firmware specific params
//			type - firmware specific params
//
// IRQL:
//		PASSIVE_LEVEL
//
// Return Mode:
//				SYNCHRONOUS
NTSTATUS USBNICAdapter::DownLoadFirmware(PUCHAR data, USHORT length, UCHAR interrupt, UCHAR type)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (length > BUF_SIZE)
		status = STATUS_UNSUCCESSFUL;
	else
	{
		memcpy((PUCHAR)FirmwareBuffer, data,length);
		status = ControlCommand (
								USBNIC_COMMAND_SCAN,
								USBD_TRANSFER_DIRECTION_OUT,
								type,
								interrupt,
								(PUCHAR)FirmwareBuffer,
								length,
								TIMEOUT);
	}

	return status;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::TriggerFirmware()
//
//		This routine fires off the hardware to that it starts
//	performing as a NIC.
//
// Parameters:
//				None
// IRQL:
//		PASSIVE_LEVEL
//
// Return Mode:
//				SYNCHRONOUS
NTSTATUS USBNICAdapter::TriggerFirmware()
{
	NTSTATUS status = STATUS_SUCCESS;

	status = ControlCommand (
							USBNIC_COMMAND_SCAN,
							USBD_TRANSFER_DIRECTION_OUT,
							0,
							0,
							(PUCHAR)trigger_code,
							trigger_code_size,
							TIMEOUT);
	return status;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::SetURBSize()
//
//		This routine is used to set the size of the URBs that the
//	adapter will accept. These URBs will contains bothe packet data
//	and configuration parameters.
//
// Parameters:
//				urb_size - size for the URB to be accepted by the
//							hardware.
//
// IRQL:
//		PASSIVE_LEVEL
//
// Return Mode:
//				SYNCHRONOUS
NTSTATUS USBNICAdapter::SetURBSize(short urb_size)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = ControlCommand (
								USBNIC_COMMAND_SET_URB_SIZE,
								USBD_TRANSFER_DIRECTION_OUT,
								urb_size,
								0,
								NULL,
								0,
								TIMEOUT);
	return status;

}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::SetSOFSWait()
//
//		This routine sets the SOFS for the device.
//
// Parameters:
//				sofs_wait - specific value to the SOFS
// IRQL:
//		PASSIVE_LEVEL
//
// Return Mode:
//				SYNCHRONOUS
NTSTATUS USBNICAdapter::SetSOFSWait(short sofs_wait)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = ControlCommand (
							USBNIC_COMMAND_SET_SOFS_WAIT,
							USBD_TRANSFER_DIRECTION_OUT,
							sofs_wait,
							0,
							NULL,
							0,
							TIMEOUT);
	return status;
}


////////////////////////////////////////////////////////////////////
// USBNICAdapter::SetReceiveFilter()
//
//		This routine converts the NDIS Rx filter values to the
//	device specific values.
//
// Parameters:
//				Rxfilters - NDIS Rx Filter values
//
// IRQL:
//		PASSIVE_LEVEL or DISPATCH_LEVEL
//
// Return Mode:
//			SYNCHRONOUS(for PASSIVE) or ASYNCHRONOUS(for DISPATCH)
// Comments:
//	During the setOID_GEN_CURRENT_PACKET_FILTER this routine runs
//	at DISPATCH_LEVEL. During device configuration this routine runs
//	at PASSIVE_LEVEL.
//
NTSTATUS USBNICAdapter::SetReceiveFilter(ULONG Rxfilters)
{
	KIRQL kIrql;
	NTSTATUS status = STATUS_SUCCESS;

	short receive_filters = 0;

	if (Rxfilters & NDIS_PACKET_TYPE_FUNCTIONAL)
		receive_filters =	USBNIC_PACKET_FILTER_DIRECTED |
							USBNIC_PACKET_FILTER_BROADCAST |
							USBNIC_PACKET_FILTER_MULTICAST |
							USBNIC_PACKET_FILTER_PROMISCUOUS |
							USBNIC_PACKET_FILTER_ALL_MULTICAST
							;

	else
	{
		receive_filters |= Rxfilters & NDIS_PACKET_TYPE_DIRECTED ? USBNIC_PACKET_FILTER_DIRECTED : 0;
		receive_filters |= Rxfilters & NDIS_PACKET_TYPE_BROADCAST  ? USBNIC_PACKET_FILTER_BROADCAST : 0;
		receive_filters |= Rxfilters & NDIS_PACKET_TYPE_MULTICAST ? USBNIC_PACKET_FILTER_MULTICAST : 0;
		receive_filters |= Rxfilters & NDIS_PACKET_TYPE_PROMISCUOUS  ? USBNIC_PACKET_FILTER_PROMISCUOUS : 0;
		receive_filters |= Rxfilters & NDIS_PACKET_TYPE_ALL_MULTICAST  ? USBNIC_PACKET_FILTER_ALL_MULTICAST : 0;
	}

	kIrql = KeGetCurrentIrql();
	if (kIrql < DISPATCH_LEVEL)
	{
		// Pass the URB to the bus driver at PASSIVE level
		status = ControlCommand (
									USBNIC_COMMAND_SET_PACKET_FILTER,
									USBD_TRANSFER_DIRECTION_OUT,
									receive_filters,
									0,
									NULL,
									0,
									TIMEOUT);
	}
	else
	{
		// Allocate and initialize the context block:
		IRP_CONTEXT* Context = new (&m_ContextHeap) IRP_CONTEXT (this, NULL);
		// Validate the new context
		if(Context == NULL)
		{
			// Return failure so that send can queue the packet
			return NDIS_STATUS_RESOURCES;
		}

		// Pass the URB to the bus driver at DISPATCH level
		PURB pUrb = m_BusDevice.BuildVendorRequest(
							NULL,
							0,
							0,
							USBNIC_COMMAND_SET_PACKET_FILTER,
							receive_filters,
							USBD_TRANSFER_DIRECTION_OUT,
							FALSE,
							NULL,
							0,
							URB_FUNCTION_VENDOR_DEVICE,
							NULL
							);

		// Validate the URB
		if (pUrb==NULL)
		{
			delete Context;
			return NDIS_STATUS_RESOURCES;
		}

		// Store the URB and allocate a Control Irp from the pool
		Context->m_pUrb = pUrb;
		KIrp I=m_CntrlIrpPool.Allocate();

		// Validate the Irp
		if (I.IsNull())
		{
			delete pUrb;
			delete Context;
			return NDIS_STATUS_RESOURCES;
		}

		// NOTES:To be fixed
		// Store the adapter "this" pointer in the m_pClass param
		// of the Context for the DriverWorks MEMBER_COMPLETEIRPWITHCONTEXT
		// macro to work
		Context->m_pClass = this;

		// send the URB to the device
		status = m_BusDevice.SubmitUrb(
										I,
										pUrb,
										LinkTo(CompletionCntrlRoutine),
										(void*)Context,
										100
										);
		if(status == STATUS_PENDING)
			status = STATUS_SUCCESS;
	}

	return status;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::ActivateDevice()
//
//		This routine is used to select the proper configuration of
//	the USB device
//
// Parameters: None
//
// IRQL:
//		PASSIVE_LEVEL
//
// Return Mode:
//		SYNCHRONOUS
//
AC_STATUS USBNICAdapter::ActivateDevice()
{
	AC_STATUS		acStatus;
	acStatus = m_BusDevice.ActivateConfiguration(1);	// Configuration value 1

	switch (acStatus)
	{
		case AC_SUCCESS:
			TRACE("USB Configuration OK\n");
			//GetStringDescriptors();
			break;

		case AC_COULD_NOT_LOCATE_INTERFACE:
			TRACE("Could not locate the interface in the config descriptor\n");
			//Check that the Interface Number and Alternate Setting
			//for the KUsbInterface object initialized in the constructor match
			//an interface descriptor reported by the hardware.
			break;

		case AC_COULD_NOT_PRECONFIGURE_INTERFACE:
			TRACE("Could not get configuration descriptor\n");
			break;

		case AC_CONFIGURATION_REQUEST_FAILED:
			TRACE("Board did not accept configuration URB\n");
			break;

		case AC_FAILED_TO_INITIALIZE_INTERFACE_OBJECT:
			TRACE("Failed to initialize interface object\n");
			break;

		case AC_FAILED_TO_LOCATE_ENDPOINT_ADDRESS:
			TRACE("Failed to locate endpoint address for pipe\n");
			break;

		case AC_FAILED_TO_OPEN_PIPE_OBJECT:
			//NOTE: this may or may not be fatal.  It could mean that
			//the device has an endpoint for which a KUsbPipe object has
			//not been instanced.  If the intention is to not use this pipe,
			//then it's ok.  Otherwise, there is a failure.  Clients can
			//iterate through the pipe array  in KUsbLowerDevice to check
			//which pipes are open/closed.
			TRACE("Failed to open pipe object \n");
			break;

		default:
			TRACE("Unexpected error activating USB configuration\n");
			// Possibly the device is disconnected
			break;
	}

	return acStatus;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::ConfigureAndProbeDevice()
//
//	This is the main routine used to configure the USB NIC device.
//	It downloads the firmware and configures the cards settings.
//
// Parameters: None
// IRQL:
//		PASSIVE_LEVEL
//
// Return Mode:
//		SYNCHRONOUS
//
NTSTATUS USBNICAdapter::ConfigureAndProbeDevice()
{
	NTSTATUS status = STATUS_SUCCESS;
	AC_STATUS		acStatus;

	status = DownLoadFirmware(new_code, new_code_size,0,0);
	if (status!=NDIS_STATUS_SUCCESS)
		return status;

	TriggerFirmware();
	if (status!=NDIS_STATUS_SUCCESS)
		return status;

	SetURBSize(BUF_SIZE);
	if (status!=NDIS_STATUS_SUCCESS)
		return status;

	SetSOFSWait(SOFS_TO_WAIT);
	if (status!=NDIS_STATUS_SUCCESS)
		return status;

	acStatus = ActivateDevice();
	if (acStatus != AC_SUCCESS)
		return STATUS_UNSUCCESSFUL;

	NdisStallExecution(100000);
	status = ReadConfiguration(&EthConfig);
	if (status!=NDIS_STATUS_SUCCESS)
		return status;

	// Cache the MAC address
	memcpy(&m_CurrentAddress.m_bytes[0], &EthConfig.MAC_Address.EthNodeAddress[0], 6);
	memcpy(&m_PermanentAddress.m_bytes[0], &EthConfig.MAC_Address.EthNodeAddress[0], 6);

	NdisStallExecution(100000);
	m_uPacketFilter = NDIS_PACKET_TYPE_DIRECTED | NDIS_PACKET_TYPE_MULTICAST |
					  NDIS_PACKET_TYPE_BROADCAST;

	SetReceiveFilter(m_uPacketFilter);

	return status;
}

////////////////////////////////////////////////////////////////////
// USBNICAdapter::ControlCommand()
//
//	This routine is used to submit all the SYNCHRONOUS URBS to the
//	device.
//
// Parameters:
//			Request - Device specific command request code
//			Direction - IN/OUT
//			Value - USB URB Value
//			Index - USB URB Index
//			Data - pointer to data that will be submitted
//			Size - size of buffer pointed to by Data
//			Timeout - unused
//
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		SYNCHRONOUS
NTSTATUS USBNICAdapter::ControlCommand (
						  UCHAR Request,
						  UCHAR Direction,
						  USHORT Value,
						  USHORT Index,
						  PUCHAR Data,
						  short Size,
						  short Timeout )
{
	NTSTATUS status = STATUS_SUCCESS;

	PURB pUrb = m_BusDevice.BuildVendorRequest(
												Data,
												Size,
												0,
												Request,
												Value,
												Direction,
												FALSE,
												NULL,
												Index,
												URB_FUNCTION_VENDOR_DEVICE,
												NULL
												);

	status = m_BusDevice.SubmitUrb(pUrb);

	delete pUrb;

	return status;
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::DescriptorInitialize
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
VOID USBNICAdapter::DescriptorInitialize(
	PWDM_RFD pWdmDesc,
	PNDIS_PHYSICAL_ADDRESS pPhysAddr
	)
{
	TRACE("USBNICAdapter::DescriptorInitialize\n");

	// Get an available IRP from the Rx Irp pool
	KIrp I = m_RxIrpPool.Allocate();
	// Validate the Irp
	if (I.IsNull())
	{
		TRACE("USBNICAdapter::DescriptorInitialize() Irp allocation error\n");
		ASSERT(!"Irp allocation error\n");
		return;
	}

	pWdmDesc->m_Irp = I;
	pWdmDesc->m_pAdapter = this;

	// Create an BULK URB
	pWdmDesc->m_pUrb = m_ReceivePipe.BuildBulkTransfer(
											pWdmDesc->m_RfdBuffer,
											BUF_SIZE,
											TRUE,
											NULL,
											TRUE
											);
	// Validate the URB
	if (pWdmDesc->m_pUrb == NULL)
	{
		TRACE("USBNICAdapter::DescriptorInitialize() Urb creation error\n");
		m_RxIrpPool.Free(pWdmDesc->m_Irp);
		ASSERT(!"Urb creation error\n");
		return;
	}

	pWdmDesc->m_RfdActualCount= 0;
	pWdmDesc->m_RfdSize = BUF_SIZE;

	// Pass down the URB to the bus driver
	NTSTATUS status = m_ReceivePipe.SubmitUrb (
												pWdmDesc->m_Irp,
												pWdmDesc->m_pUrb,
												WdmCompletionRxRoutine(this),
												pWdmDesc
												);
	// update the Rx Irp count
	++m_RxIrpsOutstanding;
	TRACE("IRP RX Outstanding Count++= %08X\n",(ULONG)m_RxIrpsOutstanding);

	// Add the IRP to the RxIrp list
	m_RxIrpList.InsertTail(pWdmDesc);
	DumpWdmRfd(pWdmDesc);

}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::DescriptorInvalidate
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
VOID USBNICAdapter::DescriptorInvalidate(
	PWDM_RFD pWdmDesc
	)
{
	TRACE("USBNICAdapter::DescriptorInvalidate\n");
	m_RxIrpPool.Free(pWdmDesc->m_Irp);
	delete pWdmDesc->m_pUrb;
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::DescriptorComplete
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
VOID USBNICAdapter::DescriptorComplete(
	PWDM_RFD pWdmDesc,
	PNDIS_PHYSICAL_ADDRESS pPhysAddr
	)
{
	//TRACE("USBNICAdapter::DescriptorComplete\n");
	DumpWdmRfd(pWdmDesc);
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::ReSubmitRfd
//
//	This member is used to pass the IRP and URB pair back to the
//	bus driver.
//
// Parameters:
//  pWdmDesc			- The descriptor to reclaim
// IRQL:
//				<= DISPATCH_LEVEL
// Returns:
// 	NONE
// Comments:
//			Resubmit the URB to the bus driver for reuse
//
void USBNICAdapter::ReSubmitRfd(PWDM_RFD pWdmDesc)
{
	//TRACE("USBNICAdapter::ReSubmitRfd\n");

	KIrp Irp(pWdmDesc->m_Irp);
	ASSERT(!Irp.IsNull());
	ASSERT(pWdmDesc->m_pUrb);
	Irp.Reuse();
	*((short *)pWdmDesc->m_RfdBuffer) = 0;

	// Reuse the existing URB. Notice that the 6th parameter is
	// the pointer to the URB that was reclaimed. This call reinitializes
	// the URB for reuse.
	m_ReceivePipe.BuildBulkTransfer(
									pWdmDesc->m_RfdBuffer,
									BUF_SIZE,
									TRUE,
									NULL,
									TRUE,
									pWdmDesc->m_pUrb
									);

	// Pass down the URB to the bus driver
	m_ReceivePipe.SubmitUrb (
							pWdmDesc->m_Irp,
							pWdmDesc->m_pUrb,
							WdmCompletionRxRoutine(this),
							pWdmDesc
							);
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::DescriptorReclaim
//
//	Once a descriptor is reclaimed, then we create a new IRP and URB
//	and submit it to the bus driver.
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
VOID USBNICAdapter::DescriptorReclaim(
	PWDM_RFD pWdmDesc,
	PNDIS_PHYSICAL_ADDRESS pPhysAddr
	)
{
	//TRACE("USBNICAdapter::DescriptorReclaim\n");
	ReSubmitRfd(pWdmDesc);
	DumpWdmRfd(pWdmDesc);
}

///////////////////////////////////////////////////////////////////////
// USBNICAdapter::DumpWdmRfd
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
VOID USBNICAdapter::DumpWdmRfd(PWDM_RFD p)
{
	//TRACE("USBNICAdapter::DumpWdmRfd\n");
	//TRACE("   RFD_STRUC* p =%lx\n", p);
}

NTSTATUS KNdisWrapper<USBNICAdapter>::WdmCompletionRxRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp, PVOID context)
{
	UNREFERENCED_PARAMETER(pDevObj);
	USBNICAdapter::PWDM_RFD pWdmRfd = (USBNICAdapter::PWDM_RFD) context;
	USBNICAdapter* a = pWdmRfd->m_pAdapter;	// maps irp => owner
	ASSERT(a);										// assert context had been alloc correctly
	return a->CompletionRxRoutine(pIrp, reinterpret_cast<USBNICAdapter::IRP_CONTEXT*>(context));
}

// end of file
