// NMNE2KAdap.cpp: implementation of the NMNE2KAdapter class.
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

#include "NMNE2KAdap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NMNE2KAdapter::NMNE2KAdapter() :
	KNdisMiniAdapter(),
			m_pTxQueue(NULL)
{
	// TODO: initialize your private data members
	//       Do NOT call any NDIS functions in here. Defer to Initialize()
	
}

NMNE2KAdapter::~NMNE2KAdapter()
{
	// TODO: deallocate all the resources allocated in constructor/Initialize()
	//
	delete m_pTxQueue;
	m_Interrupt.Invalidate();
	m_Ports.Invalidate();
}


////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::Initialize
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
NDIS_STATUS NMNE2KAdapter::Initialize
				(IN OUT KNdisMedium& Medium, IN KNdisConfig& Config)
{
	TRACE("NMNE2KAdapter::Initialize() Entered\n");

	// Select our the Medium (pretending we are Ethernet):

	if (! Medium.Select(NdisMedium802_3) )
		KNDIS_RETURN_ERROR (NDIS_STATUS_UNSUPPORTED_MEDIA);
	// Read some other config stuff from the Registry, e.g.

	ULONG	uCardType;
	Config.Read(KNDIS_STRING_CONST("CardType"), &uCardType);

    //
    // Now let NDIS know about the BUS the NIC is on. Here's where the NDIS/Adapter-instance
	// handshake takes place. This should happen *before* allocating the h/w resources:
    
	SetAttributesEx(NdisInterfaceIsa);

	// Get h/w resources assigned to our adapter. 
	// Using the parameters defined in the Registry, i.e. via the the
	// configuration wrapper (Alternatevely, for a real PCI adapter we could use
	// KNdisPciResourceRequest with all the rest code being the same):

#ifdef NDIS40_MINIPORT
	KNdisParamResourceRequest request(Config);	// for NDIS 4
#else
	KNdisPnpResourceRequest request(Config);	// for NDIS 5
#endif

	// Our adapter needs Port and Interrupt resources.
	// Get descriptors of those. We'll use the descriptors when
	// allocating h/w access objects such as KNdisIoRange and
	// KNdisInterrupt:

	KNdisResource<CmResourceTypePort>		Port(request);
	KNdisResource<CmResourceTypeInterrupt>	Int(request);

	// Make sure the resources are defined in the Registry:

	if (!Port.IsValid())
		KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);

	if (!Int.IsValid())
		KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);

	// Create a list for pending Tx packets and a pool for Rx packets:
	m_pTxQueue = new KNdisPacketList;
	if (!m_pTxQueue)
		 KNDIS_RETURN_ERROR (NDIS_STATUS_RESOURCES);

	// Create the h/w access objects. 

	//	Register our i/o ports:	

	m_Ports.Initialize(this, Port);

	if (!m_Ports.IsValid())
		 KNDIS_RETURN_ERROR (NDIS_STATUS_RESOURCES);

	// Register our interrupt. NOTE that we must register an interrupt object
	// as long as we expose Disable/EnableInterrupt() handlers. NDIS would use
	// them internally for KeSynchronizeExecition().

	m_Interrupt.Initialize(this, Int, NdisInterruptLatched);

	if (!m_Interrupt.IsValid())
		 KNDIS_RETURN_ERROR (NDIS_STATUS_RESOURCES);

	// Try to initialize the hardware. This might include a self-test, etc.

	if (CardInit() != NDIS_STATUS_SUCCESS)
		 KNDIS_RETURN_ERROR (NDIS_STATUS_ADAPTER_NOT_FOUND);

	// Get network address (if any) from the Registry. If specified
	// it will be used instead of the burned permanent address:

	m_CurrentAddress = m_PermanentAddress;
	Config.ReadNetworkAddress(m_CurrentAddress);
	
	// Set default filter and MAC options. In principle, we should also
	// relay that to our card...
	m_uPacketFilter = NDIS_PACKET_TYPE_DIRECTED |
					  NDIS_PACKET_TYPE_MULTICAST |
					  NDIS_PACKET_TYPE_BROADCAST;

	m_uMacOptions = NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |
				    NDIS_MAC_OPTION_RECEIVE_SERIALIZED  |
					NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
					NDIS_MAC_OPTION_NO_LOOPBACK;

	m_uCurrentLookahead = NMNE2K_MAX_LOOKAHEAD;

	// Start the card up. This might cause an interrupt immediately.
	//

	CardRestart(TRUE);

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::Halt
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
VOID NMNE2KAdapter::Halt(VOID)
{
	TRACE("NMNE2KAdapter::Halt() Entered\n");

	// Stop the card. Theoretically, however,
	// our Isr() still can be called, so we should be prepared.

	CardRestart(FALSE);

}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::Reset
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
NDIS_STATUS NMNE2KAdapter::Reset
			(OUT PBOOLEAN AddressingReset)
{
	TRACE("NMNE2KAdapter::Reset() Entered\n");

	// TODO:	Reset the card

	CardRestart(m_bEnabled);
	*AddressingReset = TRUE;

	return NDIS_STATUS_SUCCESS;
}
	
////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::Shutdown
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
VOID NMNE2KAdapter::Shutdown(VOID)
{
	TRACE("NMNE2KAdapter::Shutdown() Entered\n");

	// Reset the card for good

	CardRestart(FALSE);

}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::Send
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
NDIS_STATUS NMNE2KAdapter::Send(
			IN	PNDIS_PACKET			Packet,
			IN	UINT					Flags
			)
{
	// Manage the packet. Return NDIS_STATUS_SUCCESS if the
	// the packet got submitted to the h/w and NDIS_STATUS_PENDING
	// if it's been queued into m_pTxQueue.
	
	if (m_bXmitInUse)
	{	
		// transmitter is busy - queue the packet 
		m_pTxQueue->InsertTail(Packet);
		return NDIS_STATUS_PENDING;
	}

	m_bXmitInUse = TRUE;

	CopyDataDown(Packet);

	return NDIS_STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CheckForHang
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
BOOLEAN NMNE2KAdapter::CheckForHang()
{
	// TODO:	Check the h/w for hang
	return FALSE;
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::TransferData
//	Transfers the rest of the partially indicated packet into the
//	protocol's allocated packet.
// Parameters:
//	Packet
//		Packet to move the data into
//	BytesTransferred
//		Returned number of bytes actually transferred. 
//	Lookahead
//		Transfer Context. For drivers using KNdisLookahead, it is a pointer to
//		the KNdisLookahead instance used for indication. Since this driver
//		has a single built-in m_Lookahead instance, we don't care.
//	ByteOffset
//		Offset in the received packet to copy from. It does NOT include
//		the header size, so it needs to be bumped up in the routine. For IP,
//		this is usually 0x20, which brings up to the body of the IP payload.
//	BytesToTransfer
//		Bytes to transfer.
// IRQL: 
//		DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		Interrupts are still disabled when MiniportTransferData is called
//
NDIS_STATUS NMNE2KAdapter::TransferData(
			OUT PNDIS_PACKET			Packet,
			OUT PUINT					BytesTransferred,
			IN  PVOID					Lookahead, /* MiniportReceiveContext */
			IN	UINT					ByteOffset,
			IN	UINT					BytesToTransfer)
{
	// Bump the offset
    ByteOffset += NMNE2K_HEADER_SIZE;
	
	// Copy data into the protocol's packet
	*BytesTransferred = m_Lookahead.Transfer(this, Packet, BytesToTransfer, ByteOffset);

	// TRACE("NMNE2KAdapter::TransferData %u bytes: %u went\n",BytesToTransfer,*BytesTransferred); 

	return (*BytesTransferred) ?
			NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE;
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::DisableInterrupt
//	Optional function, supplied by drivers of NICs that support dynamic
//	enabling and disabling of interrupts but do not share an IRQ
// Parameters:
//		none
// IRQL: 
//		DIRQL
// Return Mode:
//		Synchronous
// NOTE:
//		MiniportDisableInterrupt typically disables interrupts by 
//		writing a mask to the NIC. If a driver does not have this function,
//		typically its MiniportISR disables interrupts on the NIC.
VOID NMNE2KAdapter::DisableInterrupt()
{
	// TODO:	Manipulate the h/w to disable INT generation

	m_Ports.outb(NS_IMR, 0);
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::EnableInterrupt
//	Optional function, supplied by some drivers of NICs that support
//  dynamic enabling and disabling of interrupts but do not share an IRQ
// Parameters:
//	none
// IRQL: 
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE:
//		A NIC driver that exports a MiniportDisableInterrupt function 
//		need not have a reciprocal MiniportEnableInterrupt function. 
//		Such a driver's MiniportHandleInterrupt function is responsible 
//		for re-enabling interrupts on the NIC. 
VOID NMNE2KAdapter::EnableInterrupt()
{
	// TODO:	Manipulate the h/w to enable INT generation

	m_Ports.outb(NS_IMR, m_bEnabled ? 0x3f : 0x00);
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::Isr
//	MiniportISR is a required function if the driver's NIC generates
//	interrupts.
// Parameters:
//	InterruptRecognized 
//		Points to a variable in which MiniportISR returns whether 
//		the NIC actually generated the interrupt. MiniportISR sets this
//		to TRUE if it detects that the interrupt came from the NIC.
//	QueueMiniportHandleInterrupt 
//		Points to a variable that MiniportISR sets to TRUE if the 
//		MiniportHandleInterrupt function should be called to complete
//		the interrupt-driven I/O operation. 
// IRQL: 
//		DIRQL
// Return Mode:
//		Synchronous
//
// NOTE
//		Miniports that do not provide MiniportDisable/EnableInterrupt 
//		functionality must have their ISRs called on every interrupt. 
//
VOID NMNE2KAdapter::Isr(
			OUT PBOOLEAN				InterruptRecognized,
			OUT PBOOLEAN				QueueMiniportHandleInterrupt
			)
{
	// Make sure this is our NIC that generated that interrupt
	// Clear the interrupt condition register before returning.

	if (m_Ports.inb(NS_ISR)&0x3f)
	{	*InterruptRecognized = TRUE; 
		*QueueMiniportHandleInterrupt = TRUE;
	}
	else
	{	*InterruptRecognized = FALSE; 
		*QueueMiniportHandleInterrupt = FALSE;
	}
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::HandleInterrupt
//	MiniportHandleInterrupt is a required function if a driver's 
//	NIC generates interrupts. Does the deferred processing of all
//  outstanding interrupt operations and starts any new operations.
// Parameters:
//	none
// IRQL: 
//		IRQL DISPATCH_LEVEL
// Return Mode:
//		Synchronous
// NOTE
//		When MiniportHandleInterrupt is called, interrupts are disabled
//		on the NIC, either by the MiniportISR or MiniportDisableInterrupt.
//		Before it returns control, MiniportHandleInterrupt can re-enable 
//		interrupts on the NIC. Otherwise, NDIS calls a driver-supplied 
//		MiniportEnableInterrupt function to do so when MiniportHandleInterrupt
//		returns control.
VOID NMNE2KAdapter::HandleInterrupt()
{	
	UCHAR status;

	// TODO:	Process NIC's events: get frames, statuses, etc.
	//			This is where the "meat" of the driver is expected to be.

	while (status = (m_Ports.inb(NS_ISR))&0x3f)
	{	m_Ports.outb(NS_ISR, status);

		if (status&N_CNT)
		{	/* one or more of the counters is going to overflow */
			m_Ports.inb(NS_CNTR0);
			m_Ports.inb(NS_CNTR1);
			m_Ports.inb(NS_CNTR2);
		}

		if (status&N_OVW)
		{	/* the receive buffer is overflowed */
			TRACE("NMNE2KAdapter::CardRestart due to OVERFLOW\n");
			CardRestart(m_bEnabled);
			continue;
		}

		for (;;)
		{	// Process receives
			ULONG offset, length, current;
			UCHAR RecvHeader[4];

			m_Ports.outb(NS_CR, N_PG1|N_ARD|N_STA);
			current = m_Ports.inb(NS_CURR);
			m_Ports.outb(NS_CR, N_PG0|N_ARD|N_STA);

			if (current == m_NextPkt)
				break;						// done with receives

			// Get to the current received packet

			offset = m_NextPkt<<8;
			CardRead(RecvHeader, offset, 4);
			offset += 4;
			length = (RecvHeader[3]<<8)|RecvHeader[2];
			if ((RecvHeader[0]&N_RXOK) &&
				(length >= 64) &&
				(length <= 1518))
			{	
				// Good packet: retrieve and indicate the lookahead portion
				// using the KNdisLookahead member. If protocols are interested,
				// the HandleInterrupt() method ought to be called immediately
				// after m_Lookahead.Indicate().

				m_GenStats->rcv_ok++;
				ULONG PacketLen = length-4;
			    ULONG IndicateLen = (PacketLen > (m_uCurrentLookahead + NMNE2K_HEADER_SIZE)) ?
                           (m_uCurrentLookahead + NMNE2K_HEADER_SIZE) : PacketLen;

				m_Lookahead.Indicate(this, 
					NMNE2K_HEADER_SIZE, IndicateLen, PacketLen-NMNE2K_HEADER_SIZE, offset);
			}
			else
				m_GenStats->rcv_error++;

			/* tell the chip we took the packet */
			m_NextPkt = RecvHeader[1];

			m_Ports.outb(NS_BNRY, ((m_NextPkt-1) < m_RecvStart) ? 
				(UCHAR) m_RecvStop-1 : (UCHAR) m_NextPkt-1);

		}

		// Process transmits
		if (status&(N_PTX|N_TXE))
		{	
			PNDIS_PACKET Packet;

			if (status&N_TXE)
				m_GenStats->xmit_error++;
			else
				m_GenStats->xmit_ok++;

			if (m_pTxQueue->IsEmpty())
			{	
				m_bXmitInUse = FALSE;
			}
			else {
				Packet = m_pTxQueue->RemoveHead();
				CopyDataDown(Packet);
				SendComplete(Packet);
			}
		}
	}

	// Indicates receives (if any)
	m_Lookahead.IndicateComplete(this);
}

////////////////////////////////////////////////////////////////////
// Card-specific methods (NS8390)
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CardInit
//		Initializes the card on driver start up: determines the start and stop
//		page boundaries, reads the permanent MAC address, etc.
// Parameters:
//		none
// Returns:
//		Success or failure
// NOTE:
//		This function is called only once, from NMNE2KAdapter::Initialize(),
//		and can be made pageable.
NDIS_STATUS NMNE2KAdapter::CardInit()
{
	ULONG i, j;

	memset(m_MulticastBits, 0, sizeof(m_MulticastBits));
	m_XmitStart = 1;
	m_bXmitInUse = FALSE;
	m_RecvStart = m_XmitStart+6;
	m_RecvStop = 255;
	m_BusWidth = 0;
	m_bEnabled = FALSE;

	// disable interrupts, loopback, stop the chip

	m_Ports.outb(NS_IMR, 0);				
    m_Ports.outb(NS_TCR, N_LB0);			
	m_Ports.outb(NS_CR, (N_ARD|N_STP));	
	for (i=0; i<20000; i++)
		if ((m_Ports.inb(NS_CR)&(N_STP|N_STA)) == N_STP)
			break;

	CardRestart(FALSE);

	// compute the bus width

	m_Ports.outb(NS_RSAR0, 0);
	m_Ports.outb(NS_RSAR1, 1);
	m_Ports.outb(NS_RBCR0, 64);
	m_Ports.outb(NS_RBCR1, 0);
	m_Ports.outb(NS_CR, N_RWR|N_STA);

	i = 0;
	while (m_Ports.inb(NS_CRDA0) < 64)
		m_Ports.outw(NS_DATA, (USHORT) i++);

	m_Ports.outb(NS_CR, N_ARD|N_STA);
	m_Ports.outb(NS_ISR, 0xff);

	if (i > 48)								
	{	m_BusWidth = N_WTS;
		m_Ports.outb(NS_DCR, N_FT1|m_BusWidth); // reconfigure the controller 
	}

	TRACE("NMNE2KAdapter::CardInit() Bus is %u bits wide\n", (m_BusWidth)?16:8);

	// Read perm address from the card

	m_Ports.outb(NS_RSAR0, 0);
    m_Ports.outb(NS_RSAR1, 0);
    m_Ports.outb(NS_RBCR0, 12);
    m_Ports.outb(NS_RBCR1, 0);
    m_Ports.outb(NS_CR, (N_RRD|N_STA));

    for (i=0; i<6; i++)
		m_PermanentAddress.m_bytes[i] = m_Ports.inb(NS_DATA);

    m_Ports.outb(NS_CR, (N_ARD|N_STA));

	// determine the start and end of adapter memory 

	USHORT temp[16];
	memset(temp, 0, sizeof(temp));
	for (i=1; i<256; i++)
		CardWrite((PUCHAR)temp, i<<8, sizeof(temp));

	// this loop scans for the first page that maintains the data pattern

	for (;;)
	{	j = m_XmitStart|((256-m_XmitStart)<<8);
		temp[0] = 0x4b50;
		temp[1] = (USHORT)j;
		CardWrite((PUCHAR)temp, m_XmitStart<<8, 4);
		CardRead((PUCHAR)temp, m_XmitStart<<8, 4);
		if ((temp[0] == 0x4b50) && (temp[1] == j))
			break;
		m_XmitStart++;
		if (m_XmitStart == 256)
		{	TRACE("NMNE2KAdapter: Cannot determine start of memory\n");
			return NDIS_STATUS_FAILURE;
		}
	}

	m_RecvStart = m_XmitStart+6;
	m_RecvStop = m_XmitStart+1;

	// this loop scans for the last page that maintains the data pattern 

	while (m_RecvStop < 256)
	{	j = m_RecvStop|((256-m_RecvStop)<<8);
		temp[0] = 0x4b50;
		temp[1] = (USHORT)j;
		CardWrite((PUCHAR)temp, m_RecvStop<<8, 4);
		CardRead((PUCHAR)temp, m_RecvStop<<8, 4);
		if ((temp[0] != 0x4b50) || (temp[1] != j))
			break;
		for (i=1; i<m_RecvStop; i++)
		{	CardRead((PUCHAR)temp, i<<8, 4);
			if ((temp[0] == 0x4b50) && (temp[1] == j))
				break;
		}
		if (i < m_RecvStop)
			break;

		m_RecvStop++;
	}

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CardRestart
//		Restarts the card.
// Parameters:
//		bEnable	-	Enables transmission and INT generation after restart.
// Returns:
//		none
// NOTE:
//
VOID NMNE2KAdapter::CardRestart(BOOLEAN	bEnable)
{
	
	m_bXmitInUse = FALSE;

	// initialize the controller registers

	m_Ports.outb(NS_IMR, 0);
	m_Ports.outb(NS_CR, N_ARD|N_STP);
	m_Ports.outb(NS_DCR, N_FT1|m_BusWidth);
	m_Ports.outb(NS_RBCR0, 0);
	m_Ports.outb(NS_RBCR1, 0);

	m_Ports.outb(NS_RCR, N_AB|N_AM);	// bcast and mcast on

	m_Ports.outb(NS_TCR, N_LB1);
	m_Ports.outb(NS_PSTART, (UCHAR) m_RecvStart);
	m_Ports.outb(NS_PSTOP, (UCHAR) m_RecvStop);
	m_Ports.outb(NS_BNRY, (UCHAR) m_RecvStart);

	m_Ports.outb(NS_ISR, 0xff);			// clear all pending interrupts 

	// set the node address (get to page 1 registers)

	int i;
	m_Ports.outb(NS_CR, N_PG1|N_ARD|N_STP);
	for (i=0; i<6; i++)
		m_Ports.outb(NS_PAR0+i, m_CurrentAddress.m_bytes[i]);

	m_NextPkt = m_RecvStart+1;
	m_Ports.outb(NS_CURR, (UCHAR) m_NextPkt);

	for (i=0; i<sizeof(m_MulticastBits); i++)
		m_Ports.outb(NS_MAR0+i, m_MulticastBits[i]);

	// start the controller (get back to page 0 registers)

	m_Ports.outb(NS_CR, N_ARD|N_STA);

	if (m_bEnabled = bEnable)
	{	m_Ports.outb(NS_DCR, N_FT1|N_LS|m_BusWidth);
		m_Ports.outb(NS_TCR, 0);
		m_Ports.outb(NS_IMR, 0x3f);
	}
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CopyDataUp
//		Moves data from the card into target buffer.
// Parameters:
//		FrameBase -	Frame Base in the h/w. In NMNE2K, this is the offset
//					to the received frame in the ring buffer (NS_RSAR0 | NS_RSAR1).
//		BufOffset -	Offset in the current packet to copy from
//		Buffer	  -	output (protocol) buffer to read data into
//		Length	  -	number of bytes to read
// Returns:
//		none
// NOTE:
//		The method is required for drivers using KNdisLookahead class.
//		It is called from KNdisLookahead's methods during partial packet
//		transfers. 
//		See NMNE2KAdapter::HandleInterrupt() and NMNE2KAdapter::TransferData().
UINT NMNE2KAdapter::CopyDataUp
	(PVOID FrameBase, PUCHAR Buffer, ULONG Length, ULONG BufOffset)
{ 
  // calculate the actial offset in the ring buffer	
  ULONG Offset = ULONG(FrameBase) + BufOffset;
  UINT len = Length;	

  // take care of odd offsets
  if (Offset & 1) {
	  UCHAR	temp[2];
	  CardRead(temp, Offset-1, 2); 
	  * Buffer ++ = temp[1];
	  Offset++;
	  Length--;	
  }

  CardRead((PUCHAR) Buffer, Offset, Length); 
  return len;
}


////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CardRead
//		Reads received data from the card
// Parameters:
//		Buffer	-	output buffer to read data into
//		Offset	-	offset from the start of the card's receive buffer(s)
//		Length	-	number of bytes to read
// Returns:
//		none
// NOTE:
//		Synchronously reads data. Offset must be an even number.
VOID NMNE2KAdapter::CardRead(PUCHAR Buffer, ULONG Offset, ULONG Length)
{
	ASSERT((Offset & 1)==0);

	m_Ports.outb(NS_RSAR0, (UCHAR)Offset);
	m_Ports.outb(NS_RSAR1, (UCHAR)(Offset>>8));
	if (m_BusWidth)
		Length += (Length&1);
	m_Ports.outb(NS_RBCR0, (UCHAR)Length);
	m_Ports.outb(NS_RBCR1, (UCHAR)(Length>>8));
	m_Ports.outb(NS_CR, N_RRD|N_STA);

	if (m_BusWidth)
	{	Length >>= 1;
		while (Length--)
		{	*((USHORT *)Buffer) = m_Ports.inw(NS_DATA);
			Buffer += 2;
		}
	}
	else
	{	while (Length--)
		{	*Buffer++ = m_Ports.inb(NS_DATA);
		}
	}

	volatile c = 10000;
	while (((m_Ports.inb(NS_ISR)&0x40) == 0) && (c--));

	m_Ports.outb(NS_ISR, 0x40);
	m_Ports.outb(NS_CR, N_ARD|N_STA);
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CardWrite
//		Writes data into the card for transmission
// Parameters:
//		Buffer	-	input buffer to get data from
//		Offset	-	offset from the start of the card's transmit buffer(s)
//		Length	-	number of bytes to transfer
// Returns:
//		none
// NOTE:
//		Synchronously writes data. Offset must be an even number.
VOID NMNE2KAdapter::CardWrite(PUCHAR Buffer, ULONG Offset, ULONG Length)
{
	ASSERT((Offset & 1)==0);

	m_Ports.outb(NS_RSAR0, (UCHAR)Offset);
	m_Ports.outb(NS_RSAR1, (UCHAR)(Offset>>8));
	if (m_BusWidth)
		Length += (Length&1);
	m_Ports.outb(NS_RBCR0, (UCHAR)Length);
	m_Ports.outb(NS_RBCR1, (UCHAR)(Length>>8));
	m_Ports.outb(NS_CR, N_RWR|N_STA);

	if (m_BusWidth)
	{	Length >>= 1;
		while (Length--)
		{	m_Ports.outw(NS_DATA, *((USHORT *)Buffer));
			Buffer += 2;
		}
	}
	else
	{	while (Length--)
		{	m_Ports.outb(NS_DATA, *((UCHAR *)Buffer));
			Buffer += 1;
		}
	}

	volatile c = 10000;
	while (((m_Ports.inb(NS_ISR)&0x40) == 0) && (c--));

	m_Ports.outb(NS_ISR, 0x40);
	m_Ports.outb(NS_CR, N_ARD|N_STA);
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CopyDataDown
//		Writes packet's buffer(s) data into the card for transmission
// Parameters:
//		Packet	-	packet to transfer
//		Offset	-	offset from the start of the card's transmit buffer(s)
//		Length	-	number of bytes to transfer
// Returns:
//		none
// NOTE:
//		Synchronously writes data
VOID NMNE2KAdapter::CopyDataDown(PNDIS_PACKET Packet)
{
	KNdisPacket pkt(Packet);
	KNdisBuffer buf;
	ULONG Offset, Size;
	PUCHAR Buffer;
	USHORT temp;

	buf = pkt.QueryFirstBuffer();
	Offset = m_XmitStart<<8;
	while (buf.IsValid())
	{	Size = buf.Length();
		Buffer = (PUCHAR)buf.Address();

		if (Offset&1)
		{	// previous buffer had an extra byte 
			temp |= (Buffer[0]<<8);
			CardWrite((PUCHAR)&temp, Offset&0xfffe, 2);

			Buffer++;
			Offset++;
			Size--;
		}

		if (Size&1)
			temp = Buffer[Size-1];
		
		CardWrite(Buffer, Offset, Size);
		Offset += Size;

		buf = buf.GetNext();
	}

	Offset -= (m_XmitStart<<8);
	if (Offset < 60)
		Offset = 60;
	m_Ports.outb(NS_TPSR,  (UCHAR) m_XmitStart);
	m_Ports.outb(NS_TBCR0, (UCHAR) Offset);
	m_Ports.outb(NS_TBCR1, (UCHAR)(Offset>>8));

	m_Ports.outb(NS_CR, N_ARD|N_TXP|N_STA);
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CardSetFilters
//		Writes current packet filter setting to the card
// Parameters:
//		none	
// Returns:
//		none
// NOTE:
//		Synchronized with m_Interrupt (runs at DIRQL)
void NMNE2KAdapter::CardSetFilters()
{
	// Convert m_uPacketFilter bits into RCR register bits

	UCHAR rcr = 0;
    if (m_uPacketFilter & (NDIS_PACKET_TYPE_ALL_MULTICAST |
                                 NDIS_PACKET_TYPE_MULTICAST |
                                 NDIS_PACKET_TYPE_PROMISCUOUS))
		rcr |= N_AM;
	//
    // The promiscuous physical bit in the RCR should be on if ANY
    // protocol wants to be promiscuous.
    //
    if (m_uPacketFilter & NDIS_PACKET_TYPE_PROMISCUOUS)
        rcr |= N_PRO;

    //
    // The broadcast bit in the RCR should be on if ANY protocol wants
    // broadcast packets (or is promiscuous).
    //
    if (m_uPacketFilter & (NDIS_PACKET_TYPE_BROADCAST |
                                 NDIS_PACKET_TYPE_PROMISCUOUS))
        rcr |= N_AB;

	m_Ports.outb(NS_RCR, rcr);
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CardSetMulticast
//		Writes current multicast setting to the card
// Parameters:
//		none	
// Returns:
//		none
// NOTE:
//		Synchronized with m_Interrupt (runs at DIRQL)
void NMNE2KAdapter::CardSetMulticast()
{
	// copy m_MulticastBits bits into h/w 
	// set to page 1
	UCHAR cr = m_Ports.inb(NS_CR);
	m_Ports.outb(NS_CR, N_PG1 | cr & (~N_PG0));

	for (int i=0; i<sizeof(m_MulticastBits); i++)
		m_Ports.outb(NS_MAR0+i, m_MulticastBits[i]);	

	// retore CR
	m_Ports.outb(NS_CR, cr);
}

////////////////////////////////////////////////////////////////////
// NMNE2KAdapter::CardComputeCrc
//		Runs the AUTODIN II CRC algorithm over the buffer.
// Parameters:
//    Buffer - the input buffer
//    Length - the length of Buffer
// Returns:
//		32-bit CRC
// NOTE:
//	  Borrowed from the NT DDK NE2000 sample.
ULONG NMNE2KAdapter::CardComputeCrc(PUCHAR Buffer, UINT Length )
{
    ULONG Crc, Carry;
    UINT i, j;
    UCHAR CurByte;

    Crc = 0xffffffff;

    for (i = 0; i < Length; i++) {
        CurByte = Buffer[i];
        for (j = 0; j < 8; j++) {
            Carry = ((Crc & 0x80000000) ? 1 : 0) ^ (CurByte & 0x01);
            Crc <<= 1;
            CurByte >>= 1;
            if (Carry) 
                Crc = (Crc ^ 0x04c11db6) | Carry;
        }
    }
    return Crc;
}

// end of file
 