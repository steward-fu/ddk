// KNdisInterrupt.h: interface for the KNdisInterrupt class.
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

#if !defined(AFX_KNDISINTERRUPT_H__A4E89637_392F_11D3_8FA2_00C04F7445B7__INCLUDED_)
#define AFX_KNDISINTERRUPT_H__A4E89637_392F_11D3_8FA2_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

//////////////////////////////////////////////////////////////////////
/* About NDIS interrupts

The way NDIS interacts with a miniport while processing interrupts
is determined by the parameters passed on interrupt registration:

NDIS_STATUS NdisMRegisterInterrupt(
       OUT PNDIS_MINIPORT_INTERRUPT Interrupt,
       IN NDIS_HANDLE MiniportAdapterHandle,
       IN UINT InterruptVector,
       IN UINT InterruptLevel,
       IN BOOLEAN RequestIsr,
       IN BOOLEAN SharedInterrupt,
       IN NDIS_INTERRUPT_MODE InterruptMode);

Miniports provide 2 callback functions: ISR() and HandleInterrupt().
The ISR() runs at DIRQL while HandleInterrupt() runs at dispatch level.
Also, miniports optionally provide EnableInterrupt() and DisableInterrupt()
callbacks. The bulk of packet processing logic happens in HandleInterrupt().
In KNDIS framework, those handlers are static members of KNdisWrapper class
that delegate control to a KNdisMiniAdapter-derived member functions.

The RequestIsr and SharedInterrupt parameters controls how and when this
callbacks are called from NDIS. Allowable combinations:

RequestIsr SharedInterrupt WhenToUse
---------- --------------- ----------
FALSE      FALSE           NIC is not sharing IRQ and DPC latency is not a concern
TRUE       FALSE           NIC is not sharing IRQ but has to be serviced ASAP
FALSE      TRUE	           INVALID: NDIS can't know whose INT was it, so ISR() is needed.
TRUE       TRUE            NIC is sharing IRQ or LevelSensitive: must dismiss in either case.

*** The simplistic view of the processing (subject to verification!!) ***

NDIS intercepts all the interrupts for the NIC.

If RequestIsr was set to FALSE, NDIS calls DisableInterrupt() at DIRQL and
then HandleInterrupt() as a DPC. After the latter returns NDIS calls
EnableInterrupt(). Done.

If RequestIsr was set to TRUE, NDIS immediately calls ISR() at DIRQL.
Its prototype is:

VOID MiniportISR(
       OUT PBOOLEAN InterruptRecognized,
       OUT PBOOLEAN QueueMiniportHandleInterrupt,
       IN NDIS_HANDLE MiniportAdapterContext);

The ISR() routine should determine 2 things: (1) whether the interrupt
has to do with miniport's NIC and (2) whether further processing
is requirted (provided (1) is TRUE). If (1) is FALSE we're done (foreign
interrupt). If (1) and (2) are TRUE, then NDIS schedules DPC to call into
HandleInterrupt() later on. Disabling/enabling interrupts in this case is
a sole responisbility of the miniport (i.e. NDIS won't use
EnableInterrupt() and DisableInterrupt() hanlders).

NB::NB The HandleInterrupt function is not queued if the driver's Halt
====== or Initialize function is currently executing. In this case, the ISR()
	   will be called regardless the RequestIsr parameter.

	   Both ISR() and HandleInterrupt() should be designed to be reentarable.
	   On SMP, both could be reentered. So, one should not rely on a one-to-one
	   correspondence between the execution of ISR and HandleInterrupt.


*/////////////////////////////////////////////////////////////////////

// forward:
class KNdisMiniAdapter;

//////////////////////////////////////////////////////////////////////
// KNdisInterrupt
//
// Wraps Interrupt object and synchronization between ISR and non-ISR
// contexts
//
class KNdisInterrupt
{
	SAFE_DESTRUCTORS
public:
	KNdisInterrupt(
		IN NDIS_HANDLE MiniportAdapterHandle,
		IN UINT InterruptVector,
		IN UINT InterruptLevel,
		IN BOOLEAN RequestIsr,
		IN BOOLEAN SharedInterrupt,
		IN NDIS_INTERRUPT_MODE InterruptMode);

	KNdisInterrupt(
		KNdisMiniAdapter* pAdapter,
		KNdisResource<CmResourceTypeInterrupt>& Resource,
		NDIS_INTERRUPT_MODE InterruptMode,
		BOOLEAN RequestIsr=FALSE);

	inline KNdisInterrupt();

	~KNdisInterrupt();

// synchronization with:
	inline BOOLEAN Synchronize(
		IN PVOID SynchronizeFunction,
		IN PVOID SynchronizeContext);

// status:
	inline NDIS_STATUS Status() const;

	inline NDIS_STATUS Initialize(
		IN NDIS_HANDLE MiniportAdapterHandle,
		IN UINT InterruptVector,
		IN UINT InterruptLevel,
		IN BOOLEAN RequestIsr=FALSE,
		IN BOOLEAN SharedInterrupt=FALSE,
		IN NDIS_INTERRUPT_MODE InterruptMode=NdisInterruptLatched);

	NDIS_STATUS Initialize(
		KNdisMiniAdapter* pAdapter,
		KNdisResource<CmResourceTypeInterrupt>& Resource,
		NDIS_INTERRUPT_MODE InterruptMode,
		BOOLEAN RequestIsr=FALSE);

	inline void Invalidate();

	inline BOOLEAN IsValid() const;

protected:
	NDIS_MINIPORT_INTERRUPT m_Interrupt;
	NDIS_STATUS				m_ConstructionStatus;

};

/////////////////////////////////////////////////////////////
// KNDIS_DECLARE_SYNCHROCALLBACK
//
// Macro used to declare a member callback invoked via
// KNdisInterrupt::Synchronize. Dispatch it using the
// KNDIS_MEMBER_CALLBACK macro, e.g.
//
// m_Interrupt.Synchronize(KNDIS_MEMBER_CALLBACK(SyncRoutine), this);
//
#define KNDIS_DECLARE_SYNCHROCALLBACK(Class, member)		    \
 static void SysCallback2##member(PVOID cxt)					\
 { reinterpret_cast<Class*>(cxt)->member (); }


///////////////////////////////////////////////////////////////////
// Inline Implementations


///////////////////////////////////////////////////////////////////////
// Constructor (non-initialized form)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisInterrupt::KNdisInterrupt()
{
	m_ConstructionStatus=NDIS_STATUS_FAILURE;
}

///////////////////////////////////////////////////////////////////////
// Get the interrupt status
//
// Parameters:
// 	none
// Returns:
// 	Returns the status of the interrupt.
// Comments:
// 	none
NDIS_STATUS KNdisInterrupt::Status() const
{
	return m_ConstructionStatus;
}

///////////////////////////////////////////////////////////////////////
// Call interrupt synchronization function
//
// Parameters:
// 	SynchronizeFunction  -   function to call
//	SynchronizeContext   -   context parameter to pass to synchronize function
// Returns:
// 	Returns the value returned by the synchronize function
// Comments:
// 	The interrupt is blocked during the execution of the synchronize
//  function.
BOOLEAN KNdisInterrupt::Synchronize(
			IN PVOID SynchronizeFunction,
			IN PVOID SynchronizeContext
			)
{
   return NdisMSynchronizeWithInterrupt(
       &m_Interrupt,
       SynchronizeFunction,
       SynchronizeContext
       );
};

///////////////////////////////////////////////////////////////////////
// Initialize the interrupt object (raw form)
//
// Parameters:
// 	MiniportAdapterHandle - adapter handle
// 	InterruptVector      -  bus vector
// 	InterruptLevel       -  bus request level
// 	RequestIsr           -  see overview
// 	SharedInterrupt      -  TRUE if the interrupt can be shared
// 	InterruptMode        -  latched or level
// Returns:
// 	Status
// Comments:
// 	Initialize() must be called once on a KNdisInterrupt object created
//	using the default constructor.
NDIS_STATUS KNdisInterrupt::Initialize(IN NDIS_HANDLE MiniportAdapterHandle,
       IN UINT InterruptVector,
       IN UINT InterruptLevel,
       IN BOOLEAN RequestIsr,
       IN BOOLEAN SharedInterrupt,
       IN NDIS_INTERRUPT_MODE InterruptMode)
{
	return	( m_ConstructionStatus = NdisMRegisterInterrupt(
						&m_Interrupt,
						MiniportAdapterHandle,
						InterruptVector,
					    InterruptLevel,
					    RequestIsr,
						SharedInterrupt,
				        InterruptMode) );
}



///////////////////////////////////////////////////////////////////////
// Deregister the interrupt and invalidate the object
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	After this function executes, the miniport's interrupt handler will not be called.
void KNdisInterrupt::Invalidate()
{
	if ( NT_SUCCESS(m_ConstructionStatus) ) {
			NdisMDeregisterInterrupt(&m_Interrupt);
			m_ConstructionStatus = NDIS_STATUS_FAILURE;
	}
}

///////////////////////////////////////////////////////////////////////
// Test if the interrupt object has been constructed properly
//
// Parameters:
// 	none
// Returns:
// 	Returns TRUE if the object is valid
// Comments:
// 	none
BOOLEAN KNdisInterrupt::IsValid() const
{
	return m_ConstructionStatus == NDIS_STATUS_SUCCESS;
}

#endif // !defined(AFX_KNDISINTERRUPT_H__A4E89637_392F_11D3_8FA2_00C04F7445B7__INCLUDED_)
