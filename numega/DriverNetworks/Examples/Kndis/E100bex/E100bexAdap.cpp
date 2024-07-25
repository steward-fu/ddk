// E100bexAdap.cpp: implementation of the E100bexAdapter class.
//
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
//
// Portions copied from Microsoft Windows 2000 DDK sample driver containing the
// following copyright
//
/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/


#include <kndis.h>
#include "E100bexInc.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


E100bexAdapter::E100bexAdapter(void) :
	KNdisMiniAdapter(),
	m_CSRIoRange(),
	m_CSRMemoryRange(),
	m_Interrupt(),
	m_Timer(),
	m_pCard(NULL),
	m_pRxShMem(NULL),
	m_pHwSendMem(NULL),
	m_pHwMiscMem(NULL),
	m_pRxArea(NULL),
	m_TxCBList(),
	m_ActiveChainList(),
	m_SendBufList(),
	m_SendWaitQueue(),
	m_pPrevHwDesc(NULL),
	m_pMpTcbMem(NULL),
	m_MpTcbMemSize(0),
	m_CurrNumRfd(0),
	m_RfdShrinkCount(0),
	//m_nReadyRecv(0),
	m_nCancelSend(0),
	m_Flags(0)
{
}

E100bexAdapter::~E100bexAdapter(void)
{
	ASSERT(m_pRxArea->GetNumberFreeDescriptors() == m_CurrNumRfd);
	delete m_pRxArea;
	m_pRxArea = NULL;

	// Delete the shared memory we allocated in initialize
	DeleteSharedAdapterMemory();

	// Delete our card object, also allocated in initialize
	delete m_pCard;
}


////////////////////////////////////////////////////////////////////
// NDIS callback handlers
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
// E100bexAdapter::Initialize
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
NDIS_STATUS E100bexAdapter::Initialize(
			IN OUT KNdisMedium& Medium,
			IN KNdisConfig& Config
			)
{
	TRACE("E100bexAdapter::Initialize()++\n");
	NDIS_STATUS Status;

	// Select our the Medium (pretending we are Ethernet):
	if (! Medium.Select(NdisMedium802_3) )
		KNDIS_RETURN_ERROR (NDIS_STATUS_UNSUPPORTED_MEDIA);

	// Create our initialization structure for registry overrides
	ADAPTER_INFO Ai;

	// Read some other config stuff from the Registry, e.g.
	if ((Status = ParseRegistryParameters(Config, Ai)) != NDIS_STATUS_SUCCESS)
		KNDIS_RETURN_ERROR (Status);

	// Initialize the number of Tcbs we will have in our queue
	// this is to work around limitations of the hardware and the S-bit
	m_NumTcb = m_RegNumTcb + 1;

	// Reset the Stats counters and other data members if any:
	m_Stats.Reset();

	// Now let NDIS know about the BUS the NIC is on, and any flags that we need set
	// for our adapter. Here's where the NDIS/Adapter-instance
	// handshake takes place. This should happen *before* allocating the h/w resources:
	SetAttributesEx(
		NdisInterfacePci,
		NDIS_ATTRIBUTE_DESERIALIZE | NDIS_ATTRIBUTE_BUS_MASTER);

	// Get h/w resources assigned to our adapter.
	KNdisPnpResourceRequest request(Config);	// for NDIS 5

	// Our adapter needs the Interrupt, Port and Memory resources.
	// Get descriptors of those. We'll use the descriptors when
	// allocating h/w access objects such as KNdisIoRange and KNdisInterrupt:
	KNdisResource<CmResourceTypeInterrupt>	Int(request);
	KNdisResource<CmResourceTypePort>		Port(request);

	// Look for the last memory bar with a length of 0x1000, as others might
	// be our flash address, a boot ROM address, or otherwise.
	ULONG Ordinal = 0;
	ULONG LastRightSizedOrdinal = 0;
	BOOLEAN Valid;
	do
	{
		KNdisResource<CmResourceTypeMemory>	Memory(request, Ordinal);
		Valid = Memory.IsValid();
		if (Valid && Memory.Length() == 0x1000)
			LastRightSizedOrdinal = Ordinal;
		Ordinal++;
	} while (Valid);
	KNdisResource<CmResourceTypeMemory>	Memory(request, LastRightSizedOrdinal);

	// Make sure the resources are defined in the Registry:
	if (!Int.IsValid())
		KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);

	if (!Port.IsValid())
		KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);

	if (!Memory.IsValid() || Memory.Length() != 0x1000)
		KNDIS_RETURN_ERROR (NDIS_STATUS_NOT_ACCEPTED);

	// Create the h/w access objects.

	// Register our I/O ports:
	m_CSRIoRange.Initialize(this, Port);

	if (! NT_SUCCESS(m_CSRIoRange.Status()))
		 KNDIS_RETURN_ERROR (m_CSRIoRange.Status());

	// Register our memory range:
	m_CSRMemoryRange.Initialize(this, Memory);

	if (! NT_SUCCESS(m_CSRMemoryRange.Status()))
		 KNDIS_RETURN_ERROR (m_CSRMemoryRange.Status());

	// Create an instance of our PCI information class to
	// query information from PCI configuration space
	E100bexPciInformation PciInfo(this);

	// Read the RevisionID
	m_AiRevID = PciInfo.ReadRevisionId();

	// Get the MemoryWriteInvalidateBit, then 'and' it with the value
	// we obtained from the registry to set the control
	Ai.MWIEnable = Ai.MWIEnable && PciInfo.GetMemoryWriteInvalidateBit();

	// Enable bus mastering if not already enabled
	if (FALSE == PciInfo.TestAndSetDmaMasterBit())
	{
		ASSERT(!"Bus master was not enabled\n");
	}

	// Read the subsystem vendor and subsystem device IDs
	PciInfo.ReadSubsystemId(m_AiSubVendor, m_AiSubDevice);

	// Create our "card" object. The real card class might want to use
	// references to m_Ports and m_Interrupt as constructor's parameters.
	m_pCard = new E100bexCard(*this, m_CSRMemoryRange, Ai);

	if ( NULL == m_pCard )
		 KNDIS_RETURN_ERROR ( NDIS_STATUS_RESOURCES );

	// Get network address (if any) from the Registry. If specified
	// it will be used instead of burned permanent address:
	if (Config.ReadNetworkAddress(m_AiNodeAddress) != NDIS_STATUS_SUCCESS)
		m_AiNodeAddress.Invalidate();

	// Check if there was a node address over-ride.  If there isn't then
	// use the adapter's permanent node address as the "current" node address.
	// If a node address over-ride is present in the registry, then use
	// that override address as the node address instead of the permanent
	// address in the adapter's EEPROM
	if ( ! m_AiNodeAddress.IsValid() )
	{
		// No node address override so use the permanent address
		TRACE("E100bexAdapter::Initialize: No node address over-ride, using permanent address\n");

		// Read our node address from the EEPROM.
		ETHERNET_ADDRESS PermanentNodeAddress = m_pCard->ReadPermanentNodeAddress();

		m_AiNodeAddress = PermanentNodeAddress;
	}

	TRACE("E100bexAdapter::Initialize: Node Address is  %.2x %.2x %.2x %.2x %.2x %.2x\n",
		m_AiNodeAddress.m_bytes[0],
		m_AiNodeAddress.m_bytes[1],
		m_AiNodeAddress.m_bytes[2],
		m_AiNodeAddress.m_bytes[3],
		m_AiNodeAddress.m_bytes[4],
		m_AiNodeAddress.m_bytes[5]
		);

	// Validate the current node address (make sure its not a mulicast)
	if ((UCHAR) m_AiNodeAddress.m_bytes[0] & 1)
	{
		TRACE("E100bexAdapter::Initialize: Node address invalid -- its a MC address\n");

		KNDIS_RETURN_ERROR (NDIS_STATUS_FAILURE);
	}

	// Setup the shared adapter memory (DMA)
	if ((Status = NICAllocAdapterMemory()) != NDIS_STATUS_SUCCESS)
		KNDIS_RETURN_ERROR (Status);

	// Setup and initialize the transmit structures
	NICInitSend();

	// Create a "receive frame" area.  Must do this before EnableCard
	if ( (Status = NICInitRecv()) != NDIS_STATUS_SUCCESS )
		KNDIS_RETURN_ERROR (Status);

	// Disable interrupts on the H/W before registering the interrupt with NDIS
	// Must NICAllocAdapterMemory() before you can do this
	m_pCard->DisableInterrupt();

	// Register our interrupt: NOTE that we must register an interrupt object
	// as long as we expose Disable/EnableInterrupt() handlers. NDIS would use
	// them internally for KeSynchronizeExecition().
	m_Interrupt.Initialize(
		this,
		Int,
		NdisInterruptLatched);

	if (! NT_SUCCESS(m_Interrupt.Status()))
		 KNDIS_RETURN_ERROR (m_Interrupt.Status());

	// set up our link indication variable
	// it doesn't matter what this is right now because it will be
	// set correctly if link fails
	m_LinkIsActive = NdisMediaStateConnected;

	// Set our power state to on.
	m_Power = NdisDeviceStateD0;

	// Initialize our timer object for use with our reset routine
	m_Timer.Initialize(this, KNDIS_MEMBER_CALLBACK(AsyncResetTimerCallback));

	// Set default filter and MAC options. In principle, we should also
	// relay that to our card...
	// This miniport indicates receive with NdisMIndicateReceivePacket
	// function. It has no MiniportTransferData function. Such a driver
	// should set this flag.
	m_uMacOptions = NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
					NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
					NDIS_MAC_OPTION_NO_LOOPBACK;

	// Initialize the hardware and enable the interrupt
	if ( (Status = EnableCard()) != NDIS_STATUS_SUCCESS )
		KNDIS_RETURN_ERROR (Status);

	TRACE("E100bexAdapter::Initialize()--\n");
	return NDIS_STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::EnableCard
//
// This routine will initializate the hardware and
// enable the card's interrupt.
//
// Parameters:
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE:
//		This method MUST NOT be called until the following have been done
//			- The card object has been created
//			- The m_AiNodeAddress has been initialized with the address that
//				will be used by the card.
//			- NICInitRecv has been called
//			- NICInitSend has been called
//
//		This routine will be called from Initialize, and also from
//		setOID_PNP_SET_POWER when transitioning from D3 to D0 power state.
//
NDIS_STATUS E100bexAdapter::EnableCard(void)
{
	NDIS_STATUS Status;

	// Test the adapter hardware
	if (( Status = m_pCard->SelfTest() ) != NDIS_STATUS_SUCCESS)
		KNDIS_RETURN_ERROR (Status);

	if ( !m_pCard->DetectPhy() )
		KNDIS_RETURN_ERROR (NDIS_STATUS_FAILURE);

	m_pCard->SetMcTimeoutFlag();

	// Start the card up. Interrupts are disabled...
	if (( Status = m_pCard->Init(m_AiNodeAddress) ) != NDIS_STATUS_SUCCESS)
		KNDIS_RETURN_ERROR (Status);

	StartReceiveUnit();

	m_pCard->EnableInterrupt();

	return NDIS_STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::StartReceiveUnit
//
// This routine will start the receive unit on the NIC.
//
// Parameters:
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
BOOLEAN E100bexAdapter::StartReceiveUnit()
{
	TRACE3(("E100bexAdapter::StartReceiveUnit++\n"));

	BOOLEAN Status = FALSE;

	// If the receiver is ready, then don't try to restart.
	if ( m_pCard->ReceiveUnitStarted() )
	{
		return Status;
	}

	TRACE3(("Re-starting receive unit...\n"))

	KNdisPacket Pkt(m_pRxArea->GetPacket());

	PHW_RFD pRfd = NULL;

	if( Pkt.IsValid() )
	{
		pRfd = reinterpret_cast<PHW_RFD>(m_pRxArea->GetDescriptor(Pkt));
		ASSERT( pRfd );
	}

	// Check to make sure that our RFD head is available.  If its not, then
	// we should process the rest of our receives
	if (( !Pkt.IsValid() || pRfd->RfdCbHeader.CbStatus))
	{
		Status = ProcessRXInterrupt();
	}

#if DBG

	// Big hack to check to make sure that all of the RFDs are indeed clear.
	// If they are not then we'll generate a break point.
	UINT nReadyRecv = m_pRxArea->GetNumberFreeDescriptors();
	for (UINT i=0; i< nReadyRecv; i++)
	{
		KNdisPacket Packet(reinterpret_cast<KNdisReceiveArea*>(m_pRxArea)
			->Complete(reinterpret_cast<PHW_RFD>(m_pRxArea->GetHeadDescriptor())));
		ASSERT( Packet.IsValid() );

		PHW_RFD pHwRfd = reinterpret_cast<PHW_RFD>(m_pRxArea->GetDescriptor(Packet));
		ASSERT( pHwRfd );

		if (pHwRfd->RfdCbHeader.CbStatus & RFD_STATUS_COMPLETE)
		{
			TRACE("RFD NOT PROCESSED!!!\n");
//            DbgBreakPoint();
		}

		ASSERT( pHwRfd->RfdRbdPointer == DRIVER_NULL);
		if(i< (nReadyRecv-1))
		{
			ASSERT( pHwRfd->RfdCbHeader.CbCommand == 0 );
			ASSERT( pHwRfd->RfdCbHeader.CbLinkPointer != 0 && pHwRfd->RfdCbHeader.CbLinkPointer != DRIVER_NULL);
		}
		else
		{
			ASSERT( pHwRfd->RfdCbHeader.CbCommand == RFD_EL_BIT );
			ASSERT( pHwRfd->RfdCbHeader.CbLinkPointer == DRIVER_NULL );
		}

		reinterpret_cast<KNdisReceiveArea*>(m_pRxArea)->Reclaim(Packet);

	}

#endif //DBG

	// Get the physical address of the current descriptor (accesses
	//		the head of the free RFD list in KNdisSharedReceiveArea)
	PNDIS_PHYSICAL_ADDRESS pPhysAddr = m_pRxArea->GetHeadPhysicalAddress();
	// pPhysAddr could be NULL if no RFD is available
	if(pPhysAddr && pPhysAddr->LowPart)
		// Call the card method to start the receive unit.
		m_pCard->StartReceiveUnit(pPhysAddr);

	TRACE3(("E100bexAdapter::StartReceiveUnit--\n"));
	return Status;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::ParseRegistryParameters
//
// This routine will read our parameters from the registry
//
// Parameters:
//		Config
//			Reference to KNdisConfig object that we use to query various
//			configuration parameters
//		Ai
//			Reference to ADAPTER_INFO object used to store certain registry paramters
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE:
//
NDIS_STATUS E100bexAdapter::ParseRegistryParameters(IN KNdisConfig& Config, IN ADAPTER_INFO& Ai)
{

#if DBG
	// debug trace mask
	ULONG nDebug = Config.Read(KNDIS_STRING_CONST("Debug"), E100BEX_Debug_DEFAULT,
		E100BEX_Debug_MIN, E100BEX_Debug_MAX, NdisParameterInteger);

	// The debug trace mask will be stored in the high word of KNdisOidDebug::sm_uTraceMask
	// Left shift the debug trace mask by 16 bits so that the high word will be used
	nDebug <<= 16;

	// Access the current KNdisOidDebug::sm_uTraceMask and or with the debug trace mask
	// KNdisOidDebug::sm_uTraceMask utilizes the low word.  The high word will be masked off
	// for our purposes (this drivers debug trace mask)
	nDebug |= ( 0x0030FFFF & KNdisOidDebug::TraceMask());

	// Update KNdisOidDebug::sm_uTraceMask to have the original KNdisOidDebug::sm_uTraceMask
	// stored in the low word and the debug trace mask in the high word.
	KNdisOidDebug::EnableTrace(nDebug);
#endif // DBG

	m_NumRfd = static_cast<UINT>(Config.Read(KNDIS_STRING_CONST("NumRfd"),
		E100BEX_NumRfd_DEFAULT, E100BEX_NumRfd_MIN, E100BEX_NumRfd_MAX));

	m_RegNumTcb = static_cast<UINT>(Config.Read(KNDIS_STRING_CONST("NumTcb"),
		E100BEX_RegNumTcb_DEFAULT, E100BEX_RegNumTcb_MIN, E100BEX_RegNumTcb_MAX));

	m_NumTbdPerTcb = static_cast<UINT>(Config.Read(KNDIS_STRING_CONST("NumTbdPerTcb"),
		E100BEX_NumTbdPerTcb_DEFAULT, E100BEX_NumTbdPerTcb_MIN, E100BEX_NumTbdPerTcb_MAX));

	m_NumBuffers = static_cast<UINT>(Config.Read(KNDIS_STRING_CONST("NumCoalesce"),
		E100BEX_NumBuffer_DEFAULT, E100BEX_NumBuffer_MIN, E100BEX_NumBuffer_MAX));

	m_AiThreshold = static_cast<USHORT>(Config.Read(KNDIS_STRING_CONST("Threshold"),
		E100BEX_AiThreshold_DEFAULT, E100BEX_AiThreshold_MIN, E100BEX_AiThreshold_MAX));

	Ai.PhyAddress = static_cast<UINT>(Config.Read(KNDIS_STRING_CONST("PhyAddress"),
		E100BEX_PhyAddress_DEFAULT, E100BEX_PhyAddress_MIN, E100BEX_PhyAddress_MAX));

	Ai.Connector = static_cast<UCHAR>(Config.Read(KNDIS_STRING_CONST("Connector"),
		E100BEX_Connector_DEFAULT, E100BEX_Connector_MIN, E100BEX_Connector_MAX));

	Ai.AiTxFifo = static_cast<USHORT>(Config.Read(KNDIS_STRING_CONST("TxFifo"),
		E100BEX_AiTxFifo_DEFAULT, E100BEX_AiTxFifo_MIN, E100BEX_AiTxFifo_MAX));

	Ai.AiRxFifo = static_cast<USHORT>(Config.Read(KNDIS_STRING_CONST("RxFifo"),
		E100BEX_AiRxFifo_DEFAULT, E100BEX_AiRxFifo_MIN, E100BEX_AiRxFifo_MAX));

	Ai.AiTxDmaCount = static_cast<UCHAR>(Config.Read(KNDIS_STRING_CONST("TxDmaCount"),
		E100BEX_AiTxDmaCount_DEFAULT, E100BEX_AiTxDmaCount_MIN, E100BEX_AiTxDmaCount_MAX));

	Ai.AiRxDmaCount = static_cast<UCHAR>(Config.Read(KNDIS_STRING_CONST("RxDmaCount"),
		E100BEX_AiRxDmaCount_DEFAULT, E100BEX_AiRxDmaCount_MIN, E100BEX_AiRxDmaCount_MAX));

	Ai.AiUnderrunRetry = static_cast<UCHAR>(Config.Read(KNDIS_STRING_CONST("UnderrunRetry"),
		E100BEX_AiUnderrunRetry_DEFAULT, E100BEX_AiUnderrunRetry_MIN, E100BEX_AiUnderrunRetry_MAX));

	Ai.AiForceDpx = static_cast<UCHAR>(Config.Read(KNDIS_STRING_CONST("ForceDpx"),
		E100BEX_AiForceDpx_DEFAULT, E100BEX_AiForceDpx_MIN, E100BEX_AiForceDpx_MAX));

	Ai.AiTempSpeed = static_cast<USHORT>(Config.Read(KNDIS_STRING_CONST("Speed"),
		E100BEX_AiTempSpeed_DEFAULT, E100BEX_AiTempSpeed_MIN, E100BEX_AiTempSpeed_MAX));

	Ai.McTimeoutFlag = static_cast<UINT>(Config.Read(KNDIS_STRING_CONST("MCWA"),
		E100BEX_McTimeoutFlag_DEFAULT, E100BEX_McTimeoutFlag_MIN, E100BEX_McTimeoutFlag_MAX));

	Ai.MWIEnable = static_cast<BOOLEAN>(Config.Read(KNDIS_STRING_CONST("MWIEnable"),
		E100BEX_MWIEnable_DEFAULT, E100BEX_MWIEnable_MIN, E100BEX_MWIEnable_MAX));

	Ai.Congest = static_cast<UCHAR>(Config.Read(KNDIS_STRING_CONST("Congest"),
		E100BEX_Congest_DEFAULT, E100BEX_Congest_MIN, E100BEX_Congest_MAX));

	return NDIS_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////
// E100bexAdapter::NICAllocAdapterMemory
//
// This routine aligns both IN OUT pLowPhysAddr physical address and IN virtual ShMem address
// Return: aligned virtual ShMem address
//
PUCHAR E100bexAdapter::AlignSharedMemory(
	IN OUT PULONG pLowPhysAddr,
	ULONG Alignment,
	KNdisSharedMemory& ShMem
	)
{
	ASSERT( pLowPhysAddr );
	ASSERT( ShMem.IsValid() );

	PUCHAR pMem = NULL;

	if ( pLowPhysAddr && ShMem.IsValid() )
	{
		ULONG LowPhysAddr = *pLowPhysAddr;
		LowPhysAddr += Alignment - 1;
		LowPhysAddr &= ~(Alignment - ULONG(1));

		pMem = reinterpret_cast<PUCHAR>(ShMem.VirtualAddress()) + LowPhysAddr - NdisGetPhysicalAddressLow( *ShMem.PhysicalAddress() );
		*pLowPhysAddr = LowPhysAddr;
	}

	return pMem;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::NICAllocAdapterMemory
//
// This routine will setup the adapter memory shared with the NIC card
//
// Parameters:
//
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE:
//
NDIS_STATUS E100bexAdapter::NICAllocAdapterMemory(void)
{
	TRACE("E100bexAdapter::NICAllocAdapterMemory++\n");
	NDIS_STATUS Status;

	// Total number of TBDs equals number of TCBs * number of TBDs per TCB
	m_NumTbd = (m_NumTcb * m_NumTbdPerTcb);

	Status = NdisMInitializeScatterGatherDma(
				 *this,
				 FALSE,
				 NIC_MAX_PACKET_SIZE);

	if (Status == NDIS_STATUS_SUCCESS)
	{
		MP_SET_FLAG(fMP_ADAPTER_SCATTER_GATHER);
	}
	else
	{
		TRACE("Failed to init ScatterGather DMA\n");

		//
		// NDIS 5.1 miniport should NOT use map registers
		//
		KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_21);
	}

	//
	//
	// Allocate MP_TCB's
	//
	// Allocate cached memory for the _D100SwTcb Transmit Control Block (TCB)
	m_MpTcbMemSize = (m_NumTcb * sizeof(D100SwTcb));
	m_pMpTcbMem = new UCHAR[m_MpTcbMemSize];
	if (m_pMpTcbMem == NULL)
	{
		TRACE("Could not allocate %d bytes for MpTcbMem mem\n", m_MpTcbMemSize);
		KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_14);
	}
	NdisZeroMemory(m_pMpTcbMem, m_MpTcbMemSize);
	TRACE("Allocated %08x %8d bytes for MpTcbMem mem\n", m_pMpTcbMem, m_MpTcbMemSize);

	//
	// Now the driver needs to allocate send buffer pool, the number
	// of send buffers the driver needs is the larger one of m_NumBuffers
	// and m_NumTcb
	//
	Status = m_SendBufferPool.Initialize(max(m_NumTcb, m_NumBuffers));
	if (Status != NDIS_STATUS_SUCCESS)
	{
		TRACE("Failed to allocate send buffer pool\n");
		KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_14);
	}


	// Allocate send buffers
	int MpTxBufMemSize = m_NumBuffers * sizeof(COALESCE);
	m_MpTxBufMem = new UCHAR[MpTxBufMemSize];
	if (m_MpTxBufMem == NULL)
	{
		TRACE("Failed to allocate MP_TXBUF's\n");
		KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_14);
	}
	NdisZeroMemory(m_MpTxBufMem, MpTxBufMemSize);
	PCOALESCE pMpTxBuf = (PCOALESCE) m_MpTxBufMem;	// PMP_TXBUF

	//
	// NdisMGetDmaAlignment is provided in XP (WINVER=0x0501) and higher
	// if you need to write a driver that runs on older versions of Windows
	// you need to compile with older versions of DDK which have WINVER < 0x0501
	// such as W2K DDK.
	//
	int CacheFillSize = NdisMGetDmaAlignment(*this);
	TRACE("CacheFillSize=%d\n", CacheFillSize);

	// Reset the coalesce list
	m_SendBufList.Reset();

	for (ULONG index = 0; index < m_NumBuffers; index++)
	{
		pMpTxBuf->BufferSize = NIC_MAX_PACKET_SIZE;

		//
		// Allocate and align the buffer on the cache line boundary
		//
		pMpTxBuf->pAllocMem = new KNdisSharedMemory(this,
											NIC_MAX_PACKET_SIZE + CacheFillSize,	// Length
											CacheFillSize,			// Alignment
											TRUE					// Cached
											);
		if( ! pMpTxBuf->pAllocMem->IsValid() )
		{
			KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_13);
		}

		// Note: pMpTxBuf->BufferSize may NOT be the same as pMpTxBuf->pAllocMem->Length()
		// Remember to set NdisBuffer length back to pMpTxBuf->BufferSize before free it.
		pMpTxBuf->NdisBuffer = m_SendBufferPool.Allocate(pMpTxBuf->pAllocMem->VirtualAddress(), pMpTxBuf->BufferSize);
		TRACE2(("NdisBuffer %d=%p\n", index, (PNDIS_BUFFER)pMpTxBuf->NdisBuffer));

		m_SendBufList.InsertTail(pMpTxBuf);
		pMpTxBuf++;
	}

	// HW_START

	//
	// Allocate shared memory for send
	//
	UINT HwSendMemAllocSize = m_NumTcb * (sizeof(TXCB_STRUC) +
								NIC_MAX_PHYS_BUF_COUNT * sizeof(TBD_STRUC));
	m_pHwSendMem = new KNdisSharedMemory(
		this,
		HwSendMemAllocSize
	);
	if( ! m_pHwSendMem->IsValid() )
	{
		TRACE("Could not allocate %d bytes for m_pHwSendMem mem\n", HwSendMemAllocSize);
		KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_13);
	}
	NdisZeroMemory(m_pHwSendMem->VirtualAddress(), m_pHwSendMem->Length());
	TRACE("Allocated %08x %8d bytes for m_pHwSendMem mem\n", m_pHwSendMem->VirtualAddress(), m_pHwSendMem->Length());
	TRACE(" Range: %08x - %08x\n", m_pHwSendMem->VirtualAddress(), (PCHAR)m_pHwSendMem->VirtualAddress()+m_pHwSendMem->Length());

	//
	// Allocate shared memory for other uses
	//
	UINT HwMiscMemAllocSize =
		sizeof(SELF_TEST_STRUC) + ALIGN_16 +
		sizeof(DUMP_AREA_STRUC) + ALIGN_16 +
		sizeof(NON_TRANSMIT_CB) + ALIGN_16 +
		sizeof(ERR_COUNT_STRUC) + ALIGN_16;

	//
	// Allocate the shared memory for the command block data structures.
	//
	m_pHwMiscMem = new KNdisSharedMemory(
		this,
		HwMiscMemAllocSize
	);
	if( ! m_pHwMiscMem->IsValid() )
	{
		TRACE("Could not allocate %d bytes for m_pHwMiscMem mem\n", HwMiscMemAllocSize);
		KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_13);
	}
	NdisZeroMemory(m_pHwMiscMem->VirtualAddress(), m_pHwMiscMem->Length());
	TRACE("Allocated %08x %8d bytes for m_pHwMiscMem mem\n", m_pHwMiscMem->VirtualAddress(), m_pHwMiscMem->Length());

	// Initialize the 82557 Control Structures pointers
	//PUCHAR pMem = NULL;
	ULONG MemPhys = NdisGetPhysicalAddressLow( *m_pHwMiscMem->PhysicalAddress() );

	// The first part of the shared memory will be reserved for the card object's structures.
	// Notify the card object of its memory (the start of the buffer).  The card object
	// will adjust the pointers to just beyond the end of the card object's structures.
	MemPhys = m_pCard->SetupSharedMemory(MemPhys, *m_pHwMiscMem);


	//
	// Recv
	//
	//m_RecvLookaside.Initialize(0, (ULONG)'001E');
	MP_SET_FLAG(fMP_ADAPTER_RECV_LOOKASIDE);

	// set the max number of RFDs
	// disable the RFD grow/shrink scheme if user specifies a NumRfd value
	// larger than NIC_MAX_GROW_RFDS
	m_MaxNumRfd = max(m_NumRfd, NIC_MAX_GROW_RFDS);

	// Allocate memory for the shared receive resources with enough
	// extra to paragraph align everything.
	ULONG Length = (m_NumRfd * sizeof(HW_RFD)) + MORE_DATA_FOR_ALIGN;
	// Allocate the initial shared memory for the receive data structures.
	// Make sure that m_NumRfd matches the "Initial number of RFD's" parameter of E100bexReceiveArea ctor
	m_pRxShMem = new KNdisSharedMemory(	this, Length);
	if( ! m_pRxShMem->IsValid() )
	{
		TRACE("Could not allocate %d bytes for m_pShMem mem\n", Length);
		KNDIS_RETURN_ERROR_1PARAM(NDIS_ERROR_CODE_OUT_OF_RESOURCES, EVENT_13);
	}
	TRACE("Allocated %08x %8d bytes for RecvUnCached mem\n", m_pRxShMem->VirtualAddress(), m_pRxShMem->Length());


	TRACE("E100bexAdapter::NICAllocAdapterMemory--\n");
	return NDIS_STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::DeleteSharedAdapterMemory
//
// This routine will delete the adapter memory shared with the NIC card
//
// Parameters:
//
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE:
//
VOID E100bexAdapter::DeleteSharedAdapterMemory(void)
{
	// Free any memory allocated for the shared receive structures (RFDs)
	if ( m_pRxShMem )
	{
		TRACE("Freeing %d bytes m_pShMem\n", m_pRxShMem->Length() );
		delete m_pRxShMem;
		m_pRxShMem = NULL;
	}

	//
	// Free SEND memory/NDIS buffer/NDIS packets/shared memory
	//
	while (!m_SendBufList.IsEmpty())
	{
		PCOALESCE pMpTxBuf = (PCOALESCE)m_SendBufList.RemoveHead();
		ASSERT(pMpTxBuf);

		// Free the NDIS buffer
		if (pMpTxBuf->NdisBuffer.IsValid())
		{
			////TRACE3(("Release NdisBuffer=%p\n", (PNDIS_BUFFER)pMpTxBuf->NdisBuffer));
			pMpTxBuf->NdisBuffer.AdjustLength(pMpTxBuf->BufferSize);
			m_SendBufferPool.Free(pMpTxBuf->NdisBuffer);
		}

		// Free the shared memory associated with each MP_TXBUF
		if (pMpTxBuf->pAllocMem)
		{
			delete pMpTxBuf->pAllocMem;
			pMpTxBuf->pAllocMem = NULL;
		}
	}

    // Free the memory for MP_TXBUF structures
    if (m_MpTxBufMem)
    {
        delete m_MpTxBufMem;
        m_MpTxBufMem = NULL;
    }

	// Free the shared memory for HW_TCB structures
	if ( m_pHwSendMem )
	{
		TRACE("Freeing %d bytes m_pHwSendMem\n", m_pHwSendMem->Length() );
		delete m_pHwSendMem;
		m_pHwSendMem = NULL;
	}

	// Free the shared memory for other command data structures
	if ( m_pHwMiscMem )
	{
		TRACE("Freeing %d bytes m_pHwMiscMem\n", m_pHwMiscMem->Length() );
		delete m_pHwMiscMem;
		m_pHwMiscMem = NULL;
	}

	// Free the memory for MP_TCB structures
	TRACE("Freeing %d bytes m_MpTcbMemSize\n", m_MpTcbMemSize);
	delete m_pMpTcbMem;
	m_pMpTcbMem = NULL;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::Halt
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
//       to undo things done by the Initialize() above - in the reverse order.
//
//		 Alternatevely, the driver writer can move all the deallocations to the
//       destructor: the destructor will be called immediatelly after Halt()
//		 returns. Our sample follows this practice.
//
VOID E100bexAdapter::Halt(void)
{
	TRACE("E100bexAdapter::Halt() Entered\n");

	DisableCard();
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::DisableCard
//
// This routine will disable the card and disable the interrupt
//
// Parameters:
// IRQL:
//		PASSIVE_LEVEL
// Return Mode:
//		Synchronous
//
// NOTE:
//		This routine will be called from Halt, and also from
//		setOID_PNP_SET_POWER when transitioning from D0 to D3 power state.
//
VOID E100bexAdapter::DisableCard(void)
{
	// Cancel our timer
	if ( /*m_ResetInProgress && */!m_Timer.Cancel())
	{
		// A reset could be in progress here.  We might want to add m_ResetInProgress
		// BOOLEAN that we wait on if TRUE and the cancel timer fails, meaning that our
		// Async callback routine will be called
	}

	// Call Shutdown handler to disable interrupt and turn the hardware off
	// by issuing a full reset
	Shutdown();

	// check to make sure there are no outstanding transmits
	while(!m_SendWaitQueue.IsEmpty())
	{
		SendComplete(m_SendWaitQueue.RemoveHead(), NDIS_STATUS_FAILURE);
	}

	// Free the packets being actively sent & stopped
	while (!m_ActiveChainList.IsEmpty())
	{
		// here we have to fail any sends that haven't completed yet
		PD100SwTcb pMpTcb = m_ActiveChainList.RemoveHead();

		// If this wasn't a multicast command, then we need to check to see
		// if we need to issue send complete
		if ((pMpTcb->HwTcb->TxCbHeader.CbCommand & CB_CMD_MASK) != CB_MULTICAST)
		{
			FreeSendPacket(pMpTcb);
		}
	}
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::Reset
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
//		DISPATCH_LEVEL. take advantage of this fact when acquiring or releasing spinlocks
// Return Mode:
//		Asynchronous
NDIS_STATUS E100bexAdapter::Reset
			(OUT PBOOLEAN AddressingReset)
{
	TRACE("E100bexAdapter::Reset() Entered\n");

	NDIS_STATUS     Status;
	*AddressingReset = TRUE;

	m_Lock.LockDpr();
	m_SendLock.LockDpr();
	m_RcvLock.LockDpr();

	ASSERT(!MP_TEST_FLAG(fMP_ADAPTER_HALT_IN_PROGRESS));

	//
	// Is this adapter already doing a reset?
	//
	if (MP_TEST_FLAG(fMP_ADAPTER_RESET_IN_PROGRESS))
	{
		Status = NDIS_STATUS_RESET_IN_PROGRESS;
		goto exit;
	}
	MP_SET_FLAG(fMP_ADAPTER_RESET_IN_PROGRESS);

	//
	// Is this adapter going to be removed
	//
	if (MP_TEST_FLAG(fMP_ADAPTER_NON_RECOVER_ERROR))
	{
		Status = NDIS_STATUS_HARD_ERRORS;
		if (MP_TEST_FLAG(fMP_ADAPTER_REMOVE_IN_PROGRESS))
		{
		   goto exit;
		}

		// This is an unrecoverable hardware failure.
		// We need to tell NDIS to remove this miniport
		MP_SET_FLAG(fMP_ADAPTER_REMOVE_IN_PROGRESS);
		MP_CLEAR_FLAG(fMP_ADAPTER_RESET_IN_PROGRESS);

		m_RcvLock.UnlockDpr();
		m_SendLock.UnlockDpr();
		m_Lock.UnlockDpr();

		KNDIS_RETURN_ERROR(NDIS_ERROR_CODE_HARDWARE_FAILURE);
		NdisMRemoveMiniport(*this);
		return Status;
	}

	// Disable interrupts while re-initializing the transmit structures
	m_pCard->DisableInterrupt();
	// Issue a selective reset to make the CU idle.  This will also abort
	// the RU, and make the receive unit go idle.
	m_pCard->IssueSelectiveReset();

	// release all the locks and then acquire back the send lock
	// we are going to clean up the send queues
	// which may involve calling Ndis APIs
	// release all the locks before grabbing the send lock to
	// avoid deadlocks
	m_RcvLock.UnlockDpr();
	m_SendLock.UnlockDpr();
	m_Lock.UnlockDpr();


	m_SendLock.LockDpr();
	// The NDIS 5 support for deserialized miniports requires that
	// when reset is called, the driver de-queue and fail all uncompleted
	// sends, and complete any uncompleted sends. Essentially there must be
	// no pending send requests left when this routine is exited.

	// we will fail all sends that are left right now.
	TRACE("FreeQueuedSendPackets...");
	while(!m_SendWaitQueue.IsEmpty())
	{
		// we must release the lock here before returning control to ndis
		// (even temporarily like this)
		m_SendLock.UnlockDpr();
		SendComplete(m_SendWaitQueue.RemoveHead(), NDIS_STATUS_FAILURE);
		m_SendLock.LockDpr();
	}
	TRACE("Done!\n");

	// clean up all the packets we have successfully TX'd
	//ProcessTXInterrupt();

	TRACE("FreeBusySendPackets...");
	while (!m_ActiveChainList.IsEmpty())
	{
		// here we have to fail any sends that haven't completed yet
		PD100SwTcb pMpTcb = m_ActiveChainList.RemoveHead();

		// If this wasn't a multicast command, then we need to check to see
		// if we need to issue send complete
		if ((pMpTcb->HwTcb->TxCbHeader.CbCommand & CB_CMD_MASK) != CB_MULTICAST)
		{
			FreeSendPacket(pMpTcb);
		}
	}
	TRACE("Done!\n");

	// Cancel the timer
	m_Timer.Cancel();

	// Clear out our software transmit structures
	NdisZeroMemory((PVOID) m_pMpTcbMem, m_MpTcbMemSize);

	// Re-initialize the send structures
	TRACE("Reset: Calling NICInitSend\n");
	NICInitSend();
	m_SendLock.UnlockDpr();

	// get all the locks again in the right order
	m_Lock.LockDpr();
	m_SendLock.LockDpr();
	m_RcvLock.LockDpr();


	// set a reset timer to call back in 500 ms so the
	// spinlock is not held too long
	//m_Timer.Set(500);

	m_pCard->EnableInterrupt();

exit:
	m_RcvLock.UnlockDpr();
	m_SendLock.UnlockDpr();
	m_Lock.UnlockDpr();

	return NDIS_STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::Shutdown
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
VOID E100bexAdapter::Shutdown(void)
{
	TRACE("E100bexAdapter::Shutdown() Entered\n");

	// Disable the device's interrupt line.
	m_pCard->DisableInterrupt();

	// Abort the Receive unit so the hardware doesn't continue to receive packets.  If
	// the Receive unit is not aborted, the hardware could still DMA receive packets
	// into host memory after a warm boot.
	m_pCard->AbortReceiveUnit();

	// Wait 30 Milliseconds for the device to abort the RU.  This really
	// is not necessary, but I'm a little paranoid about reseting the PHY
	// when the RU is active.
	StallExecution(30);

	// Reset the PHY chip.  We do this so that after a warm boot, the PHY will
	// be in a known state, with auto-negotiation enabled.
	m_pCard->ResetPhy();
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::SendPackets
//
// Transfers a protocol-supplied packet over the network
//
// Parameters:
//		PacketArray
//			Points to an array of packet descriptors specifying the data to be transmitted.
//		NumberOfPackets
//			Specifies the number of packets in the array.
// IRQL:
//		DISPATCH_LEVEL
// Return Mode:
//		Asynchronous
// NOTE:
VOID E100bexAdapter::SendPackets(
			IN	PPNDIS_PACKET			PacketArray,
			IN	UINT					NumberOfPackets
			)
{
	TRACE3(("E100bexAdapter::SendPackets() Entered\n"));

	m_SendLock.Lock();

	UINT			PacketIndex;
	KNdisPacketArray PktArray(PacketArray, NumberOfPackets);
	KNdisPacket		Pkt;

	// Adapter is ready, send these packets
	for(PacketIndex=0; PacketIndex < NumberOfPackets; PacketIndex++)
	{
		Pkt = PktArray[PacketIndex];
		// check for a zero pointer
		ASSERT(Pkt.IsValid());

		// if queue is not empty(tcb is not available), put into queue to maintain packet sequence
		if (!m_SendWaitQueue.IsEmpty())
		{
			m_SendWaitQueue.InsertTail(Pkt);
		}
		else
		{
			MpSendPacket(Pkt, FALSE);
		}
	}

	m_SendLock.Unlock();
	return;
}

//-----------------------------------------------------------------------------
// Procedure:   MpSendPacket
//
// Description: This routine is called by SendPackets.  It will setup all of the
//              necessary structures (TCBs, TBDs, etc.) to send a packet.  If
//              the device has the necessary resources, the packet will be
//              sent out on the active chain.  If a deserialized miniport driver
//              cannot immediately transmit incoming send packets, it cannot return
//              the packets to NDIS for requeuing. Instead, the miniport driver must
//              queue them internally until sufficient resources are available to
//              transmit the packets. A deserialized miniport driver's MiniportSend
//              or MiniportSendPackets function cannot return NDIS_STATUS_RESOURCES.
//
//              The intention is to have this routine run only on a 82557
//              or a 82558 in compatibility mode (indicated by Enhanced = FALSE
//              in the registry)
// Arguments:
//      Packet - The KNdisPacket that is to be transmitted.
// Returns:
//      NDIS_STATUS_SUCCESS - We copied the entire packet into a host buffer,
//                            (either it was a short frame, or we coalesced
//                            the packet into a host buffer), so we can
//                            immediately return the buffer back to the upper
//                            layers.
//      NDIS_STATUS_PENDING - If we were able to acquire the necessary TBD's
//                            or Coalesce buffer for the packet.  This means
//                            that the device will send this packet soon.
//                            Eventually we'll return the packet back to the
//                            protocol stack by using the "SendComplete" call.
//      NDIS_STATUS_RESOURCES - We didn't have the resouces (TCBs or TBDs) to
//                              accept this packet. The NDIS packet should
//                              queue this packet, and give it back to us at a
//                              later time.
//-----------------------------------------------------------------------------
NDIS_STATUS E100bexAdapter::MpSendPacket(KNdisPacket& Packet, BOOLEAN bFromQueue)
{
	NDIS_STATUS					Status = NDIS_STATUS_PENDING;
	UINT                        BytesCopied;
	PCOALESCE                   pMpTxBuf;
	PD100SwTcb					pMpTcb;
	// Mimiced frag list if the packet is too small or too fragmented.
	MP_FRAG_LIST				FragList;
	// Pointer to either the scatter gather or the local mimiced frag list
	PMP_FRAG_LIST				pFragList;

	// Attempt to acquire a Software TCB for the packet
	pMpTcb = m_TxCBList.RemoveHead();
	if (!pMpTcb)
	{
		// tcb is not available, put into queue
		TRACE2(("NoTcb, m_SendWaitQueue++\n"))
		m_SendWaitQueue.InsertTail(Packet);
		// DDK says deserialized miniport cannot return NDIS_STATUS_RESOURCES so we queue it ourself.
		return Status;
	}
	ASSERT(pMpTcb->TcbNum < m_NumTcb);
	ASSERT(pMpTcb->HwTbd != NULL);

	// Assign the packet
	pMpTcb->Packet = Packet;

	// Get a virtual buffer count and packet length.
	Packet.Query(
		&pMpTcb->PhysBufCount,
		&pMpTcb->BufferCount,
		&pMpTcb->FirstBuffer,
		&pMpTcb->PacketLength);

	ASSERT(pMpTcb->FirstBuffer);
	ASSERT(pMpTcb->BufferCount);
	ASSERT(pMpTcb->PhysBufCount);
	ASSERT(pMpTcb->PacketLength);

	//
	// Check to see if we need to coalesce
	//
	if (pMpTcb->PacketLength < NIC_MIN_PACKET_SIZE ||
		pMpTcb->PhysBufCount > NIC_MAX_PHYS_BUF_COUNT)
	{
		//
		// A local MP_TXBUF available (for local data copying)?
		//
		if(m_SendBufList.IsEmpty())
		{
			if (bFromQueue)
			{
				m_SendWaitQueue.InsertHead(Packet);
			}
			else
			{
				m_SendWaitQueue.InsertTail(Packet);
			}
			return Status;
		}

		pMpTxBuf = m_SendBufList.RemoveHead();
		ASSERT(pMpTxBuf);

		// Copy the buffers in this packet, enough to give the first buffer as they are linked
		BytesCopied = CopyPacketToBuffer(pMpTcb->FirstBuffer, pMpTxBuf);

		// CopyPacketToBuffer may return 0 if system resources are low or exhausted
		if (BytesCopied == 0)
		{
			m_SendBufList.InsertTail(pMpTxBuf);

			TRACE("Calling NdisMSendComplete with NDIS_STATUS_RESOURCES, Pkt=%p\n", Packet);

			m_SendLock.Unlock();
			SendComplete(Packet, NDIS_STATUS_RESOURCES);
			m_SendLock.Lock();
			return NDIS_STATUS_RESOURCES;
		}

		pMpTcb->MpTxBuf = pMpTxBuf;

		//
		// Set up the frag list, only one fragment after it's coalesced
		//
		pFragList = &FragList;
		pFragList->NumberOfElements = 1;
		pFragList->Elements[0].Address = *pMpTxBuf->pAllocMem->PhysicalAddress();
		pFragList->Elements[0].Length = (BytesCopied >= NIC_MIN_PACKET_SIZE) ?
										BytesCopied : NIC_MIN_PACKET_SIZE;

		// Even the driver uses its local buffer, it has to wait the send complete interrupt to
		// complete the packet. Otherwise, the driver may run into the following situation:
		// before send complete interrupt happens, its halt handler is called and the halt handler
		// deregisters the interrupt, so no send complete interrupt can happen, and the send
		// complete interrupt handle routine will never be called to free some resources used
		// by this send.
	}
	else
	{
		ASSERT(MP_TEST_FLAG(fMP_ADAPTER_SCATTER_GATHER));
		//
		// In scatter/gather case, use the frag list pointer saved
		// in the packet info field
		//
		pFragList = (PMP_FRAG_LIST) NDIS_PER_PACKET_INFO_FROM_PACKET((PNDIS_PACKET)Packet,
														   ScatterGatherListPacketInfo);

	}


	//
	// Call the NIC specific send handler, it only needs to deal with the frag list
	//
	Status = NICSendPacket(pMpTcb, pFragList);

	ASSERT(m_ActiveChainList.Count() <= m_NumTcb);

	return Status;
}


NDIS_STATUS E100bexAdapter::NICSendPacket(
	IN  PD100SwTcb		pMpTcb,
	IN  PMP_FRAG_LIST	pFragList
	)
{
	TRACE3(("E100bexAdapter::NICSendPacket++\n"));

	NDIS_STATUS  Status;
	ULONG        index;
	UCHAR        TbdCount = 0;

	PTXCB_STRUC  pHwTcb = pMpTcb->HwTcb;
	PTBD_STRUC   pHwTbd = pMpTcb->HwTbd;

	for (index = 0; index < pFragList->NumberOfElements; index++)
	{
		if (pFragList->Elements[index].Length)
		{
			pHwTbd->TbdBufferAddress = NdisGetPhysicalAddressLow(pFragList->Elements[index].Address);
			pHwTbd->TbdCount = pFragList->Elements[index].Length;

			pHwTbd++;
			TbdCount++;
		}
	}

	pHwTcb->TxCbHeader.CbStatus = 0;
	pHwTcb->TxCbHeader.CbCommand = CB_S_BIT | CB_TRANSMIT | CB_TX_SF_BIT;

	pHwTcb->TxCbTbdPointer = pMpTcb->HwTbdPhys;
	pHwTcb->TxCbTbdNumber = TbdCount;
	pHwTcb->TxCbCount = 0;
	pHwTcb->TxCbThreshold = (UCHAR) m_AiThreshold;

	Status = NICStartSend(pMpTcb);

	TRACE3(("E100bexAdapter::NICSendPacket--\n"));
	return Status;
}

NDIS_STATUS E100bexAdapter::NICStartSend(
	IN  PD100SwTcb      pMpTcb
	)
{
	TRACE3(("E100bexAdapter::NICStartSend++\n"));

	NDIS_STATUS     Status;

	m_ActiveChainList.InsertTail(pMpTcb);
	//
	// If the transmit unit is idle (very first transmit) then we must
	// setup the general pointer and issue a full CU-start
	//
	if (m_TransmitIdle)
	{
		TRACE("CU is idle -- First TCB added to Active List\n");

		Status = m_pCard->StartTransmitUnit(pMpTcb->HwTcbPhys);

		m_TransmitIdle = FALSE;
		m_ResumeWait = TRUE;
	}
	else
	{
		//
		// If the command unit has already been started, then append this
		// TCB onto the end of the transmit chain, and issue a CU-Resume.
		//
		TRACE3(("added TCB to Active chain\n"))

		//
		// Clear the suspend bit on the previous packet.
		//
		pMpTcb->PrevHwTcb->TxCbHeader.CbCommand &= ~CB_S_BIT;

		//
		// Issue a CU-Resume command to the device.  We only need to do a
		// WaitScb if the last command was NOT a RESUME.
		//
		Status = m_pCard->ResumeCommandUnit();
	}

	exit:

	TRACE3(("E100bexAdapter::NICStartSend--\n"));
	return Status;
}


//-----------------------------------------------------------------------------
// Procedure:   E100bexAdapter::CopyPacketToBuffer
//
// Description: Copy the packet data to a local buffer
//      Either the packet is too small or it has too many fragments
//      Assumption: Send spinlock has been acquired
//
// Arguments:
//      FirstBuffer Pointer to the first NDIS_BUFFER
//      pMpTxBuf    Pointer to the local buffer (MP_TXBUF)
//
// Returns:
//      Bytes copied
//-----------------------------------------------------------------------------
ULONG E100bexAdapter::CopyPacketToBuffer(
	KNdisBuffer CurrentBuffer,
	PCOALESCE   pMpTxBuf)
{
	UINT	CurrentLength;
	PVOID	pSrc;
	PUCHAR	pDest;
	UINT	BytesCopied = 0;

	pDest = (PUCHAR)pMpTxBuf->pAllocMem->VirtualAddress();

	while (((PNDIS_BUFFER)CurrentBuffer) && (BytesCopied < pMpTxBuf->BufferSize))
	{
#if KNDIS_DDK_VERSION < 0x0501
		NdisQueryBuffer(
			(PNDIS_BUFFER)CurrentBuffer,
			&pSrc,
			&CurrentLength);
#else
		NdisQueryBufferSafe(
			(PNDIS_BUFFER)CurrentBuffer,
			&pSrc,
			&CurrentLength,
			NormalPagePriority);
#endif
		if (pSrc == NULL)
		{
			return 0;
		}


		if (pMpTxBuf->BufferSize - BytesCopied < CurrentLength)
		{
			CurrentLength = pMpTxBuf->BufferSize - BytesCopied;
		}

		if (CurrentLength)
		{
			//
			// Copy the data.
			//
			NdisMoveMemory(pDest, pSrc, CurrentLength);
			BytesCopied += CurrentLength;
			pDest += CurrentLength;
		}

		CurrentBuffer = CurrentBuffer.GetNext();
	}
	//
	// Zero out the padding bytes
	//
	if (BytesCopied < NIC_MIN_PACKET_SIZE)
	{
		NdisZeroMemory(pDest, NIC_MIN_PACKET_SIZE - BytesCopied);
	}

	pMpTxBuf->NdisBuffer.AdjustLength(BytesCopied);
	pMpTxBuf->NdisBuffer.Flush(TRUE);

	ASSERT(BytesCopied <= pMpTxBuf->BufferSize);

	return BytesCopied;
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::ReturnPacket
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
VOID E100bexAdapter::ReturnPacket(IN PNDIS_PACKET Packet)
{
	m_RcvLock.LockDpr();
	TRACE3(("E100bexAdapter::ReturnPacket() Entered, Packet=%p\n", Packet));

    //if (m_RfdShrinkCount < NIC_RFD_SHRINK_THRESHOLD)
	if (m_RfdShrinkCount < 5)	// 2*5 = 10sec
    {
		m_pRxArea->Reclaim(Packet, FALSE);
    }
    else
    {
        ASSERT(m_CurrNumRfd > m_NumRfd);
		UINT OldNumRfd = m_CurrNumRfd;

		m_pRxArea->Reclaim(Packet, TRUE);
		// If this packet is not part of async allocated, it will not get deallocated automatically.
		if(OldNumRfd != m_CurrNumRfd)
	        m_RfdShrinkCount = 0;
    }

	// Reclaim the packet back into our receive area
	// Note: Can't hold spin lock while calling this method
	//m_pRxArea->Reclaim(Packet);
	UINT nReadyRecv = m_pRxArea->GetNumberFreeDescriptors();
	ASSERT(nReadyRecv <= m_CurrNumRfd);
	m_RcvLock.UnlockDpr();
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::CheckForHang
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
BOOLEAN E100bexAdapter::CheckForHang(void)
{
	TRACE3(("E100bexAdapter::CheckForHang\n"));

	// Is send stuck?
	m_SendLock.LockDpr();
	if (!m_ActiveChainList.IsEmpty())
	{
		PD100SwTcb pMpTcb = m_ActiveChainList.Head();
		pMpTcb->Count++;
		if (pMpTcb->Count > NIC_SEND_HANG_THRESHOLD)
		{
			m_SendLock.UnlockDpr();
			TRACE(("!! Send stuck - reset\n"));
			ASSERT(0);
			return(TRUE);
		}
	}
	m_SendLock.UnlockDpr();

	// Update the RFD shrink count, see ReturnPacket()
	m_RcvLock.LockDpr();
	if (m_CurrNumRfd > m_NumRfd)
	{
		m_RfdShrinkCount++;
	}
	m_RcvLock.UnlockDpr();

	m_Lock.LockDpr();
	// this is the implementation of the NDIS 4 feature for detecting
	// link status change. It effectively checks every two seconds
	// for link.
	if (m_LinkIsActive != m_pCard->GetConnectionStatus())
	{
		// if status has changed
		TRACE("E100bexAdapter::CheckForHang: media state changed to %s\n",
					((m_LinkIsActive == NdisMediaStateConnected)? "Disconnected": "Connected"));

		switch ( m_LinkIsActive )
		{
		case NdisMediaStateConnected:
			// changing from connected

			m_LinkIsActive = NdisMediaStateDisconnected;
			m_Lock.Unlock();

			IndicateStatus(NDIS_STATUS_MEDIA_DISCONNECT,NULL,0);
			// NOTE: have to indicate status complete every time you indicate status
			IndicateStatusComplete();
			break;

		case NdisMediaStateDisconnected:
			// changing from disconnected

			m_LinkIsActive = NdisMediaStateConnected;
			m_Lock.Unlock();

			IndicateStatus(NDIS_STATUS_MEDIA_CONNECT,NULL,0);
			// NOTE: have to indicate status complete every time you indicate status
			IndicateStatusComplete();
			break;
		}
	}
	else
	{
		m_Lock.Unlock();
	}

	// return FALSE to indicate that the adapter is not hung, and that
	// Reset does NOT need to be called by the wrapper
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
VOID E100bexAdapter::PnPEventNotify(
			IN NDIS_DEVICE_PNP_EVENT  PnPEvent,
			IN PVOID  InformationBuffer,
			IN ULONG  InformationBufferLength
			)
{
	UNREFERENCED_PARAMETER(InformationBufferLength);

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
		TRACE("E100bexAdapter::PnPEventNotify: Surprised Removal!\n");
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
		TRACE("E100bexAdapter::PnPEventNotify: Running on %s\n", (bOnBattery)?"Batterry":"AC");
		}
		break;
	default:
		//
		// Everything else is non-documented in XP DDK
		//
		TRACE("E100bexAdapter::PnPEventNotify: Event %u\n", PnPEvent);
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
VOID E100bexAdapter::CancelSendPackets(
			IN PVOID	CancelId
			)
{
	TRACE("E100bexAdapter::CancelSendPackets ID=%p\n", CancelId);

	// m_TxQueue contains the list of packets waiting for resources (IRPs, buffers)
	// to become available. Search for packets with the specified ID.

	KNdisPacketList CancelList;

	m_SendLock.Lock();							// protect the loop
	PNDIS_PACKET p = m_SendWaitQueue.HeadNoLock();
	while (p)
	{
			PNDIS_PACKET thepacket = p;
			p = m_SendWaitQueue.NextNoLock(p);
			if (NDIS_GET_PACKET_CANCEL_ID(thepacket)==CancelId)
			{
				m_SendWaitQueue.RemoveNoLock(thepacket);
				CancelList.InsertTailNoLock(thepacket);
			}
	}
	m_SendLock.Unlock();							// protect the loop

	// CancelList contains the packets (if any) with given ID. Abort them.

	while (!CancelList.IsEmpty())
	{
		KNdisPacket p = CancelList.RemoveHead();
		SendComplete(p, NDIS_STATUS_REQUEST_ABORTED);
	}

};
#endif // NDIS51_MINIPORT


////////////////////////////////////////////////////////////////////
// E100bexAdapter::DisableInterrupt
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
VOID E100bexAdapter::DisableInterrupt(void)
{
	// Disable interrupts on our PCI board by setting the mask bit
	m_pCard->DisableInterrupt();
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::EnableInterrupt
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
VOID E100bexAdapter::EnableInterrupt(void)
{
	// Enable interrupts on our PCI board by clearing the mask bit
	m_pCard->EnableInterrupt();
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::Isr
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
VOID E100bexAdapter::Isr(
	OUT PBOOLEAN InterruptRecognized,
	OUT PBOOLEAN QueueMiniportHandleInterrupt
	)
{
	// Make sure this is our NIC that generated that interrupt
	// Clear the interrupt condition register before returning.

	// Check if this card is causing the interrupt
	if ( m_pCard->IsInterrupting() )
	{
		*InterruptRecognized = TRUE;
		*QueueMiniportHandleInterrupt = TRUE;

		// Disable interrupts on the card
		m_pCard->DisableInterrupt();
	}
	else
	{
		*InterruptRecognized = FALSE;
		*QueueMiniportHandleInterrupt = FALSE;
	}
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::HandleInterrupt
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
VOID E100bexAdapter::HandleInterrupt(void)
{
	TRACE3(("E100bexAdapter::HandleInterrupt() Entered\n"))

	// TODO:	Process NIC's events: get frames, statuses, etc.
	//			This is where the "meat" of the driver is expected to be.

	// Acknowledge the interrupt(s)
	m_pCard->AckPendingInterrupts();

	// Count can be changed to other values
	// to optimize for hardware/protocol timing differences
	USHORT Count = 1;

	// Make no more than 'Count' loops through the interrupt processing
	// code.  We don't want to loop forever in handle interrupt if we have
	// interrupt processing to do, because this might lead to starvation
	// problems with certain protocols if we are receiving data constantly.
	while (Count--)
	{
		// Go handle receive events
		m_RcvLock.LockDpr();
		ProcessRXInterrupt();
		m_RcvLock.UnlockDpr();

		// Cleanup transmits
		m_SendLock.LockDpr();
		ProcessTXInterrupt();
		m_SendLock.UnlockDpr();

		// Start the receive unit if it had stopped
		m_RcvLock.LockDpr();
		StartReceiveUnit();
		m_RcvLock.UnlockDpr();
	}
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::AsyncResetTimerCallback
//
// Parameters:
//	none
// IRQL:
//
// Return Mode:
//
// NOTE
//
VOID E100bexAdapter::AsyncResetTimerCallback(void)
{
	TRACE("E100bexAdapter::AsyncResetTimerCallback\n");

	ResetComplete(NDIS_STATUS_SUCCESS);

	TRACE("E100bexAdapter::AsyncResetTimerCallback: Calling StartReceiveUnit\n");
	// Start the receive unit and indicate any pending receives that were
	// left in the queue.

	if ( StartReceiveUnit() )
	{
		TRACE2(("Indicating Receive complete\n"))

		NdisMEthIndicateReceiveComplete(this);
	}

	m_pCard->EnableInterrupt();
}


////////////////////////////////////////////////////////////////////
// E100bexAdapter::NICInitSend
//
// Parameters:
//	none
// IRQL:
//
// Return Mode:
//
// NOTE
//
VOID E100bexAdapter::NICInitSend(void)
{
	TRACE("E100bexAdapter::NICInitSend++\n");

	// Initialize the command unit flags
	m_TransmitIdle = TRUE;
	m_ResumeWait = TRUE;

	// Reset the TCB lists
	m_TxCBList.Reset();
	m_ActiveChainList.Reset();

	// Reset the list of queued NDIS_PACKETs
	m_SendWaitQueue.Reset();

	// Setup the initial pointers to the SW and HW TCB data space
	PD100SwTcb pMpTcb = (PD100SwTcb) m_pMpTcbMem;
	PTXCB_STRUC pHwTcb = (PTXCB_STRUC) m_pHwSendMem->VirtualAddress();
	ULONG HwTcbPhys = NdisGetPhysicalAddressLow(*m_pHwSendMem->PhysicalAddress());

	// Setup the initial pointers to the TBD data space.
	// TBDs are located immediately following the TCBs
	PTBD_STRUC pHwTbd = (PTBD_STRUC) ((PUCHAR)m_pHwSendMem->VirtualAddress() +
						(sizeof(TXCB_STRUC) * m_NumTcb));
	ULONG HwTbdPhys = HwTcbPhys +
						(sizeof(TXCB_STRUC) * m_NumTcb);

	// Go through and set up each TCB
	for (UINT TcbCount = 0; TcbCount < m_NumTcb; TcbCount++)
	{
#if DBG
		pMpTcb->TcbNum = TcbCount;
#endif

		TRACE (" TcbCount   =%d\n", TcbCount);
		TRACE ("   pMpTcb   =%lx\n", pMpTcb);
		TRACE ("   pHwTcb   =%lx\n", pHwTcb);
		TRACE ("   HwTcbPhys=%lx\n", HwTcbPhys);
		TRACE ("   pHwTbd   =%lx\n", pHwTbd);
		TRACE ("   HwTbdPhys=%lx\n", HwTbdPhys);

		// point the cached TCB to the logical address of the uncached one
		pMpTcb->HwTcb = pHwTcb;
		// save the uncached TCB physical address in the cached TCB
		pMpTcb->HwTcbPhys = (ULONG)HwTcbPhys;

		// store virtual and physical pointers to the TBD array
		pMpTcb->HwTbd = pHwTbd;
		pMpTcb->HwTbdPhys = HwTbdPhys;

		if (TcbCount)
			pMpTcb->PrevHwTcb = pHwTcb - 1;
		else
			pMpTcb->PrevHwTcb = (PTXCB_STRUC)((PUCHAR)m_pHwSendMem->VirtualAddress() +
									((m_NumTcb - 1) * sizeof(TXCB_STRUC)));
		TRACE ("   PrevHwTcb=%lx\n", pMpTcb->PrevHwTcb);

		// initialize the uncached TCB contents -- status is zeroed
		pHwTcb->TxCbHeader.CbStatus = 0;        // clear the status
		pHwTcb->TxCbHeader.CbCommand = CB_S_BIT | CB_TRANSMIT | CB_TX_SF_BIT;

		// Set the link pointer in HW TCB to the next TCB in the chain.
		// If this is the last TCB in the chain, then set it to the first TCB.
		if (TcbCount < m_NumTcb - 1)
		{
			pHwTcb->TxCbHeader.CbLinkPointer = HwTcbPhys + sizeof(TXCB_STRUC);
		}
		else
		{
			pHwTcb->TxCbHeader.CbLinkPointer =
				NdisGetPhysicalAddressLow(*m_pHwSendMem->PhysicalAddress());
		}

		// Set the D100's early transmit threshold
		pHwTcb->TxCbThreshold = (UCHAR) m_AiThreshold;
		// Pointer this TCB's TBD array
		pHwTcb->TxCbTbdPointer = HwTbdPhys;

		// Add this TCB to the free list
		m_TxCBList.InsertTail(pMpTcb);
		ASSERT(pMpTcb->HwTbd != NULL);

		pMpTcb++;
		pHwTcb++;
		HwTcbPhys += sizeof(TXCB_STRUC);
		pHwTbd = (PTBD_STRUC)((PUCHAR)pHwTbd + sizeof(TBD_STRUC) * NIC_MAX_PHYS_BUF_COUNT);
		HwTbdPhys += sizeof(TBD_STRUC) * NIC_MAX_PHYS_BUF_COUNT;
	}

	TRACE("E100bexAdapter::NICInitSend--\n");
}


NDIS_STATUS E100bexAdapter::NICInitRecv(void)
{
	TRACE("E100bexAdapter::NICInitRecv++\n");
	// This sets up the Receive Frame Area including:
	// Allocating internal software Receive Frame Descriptors
	// Allocating packet and buffer pools
	// Initializing and chaining packets, software RFD, buffers
	// Maintains list of software RFD descriptors
	// m_pShMem is a previously allocated shared memory buffer
	//		(KNdisSharedMemory) which represents the buffer of
	//		initial Hardware Receive Frame Descriptors.  The CTOR
	//		calculates the number of descriptors (software and
	//		hardware from the sizeof<RFD_STRUC> and the length
	//		of the shared memory buffer.

	// This driver's requirement is to further initialize the hardware RFD's
	// see global template specialization DescriptorInitialize(..) which
	// is called for each descriptor

	if( NULL == m_pRxShMem )
	{
		ASSERT( m_pRxShMem );
		KNDIS_RETURN_ERROR ( NDIS_STATUS_FAILURE );
	}

	// Instance an E100bexReceiveArea object
	m_pRxArea = new E100bexReceiveArea(
		*this,									// Adapter object
		*m_pRxShMem,							// KNdisSharedMemory object for the intial h/w RFD's
		m_NumRfd,								// Initial number of RFD's
		m_MaxNumRfd,							// Maximum number of RFD's
		NIC_MIN_RFDS,							// Minimum number of free RFD's
		m_MaxNumRfd*sizeof(HW_RFD),				// High water mark
		NIC_MIN_RFDS*sizeof(HW_RFD)				// Low water mark (low resource condition)
		);

	if( NULL == m_pRxArea || !m_pRxArea->IsValid())
		 KNDIS_RETURN_ERROR ( NDIS_STATUS_RESOURCES );

	// Initialize the h/w and s/w descriptors for the receive area.
	// This results in E100bexAdapter::DescriptorInitialize being called
	// for each descriptor
	m_pRxArea->AddDescriptors(*m_pRxShMem, m_NumRfd);

	DescriptorInitializeComplete();

	TRACE("E100bexAdapter::NICInitRecv--\n");
	return NDIS_STATUS_SUCCESS;
}


//-----------------------------------------------------------------------------
// Procedure:   E100bexAdapter::ProcessRXInterrupt
//
// Description: This routine will indicate any received packets to the NDIS
//              wrapper.  This routine is architected so that we will indicate
//              all of the completed receive frames that we have in our buffer,
//              before we exit the routine.  If we receive any errored frames,
//              those frames are dropped by the driver, and not indicated to
//              the ndis wrapper.
//
// Arguments:
//
// Returns:
//      TRUE if a receive was indicated
//      FALSE if a receive was not indicated
//-----------------------------------------------------------------------------
BOOLEAN E100bexAdapter::ProcessRXInterrupt(void)
{
	TRACE3(("ProcessRxInterrupt\n"))

	PHW_RFD			pHwRfd;	// pointer to HW RFD
	PNDIS_PACKET    PacketArray[NIC_DEF_RFDS];	// array of packets that will be indicated to NDIS
	PNDIS_PACKET    PacketFreeArray[NIC_DEF_RFDS];
	UINT            PacketArrayCount;	// Count of packets in the array that will be indicated to NDIS
	UINT            PacketFreeCount;	// Count of packets recieved when below low resource condition (MIN_NUM_RFD)
												// where packet status is set to NDIS_STATUS_RESOURCES
	UINT            LoopIndex = 0;
	UINT            LoopCount = NIC_MAX_RFDS / NIC_DEF_RFDS + 1;    // avoid staying here too long

	BOOLEAN         bContinue = TRUE;
	USHORT			PacketStatus;

	while (LoopIndex++ < LoopCount && bContinue)
	{
		PacketArrayCount = 0;
		PacketFreeCount = 0;

		//
		// Process up to the array size RFD's
		//
		while (PacketArrayCount < NIC_DEF_RFDS)
		{
			// Check the Free List of the Receive Frame Area - should not be empty
			UINT nReadyRecv = m_pRxArea->GetNumberFreeDescriptors();
			if ( 0 == nReadyRecv )
			{
				// This should never happen because the number of receive
				// buffers is limited that the protocol can take from us
				TRACE("Receive buffers went to 0!\n");
				bContinue = FALSE; // we don't need to looptop any more
				break;
			}

			// Get a RFD from the FreeList (head of the list, not removed)
			KNdisPacket Pkt(m_pRxArea->GetPacket());
			ASSERT( Pkt.IsValid() );

			// Get the current buffer (head of the list)
			KNdisBuffer Buf = Pkt.QueryFirstBuffer();
			ASSERT( Buf.IsValid() );

			// Get the hardware RFD from the packet

			pHwRfd = reinterpret_cast<PHW_RFD>(m_pRxArea->GetDescriptor(Pkt));

			// HW specific - If the RFD does not have its complete bit set, then bail out.
			if (!( (PacketStatus = pHwRfd->RfdCbHeader.CbStatus) & RFD_STATUS_COMPLETE))
			{
				TRACE3(("- No more complete packet, RFD status = %d\n", PacketStatus))
				// Normal exit routine
				bContinue = FALSE; // don't need to looptop any more
				break;
			}

			// HW specific - check if actual count field has been updated
			if (!( (pHwRfd->RfdActualCount & (RFD_EOF_BIT | RFD_F_BIT)) == (RFD_EOF_BIT | RFD_F_BIT)))
			{
				bContinue = FALSE;
				break;
			}

			// Remove this RFD from the FreeList (KNdisReceiveArea::Complete)
			// If RFD is running low, it will use _RxAreaAsyncMgr::Debit() to get more RFD
			m_pRxArea->Complete(pHwRfd);

			//
			// A good packet? drop it if not.
			//
			if (!(PacketStatus & RFD_STATUS_OK))
			{
				TRACE2(("Receive error!!!\n"))
				// looks like there might have been a bug here,
				// we're not doing anything useful if we got a receive error
				// so clean up the bad packet, and keep going
				m_pRxArea->ReclaimFailedPacket(Pkt);
				continue;
			}
			//
			// Do not receive any packets until a filter has been set
			//
			if (!m_pCard->GetPacketFilter())
			{
				m_pRxArea->ReclaimFailedPacket(Pkt);
				continue;
			}

			//
			// Do not receive any packets until we are at D0
			//
			if (m_Power != NdisDeviceStateD0)
			{
				m_pRxArea->ReclaimFailedPacket(Pkt);
				continue;
			}

			// Get the packet length
			UINT PacketSize = pHwRfd->RfdActualCount & RFD_ACT_COUNT_MASK;
			ASSERT( PacketSize <= NIC_MAX_PACKET_SIZE );
			TRACE3(("Received packet length %d\n", PacketSize))

			// Adjust the buffer length for this sw rfd
			Buf.AdjustLength( PacketSize );
			Buf.Flush(FALSE);
			ASSERT( PacketSize == (UINT)(pHwRfd->RfdActualCount & 0x3fff) );

			//
			// set the status on the packet, either resources or success
			//
			if (nReadyRecv >= NIC_MIN_RFDS)
			{
				// NDIS_STATUS_SUCCESS
				Pkt.STATUS(NDIS_STATUS_SUCCESS);
			}
			else
			{
				// NDIS_STATUS_RESOURCES
				Pkt.STATUS(NDIS_STATUS_RESOURCES);

				PacketFreeArray[PacketFreeCount] = Pkt;
				PacketFreeCount++;

				// Reset the RFD shrink count - don't attempt to shrink RFD
				m_RfdShrinkCount = 0;
			}

			PacketArray[PacketArrayCount] = Pkt;
			PacketArrayCount++;
		}

		//
		// if we didn't process any receives, just return from here
		//
		if (PacketArrayCount == 0)
		{
			break;
		}

		m_RcvLock.UnlockDpr();
		IndicateReceive(PacketArray, PacketArrayCount);
		m_RcvLock.Lock();

		//
		// NDIS will call our ReturnPackets handler for each packet
		// that was *not* marked NDIS_STATUS_RESOURCES. For those
		// that were marked with the resources status, the miniport
		// should assume that they are immediately returned.
		// Go through the packets that were marked NDIS_STATUS_RESOURCES
		// and reclaim them.
		//
		for (UINT Index = 0; Index < PacketFreeCount; Index++)
		{
			//
			// Decrement the number of outstanding Recvs
			//
			m_pRxArea->ReclaimFailedPacket(PacketFreeArray[Index]);
		}
	}

	// check to see if we came out of this routine with the correct amount of
	// receive buffers left over
	//ASSERT(m_pRxArea->GetNumberFreeDescriptors() >= NIC_MIN_RFDS);

	return TRUE;
}


//-----------------------------------------------------------------------------
// Procedure:   E100bexAdapter::ProcessTXInterrupt
//
// Description: This routine is called by HandleInterrupt to process transmit
//              interrupts for D100 adapters.  Basically, this routine will
//              remove any completed packets from the active transmit chain,
//              append them to the completed list.  TransmitCleanup will be
//              called to free up the TCBs, map registers, etc, that the
//              packets on the completed list consumed.  At the end of this
//              routine, the MAC version of this driver will try to send any
//              packets that it queued because of an earlier lack of resources.
//
// Arguments:
//
// Return:
//      TRUE - If we indicated any loopback packets during this function call
//      FALSE - If we didn't indicate any loopaback packets
//-----------------------------------------------------------------------------
NDIS_STATUS E100bexAdapter::ProcessTXInterrupt(void)
{
	TRACE3(("ProcessTXInterrupt\n"))

	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;;

	// If we don't have an active transmit chain, AND we haven't pended any transmits,
	// then we don't have anything to clean-up.
	if (m_ActiveChainList.IsEmpty() && m_SendWaitQueue.IsEmpty())
	{
		return FALSE;
	}

	// Look at the TCB at the head of the queue.  If it has been completed
	// then pop it off and place it at the tail of the completed list.
	// Repeat this process until all the completed TCBs have been moved to the
	// completed list
	PD100SwTcb pMpTcb;
	while (pMpTcb = m_ActiveChainList.Head())
	{
		// Is this TCB completed?
		if (pMpTcb->HwTcb->TxCbHeader.CbStatus & CB_STATUS_COMPLETE)
		{
			// If this wasn't a multicast command, then we need to check to see
			// if we need to issue send complete
			if ((pMpTcb->HwTcb->TxCbHeader.CbCommand & CB_CMD_MASK) != CB_MULTICAST)
			{
				TRACE3(("Found a completed TCB\n"))

				// Remove the TCB from the active queue.
				pMpTcb = m_ActiveChainList.RemoveHead();
				FreeSendPacket(pMpTcb);
			}
		}
		else
			break;
	}

	// If we queued any transmits because we didn't have any TCBs earlier,
	// dequeue and send those packets now, as long as we have free TCBs.
	while (!m_SendWaitQueue.IsEmpty() && m_ActiveChainList.Count() < m_NumTcb)
	{
		// If any packets are in the queue, dequeue it, send it, and
		// acknowledge success.
		KNdisPacket QueuePacket = m_SendWaitQueue.RemoveHead();

		// Attempt to put it onto the hardware
		Status = MpSendPacket(QueuePacket, TRUE);

		if (Status != NDIS_STATUS_SUCCESS)
		{
			break;
		}
	}

	return Status;
}


//-----------------------------------------------------------------------------
// Procedure:   E100bexAdapter::FreeSendPacket
//
// Description: This routine will clean up after a transmitted frame.  It will
//              update the transmit statistic counters, free up TCBs and map
//              regs, and issue a send complete which will unlock the pages.
//
// Arguments:
//
// Returns:
//      TRUE - If we indicated any loopback packets during this function call
//      FALSE - If we didn't indicate any loopaback packets
//-----------------------------------------------------------------------------
//MP_FREE_SEND_PACKET
BOOLEAN E100bexAdapter::FreeSendPacket(IN  PD100SwTcb pMpTcb)
{
	BOOLEAN             Status = FALSE;

	ASSERT(pMpTcb);

	// PACKET is nothin when the element being processed is a MC Command
	KNdisPacket Packet(pMpTcb->Packet);
	pMpTcb->Packet = NULL;
	pMpTcb->Count = 0;	// CheckForHang

	TRACE3(("Processing pMpTcb %08x, Packet %08x\n", pMpTcb, Packet))

	// Free a coalesce buffer (if there were any)
	if (pMpTcb->MpTxBuf)
	{
		// Return the coalesce buffer back to the list
		m_SendBufList.InsertTail(pMpTcb->MpTxBuf);

		// Clear the coalesce buffer pointer
		pMpTcb->MpTxBuf = NULL;
		pMpTcb->CoalesceBufferLen = 0;
	}


	// Free the TCB for the given frame
	TRACE3(("Releasing pMpTcb %08x\n", pMpTcb))

	m_TxCBList.InsertTail(pMpTcb);
	ASSERT(pMpTcb->HwTbd != NULL);

	if (Packet.IsValid())
	{
#if DBG
		m_txind++;
#endif
		m_SendLock.Unlock();
		// Do a send Complete for this frame
		SendComplete(Packet, NDIS_STATUS_SUCCESS);
		m_SendLock.Lock();
	}
	return Status;
}


VOID E100bexAdapter::AllocateComplete(
	PVOID					VirtualAddress,
	PNDIS_PHYSICAL_ADDRESS	PhysicalAddress,
	ULONG					Length,
	PVOID					Context
	)
{
	TRACE3(("E100bexAdapter::AllocateComplete++\n"));

	KNdisSharedMemory* pShMem = KNdisSharedMemory::AllocateComplete(
		VirtualAddress,
		PhysicalAddress,
		Length,
		Context
		);

	// Sanity check parameters NDIS gave us...
	if( pShMem && pShMem->IsValid() )
	{
		if( FALSE == m_pRxArea->OnAllocated(*pShMem) )
		{
			// this will call NdisFreeSharedMemory()
			// most likely due to insufficient resource in getting a SwDesc from lookaside list
			delete pShMem;
		}
		else
		{
			DescriptorInitializeStart();

			ULONG NumOfDescriptors = pShMem->Length()/sizeof(HW_RFD);
			if(m_pRxArea->AddDescriptors(*pShMem, NumOfDescriptors) == NDIS_STATUS_SUCCESS)
				DescriptorInitializeComplete();
			else
			{
				// most likely due to insufficient resource in getting a packet
				delete pShMem;
			}
		}
	}
	else
	{
		ASSERT( pShMem );
		ASSERT( VirtualAddress );
		ASSERT( PhysicalAddress );
	}
	TRACE3(("E100bexAdapter::AllocateComplete--\n"));
}


VOID E100bexAdapter::DescriptorInitializeStart()
{
	TRACE3(("E100bexAdapter::DescriptorInitializeStart\n"));
	// called once for each group of descriptors that are setup
	// called from KNdisSharedReceiveArea ctor on initial descriptor setup
	// and when groups of descriptors are added dynamically
	// called before DescriptorInitialize()

	// store the current tail descriptor before DescriptorInitialize() adds
	// new descriptors to the list.  Note: this will be NULL for the first
	// group of descriptors (since the list is empty).
	if( m_pRxArea )
	{
		m_pPrevHwDesc = (PHW_RFD) m_pRxArea->GetTailDescriptor();
	}
}


VOID E100bexAdapter::DescriptorInitializeComplete()
{
	TRACE3(("E100bexAdapter::DescriptorInitializeComplete\n"));
	// called once for each group of descriptors that are setup
	// after a call to KNdisSharedReceiveArea::AddDescriptors() during initial descriptor setup
	// and when groups of descriptors are added dynamically

	// initialize the physical RFD contents for the descriptor
	// at the tail of the list.  Note: This was originally initialized by
	// DescriptorInitialize() but needs to be marked as the end of the RFD's

	PHW_RFD pHwRfd = reinterpret_cast<PHW_RFD>(m_pRxArea->GetTailDescriptor());

	if( pHwRfd )
	{
		pHwRfd->RfdCbHeader.CbCommand = RFD_EL_BIT;
		pHwRfd->RfdCbHeader.CbLinkPointer = DRIVER_NULL;
	}
	else
	{
		ASSERT( pHwRfd );
	}

	// check if the old tail of the list needs to be updated
	if( m_pPrevHwDesc )
	{
		// The new Sw RFD's and Hw Rfd's have been allocated and initialized.
		// This routine is to initialize the current end of the hardware list
		// with the first 'new' Hw RFD.

		PNDIS_PHYSICAL_ADDRESS p = m_pRxArea->GetTailPhysicalAddress();

		//update the current tail to point to the next descriptor
		ULONG PhysAddrLow = NdisGetPhysicalAddressLow(*p);
		TRACE3(("	PhysAddrLow = %d\n", PhysAddrLow));
		m_pPrevHwDesc->RfdCbHeader.CbLinkPointer = PhysAddrLow;
		m_pPrevHwDesc->RfdCbHeader.CbCommand = 0;
	}
}

// Recycle a RFD and put it back onto the receive list
// Assumption: Rcv spinlock has been acquired
VOID E100bexAdapter::DescriptorInitialize(
	PHW_RFD pHwRfd,						//The descriptor to initialize
	PNDIS_PHYSICAL_ADDRESS pPhysAddr	//Pointer to the physical address of the descriptor
	)
{
	TRACE2(("E100bexAdapter::DescriptorInitialize\n"));
	// called from KNdisSharedReceiveArea ctor on initial descriptor chain setup (& when descriptors are Added dynamically)
	// for each hardware Receive Frame Descriptor (FRD)
	// must initialize the hardware Receive Frame Descriptor

	ULONG PhysAddrLow = NdisGetPhysicalAddressLow(*pPhysAddr);

	// Init each RFD header (NICAllocRfd)
	pHwRfd->RfdRbdPointer = DRIVER_NULL;
	pHwRfd->RfdSize = NIC_MAX_PACKET_SIZE;

	//
	// HW_SPECIFIC_START
	//
	pHwRfd->RfdCbHeader.CbStatus = 0;
	pHwRfd->RfdActualCount= 0;
	pHwRfd->RfdCbHeader.CbCommand = 0;
	pHwRfd->RfdCbHeader.CbLinkPointer = PhysAddrLow + sizeof(HW_RFD);	// point to the next Rfd
	//
	// We don't use any of the OOB data besides status
	// Otherwise, we need to clean up OOB data
	// NdisZeroMemory(NDIS_OOB_DATA_FROM_PACKET(pMpRfd->NdisPacket),14);
	//
	// HW_SPECIFIC_END
	//

	m_CurrNumRfd++;
	TRACE3(("	sizeof(RFD_STRUC) = %x\n",  sizeof(HW_RFD)));
	TRACE2(("	m_CurrNumRfd = %d\n",		m_CurrNumRfd));
	DumpHwRfd(pHwRfd);
	TRACE3(("   PhysAddr = %lx\n",			PhysAddrLow));
}

VOID E100bexAdapter::DescriptorInvalidate(
	PHW_RFD pHwRfd						//The descriptor to invalidate
	)
{
	UNREFERENCED_PARAMETER(pHwRfd);

	m_CurrNumRfd--;
	TRACE2(("E100bexAdapter::DescriptorInvalidate m_CurrNumRfd = %d\n", m_CurrNumRfd));
}

VOID E100bexAdapter::DescriptorComplete(
	PHW_RFD pHwRfd,						//The descriptor to complete
	PNDIS_PHYSICAL_ADDRESS pPhysAddr	//Pointer to the physical address of the descriptor
	)
{
	UNREFERENCED_PARAMETER(pPhysAddr);

	TRACE3(("E100bexAdapter::DescriptorComplete\n"));
	DumpHwRfd(pHwRfd);
}

VOID E100bexAdapter::DescriptorReclaim(
	PHW_RFD pHwRfd,						//The descriptor to reclaim
	PNDIS_PHYSICAL_ADDRESS pPhysAddr	//Pointer to the physical address of the descriptor
	)
{
	TRACE3(("E100bexAdapter::DescriptorReclaim\n"));

	// Called from KNdisSharedReceiveArea Reclaim method
	// for each hardware Receive Frame Descriptor (HW_RFD) we
	// must re-initialize

	// Now we can release the receive resources back to the 82557.
	// We are done with this RFD so re-initialize it.
	pHwRfd->RfdCbHeader.CbStatus = 0;
	pHwRfd->RfdActualCount = 0;
	pHwRfd->RfdCbHeader.CbCommand = RFD_EL_BIT;
	pHwRfd->RfdCbHeader.CbLinkPointer = DRIVER_NULL;

	// update the last hardware RFD on the list to point at the new
	// last RFD = desc
	PHW_RFD pLastDesc = reinterpret_cast<PHW_RFD>(m_pRxArea->GetTailDescriptor());
	if( pLastDesc )
	{
		ULONG PhysAddrLow = NdisGetPhysicalAddressLow(*pPhysAddr);

		pLastDesc->RfdCbHeader.CbLinkPointer = PhysAddrLow;
		pLastDesc->RfdCbHeader.CbCommand = 0;
	}

	DumpHwRfd(pHwRfd);
	TRACE3(("   HwRfdPhys=%lx\n", NdisGetPhysicalAddressLow(*pPhysAddr)));
}


VOID E100bexAdapter::DumpHwRfd(PHW_RFD pHwRfd)
{
	TRACE3(("   RFD_STRUC* p =%lx\n", pHwRfd));
	TRACE3(("		p->RfdCbHeader.CbStatus=%lx\n",		 pHwRfd->RfdCbHeader.CbStatus));
	TRACE3(("		p->RfdCbHeader.CbCommand=%lx\n",	 pHwRfd->RfdCbHeader.CbCommand));
	TRACE3(("		p->RfdCbHeader.CbLinkPointer=%lx\n", pHwRfd->RfdCbHeader.CbLinkPointer));
	TRACE3(("		p->RfdRbdPointer=%lx\n",			 pHwRfd->RfdRbdPointer));
	TRACE3(("		p->RfdActualCount=%lx\n",			 pHwRfd->RfdActualCount));
}


// end of file

