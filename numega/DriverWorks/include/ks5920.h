// KS5920.h - include file for class KS5920 and related classes
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

#ifndef __KS5920__
#define __KS5920__

// Handler routine for addon interrupt.  This will be called
// at DIRQL.
typedef
VOID
(*PKS5920_ADDON_INTERRUPT_HANDLER_ROUTINE) (
	PVOID context
    );

///////////////////////////////////////////////////////////////////////////////
// Class declaration for KS5920Pci
///////////////////////////////////////////////////////////////////////////////
template <class TRange, class TRegister> class KS5920Pci :
	public KS59XXPci <TRange, TRegister>
{

// Construction
public:
	SAFE_DESTRUCTORS

	KS5920Pci(void);

#if ! _WDM_

	KS5920Pci(
		USHORT VendorId,
		USHORT DeviceId,
		KPciSlot* pSlot=NULL
		);

	KS5920Pci(
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

	NTSTATUS Initialize(
		PDEVICE_OBJECT TopOfStackDevice,
		PCM_RESOURCE_LIST pTranslatedResourceList,
		PCM_RESOURCE_LIST pRawResourceList,
		ULONG OperationRegisterBARIndex,
		BOOLEAN NvramIsSerialType = TRUE,
		USHORT NvramPageSize = 1
		);

#endif // ! _WDM_

protected:
	NTSTATUS InitializeS5920(void);

// Methods
public:
	NTSTATUS SetupAddonInterrupt(
		PKS5920_ADDON_INTERRUPT_HANDLER_ROUTINE pCallback,
		PVOID pContext = NULL
		);

	NTSTATUS ReadOperationRegisters(PAMCCS5920_PCI_BUS_OPERATION_REGISTERS pOpRegs5920);

	NTSTATUS WriteOperationRegisters(PAMCCS5920_PCI_BUS_OPERATION_REGISTERS pOpRegs5920);

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
	MEMBER_ISR( KS5920Pci, Isr );

	// The DPC (deferred procedure call) for the ISR
	MEMBER_DPC( KS5920Pci, DpcForIsr );

#ifdef _COMMENT_ONLY

		// The following member functions are actually defined by the
		// a DEVMEMBER_xxx or MEMBER_xxx macro (such as DEVMEMBER_DISPATCHERS).
		// The macro __COMMENT_ONLY never gets defined.  These comment-only
		// definitions simply allow easy navigation to the functions within
		// the Developer Studio using the class browser.

	BOOLEAN Isr(void);								// COMMENT_ONLY
	VOID DpcForIsr( PVOID Context1, PVOID Context2 ); // COMMENT_ONLY
#endif

// Data
public:
	// Register data members to access the registers of the S5920.  Note that
	// the INTCSR and RSR are defined in KS59XXPci
	TRegister m_OutgoingMailbox;			// OMB  - Outgoing Mailbox Reg
	TRegister m_IncomingMailbox;			// IMB  - Incoming Mailbox Reg
	TRegister m_MailboxEmptyFullStatus;		// MBEF - Mailbox Empty/Full Status Reg
	TRegister m_PassThruConfiguration;		// PTCR - Pass-Thru Configuration Reg

protected:
	KDeferredCall m_IsrDpc;
	PKS5920_ADDON_INTERRUPT_HANDLER_ROUTINE m_pAddonCallback;
	PVOID m_pAddonContext;
};

//////////////////////////////////////////////////////////////////////////////
// Typedefs for the two allowable kinds of KS5920Pci
//////////////////////////////////////////////////////////////////////////////
typedef KS5920Pci<KIoRange, KIoRegister> KS5920PciIoMapped;
typedef KS5920Pci<KMemoryRange, KMemoryRegister> KS5920PciMemoryMapped;

//////////////////////////////////////////////////////////////////////////////
// Class Definition for KS5920Pci
//////////////////////////////////////////////////////////////////////////////

template <class TRange, class TRegister>
KS5920Pci<TRange, TRegister>::KS5920Pci(void) :
	KS59XXPci<TRange, TRegister>(),
	m_pAddonCallback(NULL),
	m_pAddonContext(NULL)
{
}

#if ! _WDM_

template <class TRange, class TRegister>
KS5920Pci<TRange, TRegister>::KS5920Pci(
	USHORT VendorId,
	USHORT DeviceId,
	KPciSlot* pSlot
	) :
	KS59XXPci<TRange, TRegister>(VendorId, DeviceId, pSlot),
	m_pAddonCallback(NULL),
	m_pAddonContext(NULL)
{
}

template <class TRange, class TRegister>
KS5920Pci<TRange, TRegister>::KS5920Pci(
	ULONG Bus,
	ULONG Device,
	ULONG Function
	) :
	KS59XXPci<TRange, TRegister>(Bus, Device, Function),
	m_pAddonCallback(NULL),
	m_pAddonContext(NULL)
{
}

template <class TRange, class TRegister>
NTSTATUS KS5920Pci<TRange, TRegister>::Initialize(
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
		status = InitializeS5920();
	}

	return status;
}

template <class TRange, class TRegister>
NTSTATUS KS5920Pci<TRange, TRegister>::Initialize(
	KDevice* pDevice,
	PWSTR RegistryPath,
	ULONG OperationRegisterBARIndex,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize
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
		status = InitializeS5920();
	}

	return status;
}

#else // ! _WDM_
template <class TRange, class TRegister>
NTSTATUS KS5920Pci<TRange, TRegister>::Initialize(
	PDEVICE_OBJECT TopOfStackDevice,
	PCM_RESOURCE_LIST pTranslatedResourceList,
	PCM_RESOURCE_LIST pRawResourceList,
	ULONG OperationRegisterBARIndex,
	BOOLEAN NvramIsSerialType,
	USHORT NvramPageSize
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

		status = InitializeS5920();

		// The resource list won't be valid after we return so clear the
		// pointer to it.
		m_pResourceList = NULL;
	}

	return status;
}


#endif // ! _WDM_

template <class TRange, class TRegister>
NTSTATUS KS5920Pci<TRange, TRegister>::InitializeS5920(void)
{
	NTSTATUS status = STATUS_SUCCESS;

	ASSERT( m_OperationRegisterBase.Count() >= AMCCS5920_PCI_BUS_OPERATION_REGISTERS_SIZE );

	// Initialize register data members specific to S5920. Base the register offsets
	// on the equivalent registers in the S5933 except for the pass thru configuration
	// register, which is unique to the S5920.
	m_OutgoingMailbox = m_OperationRegisterBase[OP_REG_OFFSET(OutgoingMailbox4)];
	m_IncomingMailbox = m_OperationRegisterBase[OP_REG_OFFSET(IncomingMailbox4)];
	m_MailboxEmptyFullStatus =
		m_OperationRegisterBase[OP_REG_OFFSET(MailboxEmptyFullStatus)];
	m_PassThruConfiguration = m_OperationRegisterBase[AMCCS5920_PASS_THRU_CONFIG_OFFSET];

	// Check to see if an interrupt exists in our resource list.
	// If it does, connect it.
	KResourceAssignment theIrqAssignment(
		m_pResourceList,
		CmResourceTypeInterrupt );
	if ( theIrqAssignment.Status() )
	{
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
BOOLEAN KS5920Pci<TRange, TRegister>::Isr()
{

	ULONG IntcsrRegister;
	ULONG RcrRegister;

	// Read the interrupt control/status register and the reset control
	// register.  Mask the RCR register appropriately
	IntcsrRegister = m_InterruptControlStatus;
	RcrRegister = m_ResetControl;
	RcrRegister &= KS59XX_MCSR_PRESERVE_MASK;

	if ( !(IntcsrRegister & KS59XX_INTCSR_INTERRUPT_ASSERTED) )
	{
		// Return FALSE to indicate that this device did not cause the interrupt.
		return FALSE;
	}

	// Based on the type of interrupt that occurred, we will disable the
	// interrupt source if appropriate

	// Check for addon interrupts and call the appropriate handler. NOTE The interrupt
	// handler MUST clear the interrupt condition in the addon hardware before returning.
	if (IntcsrRegister & KS59XX_INTCSR_ADDON)
	{
		if (m_pAddonCallback != NULL)
			(*m_pAddonCallback)(m_pAddonContext);
		else
			ASSERT(m_pAddonCallback != NULL);
	}

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

	// Write back the modified MCSR to disable complete or aborted transfers
	m_BusMasterControlStatus = RcrRegister;

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
VOID KS5920Pci<TRange, TRegister>::DpcForIsr( PVOID Context1, PVOID Context2 )
{

	// Get the interrupt bits that have occurred.
	KS59XX_FLEXIBLE_CONTEXT FlexContext;
	RtlZeroMemory(&FlexContext, sizeof(FlexContext));
	FlexContext.m_pClass = this;

	m_Irq.Synchronize( LinkTo(GetAccumulatedInterrupts), &FlexContext );

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

template <class TRange, class TRegister>
NTSTATUS KS5920Pci<TRange, TRegister>::SetupAddonInterrupt(
	PKS5920_ADDON_INTERRUPT_HANDLER_ROUTINE pCallback,
	PVOID pContext
	)
{
	// Setup the data members that will be used in the callback
	m_pAddonCallback = pCallback;
	m_pAddonContext = pContext;
    return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
NTSTATUS KS5920Pci<TRange, TRegister>::ReadOperationRegisters(
	PAMCCS5920_PCI_BUS_OPERATION_REGISTERS pOpRegs5920
	)
{
	// Fill in all of the mailbox registers
	pOpRegs5920->OutgoingMailbox = m_OutgoingMailbox;
	pOpRegs5920->IncomingMailbox = m_IncomingMailbox;
    pOpRegs5920->MailboxEmptyFullStatus = m_MailboxEmptyFullStatus;
    pOpRegs5920->InterruptControlStatus = m_InterruptControlStatus;
    pOpRegs5920->ResetControl = m_ResetControl;

	return STATUS_SUCCESS;
}

template <class TRange, class TRegister>
NTSTATUS KS5920Pci<TRange, TRegister>::WriteOperationRegisters(
	PAMCCS5920_PCI_BUS_OPERATION_REGISTERS pOpRegs5920
	)
{
	// Fill in all of the mailbox registers
	m_OutgoingMailbox = pOpRegs5920->OutgoingMailbox;
    m_InterruptControlStatus = pOpRegs5920->InterruptControlStatus;
    m_ResetControl = pOpRegs5920->ResetControl;

	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Macros

//////////////////////////////////////////////////////////////////////
//
// Macro MEMBER_KS5920_ADDON_INTERRUPT_HANDLER_ROUTINE
//
// To make a the handler routine for an ADDON interrupt callback function
// a member in any class, use the following macro inside
// the definition of the class. If the callback function member name is X, then
// the callback function parameter to KS5920Pci::SetupAddonInterrupt must be
// passed LinkTo(X).  Also, the callback context parameter must be the address
// of the object (i.e."this") of which the callback function X is a member.
//
// The first parameter to the macro is the name of class in which the
// addon interrupt callback routine member is being declared. The second
// parameter is the name of the addon interrupt handler routine function.
//
// The routine member declared by this macro has the following prototype:
//
// VOID
// class_name::function_name( VOID );
//
// NOTE This routine will be called at DIRQL (Device IRQL), and MUST clear
// the interrupt condition causing the interrupt to be asserted by the
// addon device to the S5920.
//
#define MEMBER_KS5920_ADDON_INTERRUPT_HANDLER_ROUTINE(objclass, func)	\
	VOID __stdcall func();										\
	static VOID __stdcall LinkTo(func)(							\
		PVOID context )											\
	{															\
		((objclass*)context)->func();							\
	}

#endif // __KS5920__

#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
