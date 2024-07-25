// KS59XX.h - include file for class KS59XX and related classes
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

#ifndef __KS59XX__
#define __KS59XX__

#define NVRAM_POLLING_RETRIES 5
#define NVRAM_WRITE_RETRIES 5
#define NVRAM_POLLING_TIMEOUT 1
#define NVRAM_SERIAL_RECOVERY_TIMEOUT 10
#define NVRAM_FLASH_RECOVERY_TIMEOUT 20

// Handler routine for mailbox interrupt.  This will be called
// at DIRQL or DISPATCH_LEVEL depending on how it was registered
typedef
VOID
(*PKS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE) (
	PVOID context
    );

#define KS59XX_FLEXIBLE_CONTEXT_SIGNATURE 0xdeed1d00

///////////////////////////////////////////////////////////////////////////////
// Class declaration for KS59XXPci
///////////////////////////////////////////////////////////////////////////////
template <class TRange, class TRegister> class KS59XXPci
{

// Data Types
public:
	///////////////////////////////////////////////////////////////////////////
	// Class specific data types.  These must be part of the template class
	// since they contain a pointer to it, and are not fully specified until
	// the class is instantiated.

	struct _KS59XX_FLEXIBLE_CONTEXT
	{
		ULONG m_Signature;
		KS59XXPci* m_pClass;
		PVOID m_pPtr1;
		ULONG m_dwParam1;
		ULONG m_dwParam2;
		ULONG m_dwParam3;
	};
	typedef struct _KS59XX_FLEXIBLE_CONTEXT KS59XX_FLEXIBLE_CONTEXT,
		*PKS59XX_FLEXIBLE_CONTEXT;

// Construction
public:
	SAFE_DESTRUCTORS

#if ! _WDM_
	KS59XXPci(void);

	KS59XXPci(
		USHORT VendorId,
		USHORT DeviceId,
		KPciSlot* pSlot=NULL
		);

	KS59XXPci(
		ULONG Bus,
		ULONG Device,
		ULONG Function
		);


	NTSTATUS Initialize(
		KDevice* pDevice,
		PWSTR RegistryPath,
		ULONG OperationRegisterBARIndex,
		BOOLEAN NvramIsSerialType = TRUE,
		USHORT NvramPageSize = 1
		);

	NTSTATUS Initialize(
		USHORT VendorId,
		USHORT DeviceId,
		KPciSlot* pSlot,
		KDevice* pDevice,
		PWSTR RegistryPath,
		ULONG OperationRegisterBARIndex,
		BOOLEAN NvramIsSerialType = TRUE,
		USHORT NvramPageSize = 1
		);

#else // ! _WDM_
	KS59XXPci(void);

	NTSTATUS Initialize(
		PDEVICE_OBJECT TopOfStackDevice,
		PCM_RESOURCE_LIST pTranslatedResourceList,
		PCM_RESOURCE_LIST pRawResourceList,
		ULONG OperationRegisterBARIndex,
		BOOLEAN NvramIsSerialType = TRUE,
		USHORT NvramPageSize = 1
		);

#endif // ! _WDM_

	~KS59XXPci(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return m_Initialized; }

protected:
	VOID InitializeS59XX(void);

// Methods
public:
	NTSTATUS AddonReset(
		BOOLEAN Assert,
		BOOLEAN Lock = TRUE
		);

	NTSTATUS ResetMailboxFlags( BOOLEAN Lock = TRUE );

	NTSTATUS SetupMailboxInterrupt(
		KS59XX_DIRECTION direction,
		ULONG MessageMailBox,
		ULONG ByteTrigger,
		PKS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE pCallback,
		PVOID pContext = NULL,
		BOOLEAN CallAtDirql = FALSE,
		BOOLEAN SingleShot = TRUE,
		BOOLEAN Lock = TRUE
		);

	BOOLEAN CancelMailboxInterrupt(
		KS59XX_DIRECTION direction,
		BOOLEAN Lock = TRUE
		);

	VOID SetNvramPollingRetries( ULONG NvramPollingRetries )
		{
			m_NvramPollingRetries = NvramPollingRetries;
		}

	VOID SetNvramWriteRetries( ULONG NvramWriteRetries )
		{
			m_NvramWriteRetries = NvramWriteRetries;
		}

	VOID SetNvramPollTimeout( ULONG NvramPollingTimeout )
		{
			m_NvramPollingTimeout = NvramPollingTimeout;
		}

	VOID SetNvramRecoveryTimeout( ULONG NvramRecoveryTimeout )
		{
			m_NvramRecoveryTimeout = NvramRecoveryTimeout;
		}

	NTSTATUS ReadNvram(
		PUCHAR pBuffer,
		USHORT Offset,
		USHORT Count = 1
		);

	NTSTATUS WriteNvram(
		PUCHAR pBuffer,
		USHORT Offset,
		USHORT Count = 1
		);

	BOOLEAN SynchronizeRegisterAccess(
		PKSYNCHRONIZE_ROUTINE pCallback,
		PVOID pContext = NULL,
		BOOLEAN Lock = TRUE
		);

protected:

	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, GetAccumulatedInterrupts );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, McsrReadModifyWrite );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, IntcsrReadModifyWrite );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, CancelMailboxInterruptOnDevice );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, NvramReadOperation );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, NvramWriteOperationFlash );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, NvramWriteOperationSer1 );
	MEMBER_SYNCHCRITSECTIONWITHCONTEXT( KS59XX_FLEXIBLE_CONTEXT, NvramWriteOperationSer2 );

#ifdef _COMMENT_ONLY

		// The following member functions are actually defined by the
		// a DEVMEMBER_XXx or MEMBER_XXx macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	BOOLEAN GetAccumulatedInterrupts( KS59XX_FLEXIBLE_CONTEXT* pContext );
	BOOLEAN McsrReadModifyWrite( KS59XX_FLEXIBLE_CONTEXT* pContext );
	BOOLEAN IntcsrReadModifyWrite( KS59XX_FLEXIBLE_CONTEXT* pContext );
	BOOLEAN CancelMailboxInterruptOnDevice( KS59XX_FLEXIBLE_CONTEXT* pContext );
	BOOLEAN NvramReadOperation( KS59XX_FLEXIBLE_CONTEXT* pContext );
	BOOLEAN NvramWriteOperationFlash( KS59XX_FLEXIBLE_CONTEXT* pContext );
	BOOLEAN NvramWriteOperationSer1( KS59XX_FLEXIBLE_CONTEXT* pContext );
	BOOLEAN NvramWriteOperationSer2( KS59XX_FLEXIBLE_CONTEXT* pContext );
#endif

// Data
public:
	KSpinLock m_LockRegisterAccess;
	KPciConfiguration m_PciConfig;
	PCM_RESOURCE_LIST m_pResourceList;

	TRange m_OperationRegisterBase;

	// Registers used by base class, that are common to both S5933 and S5920
	// MCSR and RCR are at the same offset
	TRegister m_InterruptControlStatus;		// INTCSR - Interrupt Control/Status Reg
	TRegister m_BusMasterControlStatus;		// MCSR - Bus Master Control/Status Reg		S5933 Definition
	TRegister m_ResetControl;				// RCR - Bus Master Control/Status Reg		S5920 Definition

protected:

	TRegister m_NvramControl;				// NVRAM Control Reg
	TRegister m_NvramData;					// NVRAM Data Reg

#if ! _WDM_

	KUstring m_RegistryPath;
	KDevice* m_pDevice;

#endif // ! _WDM_

	KInterrupt m_Irq;
	BOOLEAN	m_Initialized;
	BOOLEAN m_NoInterrupt;
	KSpinLock m_IrqSurrogate;
	ULONG m_AccumulatedInterrupts;
	PKS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE m_pIncomingMailboxCallback;
	PVOID m_pIncomingMailboxContext;
	BOOLEAN m_IncomingMailboxCallAtDirql;
	BOOLEAN m_IncomingMailboxSingleShot;
	PKS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE m_pOutgoingMailboxCallback;
	PVOID m_pOutgoingMailboxContext;
	BOOLEAN m_OutgoingMailboxCallAtDirql;
	BOOLEAN m_OutgoingMailboxSingleShot;
	KMutex m_NvramMutex;
	BOOLEAN m_NvramIsSerialType;
	USHORT m_NvramPageSize;
	ULONG m_NvramPollingRetries;
	ULONG m_NvramPollingTimeout;
	ULONG m_NvramWriteRetries;
	ULONG m_NvramRecoveryTimeout;
};

//////////////////////////////////////////////////////////////////////////////
// Class Definition for KS59XXPci
//////////////////////////////////////////////////////////////////////////////

#if ! _WDM_

template <class TRange, class TRegister>
KS59XXPci<TRange, TRegister>::KS59XXPci(void) :
	m_PciConfig(),
	m_pResourceList(NULL),
	m_OperationRegisterBase(),
	m_InterruptControlStatus(),
	m_BusMasterControlStatus(),
	m_ResetControl(),
	m_RegistryPath(),
	m_pDevice(NULL),
	m_Irq(),
	m_Initialized(FALSE),
	m_NoInterrupt(TRUE),
	m_IrqSurrogate(),
	m_AccumulatedInterrupts(0),
	m_pIncomingMailboxCallback(NULL),
	m_pIncomingMailboxContext(NULL),
	m_IncomingMailboxCallAtDirql(FALSE),
	m_IncomingMailboxSingleShot(TRUE),
	m_pOutgoingMailboxCallback(NULL),
	m_pOutgoingMailboxContext(NULL),
	m_OutgoingMailboxCallAtDirql(FALSE),
	m_OutgoingMailboxSingleShot(TRUE),
	m_NvramMutex(0xFFFFFFFF),
	m_NvramIsSerialType(TRUE),
	m_NvramPageSize(1),
	m_NvramPollingRetries(NVRAM_POLLING_RETRIES),
	m_NvramWriteRetries(NVRAM_WRITE_RETRIES),
	m_NvramPollingTimeout(NVRAM_POLLING_TIMEOUT),
	m_NvramRecoveryTimeout(static_cast<ULONG>(-1))
{
}

template <class TRange, class TRegister>
KS59XXPci<TRange, TRegister>::KS59XXPci(
	USHORT VendorId,
	USHORT DeviceId,
	KPciSlot* pSlot
	) :
	m_PciConfig(VendorId, DeviceId, pSlot),
	m_pResourceList(NULL),
	m_OperationRegisterBase(),
	m_InterruptControlStatus(),
	m_BusMasterControlStatus(),
	m_ResetControl(),
	m_RegistryPath(),
	m_pDevice(NULL),
	m_Irq(),
	m_Initialized(FALSE),
	m_NoInterrupt(TRUE),
	m_IrqSurrogate(),
	m_AccumulatedInterrupts(0),
	m_pIncomingMailboxCallback(NULL),
	m_pIncomingMailboxContext(NULL),
	m_IncomingMailboxCallAtDirql(FALSE),
	m_IncomingMailboxSingleShot(TRUE),
	m_pOutgoingMailboxCallback(NULL),
	m_pOutgoingMailboxContext(NULL),
	m_OutgoingMailboxCallAtDirql(FALSE),
	m_OutgoingMailboxSingleShot(TRUE),
	m_NvramMutex(0xFFFFFFFF),
	m_NvramIsSerialType(TRUE),
	m_NvramPageSize(1),
	m_NvramPollingRetries(NVRAM_POLLING_RETRIES),
	m_NvramWriteRetries(NVRAM_WRITE_RETRIES),
	m_NvramPollingTimeout(NVRAM_POLLING_TIMEOUT),
	m_NvramRecoveryTimeout(static_cast<ULONG>(-1))
{
}

template <class TRange, class TRegister>
KS59XXPci<TRange, TRegister>::KS59XXPci(
	ULONG Bus,
	ULONG Device,
	ULONG Function
	) :
	m_PciConfig(Bus, Device, Function),
	m_pResourceList(NULL),
	m_OperationRegisterBase(),
	m_InterruptControlStatus(),
	m_BusMasterControlStatus(),
	m_ResetControl(),
	m_RegistryPath(),
	m_pDevice(NULL),
	m_Irq(),
	m_Initialized(FALSE),
	m_NoInterrupt(TRUE),
	m_IrqSurrogate(),
	m_AccumulatedInterrupts(0),
	m_pIncomingMailboxCallback(NULL),
	m_pIncomingMailboxContext(NULL),
	m_IncomingMailboxCallAtDirql(FALSE),
	m_IncomingMailboxSingleShot(TRUE),
	m_pOutgoingMailboxCallback(NULL),
	m_pOutgoingMailboxContext(NULL),
	m_OutgoingMailboxCallAtDirql(FALSE),
	m_OutgoingMailboxSingleShot(TRUE),
	m_NvramMutex(0xFFFFFFFF),
	m_NvramIsSerialType(TRUE),
	m_NvramPageSize(1),
	m_NvramPollingRetries(NVRAM_POLLING_RETRIES),
	m_NvramWriteRetries(NVRAM_WRITE_RETRIES),
	m_NvramPollingTimeout(NVRAM_POLLING_TIMEOUT),
	m_NvramRecoveryTimeout(static_cast<ULONG>(-1))
{
}

// Note this method is usually only called implicitly when the containing device is
// destroyed by the destructor.
template <class TRange, class TRegister>
VOID KS59XXPci<TRange, TRegister>::Invalidate()
{
	// If we were initialized, clean up
	if (m_Initialized)
	{
		KS59XX_FLEXIBLE_CONTEXT FlexContext;
		RtlZeroMemory(&FlexContext, sizeof(FlexContext));
		FlexContext.m_pClass = this;

		// Initialize the InterruptControlStatus register to a known state with
		// no interrupt sources enabled.
		FlexContext.m_dwParam1 = KS59XX_INTCSR_INIT_MASK;
		FlexContext.m_dwParam2 = 0;

		// Modify the S59XX's registers in a synchronized manner
		SynchronizeRegisterAccess( LinkTo(IntcsrReadModifyWrite), &FlexContext );

		KResourceRequest theRequest(
			PCIBus,
			m_PciConfig.Bus(),
			m_PciConfig.Slot() );

		// Release all resources allocated by the containing device.
		// This class takes this responsibilty since it called Submit() to
		// claim the resources.
		theRequest.Release(m_pDevice, m_RegistryPath);

		// We must free any resources we are holding and disconnect our interrupt.
		// Note it is safe to call Invalidate() for the memory range and interrupt
		// (which causes a disconnect) even if we didn't initialize them.
		m_OperationRegisterBase.Invalidate();
		m_Irq.Invalidate();
		m_RegistryPath.Invalidate();

		// Free the memory we allocated to hold m_pResourceList
		delete m_pResourceList;
		m_pResourceList = NULL;
	}
	m_Initialized = FALSE;
}

template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::Initialize(
	USHORT VendorId,
	USHORT DeviceId,
	KPciSlot* pSlot,
	KDevice* pDevice,
	PWSTR RegistryPath,
	ULONG OperationRegisterBARIndex,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize
	)
{
	m_PciConfig.Initialize(VendorId, DeviceId, pSlot);

	return Initialize(
		pDevice,
		RegistryPath,
		OperationRegisterBARIndex,
		NvramIsSerialType,
		NvramPageSize
		);
}

template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::Initialize(
	KDevice* pDevice,
	PWSTR RegistryPath,
	ULONG OperationRegisterBARIndex,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ( !m_PciConfig.IsValid() )
		return STATUS_DEVICE_DOES_NOT_EXIST;

	m_RegistryPath.Initialize(RegistryPath, PagedPool);
	if (!m_RegistryPath.IsValid())
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	ULONG BusNumber = m_PciConfig.Bus();
	KResourceRequest theRequest(
		PCIBus,
		BusNumber,
		m_PciConfig.Slot() );

	status = theRequest.Submit(pDevice, m_RegistryPath);
	if ( !NT_SUCCESS(status) )
	{
		m_RegistryPath.Invalidate();
		return status;
	}

	// Allocate memory to store the PCM_RESOURCE_LIST and copy from
	// the theRequest. NOTE since this is allocated from PagedPool, it can only
	// be accessed at PASSIVE_LEVEL.
	ULONG ConfigSize = theRequest.AssignmentsSize();
	PUCHAR pTemp = new(PagedPool) UCHAR[ConfigSize];
	if ( pTemp == NULL )
	{
		// Clean up and return error
		m_RegistryPath.Invalidate();
		theRequest.Release();
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	m_pResourceList = reinterpret_cast<PCM_RESOURCE_LIST>(pTemp);
	RtlCopyMemory( m_pResourceList,	theRequest.Assignments(), ConfigSize );

	// Store the device away for use when we release the resources we're about to claim
	m_pDevice = pDevice;

	// Now initialize memory or port resource at the BAR index specified for our
	// configuration register space.  If we don't have a valid configuration space
	// return with an error.
	ULONGLONG BaseAddress;
	BaseAddress = m_PciConfig.ReadBaseAddress(OperationRegisterBARIndex);
	ULONG ConfigLength = 0;
	if ( IS_MEMORY_BAR(BaseAddress) )
	{
		KResourceAssignment theIoAssignment(
			&theRequest,
			CmResourceTypeMemory,
			OperationRegisterBARIndex );
		if ( !theIoAssignment.Status() )
		{
			// Clean up and return error
			delete m_pResourceList;
			m_pResourceList = NULL;
			theRequest.Release();
			m_RegistryPath.Invalidate();
			return STATUS_INVALID_PARAMETER;
		}

		ConfigLength = theIoAssignment.Length();

		// Mask the base address for proper initialization
		BaseAddress &= PCI_BAR_MEMORY_ADDRESS_MASK;
	}

	else
	{
		KResourceAssignment theIoAssignment(
			&theRequest,
			CmResourceTypePort,
			OperationRegisterBARIndex );
		if ( !theIoAssignment.Status() )
		{
			// Clean up and return error
			delete m_pResourceList;
			m_pResourceList = NULL;
			theRequest.Release();
			m_RegistryPath.Invalidate();
			return STATUS_INVALID_PARAMETER;
		}

		ConfigLength = theIoAssignment.Length();

		// Mask the base address for proper initialization
		BaseAddress &= PCI_BAR_IO_ADDRESS_MASK;
	}

	ASSERT( ConfigLength >= AMCCS5933_PCI_BUS_OPERATION_REGISTERS_SIZE );

	status = m_OperationRegisterBase.Initialize(
		PCIBus,
		BusNumber,
		BaseAddress,
		ConfigLength,
		TRUE );
	if ( !NT_SUCCESS(status) )
	{
		// Clean up and return error
		delete m_pResourceList;
		m_pResourceList = NULL;
		theRequest.Release();
		m_RegistryPath.Invalidate();
		return status;
	}

	// Next check to see if an interrupt exists in our resource list.  If not
	// we must disable everything that relies on interrupts, and instead use
	// a spin lock to synchronize access to atomic S59XX resources.
	// This makes the S59XX pretty boring, but I guess it may still have a use.
	KResourceAssignment theIrqAssignment(
		&theRequest,
		CmResourceTypeInterrupt );
	if ( theIrqAssignment.Status() )
	{
		// Initialize our interrupt
		status = m_Irq.Initialize( theRequest.Assignments() );
		if (!NT_SUCCESS(status))
		{
			// Clean up and return error
			m_OperationRegisterBase.Invalidate();
			delete m_pResourceList;
			m_pResourceList = NULL;
			theRequest.Release();
			m_RegistryPath.Invalidate();
			return status;
		}

		// We have an interrupt so clear the member flag indicating we don't
		m_NoInterrupt = FALSE;

	}

	// Setup the Nvram type and page size
	m_NvramIsSerialType = NvramIsSerialType;
	m_NvramPageSize = NvramPageSize;

	// If the user hasn't already set the recovery timeout, use the defaults
	if (m_NvramRecoveryTimeout == static_cast<ULONG>(-1))
	{
		m_NvramRecoveryTimeout = (m_NvramIsSerialType) ? NVRAM_SERIAL_RECOVERY_TIMEOUT :
			NVRAM_FLASH_RECOVERY_TIMEOUT;
	}

	// Do the initialization tasks common to NT4 and WDM
	InitializeS59XX();

	// Set the initialized flag.
	m_Initialized = TRUE;

	return status;
}

#else // ! _WDM_

template <class TRange, class TRegister>
KS59XXPci<TRange, TRegister>::KS59XXPci() :
	m_PciConfig(NULL),
	m_pResourceList(NULL),
	m_OperationRegisterBase(),
	m_InterruptControlStatus(),
	m_BusMasterControlStatus(),
	m_ResetControl(),
	m_Irq(),
	m_Initialized(FALSE),
	m_NoInterrupt(TRUE),
	m_IrqSurrogate(),
	m_AccumulatedInterrupts(0),
	m_pIncomingMailboxCallback(NULL),
	m_pIncomingMailboxContext(NULL),
	m_IncomingMailboxCallAtDirql(FALSE),
	m_IncomingMailboxSingleShot(TRUE),
	m_pOutgoingMailboxCallback(NULL),
	m_pOutgoingMailboxContext(NULL),
	m_OutgoingMailboxCallAtDirql(FALSE),
	m_OutgoingMailboxSingleShot(TRUE),
	m_NvramMutex(0xFFFFFFFF),
	m_NvramIsSerialType(TRUE),
	m_NvramPageSize(1),
	m_NvramPollingRetries(NVRAM_POLLING_RETRIES),
	m_NvramWriteRetries(NVRAM_WRITE_RETRIES),
	m_NvramPollingTimeout(NVRAM_POLLING_TIMEOUT),
	m_NvramRecoveryTimeout(static_cast<ULONG>(-1))
{
}

// Note this method may be called explicitly when the device is stopped.  The device
// may be restarted by calling Initialize() again.  This destructor also calls
// this method when the containing device is destroyed.
template <class TRange, class TRegister>
VOID KS59XXPci<TRange, TRegister>::Invalidate(void)
{
	// If we were initialized, clean up
	if (m_Initialized)
	{
		KS59XX_FLEXIBLE_CONTEXT FlexContext;
		RtlZeroMemory(&FlexContext, sizeof(FlexContext));
		FlexContext.m_pClass = this;

		// Initialize the InterruptControlStatus register to a known state with
		// no interrupt sources enabled.
		FlexContext.m_dwParam1 = KS59XX_INTCSR_INIT_MASK;
		FlexContext.m_dwParam2 = 0;

		// Modify the S59XX's registers in a synchronized manner
		SynchronizeRegisterAccess( LinkTo(IntcsrReadModifyWrite), &FlexContext );

		// We must free any resources we are holding and disconnect our interrupt.
		// Note it is safe to call Invalidate() for the memory range and interrupt
		// (which causes a disconnect) even if we didn't initialize them.
		m_OperationRegisterBase.Invalidate();
		m_Irq.Invalidate();
	}
	m_Initialized = FALSE;
}

template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::Initialize(
	PDEVICE_OBJECT TopOfStackDevice,
	PCM_RESOURCE_LIST pTranslatedResourceList,
	PCM_RESOURCE_LIST pRawResourceList,
	ULONG OperationRegisterBARIndex,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	// Initialize our Pci configuration data member. This form of
	// initialize doesn't fail so no need to check status.
	m_PciConfig.Initialize(TopOfStackDevice);

	// Convert the BAR index passed in to an ordinal using our PciConfig
	// data member and then initialize the memory or port resource at that
	// ordinal index specified for our configuration register space.  If we don't
	// have a valid configuration space return with an error.
	status = m_OperationRegisterBase.Initialize(pTranslatedResourceList,
		pRawResourceList,
		m_PciConfig.BaseAddressIndexToOrdinal(OperationRegisterBARIndex));
	if ( !NT_SUCCESS(status) )
		return status;

	// Next check to see if an interrupt exists in our resource list.  If not
	// we must disable everything that relies on interrupts, and instead use
	// a spin lock to synchronize access to atomic S59XX resources.
	// This makes the S59XX pretty boring, but I guess it may still have a use.
	KResourceAssignment theIrqAssignment(
		pTranslatedResourceList,
		CmResourceTypeInterrupt );
	if ( theIrqAssignment.Status() )
	{
		// Initialize our interrupt
		status = m_Irq.Initialize( pTranslatedResourceList );
		if (!NT_SUCCESS(status))
		{
			// Clean up and return error
			m_OperationRegisterBase.Invalidate();
			return status;
		}

		// We have an interrupt so clear the member flag indicating we don't
		m_NoInterrupt = FALSE;
	}

	// Setup the Nvram type and page size
	m_NvramIsSerialType = NvramIsSerialType;
	m_NvramPageSize = NvramPageSize;

	// If the user hasn't already set the recovery timeout, use the defaults
	if (m_NvramRecoveryTimeout == static_cast<ULONG>(-1))
	{
		m_NvramRecoveryTimeout = (m_NvramIsSerialType) ? NVRAM_SERIAL_RECOVERY_TIMEOUT :
			NVRAM_FLASH_RECOVERY_TIMEOUT;
	}

	// Do the initialization tasks common to NT4 and WDM
	InitializeS59XX();

	// Set the initialized flag.
	m_Initialized = TRUE;

	return status;
}



#endif // ! _WDM_

template <class TRange, class TRegister>
VOID KS59XXPci<TRange, TRegister>::InitializeS59XX(void)
{
	m_InterruptControlStatus =
		m_OperationRegisterBase[OP_REG_OFFSET(InterruptControlStatus)];
	m_BusMasterControlStatus =
		m_OperationRegisterBase[OP_REG_OFFSET(BusMasterControlStatus)];
	m_ResetControl =
		m_OperationRegisterBase[OP_REG_OFFSET(BusMasterControlStatus)];
	m_NvramControl =
		m_OperationRegisterBase[OP_REG_OFFSET(BusMasterControlStatus) + 3];
	m_NvramData =
		m_OperationRegisterBase[OP_REG_OFFSET(BusMasterControlStatus) + 2];

	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Initialize the InterruptControlStatus register to a known state with
	// no interrupt sources enabled.
	FlexContext.m_dwParam1 = KS59XX_INTCSR_INIT_MASK;
	FlexContext.m_dwParam2 = 0;

	// Modify the S59XX's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(IntcsrReadModifyWrite), &FlexContext );
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::SynchronizeRegisterAccess(
	PKSYNCHRONIZE_ROUTINE pCallback,
	PVOID pContext,
	BOOLEAN Lock
	)
{
	BOOLEAN Return;

	if ( !Lock )
	{
		Return = (*pCallback)(pContext);
	}
	else if ( m_NoInterrupt )
	{
		m_IrqSurrogate.Lock();
		Return = (*pCallback)(pContext);
		m_IrqSurrogate.Unlock();
	}
	else
	{
		Return = m_Irq.Synchronize( *pCallback, pContext );
	}

	return Return;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::GetAccumulatedInterrupts(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{
	pContext->m_dwParam1 = m_AccumulatedInterrupts;
	m_AccumulatedInterrupts = 0;
	return TRUE;
}

template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::AddonReset( BOOLEAN Assert, BOOLEAN Lock )
{
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Setup a mask that will be ANDed with the MCSR.  We will
	// preserve the bits we need to and clear the addon reset
	// bit.
	FlexContext.m_dwParam1 = KS59XX_MCSR_PRESERVE_MASK &
		~KS59XX_MCSR_ADDON_PIN_RESET;

	// Setup a mask that will be ORed with the masked MCSR.  We do
	// this based on the Assert flag.  If the flag is TRUE, we will
	// set the addon reset bit, otherwise we won't.
	if (Assert)
		FlexContext.m_dwParam2 = KS59XX_MCSR_ADDON_PIN_RESET;
	else
		FlexContext.m_dwParam2 = 0;

	// Modify the S59XX's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(McsrReadModifyWrite), &FlexContext, Lock );

	return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::ResetMailboxFlags( BOOLEAN Lock )
{
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Setup a mask that will be ANDed with the MCSR.  We will
	// preserve the bits we need to.
	FlexContext.m_dwParam1 = KS59XX_MCSR_PRESERVE_MASK;

	// Setup a mask that will be ORed with the masked MCSR.  In this
	// case we will set the mailbox flag reset bit.
	FlexContext.m_dwParam2 = KS59XX_MCSR_MAILBOX_FLAG_RESET;

	// Modify the S59XX's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(McsrReadModifyWrite), &FlexContext, Lock );

	return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::McsrReadModifyWrite(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{
    ULONG Temp = 0;

	// Read the MCSR and AND with the first parameter from the passed in context
	// structure.
    Temp = m_BusMasterControlStatus;
	Temp &= pContext->m_dwParam1;

	// OR the result with the second parameter passed in the context structure,
    // and then write back the value to the register.
	Temp |= pContext->m_dwParam2;
	m_BusMasterControlStatus = Temp;

	return TRUE;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::IntcsrReadModifyWrite(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{
    ULONG Temp = 0;

	// Read the INTCSR and AND with the first parameter from the passed in context
	// structure.
    Temp = m_InterruptControlStatus;
	Temp &= pContext->m_dwParam1;

	// OR the result with the second parameter passed in the context structure,
    // and then write back the value to the register.
	Temp |= pContext->m_dwParam2;
	m_InterruptControlStatus = Temp;

	return TRUE;
}

template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::SetupMailboxInterrupt(
	KS59XX_DIRECTION direction,
	ULONG MessageMailBox,
	ULONG ByteTrigger,
	PKS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE pCallback,
	PVOID pContext,
	BOOLEAN CallAtDirql,
	BOOLEAN SingleShot,
	BOOLEAN Lock
	)
{
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	// Validate parameters to the extent possible
	if( (MessageMailBox < KS59XX_MAILBOX_MINIMUM)
		|| (MessageMailBox > KS59XX_MAILBOX_MAXIMUM)
		|| (ByteTrigger < KS59XX_BYTE_MINIMUM)
		|| (ByteTrigger > KS59XX_BYTE_MAXIMUM) )
	{
		return STATUS_INVALID_PARAMETER;
    }

	// Based on direction setup bitfields in the context struct
	if (direction == KS59XX_PCI_TO_ADDON)
	{
		// Setup the data members that will be used in the callback
		m_pOutgoingMailboxCallback = pCallback;
		m_pOutgoingMailboxContext = pContext;
		m_OutgoingMailboxCallAtDirql = CallAtDirql;
		m_OutgoingMailboxSingleShot = SingleShot;

		// Setup a mask to be used on the INTCSR.  This mask preserves
		// most bits and clears those that will be used to set mailbox number
		// and byte index for the interrupt
		FlexContext.m_dwParam1 = KS59XX_INTCSR_OUTGOING_MAILBOX_MASK;

		// Setup the bit to the enable the interrupt
		FlexContext.m_dwParam2 = KS59XX_INTCSR_INT_ON_OUTGOING_MAILBOX;
	}
	else
	{
		m_pIncomingMailboxCallback = pCallback;
		m_pIncomingMailboxContext = pContext;
		m_IncomingMailboxCallAtDirql = CallAtDirql;
		m_IncomingMailboxSingleShot = SingleShot;
		FlexContext.m_dwParam1 = KS59XX_INTCSR_INCOMING_MAILBOX_MASK;

		FlexContext.m_dwParam2 = KS59XX_INTCSR_INT_ON_INCOMING_MAILBOX;
	}

	// OR in the bits for the appropriate mailbox and byte trigger
	FlexContext.m_dwParam2 |= KS59XX_INTCSR_MAILBOX_INT_CONDITION(direction,
		MessageMailBox,
		ByteTrigger);

	// Modify the S59XX's registers in a synchronized manner
	SynchronizeRegisterAccess( LinkTo(IntcsrReadModifyWrite), &FlexContext,
		Lock );

    return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::CancelMailboxInterrupt(
	KS59XX_DIRECTION direction,
	BOOLEAN Lock
	)
{
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	if (direction == KS59XX_PCI_TO_ADDON)
	{
		// Setup the first parameter in the context structure to correspond
		// to the enable bit
		FlexContext.m_dwParam1 = KS59XX_INTCSR_INT_ON_OUTGOING_MAILBOX;

		// Setup the first parameter in the context structure to correspond
		// to the interrupt condition
		FlexContext.m_dwParam2 = KS59XX_INTCSR_OUTGOING_MAILBOX;
	}
	else
	{
		FlexContext.m_dwParam1 = KS59XX_INTCSR_INT_ON_INCOMING_MAILBOX;
		FlexContext.m_dwParam2 = KS59XX_INTCSR_INCOMING_MAILBOX;
	}

	// Modify the S59XX's registers in a synchronized manner
	return SynchronizeRegisterAccess( LinkTo(CancelMailboxInterruptOnDevice), &FlexContext,
		Lock );
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::CancelMailboxInterruptOnDevice(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{
    ULONG Temp = 0;

	// Read the INTCSR
    Temp = m_InterruptControlStatus;

	// If the bit corresponding to the interrupt enable is set, clear it, clear the
	// status bit by setting it (in case the interrupt just occurred) and return
	// TRUE, otherwise return FALSE
	if ( Temp & pContext->m_dwParam1 )
	{

		Temp &= (KS59XX_INTCSR_SET_CONTROL_MASK & ~pContext->m_dwParam1);
		Temp |= pContext->m_dwParam2;
		m_InterruptControlStatus = Temp;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Passive level call
template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::ReadNvram(PUCHAR pBuffer, USHORT Offset, USHORT Count)
{

	// Acquire the NVRAM mutex to prevent any other NVRAM access
	m_NvramMutex.Wait();

	// The device should not be busy.  If it is, an error has occurred and
	// we can't perform the read.  Note it is safe to read the MCSR since
	// we are not modifying it.
	if (static_cast<ULONG>(m_BusMasterControlStatus) & KS59XX_MCSR_NVRAM_BUSY)
	{
		m_NvramMutex.Release();
		return STATUS_IO_DEVICE_ERROR;
	}

	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;
	ULONG	dwPoll;
	NTSTATUS status = STATUS_SUCCESS;

    // Loop over the count and perform the reads from the Nvram to the buffer
	for (; Count; Count--)
	{

		// Set lower NVRAM address byte
		FlexContext.m_dwParam1 = Offset;

		// Set upper NVRAM address byte
		FlexContext.m_dwParam2 = (Offset++ >> 8);

		SynchronizeRegisterAccess( LinkTo(NvramReadOperation), &FlexContext );

		// Now we must poll the MCSR and wait for it to be not busy.  Reading
		// this register does not require synchronization since we don't write
		// the value back.
		ULONG Retries = m_NvramPollingRetries;
		BOOLEAN Ready = FALSE;
		while ((dwPoll = m_BusMasterControlStatus) & KS59XX_MCSR_NVRAM_BUSY
			&& Retries--)
		{
			LARGE_INTEGER Pause;
			Pause.QuadPart = -10000 * static_cast<LONGLONG>(m_NvramPollingTimeout);
			KeDelayExecutionThread(KernelMode, TRUE, &Pause);
		}

		Ready = (dwPoll & KS59XX_MCSR_NVRAM_BUSY) == 0;

		if (Ready)
		{

			// The last value polled has the correct data from the read.
			*pBuffer++ = static_cast<UCHAR>(dwPoll >> 16);
		}
		else
		{
			status = STATUS_IO_DEVICE_ERROR;
			break;
		}
	}

	// Release the NVRAM mutex
	m_NvramMutex.Release();

	return status;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::NvramReadOperation(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{
    ULONG Temp = 0;

	// Read the MCSR and preserve the appropriate bits in the register,
	// clearing others.
    Temp = m_NvramControl;
	Temp &= KS59XX_MCSR_NVRAM_PRESERVE_MASK;

	// Setup for low address latch
	Temp |= KS59XX_MCSR_NVRAM_LOAD_LOW_ADDR;
	m_NvramControl = Temp;

	// Write low address value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam1);

	// Latch low address and setup for high address latch
	// (Clear last command first)
	Temp &= ~KS59XX_MCSR_NVRAM_LOAD_LOW_ADDR;
	Temp |= KS59XX_MCSR_NVRAM_LOAD_HIGH_ADDR;
	m_NvramControl = Temp;

	// Write high address value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam2);

	// Latch high address and begin read operation
	// (Clear last command first)
	Temp &= ~KS59XX_MCSR_NVRAM_LOAD_HIGH_ADDR;
	Temp |= KS59XX_MCSR_NVRAM_BEGIN_READ;
	m_NvramControl = Temp;

	return TRUE;
}

// Passive level call
template <class TRange, class TRegister>
NTSTATUS KS59XXPci<TRange, TRegister>::WriteNvram(PUCHAR pBuffer, USHORT Offset, USHORT Count)
{

	// Acquire the NVRAM mutex to prevent any other NVRAM access
	m_NvramMutex.Wait();

	// The device should not be busy.  If it is, an error has occurred and
	// we can't perform the write.  Note it is safe to read the MCSR since
	// we are not modifying it.
	if (static_cast<ULONG>(m_BusMasterControlStatus) & KS59XX_MCSR_NVRAM_BUSY)
	{
		m_NvramMutex.Release();
		return STATUS_IO_DEVICE_ERROR;
	}

	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;
	ULONG	dwPoll;
	NTSTATUS status = STATUS_SUCCESS;

	if (m_NvramIsSerialType)
	{

		// Loop over the count and perform the writes from the Nvram to the buffer
		for (; Count; Count--)
		{

			ULONG ByteRetries = m_NvramWriteRetries;
			BOOLEAN ByteCorrupt = FALSE;
			UCHAR CheckByte;
			do
			{

				// Set lower NVRAM address byte
				FlexContext.m_dwParam1 = Offset;

				// Set upper NVRAM address byte
				FlexContext.m_dwParam2 = (Offset >> 8);

				// Set NVRAM data byte
				FlexContext.m_dwParam3 = *pBuffer;

				// Modify the S59XX's registers in a synchronized manner
				SynchronizeRegisterAccess( LinkTo(NvramWriteOperationSer1), &FlexContext );

				// Now we must poll the MCSR and wait for it to be not busy.  Reading
				// this register does not require synchronization since we don't write
				// the value back.
				ULONG Retries = m_NvramPollingRetries;
				BOOLEAN Ready = FALSE;
				while ((dwPoll = m_BusMasterControlStatus) & KS59XX_MCSR_NVRAM_BUSY
					&& Retries--)
				{
					LARGE_INTEGER Pause;
					Pause.QuadPart = -10000 * static_cast<LONGLONG>(m_NvramPollingTimeout);
					KeDelayExecutionThread(KernelMode, TRUE, &Pause);
				}

				Ready = (dwPoll & KS59XX_MCSR_NVRAM_BUSY) == 0;

				if (!Ready)
				{
					status = STATUS_IO_DEVICE_ERROR;
					break;
				}

				// Modify the S59XX's registers in a synchronized manner
				SynchronizeRegisterAccess( LinkTo(NvramWriteOperationSer2), &FlexContext );

				// Now we must poll the MCSR and wait for it to be not busy.  Reading
				// this register does not require synchronization since we don't write
				// the value back.
				Retries = m_NvramPollingRetries;
				while ((dwPoll = m_BusMasterControlStatus) & KS59XX_MCSR_NVRAM_BUSY
					&& Retries--)
				{
					LARGE_INTEGER Pause;
					Pause.QuadPart = -10000 * static_cast<LONGLONG>(m_NvramPollingTimeout);
					KeDelayExecutionThread(KernelMode, TRUE, &Pause);
				}

				Ready = (dwPoll & KS59XX_MCSR_NVRAM_BUSY) == 0;

				if (!Ready)
				{
					status = STATUS_IO_DEVICE_ERROR;
					break;
				}

				// Delay to allow recovery time that the device seems to need beyond the polling interval.
				LARGE_INTEGER Pause;
				Pause.QuadPart = -10000 * static_cast<LONGLONG>(m_NvramRecoveryTimeout);
				KeDelayExecutionThread(KernelMode, TRUE, &Pause);

				// Read the data back from the NVRAM
				status = ReadNvram(&CheckByte, Offset);
				if ( !NT_SUCCESS(status) )
				{
					break;
				}

				// Test it against what was written
				ByteCorrupt = *pBuffer != CheckByte;
			} while ( ByteCorrupt && ByteRetries-- );

			// See if we must break out of the outer loop
			if ( !NT_SUCCESS(status) )
			{
				break;
			}

			if (ByteCorrupt)
			{
				status = STATUS_IO_DEVICE_ERROR;
				break;
			}

			// Update counters for next byte
			Offset++;
			pBuffer++;
		}
	}
	else
	{
		// Flash memory must be accessed only in page chunks for write operation.

		USHORT PageStartOffset;
		USHORT PageIndex;
		USHORT CountAdd;

		PUCHAR pPageBuffer = new(NonPagedPool)UCHAR[m_NvramPageSize];
		if (pPageBuffer == NULL)
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		PUCHAR pCheckBuffer = new(NonPagedPool)UCHAR[m_NvramPageSize];
		if (pCheckBuffer == NULL)
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		while ( Count > 0 )
		{
			PageStartOffset = static_cast<USHORT>(Offset % m_NvramPageSize);

			// Check if this is a partial page access.  The first and last pages
			// accesses may fit this criteria.
			if (PageStartOffset || (PageStartOffset + Count < m_NvramPageSize))
			{

				// This is a partial access, we must read the page into the page buffer
				// and then copy our new data over it at the appropriate offset.
				Offset = static_cast<USHORT>(Offset - PageStartOffset);
				status = ReadNvram(pPageBuffer, Offset, m_NvramPageSize);
				if ( !NT_SUCCESS(status) )
				{
					break;
				}
				CountAdd = (Count < m_NvramPageSize - PageStartOffset) ?
					Count : static_cast<USHORT>(m_NvramPageSize - PageStartOffset);
				RtlCopyMemory( (pPageBuffer + PageStartOffset), pBuffer, CountAdd );
				Count = static_cast<USHORT>(Count - CountAdd);
				pBuffer += m_NvramPageSize - PageStartOffset;
			}
			else
			{

				// This is a full page access, simply copy our data to the page buffer.
				RtlCopyMemory( pPageBuffer, pBuffer, m_NvramPageSize );
				Count = static_cast<USHORT>(Count - m_NvramPageSize);
				pBuffer += m_NvramPageSize;
			}

			ULONG PageRetries = m_NvramWriteRetries;
			BOOLEAN PageCorrupt = FALSE;

			// Page accesses need to be done all at once, otherwise the page ends up corrupted.
			// We could do the entire page at high IRQL, but that would take too long.  Instead
			// write each byte individually and check to see that the page was correctly written.
			// If not retry page write until retries exhanusted.
			do
			{
				// Loop over the count and perform the writes from the Nvram to the buffer
				for (PageIndex = 0; PageIndex < m_NvramPageSize; PageIndex++)
				{
					// Set lower NVRAM address byte
					FlexContext.m_dwParam1 = Offset;

					// Set upper NVRAM address byte
					FlexContext.m_dwParam2 = (Offset++ >> 8);

					// Set NVRAM data byte
					FlexContext.m_dwParam3 = pPageBuffer[PageIndex];

					// Modify the S59XX's registers in a synchronized manner
					SynchronizeRegisterAccess( LinkTo(NvramWriteOperationFlash), &FlexContext );

					// Now we must poll the MCSR and wait for it to be not busy.  Reading
					// this register does not require synchronization since we don't write
					// the value back.
					ULONG Retries = m_NvramPollingRetries;
					BOOLEAN Ready = FALSE;
					while ((dwPoll = m_BusMasterControlStatus) & KS59XX_MCSR_NVRAM_BUSY
						&& Retries--)
					{
						LARGE_INTEGER Pause;
						Pause.QuadPart = -10000 * static_cast<LONGLONG>(m_NvramPollingTimeout);
						KeDelayExecutionThread(KernelMode, TRUE, &Pause);
					}

					Ready = (dwPoll & KS59XX_MCSR_NVRAM_BUSY) == 0;

					if (!Ready)
					{
						status = STATUS_IO_DEVICE_ERROR;
						break;
					}
				}

				// See if we must break out of the outer loop
				if ( !NT_SUCCESS(status) )
				{
					break;
				}

				// Delay for a recovery time that the device seems to need between page writes.
				LARGE_INTEGER Pause;
				Pause.QuadPart = -10000 * static_cast<LONGLONG>(m_NvramRecoveryTimeout);
				KeDelayExecutionThread(KernelMode, TRUE, &Pause);

				// Read the data back from the NVRAM
				status = ReadNvram(pCheckBuffer, static_cast<USHORT>(Offset - m_NvramPageSize), m_NvramPageSize);
				if ( !NT_SUCCESS(status) )
				{
					break;
				}

				// Test it against what was written
				PageCorrupt = FALSE;
				for (PageIndex = 0; PageIndex < m_NvramPageSize; PageIndex++)
				{
					if (pPageBuffer[PageIndex] != pCheckBuffer[PageIndex])
					{
						PageCorrupt = TRUE;
						Offset = static_cast<USHORT>(Offset - m_NvramPageSize);
						break;
					}
				}
			} while ( PageCorrupt && PageRetries-- );

			// See if we must break out of the outer loop
			if ( !NT_SUCCESS(status) )
			{
				break;
			}

			if (PageCorrupt)
			{
				status = STATUS_IO_DEVICE_ERROR;
				break;
			}
		}
		delete pPageBuffer;
		delete pCheckBuffer;
	}

	// Release the NVRAM mutex
	m_NvramMutex.Release();

	return status;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::NvramWriteOperationFlash(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{
	UCHAR Temp = 0;

	// Read the MCSR and preserve the appropriate bits in the register,
	// clearing others.
    Temp = m_NvramControl;
	Temp &= KS59XX_MCSR_NVRAM_PRESERVE_MASK;

	// Setup for low address latch
	Temp |= KS59XX_MCSR_NVRAM_LOAD_LOW_ADDR;
	m_NvramControl = Temp;

	// Write low address value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam1);

	// Latch low address and setup for high address latch
	// (Clear last command first)
	Temp &= ~KS59XX_MCSR_NVRAM_LOAD_LOW_ADDR;
	Temp |= KS59XX_MCSR_NVRAM_LOAD_HIGH_ADDR;
	m_NvramControl = Temp;

	// Write high address value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam2);

	// Latch high address
	// (Clear last command first)
	Temp &= ~KS59XX_MCSR_NVRAM_LOAD_HIGH_ADDR;
	Temp |= KS59XX_MCSR_NVRAM_INACTIVE;
	m_NvramControl = Temp;

	// Write data value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam3);

	// Latch data and begin write operation
	// (Clear last command first)
	Temp &= ~KS59XX_MCSR_NVRAM_INACTIVE;
	Temp |= KS59XX_MCSR_NVRAM_BEGIN_WRITE;
	m_NvramControl = Temp;

	return TRUE;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::NvramWriteOperationSer1(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{

	UCHAR Temp = 0;

	// Read the MCSR and preserve the appropriate bits in the register,
	// clearing others.
    Temp = m_NvramControl;
	Temp &= KS59XX_MCSR_NVRAM_PRESERVE_MASK;

	// Setup for low address latch
	Temp |= KS59XX_MCSR_NVRAM_LOAD_LOW_ADDR;
	m_NvramControl = Temp;

	// Write low address value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam1);

	// Latch low address and setup for high address latch
	// (Clear last command first)
	Temp &= ~KS59XX_MCSR_NVRAM_LOAD_LOW_ADDR;
	Temp |= KS59XX_MCSR_NVRAM_LOAD_HIGH_ADDR;
	m_NvramControl = Temp;

	// Write high address value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam2);

	// Latch high address
	// (Clear last command first)
	Temp &= ~KS59XX_MCSR_NVRAM_LOAD_HIGH_ADDR;
	Temp |= KS59XX_MCSR_NVRAM_BEGIN_READ;
	m_NvramControl = Temp;

	return TRUE;
}

template <class TRange, class TRegister>
BOOLEAN KS59XXPci<TRange, TRegister>::NvramWriteOperationSer2(
	KS59XX_FLEXIBLE_CONTEXT* pContext
	)
{
	UCHAR Temp = 0;

	// Read the MCSR and preserve the appropriate bits in the register,
	// clearing others.
    Temp = m_NvramControl;
	Temp &= KS59XX_MCSR_NVRAM_PRESERVE_MASK;

	// Write data value
	m_NvramData = static_cast<UCHAR>(pContext->m_dwParam3);

	// Latch data and begin write operation
	Temp |= KS59XX_MCSR_NVRAM_BEGIN_WRITE;
	m_NvramControl = Temp;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Macros

//////////////////////////////////////////////////////////////////////
//
// Macro MEMBER_KS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE
//
// To make a the handler routine for a mailbox interrupt callback function
// a member in any class, use the following macro inside
// the definition of the class. If the callback function member name is X, then
// the callback function parameter to KS59XXPci::SetupMailboxInterrupt must be
// passed LinkTo(X).  Also, the callback context parameter must be the address
// of the object (i.e."this") of which the callback function X is a member.
//
// The first parameter to the macro is the name of class in which the
// mailbox interrupt callback routine member is being declared. The second
// parameter is the name of the mailbox interrupt handler routine function.
//
// The routine member declared by this macro has the following prototype:
//
// VOID
// class_name::function_name( VOID );
//
// NOTE This routine will be called at either DIRQL (Device IRQL), or
// DISPATCH_LEVEL depending on the parameters passed to SetupMailboxInterrupt
//
#define MEMBER_KS59XX_MAILBOX_INTERRUPT_HANDLER_ROUTINE(objclass, func)	\
	VOID __stdcall func();										\
	static VOID __stdcall LinkTo(func)(							\
		PVOID context )											\
	{															\
		((objclass*)context)->func();							\
	}

#endif // __KS59XX__

#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
