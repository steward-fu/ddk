// kintrupt.h - Include file for class KInterrupt
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


// KInterrupt is not available to 64-bit NT4-style drivers since
// they cannot access the hardware using the old NT4 functions
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

#ifndef __KINTRUPT__
#define __KINTRUPT__

class KResourceAssignment; // forward

class KInterrupt
{

// Construction
public:
	SAFE_DESTRUCTORS
	KInterrupt(void);

#if !_WDM_
	KInterrupt(
		INTERFACE_TYPE IntfType,
		ULONG BusNumber,
		ULONG BusInterruptLevel,
		ULONG BusInterruptVector,
		KINTERRUPT_MODE Mode,
		BOOLEAN bShareVector,
		BOOLEAN bSaveFloat
		);

	VOID Initialize(
		INTERFACE_TYPE IntfType,
		ULONG BusNumber,
		ULONG BusInterruptLevel,
		ULONG BusInterruptVector,
		KINTERRUPT_MODE Mode,
		BOOLEAN bShareVector=FALSE,
		BOOLEAN bSaveFloat=FALSE
		);

#endif // !_WDM_

#if _WDM_
	KInterrupt(
		KIRQL irql,
		ULONG vector,
		KINTERRUPT_MODE mode,
		BOOLEAN bShareVector=FALSE,
		KAFFINITY affinity=1,
		BOOLEAN bSaveFloat=FALSE
		);

	VOID Initialize(
		KIRQL irql,
		ULONG vector,
		KINTERRUPT_MODE mode,
		BOOLEAN bShareVector=FALSE,
		KAFFINITY affinity=1,
		BOOLEAN bSaveFloat=FALSE
		);
#endif // _WDM_

#ifndef VXD_COMPATLIB
	NTSTATUS Initialize(
		PCM_RESOURCE_LIST pResourceList,
		ULONG Ordinal=0,
		BOOLEAN bSaveFloat=FALSE
		);

	NTSTATUS InitializeAndConnect(
		PCM_RESOURCE_LIST pResourceList,
		PKSERVICE_ROUTINE Isr,
		PVOID IsrContext,
		ULONG Ordinal=0,
		BOOLEAN bSaveFloat=FALSE
		);
#endif // VXD_COMPATLIB

	~KInterrupt(void){ Disconnect(); }
	VOID Invalidate(void){ Disconnect(); }
	BOOLEAN IsValid(void){ return (m_pInterruptObject != NULL); }

// Methods
public:
	NTSTATUS Connect(
		PKSERVICE_ROUTINE Isr,
		PVOID Context
		);

	NTSTATUS Connect(
		PKSERVICE_ROUTINE Isr,
		PVOID Context,
		PKSPIN_LOCK pSpin,
		KIRQL SynchIrql
		);

	VOID Disconnect(void);

	virtual BOOLEAN Synchronize(PKSYNCHRONIZE_ROUTINE SynchRoutine, PVOID SynchContext);

#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x20))))
	KIRQL AcquireSpinLock() { return KeAcquireInterruptSpinLock(m_pInterruptObject); }
	VOID ReleaseSpinLock(KIRQL OldIrql) { KeReleaseInterruptSpinLock(m_pInterruptObject, OldIrql); }
#endif

	operator PKINTERRUPT (){ return m_pInterruptObject; }

	ULONG Vector(void){ return m_Vector; }
	KIRQL Irql(void){ return m_SynchIrql; }
	KAFFINITY& Affinity(void){ return m_Affinity; }
	KINTERRUPT_MODE Mode(void){ return m_Mode; }
	ULONG BusLevel(void){ return m_BusInterruptLevel; }
	ULONG BusVector(void){ return m_BusInterruptVector; }
	INTERFACE_TYPE BusType(void){ return m_IntfType; }

// Data
protected:
	PKINTERRUPT m_pInterruptObject;
	ULONG m_Vector;
	KIRQL m_Irql;
	KIRQL m_SynchIrql;
	KAFFINITY m_Affinity;

	INTERFACE_TYPE m_IntfType;
	ULONG m_BusNumber;
	ULONG m_BusInterruptLevel;
	ULONG m_BusInterruptVector;
	PKSPIN_LOCK m_pSpin;
	KINTERRUPT_MODE m_Mode;
	BOOLEAN m_bShareVector;
	BOOLEAN m_bSaveFloat;
};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

inline KInterrupt::KInterrupt(void) :
	m_pInterruptObject(NULL)
{
}

#if !_WDM_
inline KInterrupt::KInterrupt(
	INTERFACE_TYPE IntfType,
	ULONG BusNumber,
	ULONG BusInterruptLevel,
	ULONG BusInterruptVector,
	KINTERRUPT_MODE Mode,
	BOOLEAN bShareVector,
	BOOLEAN bSaveFloat
	) :
	m_pInterruptObject(NULL)
{
	Initialize(
		IntfType,
		BusNumber,
		BusInterruptLevel,
		BusInterruptVector,
		Mode,
		bShareVector,
		bSaveFloat
		);
}


#endif // !_WDM_

#if _WDM_
inline KInterrupt::KInterrupt(
	KIRQL irql,
	ULONG vector,
	KINTERRUPT_MODE mode,
	BOOLEAN bShareVector,
	KAFFINITY affinity,
	BOOLEAN bSaveFloat
	) :
	m_pInterruptObject(NULL)
{
	Initialize(
		irql,
		vector,
		mode,
		bShareVector,
		affinity,
		bSaveFloat
		);
}

#endif	// _WDM_

#ifndef VXD_COMPATLIB
inline NTSTATUS KInterrupt::InitializeAndConnect(
	PCM_RESOURCE_LIST pResourceList,
	PKSERVICE_ROUTINE Isr,
	PVOID IsrContext,
	ULONG Ordinal,
	BOOLEAN bSaveFloat
	)
{
	NTSTATUS status;

	status = Initialize(
		pResourceList,
		Ordinal,
		bSaveFloat
		);

	if ( NT_SUCCESS(status) )
	{
		status = Connect(Isr, IsrContext);
	}
	return status;
}
#endif // VXD_COMPATLIB

////////////////////////////////////////////////////////////////////
// Synchronize
//
inline BOOLEAN KInterrupt::Synchronize(
	PKSYNCHRONIZE_ROUTINE SynchRoutine,
	PVOID SynchContext
	)
{
	if (m_pInterruptObject)
		return KeSynchronizeExecution(
			m_pInterruptObject,
			SynchRoutine,
			SynchContext
			);
	else
		return SynchRoutine(SynchContext);
}

//////////////////////////////////////////////////////////////////////
//
// Macro MEMBER_ISR
//
// To make an ISR member in any class, use the following macro inside
// the definition of the class. If the ISR member name is X, then
// the call to KInterrupt::Connect must be passed LinkTo(X) as the ISR
// address, and the address of the object (i.e."this") of which the
// ISR is a member.
//
// The first parameter to the macro is the name of class in which the
// ISR member is being declared. The second parameter is the
// name of the ISR function.
//
// The ISR member declared by this macro has the following prototype:
//
//		BOOLEAN Isr(void);
//
// Like KSERVICE_ROUTINE, the function returns TRUE if the interrupt
// was handled, or FALSE if the system should pass it on to another
// driver sharing the interrupt.
//
// This macro may be used inside any class definition, including classes
// derived from KDevice and KInterrupt. It actually declares two members:
// One is a non-static member which is used as the ISR, and the other
// is a static member whose declaration matches that required by the
// system for an ISR. The static member invokes the non-static
// member, relying on the fact that the object address was passed as
// parameter Isr to KInterrupt::Connect.
//

#define MEMBER_ISR(objclass, func)					\
	BOOLEAN __stdcall func(void);					\
	static BOOLEAN __stdcall LinkTo(func)(			\
		PKINTERRUPT intr,							\
		PVOID context)								\
	{												\
		return ((objclass*)context)->func();		\
		UNREFERENCED_PARAMETER(intr);				\
	}

///////////////////////////////////////////////////////////////////////////////

#define MEMBER_SYNCHCRITSECTIONWITHCONTEXT(contextname, func)		\
	BOOLEAN __stdcall func(contextname* pContext);	\
	static BOOLEAN __stdcall LinkTo(func)(			\
		PVOID context)								\
	{												\
		return ((contextname*)context)->m_pClass	\
			->func((contextname*)context);			\
	}

#endif // __KINTRUPT__

#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
