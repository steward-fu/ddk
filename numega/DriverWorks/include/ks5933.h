// KS5933.h - include file for class KS5933 and related classes
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

#ifndef __KS5933__
#define __KS5933__


// Structure for storing flags indicating the state of the DMA transfer object
struct _KS5933_TRANSFER_STATE
{
	unsigned int m_Canceled:1;
	unsigned int m_DataError:1;
	unsigned int m_InitiateFailed:1;
	ULONG m_BytesTransferred;
};
typedef struct _KS5933_TRANSFER_STATE KS5933_TRANSFER_STATE,
	*PKS5933_TRANSFER_STATE;

// Handler routine for completion of DMA.  This will be called
// at DISPATCH_LEVEL
typedef
VOID
(*PKS5933_DMA_DONE_HANDLER_ROUTINE) (
	PVOID context,
	KS5933_TRANSFER_STATE state
    );

#define KS5933_TRANSFER_CONTEXT_SIGNATURE 0xdeed1dee

///////////////////////////////////////////////////////////////////////////////
// Class declaration for KS5933Pci
///////////////////////////////////////////////////////////////////////////////
template <class TRange, class TRegister> class KS5933Pci :
	public KS59XXPci <TRange, TRegister>
{

// Data Types
public:
	///////////////////////////////////////////////////////////////////////////
	// Class specific data types.  These must be part of the template class
	// since they contain a pointer to it, and are not fully specified until
	// the class is instantiated.

	struct _KS5933_TRANSFER_CONTEXT
	{
		ULONG m_Signature;
		KS5933Pci* m_pClass;
		KDmaTransfer m_Transfer;
		PKS5933_DMA_DONE_HANDLER_ROUTINE m_HandlerRoutine;
		PVOID m_HandlerContext;
		PTRANSFER_DESCRIPTOR m_pDescriptor;
		ULONG m_Size;
		KS5933_TRANSFER_STATE m_State;
	};
	typedef struct _KS5933_TRANSFER_CONTEXT KS5933_TRANSFER_CONTEXT,
		*PKS5933_TRANSFER_CONTEXT;

// Construction
public:
	SAFE_DESTRUCTORS

	KS5933Pci(void);

#if ! _WDM_

	KS5933Pci(
		USHORT VendorId,
		USHORT DeviceId,
		KPciSlot* pSlot=NULL
		);

	KS5933Pci(
		ULONG Bus,
		ULONG Device,
		ULONG Function
		);


	NTSTATUS Initialize(
		KDevice* pDevice,
		PWSTR RegistryPath,
		ULONG OperationRegisterBARIndex,
		ULONG DmaMaxReadLength,
		ULONG DmaMaxWriteLength,
		BOOLEAN NvramIsSerialType = TRUE,
		USHORT NvramPageSize = 1,
		BOOLEAN UseSystemScatterGather = TRUE
		);

	NTSTATUS Initialize(
		USHORT VendorId,
		USHORT DeviceId,
		KPciSlot* pSlot,
		KDevice* pDevice,
		PWSTR RegistryPath,
		ULONG OperationRegisterBARIndex,
		ULONG DmaMaxReadLength,
		ULONG DmaMaxWriteLength,
		BOOLEAN NvramIsSerialType = TRUE,
		USHORT NvramPageSize = 1,
		BOOLEAN UseSystemScatterGather = TRUE
		);

protected:
	NTSTATUS InitializeS5933(
		ULONG DmaMaxReadLength,
		ULONG DmaMaxWriteLength,
		BOOLEAN UseSystemScatterGather
		);

#else // ! _WDM_

public:
	NTSTATUS Initialize(
		PDEVICE_OBJECT TopOfStackDevice,
		PCM_RESOURCE_LIST pTranslatedResourceList,
		PCM_RESOURCE_LIST pRawResourceList,
		ULONG OperationRegisterBARIndex,
		ULONG DmaMaxReadLength,
		ULONG DmaMaxWriteLength,
		BOOLEAN NvramIsSerialType = TRUE,
		USHORT NvramPageSize = 1,
		BOOLEAN UseSystemScatterGather = TRUE
		);

protected:
	NTSTATUS InitializeS5933(
		PDEVICE_OBJECT TopOfStackDevice,
		ULONG DmaMaxReadLength,
		ULONG DmaMaxWriteLength,
		BOOLEAN UseSystemScatterGather
		);

#endif // ! _WDM_

// Methods
public:
	NTSTATUS ResetFifoStatusFlags(
		KS59XX_DIRECTION direction,
		BOOLEAN Lock = TRUE
		);

	NTSTATUS SetFifoProperties(
		KS59XX_DIRECTION direction,
		BOOLEAN SetBurstManagementScheme = FALSE,
		BOOLEAN SetPriority = FALSE,
		BOOLEAN Lock = TRUE
		);

	NTSTATUS SetFifoBusProperties(
		KS59XX_ENDIAN_CONVERSION EndianConversion = KS59XX_NO_CONVERSION,
		ULONG PciFifoAdvanceByte = 0,
		ULONG AddonFifoAdvanceByte = 0,
		BOOLEAN Lock = TRUE
		);

	NTSTATUS ReadOperationRegisters(PAMCCS5933_PCI_BUS_OPERATION_REGISTERS pOpRegs5933);

	NTSTATUS WriteOperationRegisters(PAMCCS5933_PCI_BUS_OPERATION_REGISTERS pOpRegs5933);

	VOID DmaRead(
		KDevice* pDevice,
		PMDL pMdl,
		PKS5933_DMA_DONE_HANDLER_ROUTINE pCallback,
		PVOID pContext = NULL
		);

	VOID DmaWrite(
		KDevice* pDevice,
		PMDL pMdl,
		PKS5933_DMA_DONE_HANDLER_ROUTINE pCallback,
		PVOID pContext = NULL
		);

	BOOLEAN CancelDmaRead();

	BOOLEAN CancelDmaWrite();

	VOID DisconnectIsr(){m_Irq.Disconnect();}

	NTSTATUS ConnectIsr(){
		NTSTATUS status;
		status = m_Irq.Connect(
			LinkTo(Isr),
			this );
		if (!NT_SUCCESS(status))
		{
			// Clean up and return error
			Invalidate();
		}
		return status;
	}

protected:
	// The ISR (interrupt service routine)
	MEMBER_ISR( KS5933Pci, Isr );

	// The DPC (deferred procedure call) for the ISR
	MEMBER_DPC( KS5933Pci, DpcForIsr );

	MEMBER_DMAREADYWITHCONTEXT( KS5933_TRANSFER_CONTEXT, OnDmaReady );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS5933_TRANSFER_CONTEXT, StartDmaReadOnDevice );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS5933_TRANSFER_CONTEXT, StartDmaWriteOnDevice );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS5933_TRANSFER_CONTEXT, CancelDmaReadOnDevice );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS5933_TRANSFER_CONTEXT, CancelDmaWriteOnDevice );

#ifdef _COMMENT_ONLY

		// The following member functions are actually defined by the
		// a DEVMEMBER_xxx or MEMBER_xxx macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	BOOLEAN Isr(void);								// COMMENT_ONLY
	VOID DpcForIsr( PVOID Context1, PVOID Context2 ); // COMMENT_ONLY
	VOID OnDmaReady( KDmaTransfer* pXfer, KS5933_TRANSFER_CONTEXT* pXferContext );
	BOOLEAN StartDmaReadOnDevice( KS5933_TRANSFER_CONTEXT* pXferContext );
	BOOLEAN StartDmaWriteOnDevice( KS5933_TRANSFER_CONTEXT* pXferContext );
	BOOLEAN CancelDmaReadOnDevice( KS5933_TRANSFER_CONTEXT* pXferContext );
	BOOLEAN CancelDmaWriteOnDevice( KS5933_TRANSFER_CONTEXT* pXferContext );
#endif

// Data
public:
	// Register data members to access the registers of the S5933.  Note that
	// the INTCSR and MCSR are defined in KS59XXPci
	TRegister m_OutgoingMailbox1;			// OMB1 - Outgoing Mailbox Reg 1
	TRegister m_OutgoingMailbox2;			// OMB2 - Outgoing Mailbox Reg 2
	TRegister m_OutgoingMailbox3;			// OMB3 - Outgoing Mailbox Reg 3
	TRegister m_OutgoingMailbox4;			// OMB4 - Outgoing Mailbox Reg 4
	TRegister m_IncomingMailbox1;			// IMB1 - Incoming Mailbox Reg 1
	TRegister m_IncomingMailbox2;			// IMB2 - Incoming Mailbox Reg 2
	TRegister m_IncomingMailbox3;			// IMB3 - Incoming Mailbox Reg 3
	TRegister m_IncomingMailbox4;			// IMB4 - Incoming Mailbox Reg 4
	TRegister m_FIFORegisterPort;			// FIFO - FIFO Reg Port (bidirectional)
	TRegister m_MasterWriteAddress;			// MWAR - Master Write Address Reg
	TRegister m_MasterWriteTransferCount;	// MWTC - Master Write Transfer Count Reg
	TRegister m_MasterReadAddress;			// MRAR - Master Read Address Reg
	TRegister m_MasterReadTransferCount;	// MRTC - Master Read Transfer Count Reg
	TRegister m_MailboxEmptyFullStatus;		// MBEF - Mailbox Empty/Full Status Reg

protected:
	KDeferredCall m_IsrDpc;
	KDmaAdapter m_ReadDmaAdapter;
	KDmaAdapter m_WriteDmaAdapter;
	KS5933_TRANSFER_CONTEXT m_ReadXferContext;
	LONG m_ReadXferInUse;
	KS5933_TRANSFER_CONTEXT m_WriteXferContext;
	LONG m_WriteXferInUse;
};

//////////////////////////////////////////////////////////////////////////////
// Typedefs for the two allowable kinds of KS5933Pci
//////////////////////////////////////////////////////////////////////////////
typedef KS5933Pci<KIoRange, KIoRegister> KS5933PciIoMapped;
typedef KS5933Pci<KMemoryRange, KMemoryRegister> KS5933PciMemoryMapped;

//////////////////////////////////////////////////////////////////////////////
// Class Definition for KS5933Pci
//////////////////////////////////////////////////////////////////////////////

template <class TRange, class TRegister>
KS5933Pci<TRange, TRegister>::KS5933Pci(void) :
	KS59XXPci<TRange, TRegister>(),
	m_OutgoingMailbox1(),
	m_OutgoingMailbox2(),
	m_OutgoingMailbox3(),
	m_OutgoingMailbox4(),
	m_IncomingMailbox1(),
	m_IncomingMailbox2(),
	m_IncomingMailbox3(),
	m_IncomingMailbox4(),
	m_FIFORegisterPort(),
	m_MasterWriteAddress(),
	m_MasterWriteTransferCount(),
	m_MasterReadAddress(),
	m_MasterReadTransferCount(),
	m_MailboxEmptyFullStatus(),
	m_ReadDmaAdapter(),
	m_WriteDmaAdapter(),
	m_ReadXferInUse(0),
	m_WriteXferInUse(0)
{
	m_ReadXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_ReadXferContext.m_pClass = this;
	m_ReadXferContext.m_HandlerRoutine = NULL;
	m_ReadXferContext.m_HandlerContext = NULL;
	m_ReadXferContext.m_pDescriptor = NULL;
	m_ReadXferContext.m_Size = 0;
	m_ReadXferContext.m_State.m_Canceled = FALSE;
	m_ReadXferContext.m_State.m_DataError = FALSE;
	m_ReadXferContext.m_State.m_InitiateFailed = FALSE;
	m_ReadXferContext.m_State.m_BytesTransferred = 0;

	m_WriteXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_WriteXferContext.m_pClass = this;
	m_WriteXferContext.m_HandlerRoutine = NULL;
	m_WriteXferContext.m_HandlerContext = NULL;
	m_WriteXferContext.m_pDescriptor = NULL;
	m_WriteXferContext.m_Size = 0;
	m_WriteXferContext.m_State.m_Canceled = FALSE;
	m_WriteXferContext.m_State.m_DataError = FALSE;
	m_WriteXferContext.m_State.m_InitiateFailed = FALSE;
	m_WriteXferContext.m_State.m_BytesTransferred = 0;
}

#if ! _WDM_

template <class TRange, class TRegister>
KS5933Pci<TRange, TRegister>::KS5933Pci(
	USHORT VendorId,
	USHORT DeviceId,
	KPciSlot* pSlot
	) :
	KS59XXPci<TRange, TRegister>(VendorId, DeviceId, pSlot),
	m_OutgoingMailbox1(),
	m_OutgoingMailbox2(),
	m_OutgoingMailbox3(),
	m_OutgoingMailbox4(),
	m_IncomingMailbox1(),
	m_IncomingMailbox2(),
	m_IncomingMailbox3(),
	m_IncomingMailbox4(),
	m_FIFORegisterPort(),
	m_MasterWriteAddress(),
	m_MasterWriteTransferCount(),
	m_MasterReadAddress(),
	m_MasterReadTransferCount(),
	m_MailboxEmptyFullStatus(),
	m_ReadDmaAdapter(),
	m_WriteDmaAdapter(),
	m_ReadXferInUse(0),
	m_WriteXferInUse(0)
{
	m_ReadXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_ReadXferContext.m_pClass = this;
	m_ReadXferContext.m_HandlerRoutine = NULL;
	m_ReadXferContext.m_HandlerContext = NULL;
	m_ReadXferContext.m_pDescriptor = NULL;
	m_ReadXferContext.m_Size = 0;
	m_ReadXferContext.m_State.m_Canceled = FALSE;
	m_ReadXferContext.m_State.m_DataError = FALSE;
	m_ReadXferContext.m_State.m_InitiateFailed = FALSE;
	m_ReadXferContext.m_State.m_BytesTransferred = 0;

	m_WriteXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_WriteXferContext.m_pClass = this;
	m_WriteXferContext.m_HandlerRoutine = NULL;
	m_WriteXferContext.m_HandlerContext = NULL;
	m_WriteXferContext.m_pDescriptor = NULL;
	m_WriteXferContext.m_Size = 0;
	m_WriteXferContext.m_State.m_Canceled = FALSE;
	m_WriteXferContext.m_State.m_DataError = FALSE;
	m_WriteXferContext.m_State.m_InitiateFailed = FALSE;
	m_WriteXferContext.m_State.m_BytesTransferred = 0;
}

template <class TRange, class TRegister>
KS5933Pci<TRange, TRegister>::KS5933Pci(
	ULONG Bus,
	ULONG Device,
	ULONG Function
	) :
	KS59XXPci<TRange, TRegister>(Bus, Device, Function),
	m_OutgoingMailbox1(),
	m_OutgoingMailbox2(),
	m_OutgoingMailbox3(),
	m_OutgoingMailbox4(),
	m_IncomingMailbox1(),
	m_IncomingMailbox2(),
	m_IncomingMailbox3(),
	m_IncomingMailbox4(),
	m_FIFORegisterPort(),
	m_MasterWriteAddress(),
	m_MasterWriteTransferCount(),
	m_MasterReadAddress(),
	m_MasterReadTransferCount(),
	m_MailboxEmptyFullStatus(),
	m_ReadDmaAdapter(),
	m_WriteDmaAdapter(),
	m_ReadXferInUse(0),
	m_WriteXferInUse(0)
{
	m_ReadXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_ReadXferContext.m_pClass = this;
	m_ReadXferContext.m_HandlerRoutine = NULL;
	m_ReadXferContext.m_HandlerContext = NULL;
	m_ReadXferContext.m_pDescriptor = NULL;
	m_ReadXferContext.m_Size = 0;
	m_ReadXferContext.m_State.m_Canceled = FALSE;
	m_ReadXferContext.m_State.m_DataError = FALSE;
	m_ReadXferContext.m_State.m_InitiateFailed = FALSE;
	m_ReadXferContext.m_State.m_BytesTransferred = 0;

	m_WriteXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_WriteXferContext.m_pClass = this;
	m_WriteXferContext.m_HandlerRoutine = NULL;
	m_WriteXferContext.m_HandlerContext = NULL;
	m_WriteXferContext.m_pDescriptor = NULL;
	m_WriteXferContext.m_Size = 0;
	m_WriteXferContext.m_State.m_Canceled = FALSE;
	m_WriteXferContext.m_State.m_DataError = FALSE;
	m_WriteXferContext.m_State.m_InitiateFailed = FALSE;
	m_WriteXferContext.m_State.m_BytesTransferred = 0;
}

template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::Initialize(
	USHORT VendorId,
	USHORT DeviceId,
	KPciSlot* pSlot,
	KDevice* pDevice,
	PWSTR RegistryPath,
	ULONG OperationRegisterBARIndex,
	ULONG DmaMaxReadLength,
	ULONG DmaMaxWriteLength,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize,
	BOOLEAN UseSystemScatterGather
	)
{
	NTSTATUS status;

	status = KS59XXPci<TRange, TRegister>::Initialize(
		VendorId,
		DeviceId,
		pSlot,
		pDevice,
		RegistryPath,
		OperationRegisterBARIndex,
		NvramIsSerialType,
		NvramPageSize
		);

	if ( NT_SUCCESS(status) )
	{
		status = InitializeS5933(
			DmaMaxReadLength,
			DmaMaxWriteLength,
			UseSystemScatterGather
			);
	}

	return status;
}

template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::Initialize(
	KDevice* pDevice,
	PWSTR RegistryPath,
	ULONG OperationRegisterBARIndex,
	ULONG DmaMaxReadLength,
	ULONG DmaMaxWriteLength,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize,
	BOOLEAN UseSystemScatterGather
	)
{
	NTSTATUS status;

	status = KS59XXPci<TRange, TRegister>::Initialize(
		pDevice,
		RegistryPath,
		OperationRegisterBARIndex,
		NvramIsSerialType,
		NvramPageSize
		);

	if ( NT_SUCCESS(status) )
	{
		status = InitializeS5933(
			DmaMaxReadLength,
			DmaMaxWriteLength,
			UseSystemScatterGather
			);
	}

	return status;
}

#else // ! _WDM_
template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::Initialize(
	PDEVICE_OBJECT TopOfStackDevice,
	PCM_RESOURCE_LIST pTranslatedResourceList,
	PCM_RESOURCE_LIST pRawResourceList,
	ULONG OperationRegisterBARIndex,
	ULONG DmaMaxReadLength,
	ULONG DmaMaxWriteLength,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize,
	BOOLEAN UseSystemScatterGather
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = KS59XXPci<TRange, TRegister>::Initialize(
		TopOfStackDevice,
		pTranslatedResourceList,
		pRawResourceList,
		OperationRegisterBARIndex,
		NvramIsSerialType,
		NvramPageSize
		);

	if ( NT_SUCCESS(status) )
	{
		// Initialize m_pResourceList
		m_pResourceList = pTranslatedResourceList;
		//InitializeS5933 only needs the translated resource list to
		//check for an interrupt resource.

		status = InitializeS5933(
			TopOfStackDevice,
			DmaMaxReadLength,
			DmaMaxWriteLength,
			UseSystemScatterGather
			);

		// The resource list won't be valid after we return so clear the
		// pointer to it.
		m_pResourceList = NULL;

	}

	return status;
}


#endif // ! _WDM_

#if ! _WDM_
template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::InitializeS5933(
	ULONG DmaMaxReadLength,
	ULONG DmaMaxWriteLength,
	BOOLEAN UseSystemScatterGather
	)
#else
template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::InitializeS5933(
	PDEVICE_OBJECT TopOfStackDevice,
	ULONG DmaMaxReadLength,
	ULONG DmaMaxWriteLength,
	BOOLEAN UseSystemScatterGather
	)
#endif
{
	NTSTATUS status = STATUS_SUCCESS;

	// Initialize register data members specific to S5933
	m_OutgoingMailbox1 = m_OperationRegisterBase[OP_REG_OFFSET(OutgoingMailbox1)];
	m_OutgoingMailbox2 = m_OperationRegisterBase[OP_REG_OFFSET(OutgoingMailbox2)];
	m_OutgoingMailbox3 = m_OperationRegisterBase[OP_REG_OFFSET(OutgoingMailbox3)];
	m_OutgoingMailbox4 = m_OperationRegisterBase[OP_REG_OFFSET(OutgoingMailbox4)];
	m_IncomingMailbox1 = m_OperationRegisterBase[OP_REG_OFFSET(IncomingMailbox1)];
	m_IncomingMailbox2 = m_OperationRegisterBase[OP_REG_OFFSET(IncomingMailbox2)];
	m_IncomingMailbox3 = m_OperationRegisterBase[OP_REG_OFFSET(IncomingMailbox3)];
	m_IncomingMailbox4 = m_OperationRegisterBase[OP_REG_OFFSET(IncomingMailbox4)];
	m_FIFORegisterPort = m_OperationRegisterBase[OP_REG_OFFSET(FIFORegisterPort)];
	m_MasterWriteAddress =
		m_OperationRegisterBase[OP_REG_OFFSET(MasterWriteAddress)];
	m_MasterWriteTransferCount =
		m_OperationRegisterBase[OP_REG_OFFSET(MasterWriteTransferCount)];
	m_MasterReadAddress =
		m_OperationRegisterBase[OP_REG_OFFSET(MasterReadAddress)];
	m_MasterReadTransferCount =
		m_OperationRegisterBase[OP_REG_OFFSET(MasterReadTransferCount)];
	m_MailboxEmptyFullStatus =
		m_OperationRegisterBase[OP_REG_OFFSET(MailboxEmptyFullStatus)];

	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Reset the FIFO and mailbox status flags.
	FlexContext.m_dwParam1 = KS59XX_MCSR_MAILBOX_FLAG_RESET
        | KS59XX_MCSR_ADDON_TO_PCI_FIFO_RESET
        | KS59XX_MCSR_PCI_TO_ADDON_FIFO_RESET;

	// Modify the S5933's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(McsrReadModifyWrite), &FlexContext );

	// Check to see if an interrupt exists in our resource list.
	// If it does, setup the DMA adapters and then connect interrupt.
	KResourceAssignment theIrqAssignment(
		m_pResourceList,
		CmResourceTypeInterrupt );
	if ( theIrqAssignment.Status() )
	{
		// Initialize KDmaAdapters
		DEVICE_DESCRIPTION dd;

		// Set up the adapter description for the device
		RtlZeroMemory(&dd, sizeof(DEVICE_DESCRIPTION));

		dd.Version			= DEVICE_DESCRIPTION_VERSION;
		dd.Master			= TRUE;

		// The S5933 is not a scatter gather device.  The choice remains to
		// use system scatter gather, which incurs a copy cost on most X86 HALs
		// or to use multiple transactions each as small as a page, at the cost of
		// setting each up.
		dd.ScatterGather 	= (UseSystemScatterGather) ? static_cast<UCHAR>(FALSE) :
			static_cast<UCHAR>(TRUE);
		dd.DemandMode		= FALSE;
		dd.AutoInitialize 	= FALSE;
		dd.Dma32BitAddresses= TRUE;
		dd.IgnoreCount 		= FALSE;
#if ! _WDM_
		dd.BusNumber 		= m_PciConfig.Bus();
#endif
		dd.DmaChannel		= 0;
		dd.InterfaceType	= PCIBus;
		dd.DmaWidth 		= Width32Bits;
		dd.DmaSpeed			= Compatible;
		dd.MaximumLength	= DmaMaxReadLength;

		// Initialize the read dma adapter object.  Note we specify that we
		// have a max of one scatter/gather pair.  If we use system scatter/gather
		// this will be ignored.  If we don't use system scatter/gather (we have told
		// the system we support bus master scatter/gather), we have informed
		// the adapter class that we can only handle a single map register at a time.
#if ! _WDM_
		if ( !m_ReadDmaAdapter.Initialize(&dd, 1) )
#else
		if ( !m_ReadDmaAdapter.Initialize(&dd, TopOfStackDevice, 1) )
#endif
		{
			// Clean up and return error
			Invalidate();
			return STATUS_UNSUCCESSFUL;
		}

		dd.MaximumLength	= DmaMaxWriteLength;

		// Initialize the write dma adapter object.
#if ! _WDM_
		if ( !m_WriteDmaAdapter.Initialize(&dd, 1) )
#else
		if ( !m_WriteDmaAdapter.Initialize(&dd, TopOfStackDevice, 1) )
#endif
		{
			// Clean up and return error
			Invalidate();
			return STATUS_UNSUCCESSFUL;
		}

		// Connect our interrupt
		status = m_Irq.Connect(
			LinkTo(Isr),
			this );
		if (!NT_SUCCESS(status))
		{
			// Clean up and return error
			Invalidate();
			return status;
		}

		// Initialize DPC
		m_IsrDpc.Setup(LinkTo(DpcForIsr), this);

	}

	return status;
}

template <class TRange, class TRegister>
VOID KS5933Pci<TRange, TRegister>::DmaRead(
	KDevice* pDevice,
	PMDL pMdl,
	PKS5933_DMA_DONE_HANDLER_ROUTINE pCallback,
	PVOID pContext
	)
{

	// If we have no interrupt we can't do this.  Declare an error
	// and return.
	if (m_NoInterrupt)
	{
		ASSERT(FALSE);
		return;
	}

	ASSERT(pCallback != NULL);

	// Check to see if there is a read transfer already in progress.
	// If not, claim the transfer object.  If so, declare an error and return.
	if ( INTERLOCKED_COMPARE_EXCHANGE( &m_ReadXferInUse, 1, 0L ) )
	{
		ASSERT(FALSE);
		return;
	}

	// Initialize the transfer structure
	m_ReadXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_ReadXferContext.m_pClass = this;
	m_ReadXferContext.m_HandlerRoutine = pCallback;
	m_ReadXferContext.m_HandlerContext = pContext;
	m_ReadXferContext.m_pDescriptor = NULL;
	m_ReadXferContext.m_Size = 0;
	m_ReadXferContext.m_State.m_Canceled = FALSE;
	m_ReadXferContext.m_State.m_DataError = FALSE;
	m_ReadXferContext.m_State.m_InitiateFailed = FALSE;
	m_ReadXferContext.m_State.m_BytesTransferred = 0;

	// Save the size of the transfer
	KMemory Mem(pMdl);
	m_ReadXferContext.m_Size = Mem.Size();

	// Initiate the transfer on the read dma adapter object
	NTSTATUS status;
	status = m_ReadXferContext.m_Transfer.Initiate(
		pDevice,
		&m_ReadDmaAdapter,
		pMdl,
		FromDeviceToMemory,
		LinkTo(OnDmaReady),
		NULL,
		&m_ReadXferContext,
		TRUE
		);

	// If the transfer cannot be initiated, call the user's callback function.
	// Note this should never happen unless the underlying call to
	// IoAllocateAdapterChannel fails.
	if ( ! NT_SUCCESS(status) )
	{

		// Set the state flag indicating that the initiate failed
		m_ReadXferContext.m_State.m_InitiateFailed = TRUE;

		// Copy the callback parameters to local storage
		KS5933_TRANSFER_STATE State = m_ReadXferContext.m_State;

		// Release the transfer object
		InterlockedExchange( &m_ReadXferInUse, 0 );

		// Call back the user's notification routine and pass the state flags for this
		// transfer.
		pCallback( pContext, State );
	}
}

template <class TRange, class TRegister>
VOID KS5933Pci<TRange, TRegister>::DmaWrite(
	KDevice* pDevice,
	PMDL pMdl,
	PKS5933_DMA_DONE_HANDLER_ROUTINE pCallback,
	PVOID pContext
	)
{

	// If we have no interrupt we can't do this.  Declare an error
	// and return.
	if (m_NoInterrupt)
	{
		ASSERT(FALSE);
		return;
	}

	ASSERT(pCallback != NULL);

	// Check to see if there is a write transfer already in progress.
	// If not, claim the transfer object.  If so, declare an error and return.
	if ( INTERLOCKED_COMPARE_EXCHANGE( &m_WriteXferInUse, 1, 0L ) )
	{
		ASSERT(FALSE);
		return;
	}

	// Initialize the transfer structure
	m_WriteXferContext.m_Signature = KS5933_TRANSFER_CONTEXT_SIGNATURE;
	m_WriteXferContext.m_pClass = this;
	m_WriteXferContext.m_HandlerRoutine = pCallback;
	m_WriteXferContext.m_HandlerContext = pContext;
	m_WriteXferContext.m_pDescriptor = NULL;
	m_WriteXferContext.m_Size = 0;
	m_WriteXferContext.m_State.m_Canceled = FALSE;
	m_WriteXferContext.m_State.m_DataError = FALSE;
	m_WriteXferContext.m_State.m_InitiateFailed = FALSE;
	m_WriteXferContext.m_State.m_BytesTransferred = 0;

	// Save the size of the transfer
	KMemory Mem(pMdl);
	m_WriteXferContext.m_Size = Mem.Size();

	// Initiate the transfer on the write dma adapter object
	NTSTATUS status;
	status = m_WriteXferContext.m_Transfer.Initiate(
		pDevice,
		&m_WriteDmaAdapter,
		pMdl,
		FromMemoryToDevice,
		LinkTo(OnDmaReady),
		NULL,
		&m_WriteXferContext,
		TRUE
		);

	// If the transfer cannot be initiated, call the user's callback function.
	// Note this should never happen unless the underlying call to
	// IoAllocateAdapterChannel fails.
	if ( ! NT_SUCCESS(status) )
	{

		// Set the state flag indicating that the initiate failed
		m_WriteXferContext.m_State.m_InitiateFailed = TRUE;

		// Copy the callback parameters to local storage
		KS5933_TRANSFER_STATE State = m_WriteXferContext.m_State;

		// Release the transfer object
		InterlockedExchange( &m_WriteXferInUse, 0 );

		// Call back the user's notification routine and pass the state flags for this
		// transfer.
		pCallback( pContext, State );
	}
}

template <class TRange, class TRegister>
BOOLEAN KS5933Pci<TRange, TRegister>::CancelDmaRead()
{
	return m_Irq.Synchronize( LinkTo(CancelDmaReadOnDevice), &m_ReadXferContext );
}

template <class TRange, class TRegister>
BOOLEAN KS5933Pci<TRange, TRegister>::CancelDmaWrite()
{
	return m_Irq.Synchronize( LinkTo(CancelDmaWriteOnDevice), &m_WriteXferContext );
}

//
// The AMCC S5933 is a busmaster, so it can write data directly into
// physical memory without the aid of the motherboard DMA hardware.
// The AMCC S5933 chip does not support true scatter/gather,
// so our routine to reload the DMA hardware must be called for each
// "chunk" of the transfer.  The size of a chunk is out of our control
// and is dependant on the size of the buffer that NT uses for our DMA transfer.
// If our buffer size is larger than the size of the system buffer, then
// the transfer request is broken into multiple DMA operations.
//

//
// The order of operations is as follows:
//
//  --> OnDmaReady() - This routine is called at least twice for
//	|		their DMA operations.  The first call starts the
//  |		DMA operation.  The last call terminates the operation
//  |		and completes the IRP.  If the size of the Read()/Write() request
//  |		is larger than can be satisfied in a single DMA transfer,
//  |		then this routine will be called more than two times.
//  |
//  |  Isr() - Invoked when our DMA hardware signals that its count has
//  |		run down to 0.  This does not necessarily mean that the
//  |		Read() or Write() is done.  It may mean that we have reached the
//  |		end of the current chunk of data, and that NT must give us
//  |		the address and length of the next chunk.  Note that the Isr
//	|		handles both the Read() and Write() interrupts as well as all others
//	|		generated by the S5933.
//	|
//  |
//  -- DpcForIsr() - This is the DPC that is invoked by the Isr().  For DMA
//			operations, it calls the Continue() member of the transfer class,
//			which causes the DMA controller to possibly be set up
//			again (if there are more bytes remaining), and, in any case,
//			causes OnReadDmaReady()/OnWriteDmaReady to be called again.
//

template <class TRange, class TRegister>
VOID KS5933Pci<TRange, TRegister>::OnDmaReady(
	KDmaTransfer* pXfer,
	KS5933_TRANSFER_CONTEXT* pXferContext
	)
{

	// Whenever this routine is called, there should be no transfer enabled of the
	// type we are here to setup.  If there is, it is an error.  To check
	// read the apporpriate bit in the S5933's MCSR.
	// Note that from the S5933's perspective, "write" enable correponds to
	// a read DMA transaction (FromDeviceToMemory) for us.  Also, we don't need
	// to protect this access since we are only reading and not writing it back.
	ULONG McsrRegister = m_BusMasterControlStatus;
	if ( pXfer->Direction() == FromDeviceToMemory )
	{
		ASSERT( !(McsrRegister & KS59XX_MCSR_WRITE_XFER_ENABLE) );
	}
	else
	{
		ASSERT( !(McsrRegister & KS59XX_MCSR_READ_XFER_ENABLE) );
	}

	// Check for valid context structure
	ASSERT( pXferContext->m_Signature == KS5933_TRANSFER_CONTEXT_SIGNATURE );

	// All KDmaTransfer callbacks must first check to see if there are any bytes
	// left to transfer, or if the state of the transfer has changed (ie it
	// was canceled or aborted with a data error).  If we are finished for any
	// of these reasons, complete the transfer and pass the state flags.
	if ( (pXfer->BytesRemaining() == 0) || pXferContext->m_State.m_Canceled ||
		pXferContext->m_State.m_DataError )
	{

		// Terminate the transfer.
		pXfer->Terminate();

		// Copy the callback parameters to local storage
		PKS5933_DMA_DONE_HANDLER_ROUTINE HandlerCallback = pXferContext->m_HandlerRoutine;
		PVOID HandlerContext = pXferContext->m_HandlerContext;
		KS5933_TRANSFER_STATE State = pXferContext->m_State;

		// Release the transfer object
		if ( pXfer->Direction() == FromDeviceToMemory )
		{
			InterlockedExchange( &m_ReadXferInUse, 0 );
		}
		else
		{
			InterlockedExchange( &m_WriteXferInUse, 0 );
		}

		// Call back the user's notification routine and pass the state flags for this
		// transfer.
		HandlerCallback( HandlerContext, State );
		return;
	}

	// We must get the descriptor for the physical memory location for
	// the DMA transfer.
	pXfer->GetTransferDescriptors(&pXferContext->m_pDescriptor);

	// With the memory buffer information, we can now tell the S5933
	// where to send the DMA data.  If the synch routine returns TRUE, the transfer
	// started successfully.  If FALSE is returned, the transfer was cancelled
	// and we must terminate and complete the transfer now.
	BOOLEAN Started = FALSE;
	if ( pXfer->Direction() == FromDeviceToMemory )
	{
		Started = m_Irq.Synchronize( LinkTo(StartDmaReadOnDevice), pXferContext );
	}
	else
	{
		Started = m_Irq.Synchronize( LinkTo(StartDmaWriteOnDevice), pXferContext );
	}
	if (!Started)
	{

		// Terminate the transfer.
		pXfer->Terminate();

		// Copy the callback parameters to local storage
		PKS5933_DMA_DONE_HANDLER_ROUTINE HandlerCallback = pXferContext->m_HandlerRoutine;
		PVOID HandlerContext = pXferContext->m_HandlerContext;
		KS5933_TRANSFER_STATE State = pXferContext->m_State;

		// Release the transfer object
		if ( pXfer->Direction() == FromDeviceToMemory )
		{
			InterlockedExchange( &m_ReadXferInUse, 0 );
		}
		else
		{
			InterlockedExchange( &m_WriteXferInUse, 0 );
		}

		// Call back the user's notification routine with the state flags for this
		// transfer.
		HandlerCallback( HandlerContext, State );
	}
}

template <class TRange, class TRegister>
BOOLEAN KS5933Pci<TRange, TRegister>::Isr()
{

	ULONG IntcsrRegister;
	ULONG McsrRegister;

	// Read the interrupt control/status register and the master control/status
	// register.  Mask the MCSR register appropriately
	IntcsrRegister = m_InterruptControlStatus;
	McsrRegister = m_BusMasterControlStatus;
	McsrRegister &= KS59XX_MCSR_PRESERVE_MASK;

	if ( !(IntcsrRegister & KS59XX_INTCSR_INTERRUPT_ASSERTED) )
	{
		// Return FALSE to indicate that this device did not cause the interrupt.
		return FALSE;
	}

	// Based on the type of interrupt that occurred, we will disable the
	// interrupt source and in the case of a DMA interrupt source the transfer enable
	// for the appropirate DMA operation.

	// Check for mailbox interrupts and disable the appropriate interrupt if
	// the interrupt source is setup for a single shot.
	if ( (IntcsrRegister & KS59XX_INTCSR_INCOMING_MAILBOX) &&
		m_IncomingMailboxSingleShot )
	{
		IntcsrRegister &= ~KS59XX_INTCSR_INT_ON_INCOMING_MAILBOX;
	}
	if ( (IntcsrRegister & KS59XX_INTCSR_OUTGOING_MAILBOX) &&
		m_OutgoingMailboxSingleShot )
	{
		IntcsrRegister &= ~KS59XX_INTCSR_INT_ON_OUTGOING_MAILBOX;
	}

	// If there was a DMA abort error interrupt, we must abort any enabled DMA
	// transfers.
	if (IntcsrRegister & ( KS59XX_INTCSR_TARGET_ABORT | KS59XX_INTCSR_MASTER_ABORT ))
	{

		// If there is a current read or write transfer in progress indicate that
		// there was a data error by setting the m_DataError bit in the
		// transfer context state structure. Also, if the transfer was enabled,
		// set the interrupt bit in the IntcsrRegister so that we will process the
		// read or write in the DPC as if an interrupt has occurred (regardless
		// of whether one has or not).
		if (INTERLOCKED_COMPARE_EXCHANGE( &m_ReadXferInUse, 1, 1 ))
		{
			m_ReadXferContext.m_State.m_DataError = TRUE;
			if (McsrRegister & KS59XX_MCSR_WRITE_XFER_ENABLE)
				IntcsrRegister |= KS59XX_INTCSR_WRITE_XFER_COMPLETE;
		}
		if (INTERLOCKED_COMPARE_EXCHANGE( &m_WriteXferInUse, 1, 1 ))
		{
			m_WriteXferContext.m_State.m_DataError = TRUE;
			if (McsrRegister & KS59XX_MCSR_READ_XFER_ENABLE)
				IntcsrRegister |= KS59XX_INTCSR_READ_XFER_COMPLETE;
		}
	}

	// Check for DMA interrupts and disable operation for any read and/or write
	// operation corresponding to the interrupt bits set in the INTCSR.  Note
	// that if an abort error occurrs, we will be disabling any enabled
	// read or write transfer, since we would have artificially set the interrupt
	// bit above.
	if (IntcsrRegister & KS59XX_INTCSR_READ_XFER_COMPLETE)
	{
		IntcsrRegister &= ~KS59XX_INTCSR_INT_ON_READ_XFER_COMPLETE;
		McsrRegister &= ~KS59XX_MCSR_READ_XFER_ENABLE;
	}
	if (IntcsrRegister & KS59XX_INTCSR_WRITE_XFER_COMPLETE)
	{
		IntcsrRegister &= ~KS59XX_INTCSR_INT_ON_WRITE_XFER_COMPLETE;
		McsrRegister &= ~KS59XX_MCSR_WRITE_XFER_ENABLE;
	}

	// Write back the modified MCSR to disable complete or aborted transfers
	m_BusMasterControlStatus = McsrRegister;

	// Now that any DMA transfers that we will be servicing (as complete or aborted)
	// are stopped, add to the number of bytes tranferred for each one being
	// serviced.  If the transfer is completing normally, we can just add the
	// length of the transfer, otherwise we must read from the device, the number
	// of bytes left and subtract this from the total.
	if (IntcsrRegister & KS59XX_INTCSR_WRITE_XFER_COMPLETE)
	{
		m_ReadXferContext.m_State.m_BytesTransferred +=
			m_ReadXferContext.m_pDescriptor->td_Length;
		if ( IntcsrRegister & ( KS59XX_INTCSR_TARGET_ABORT | KS59XX_INTCSR_MASTER_ABORT ) )
		{
			m_ReadXferContext.m_State.m_BytesTransferred -=
				static_cast<ULONG>(m_MasterWriteTransferCount);
		}
	}
	if (IntcsrRegister & KS59XX_INTCSR_READ_XFER_COMPLETE)
	{
		m_WriteXferContext.m_State.m_BytesTransferred +=
			m_WriteXferContext.m_pDescriptor->td_Length;
		if ( IntcsrRegister & ( KS59XX_INTCSR_TARGET_ABORT | KS59XX_INTCSR_MASTER_ABORT ) )
		{
			m_WriteXferContext.m_State.m_BytesTransferred -=
				static_cast<ULONG>(m_MasterReadTransferCount);
		}
	}

	// Accumulate interrupt bits to be processed by our DPC.  Since multiple
	// interrupts can occur before the DPC runs, we must accumulate these bits.
	// Note this data member must only be accessed with the interrupt spin lock held.
	m_AccumulatedInterrupts |= IntcsrRegister & KS59XX_INTCSR_INTERRUPT_STATUS_MASK;

	// Write back the modified INTCSR.  This will acknowledge the interrupt bits
	// that are set.
	m_InterruptControlStatus = IntcsrRegister;

	// Check for mailbox interrupts and call the appropriate handler if it
	// is registered to be called at DIRQL.  If we call the handler, unset
	// the bit in m_AccumulatedInterrupts so we don't call again at DPC time
	if ( (IntcsrRegister & KS59XX_INTCSR_INCOMING_MAILBOX) &&
		m_IncomingMailboxCallAtDirql )
	{
		if (m_pIncomingMailboxCallback != NULL)
			(*m_pIncomingMailboxCallback)(m_pIncomingMailboxContext);
		else
			ASSERT(m_pIncomingMailboxCallback != NULL);
		m_AccumulatedInterrupts &= ~KS59XX_INTCSR_INCOMING_MAILBOX;
	}
	if ( (IntcsrRegister & KS59XX_INTCSR_OUTGOING_MAILBOX) &&
		m_OutgoingMailboxCallAtDirql )
	{
		if (m_pOutgoingMailboxCallback != NULL)
			(*m_pOutgoingMailboxCallback)(m_pOutgoingMailboxContext);
		else
			ASSERT(m_pOutgoingMailboxCallback != NULL);
		m_AccumulatedInterrupts &= ~KS59XX_INTCSR_OUTGOING_MAILBOX;
	}

	// Request our DPC to finish processing the interrupts
	m_IsrDpc.Request(NULL, NULL);

	// Return TRUE to indicate that our device caused the interrupt
	return TRUE;
}

template <class TRange, class TRegister>
VOID KS5933Pci<TRange, TRegister>::DpcForIsr( PVOID Context1, PVOID Context2 )
{

	// Get the interrupt bits that have occurred.
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	m_Irq.Synchronize( LinkTo(GetAccumulatedInterrupts), &FlexContext );

	// Determine if a target abort occurred.
	//if ( FlexContext.m_dwParam1 & KS59XX_INTCSR_TARGET_ABORT )
	//{
	//}

	// Determine if a master abort occurred.
	//if ( FlexContext.m_dwParam1 & KS59XX_INTCSR_MASTER_ABORT )
	//{
	//}

	// Determine if a read transfer has completed and if so process it.
	// Read is a "write" to AMCC.
	if ( FlexContext.m_dwParam1 & KS59XX_INTCSR_WRITE_XFER_COMPLETE )
	{
		// We should never get this interrupt if we don't have a current read
		// transfer in progress.
		ASSERT( INTERLOCKED_COMPARE_EXCHANGE( &m_ReadXferInUse, 1, 1 ) == 1 );

		// Continue the transfer.  Note that we aren't holding any spinlock
		// to protect us from reentrancy of the DPC on an MP machine.  We expect
		// that a second read interrupt is not possible (other than from spurious
		// sources), since the read has been disabled in the ISR and won't get
		// reenabled until we start the next trasfer segement, at which time we
		// will have finished touching any shared resources that read uses.
		m_ReadXferContext.m_Transfer.Continue(UseTransferSize);
	}

	// Determine if a write transfer has completed and if so process it.
	// Write is a "read" to AMCC
	if ( FlexContext.m_dwParam1 & KS59XX_INTCSR_READ_XFER_COMPLETE )
	{
		// We should never get this interrupt if we don't have a current write
		// transfer in progress.
		ASSERT( INTERLOCKED_COMPARE_EXCHANGE( &m_WriteXferInUse, 1, 1 ) == 1 );

		// Continue the transfer.  Note that we aren't holding any spinlock
		// to protect us from reentrancy of the DPC on an MP machine. See read
		// case above.
		m_WriteXferContext.m_Transfer.Continue(UseTransferSize);
	}

	// Determine if an incoming mailbox interrupt occurred and if so process it.
	if ( FlexContext.m_dwParam1 & KS59XX_INTCSR_INCOMING_MAILBOX )
	{
		// We should never process this interrupt here if the call at DIRQL
		// flag is set.
		ASSERT(!m_IncomingMailboxCallAtDirql);

		// Callback the user's function with their context.  Note that the user's
		// function may be reentered if they did did not specify a single shot
		// interrupt, and two (or more) occurred, each causing an invocation of the
		// DPC.  In a MP environment, it is possible for the DPC to be running on
		// one processor and get queued again and then run simultaneously on a
		// second processor.  Note also that if the DPC was already queued for
		// one interrupt when a second occured, the DPC will not be queued again
		// and the DPC will only occur one time for both interrupts.
		if (m_pIncomingMailboxCallback != NULL)
			(*m_pIncomingMailboxCallback)(m_pIncomingMailboxContext);
		else
			ASSERT(m_pIncomingMailboxCallback != NULL);
	}

	// Determine if an outgoing mailbox interrupt occurred and if so process it.
	if ( FlexContext.m_dwParam1 & KS59XX_INTCSR_OUTGOING_MAILBOX )
	{
		// We should never process this interrupt here if the call at DIRQL
		// flag is set.
		ASSERT(!m_OutgoingMailboxCallAtDirql);

		// Callback the user's function with their context.  The user function
		// may be reentered.  See Incoming case above for details on how.
		if (m_pOutgoingMailboxCallback != NULL)
			(*m_pOutgoingMailboxCallback)(m_pOutgoingMailboxContext);
		else
			ASSERT(m_pOutgoingMailboxCallback != NULL);
	}

	UNREFERENCED_PARAMETER(Context1);
	UNREFERENCED_PARAMETER(Context2);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KS5933Pci<TRange, TRegister>::StartDmaReadOnDevice
//
//	This function performs the actual hardware manipulation to initiate
//		a new read request on the AMCCS5933 device.  When called, all resources
//      (mapping registers) have been allocated for the operation, and we have
//      a base address and length of a buffer fragment to be DMA'ed.
//
//	INPUTS:
//		pXferContext - Pointer to a KS5933_TRANSFER_CONTEXT structure, which
//			contains a pointer to the PTRANSFER_DESCRIPTOR associated
//			with the next segment of the transfer for this KDmaTransfer
//			object.
//
//	OUTPUTS:
//		None.
//
//	RETURN:
//		TRUE
//
//	IRQL:
//		This routine is called at IRQL_DISPATCH_LEVEL.
//
//	NOTES:
//		1. When this routine is called, no other Read operations are in progress on
//			the device.
//		2. To do a read we actually program the "write" function on the S5933.  This
//			is due to the fact that a "write" from the chip's perspective is a read
//			for the driver.
//
///////////////////////////////////////////////////////////////////////////////
template <class TRange, class TRegister>
BOOLEAN KS5933Pci<TRange, TRegister>::StartDmaReadOnDevice(
	KS5933_TRANSFER_CONTEXT* pXferContext
	)
{

	// Check to see if the transfer got cancelled or an error occurred somewhere
	// before we got here, in which case we shouldn't start it on the device.
	if (pXferContext->m_State.m_Canceled || pXferContext->m_State.m_DataError)
	{
		return FALSE;
	}

	// Setup the S5933's MCSR to suspend any read DMA operation.
	// This should already be done, but it is nice to be sure.
	// Note that from the S5933's perspective, we enable the "write" DMA
	ULONG Temp;
	Temp = m_BusMasterControlStatus;
	Temp &= (KS59XX_MCSR_PRESERVE_MASK & ~KS59XX_MCSR_WRITE_XFER_ENABLE);
	m_BusMasterControlStatus = Temp;

	// Write into the S5933's MWAR the address for the start of the segment.
	// Note the HighPart of the Physical address should always be zero and the
	// physical address must start on a DWORD boundary.
	ASSERT(!pXferContext->m_pDescriptor->td_PhysAddr.HighPart);
	ASSERT(!(pXferContext->m_pDescriptor->td_PhysAddr.LowPart & 0x3));
	m_MasterWriteAddress = pXferContext->m_pDescriptor->td_PhysAddr.LowPart;

	// Write into the S5933's MWTC the byte count for the segment.
	// Note the maximum transfer size is 0x3ffffff
	ASSERT(!(pXferContext->m_pDescriptor->td_Length & 0xFC000000));
	m_MasterWriteTransferCount = pXferContext->m_pDescriptor->td_Length;

	// Setup the S5933's INTCSR to generate an interrupt when the read is complete.
	// Note that from the S5933's perspective, this is a write operation
	Temp = m_InterruptControlStatus;
	Temp &= KS59XX_INTCSR_SET_CONTROL_MASK;
	Temp |= KS59XX_INTCSR_INT_ON_WRITE_XFER_COMPLETE;
	m_InterruptControlStatus = Temp;

	// Setup the S5933's MCSR to start the read DMA operation.
	// Note that from the S5933's perspective, we enable the "write" DMA
	Temp = m_BusMasterControlStatus;
	Temp &= KS59XX_MCSR_PRESERVE_MASK;
	Temp |= KS59XX_MCSR_WRITE_XFER_ENABLE;
	m_BusMasterControlStatus = Temp;

	return TRUE;
}

template <class TRange, class TRegister>
BOOLEAN KS5933Pci<TRange, TRegister>::StartDmaWriteOnDevice(
	KS5933_TRANSFER_CONTEXT* pXferContext
	)
{

	// Check to see if the transfer got cancelled or an error occurred somewhere
	// before we got here, in which case we shouldn't start it on the device.
	if (pXferContext->m_State.m_Canceled || pXferContext->m_State.m_DataError)
	{
		return FALSE;
	}

	// Setup the S5933's MCSR to suspend any write DMA operation.
	// This should already be done, but it is nice to be sure.
	// Note that from the S5933's perspective, we enable the "read" DMA
	ULONG Temp;
	Temp = m_BusMasterControlStatus;
	Temp &= (KS59XX_MCSR_PRESERVE_MASK & ~KS59XX_MCSR_READ_XFER_ENABLE);
	m_BusMasterControlStatus = Temp;

	// Write into the S5933's MRAR the address for the start of the segment.
	// Note the HighPart of the Physical address should always be zero and the
	// physical address must start on a DWORD boundary.
	ASSERT(!pXferContext->m_pDescriptor->td_PhysAddr.HighPart);
	ASSERT(!(pXferContext->m_pDescriptor->td_PhysAddr.LowPart & 0x3));
	m_MasterReadAddress = pXferContext->m_pDescriptor->td_PhysAddr.LowPart;

	// Write into the S5933's MRTC the byte count for the segment.
	// Note the maximum transfer size is 0x3ffffff
	ASSERT(!(pXferContext->m_pDescriptor->td_Length & 0xFC000000));
	m_MasterReadTransferCount = pXferContext->m_pDescriptor->td_Length;

	// Setup the S5933's INTCSR to generate an interrupt when the write is complete.
	// Note that from the S5933's perspective, this is a read operation
	Temp = m_InterruptControlStatus;
	Temp &= KS59XX_INTCSR_SET_CONTROL_MASK;
	Temp |= KS59XX_INTCSR_INT_ON_READ_XFER_COMPLETE;
	m_InterruptControlStatus = Temp;

	// Setup the S5933's MCSR to start the write DMA operation.
	// Note that from the S5933's perspective, we enable the "read" DMA
	Temp = m_BusMasterControlStatus;
	Temp &= KS59XX_MCSR_PRESERVE_MASK;
	Temp |= KS59XX_MCSR_READ_XFER_ENABLE;
	m_BusMasterControlStatus = Temp;

	return TRUE;
}

template <class TRange, class TRegister>
BOOLEAN KS5933Pci<TRange, TRegister>::CancelDmaReadOnDevice(
	KS5933_TRANSFER_CONTEXT* pTransferContext
	)
{
	BOOLEAN Return = FALSE;

	// Check to see if there is a read transfer in progress.
	if ( INTERLOCKED_COMPARE_EXCHANGE( &m_ReadXferInUse, 1, 1 ) )
	{

		// We found the transfer in progress so we can cancel it.
		Return = TRUE;

		// Check for valid context structure
		ASSERT( m_ReadXferContext.m_Signature == KS5933_TRANSFER_CONTEXT_SIGNATURE );

		// Set the bit indicating the transfer has been cancelled.
		m_ReadXferContext.m_State.m_Canceled = TRUE;

		// Check to see if the read is currently enabled.  If so, we will
		// stop the read and setup the DPC to complete it as cancelled.
		// If the read is not currently enabled the transfer either hasn't started,
		// just completed, or we are waiting for the next segment to begin.  In
		// any of these cases, we have nothing additional to do.  If the transfer
		// just started or completed, OnDmaReady will execute soon and
		// will finish the transfer in a cancelled state.  If OnDmaReady is
		// running now, the transfer will be cancelled the next time we attempt to
		// start a segment on the device.  Note that starting a segment
		// requires obtaining the interrupt spin lock, which we currently
		// own, so the next segment can't start in parallel to this routine.
		// Note that from the S5933's perspective, we enable the "write" DMA
		ULONG Temp;
		Temp = m_BusMasterControlStatus;
		if ( Temp & KS59XX_MCSR_WRITE_XFER_ENABLE )
		{
			Temp &= (KS59XX_MCSR_PRESERVE_MASK & ~KS59XX_MCSR_WRITE_XFER_ENABLE);
			m_BusMasterControlStatus = Temp;

			// Now that the read is stopped, we must disable the read
			// DMA interrupt and clear it in case the read interrupted,
			// but has not been serviced yet (we don't want to trigger
			// the DPC twice!).
			// Note that from the S5933's perspective, this is a write operation
			Temp = m_InterruptControlStatus;
			Temp &= (KS59XX_INTCSR_SET_CONTROL_MASK &
				~KS59XX_INTCSR_INT_ON_WRITE_XFER_COMPLETE);
			Temp |= KS59XX_INTCSR_WRITE_XFER_COMPLETE;
			m_InterruptControlStatus = Temp;

			// Add the number of bytes transferred before the read was stopped
			// to the count for the transfer
			m_ReadXferContext.m_State.m_BytesTransferred +=
				m_ReadXferContext.m_pDescriptor->td_Length
				- static_cast<ULONG>(m_MasterWriteTransferCount);

			// We must set a bit in the m_AccumulatedInterrupts and request the
			// interrupt's DPC, just as if the real interrupt had occurred to
			// finish the read operation.
			m_AccumulatedInterrupts |= KS59XX_INTCSR_WRITE_XFER_COMPLETE;
			m_IsrDpc.Request(NULL, NULL);
		}
	}
	return Return;
	UNREFERENCED_PARAMETER(pTransferContext);
}

template <class TRange, class TRegister>
BOOLEAN KS5933Pci<TRange, TRegister>::CancelDmaWriteOnDevice(
	KS5933_TRANSFER_CONTEXT* pTransferContext
	)
{
	BOOLEAN Return = FALSE;

	// Check to see if there is a read transfer in progress.
	if ( INTERLOCKED_COMPARE_EXCHANGE( &m_WriteXferInUse, 1, 1 ) )
	{

		// We found the transfer in progress so we can cancel it.
		Return = TRUE;

		// Check for valid context structure
		ASSERT( m_WriteXferContext.m_Signature == KS5933_TRANSFER_CONTEXT_SIGNATURE );

		// Set the bit indicating the transfer has been cancelled.
		m_WriteXferContext.m_State.m_Canceled = TRUE;

		// Check to see if the write is currently enabled.  If so, we will
		// stop the write and setup the DPC to complete it as cancelled.
		// If the write is not currently enabled the transfer either hasn't started,
		// just completed, or we are waiting for the next segment to begin.  In
		// any of these cases, we have nothing additional to do.  If the transfer
		// just started or completed, OnDmaReady will execute soon and
		// will finish the transfer in a cancelled state.  If OnDmaReady is
		// running now, the transfer will be cancelled the next time we attempt to
		// start a segment on the device.  Note that starting a segment
		// requires obtaining the interrupt spin lock, which we currently
		// own, so the next segment can't start in parallel to this routine.
		// Note that from the S5933's perspective, we enable the "read" DMA
		ULONG Temp;
		Temp = m_BusMasterControlStatus;
		if ( Temp & KS59XX_MCSR_READ_XFER_ENABLE )
		{
			Temp &= (KS59XX_MCSR_PRESERVE_MASK & ~KS59XX_MCSR_READ_XFER_ENABLE);
			m_BusMasterControlStatus = Temp;

			// Now that the write is stopped, we must disable the write
			// DMA interrupt and clear it in case the write interrupted,
			// but has not been serviced yet (we don't want to trigger
			// the DPC twice!).
			// Note that from the S5933's perspective, this is a read operation
			Temp = m_InterruptControlStatus;
			Temp &= (KS59XX_INTCSR_SET_CONTROL_MASK &
				~KS59XX_INTCSR_INT_ON_READ_XFER_COMPLETE);
			Temp |= KS59XX_INTCSR_READ_XFER_COMPLETE;
			m_InterruptControlStatus = Temp;

			// Add the number of bytes transferred before the write was stopped
			// to the count for the transfer
			m_WriteXferContext.m_State.m_BytesTransferred +=
				m_WriteXferContext.m_pDescriptor->td_Length
				- static_cast<ULONG>(m_MasterReadTransferCount);

			// We must set a bit in the m_AccumulatedInterrupts and request the
			// interrupt's DPC, just as if the real interrupt had occurred to
			// finish the write operation.
			m_AccumulatedInterrupts |= KS59XX_INTCSR_READ_XFER_COMPLETE;
			m_IsrDpc.Request(NULL, NULL);
		}
	}
	return Return;
	UNREFERENCED_PARAMETER(pTransferContext);
}

template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::ResetFifoStatusFlags(
	KS59XX_DIRECTION direction,
	BOOLEAN Lock
	)
{
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Setup a mask that will be ANDed with the MCSR.  We will
	// preserve the bits we need to.
	FlexContext.m_dwParam1 = KS59XX_MCSR_PRESERVE_MASK;

	// Setup a mask that will be ORed with the masked MCSR.  In this
	// case based on the direction, we will set the appropriate
	// fifo status reset bit.
	if (direction == KS59XX_PCI_TO_ADDON)
		FlexContext.m_dwParam2 = KS59XX_MCSR_PCI_TO_ADDON_FIFO_RESET;
	else
		FlexContext.m_dwParam2 = KS59XX_MCSR_ADDON_TO_PCI_FIFO_RESET;

	// Modify the S5933's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(McsrReadModifyWrite), &FlexContext, Lock );

	return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::SetFifoProperties(
	KS59XX_DIRECTION direction,
	BOOLEAN SetBurstManagementScheme,
	BOOLEAN SetPriority,
	BOOLEAN Lock
	)
{
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Setup a mask that will be ANDed with the MCSR.  We will
	// preserve the bits we need to.
	FlexContext.m_dwParam1 = KS59XX_MCSR_PRESERVE_MASK;

	// Setup a mask that will be ORed with the masked MCSR.  In this
	// case based on the direction and flags passed in, we will set the
	// appropriate fifo control bits.
	FlexContext.m_dwParam2 = 0;
	if (direction == KS59XX_PCI_TO_ADDON)
	{
		if (SetBurstManagementScheme)
			FlexContext.m_dwParam2 |= KS59XX_MCSR_READ_FIFO_SCHEME;
		if (SetPriority)
			FlexContext.m_dwParam2 |= KS59XX_MCSR_READ_VS_WRITE_PRIORITY;
	}
	else
	{
		if (SetBurstManagementScheme)
			FlexContext.m_dwParam2 |= KS59XX_MCSR_WRITE_FIFO_SCHEME;
		if (SetPriority)
			FlexContext.m_dwParam2 |= KS59XX_MCSR_WRITE_VS_READ_PRIORITY;
	}

	// Modify the S5933's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(McsrReadModifyWrite), &FlexContext, Lock );

	return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::SetFifoBusProperties(
	KS59XX_ENDIAN_CONVERSION EndianConversion,
	ULONG PciFifoAdvanceByte,
	ULONG AddonFifoAdvanceByte,
	BOOLEAN Lock
	)
{

	// Validate parameters to the extent possible
	if( (PciFifoAdvanceByte < KS59XX_BYTE_MINIMUM)
		|| (PciFifoAdvanceByte > KS59XX_BYTE_MAXIMUM)
		|| (AddonFifoAdvanceByte < KS59XX_BYTE_MINIMUM)
		|| (AddonFifoAdvanceByte > KS59XX_BYTE_MAXIMUM) )
	{
		return STATUS_INVALID_PARAMETER;
    }

	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Setup a mask that will be ANDed with the INTCSR.  We will
	// preserve the bits we need to.
	FlexContext.m_dwParam1 = KS59XX_INTCSR_SET_CONTROL_MASK;

	// Setup a mask that will be ORed with the masked INTCSR.  In this
	// case, we will set the appropriate bus related fifo control bits.
	switch (EndianConversion)
	{
	case KS59XX_16_BIT_CONVERSION:
		FlexContext.m_dwParam2 = KS59XX_INTCSR_16BIT_ENDIAN_CONVERSION;
		break;

	case KS59XX_32_BIT_CONVERSION:
		FlexContext.m_dwParam2 = KS59XX_INTCSR_32BIT_ENDIAN_CONVERSION;
		break;

	case KS59XX_64_BIT_CONVERSION:
		FlexContext.m_dwParam2 = KS59XX_INTCSR_64BIT_ENDIAN_CONVERSION;
		break;

	case KS59XX_NO_CONVERSION:
	default:
		FlexContext.m_dwParam2 = KS59XX_INTCSR_NO_ENDIAN_CONVERSION;
		break;
	}
	FlexContext.m_dwParam2 |= KS59XX_INTCSR_FIFO_ADVANCE_CONDITION(
		KS59XX_PCI_INTERFACE, PciFifoAdvanceByte );
	FlexContext.m_dwParam2 |= KS59XX_INTCSR_FIFO_ADVANCE_CONDITION(
		KS59XX_ADDON_INTERFACE, AddonFifoAdvanceByte );

	// Modify the S5933's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(IntcsrReadModifyWrite), &FlexContext, Lock );

	return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::ReadOperationRegisters(
	PAMCCS5933_PCI_BUS_OPERATION_REGISTERS pOpRegs5933
	)
{
	// Fill in all of the mailbox registers
	pOpRegs5933->OutgoingMailbox1 = m_OutgoingMailbox1;
	pOpRegs5933->OutgoingMailbox2 = m_OutgoingMailbox2;
	pOpRegs5933->OutgoingMailbox3 = m_OutgoingMailbox3;
	pOpRegs5933->OutgoingMailbox4 = m_OutgoingMailbox4;
	pOpRegs5933->IncomingMailbox1 = m_IncomingMailbox1;
	pOpRegs5933->IncomingMailbox2 = m_IncomingMailbox2;
	pOpRegs5933->IncomingMailbox3 = m_IncomingMailbox3;
	pOpRegs5933->IncomingMailbox4 = m_IncomingMailbox4;

	// Skip the FIFO port, since reading this at the wrong time can hang the bus
    pOpRegs5933->FIFORegisterPort = 0;

	// Fill in all of the FIFO and status registers
    pOpRegs5933->MasterWriteAddress = m_MasterWriteAddress;
    pOpRegs5933->MasterWriteTransferCount = m_MasterWriteTransferCount;
    pOpRegs5933->MasterReadAddress = m_MasterReadAddress;
    pOpRegs5933->MasterReadTransferCount = m_MasterReadTransferCount;
    pOpRegs5933->MailboxEmptyFullStatus = m_MailboxEmptyFullStatus;
    pOpRegs5933->InterruptControlStatus = m_InterruptControlStatus;
    pOpRegs5933->BusMasterControlStatus = m_BusMasterControlStatus;

	return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
NTSTATUS KS5933Pci<TRange, TRegister>::WriteOperationRegisters(
	PAMCCS5933_PCI_BUS_OPERATION_REGISTERS pOpRegs5933
	)
{
	// Fill in all of the mailbox registers
	m_OutgoingMailbox1 = pOpRegs5933->OutgoingMailbox1;
	m_OutgoingMailbox2 = pOpRegs5933->OutgoingMailbox2;
	m_OutgoingMailbox3 = pOpRegs5933->OutgoingMailbox3;
	m_OutgoingMailbox4 = pOpRegs5933->OutgoingMailbox4;

	// Fill in all of the FIFO and status registers
    m_MasterWriteAddress = pOpRegs5933->MasterWriteAddress;
    m_MasterWriteTransferCount = pOpRegs5933->MasterWriteTransferCount;
    m_MasterReadAddress = pOpRegs5933->MasterReadAddress;
    m_MasterReadTransferCount = pOpRegs5933->MasterReadTransferCount;
    m_InterruptControlStatus = pOpRegs5933->InterruptControlStatus;
    m_BusMasterControlStatus = pOpRegs5933->BusMasterControlStatus;

	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Macros

//////////////////////////////////////////////////////////////////////
//
// Macro MEMBER_KS5933_DMA_DONE_HANDLER_ROUTINE
//
// To make a the handler routine for a DMA done callback function
// a member in any class, use the following macro inside the definition
// of the class. If the DMA done callback routine member name
// is X, then the Callback parameter in the DmaRead() or DmaWrite() function
// call must be passed as LinkTo(X). Furthermore, the Context parameter of
// the constructor for K1394AdddressRange must be the "this" pointer of
// the class containing the DMA done handler routine.
//
// The first parameter to the macro is the name of class in which the
// DMA done callback routine member is being declared. The second
// parameter is the name of the DMA done handler routine function.
//
// The routine member declared by this macro has the following prototype:
//
// VOID
// class_name::function_name( KS5933_TRANSFER_STATE state );
//
// NOTE This routine will be called at DISPATCH_LEVEL
//
#define MEMBER_KS5933_DMA_DONE_HANDLER_ROUTINE(objclass, func)	\
	VOID __stdcall func(KS5933_TRANSFER_STATE state);			\
	static VOID __stdcall LinkTo(func)(							\
		PVOID context,											\
		KS5933_TRANSFER_STATE state )							\
	{															\
		((objclass*)context)->func(state);						\
	}

#endif // __KS5933__

#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
