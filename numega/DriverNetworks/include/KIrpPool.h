// KIrpPool.h: interface for the KIrpPool class.
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
// CONSIDER: Move this file under DriverWorks\include
//

#if !defined(AFX_KIRPPOOL_H__25E809A3_B3EF_11D3_9005_00C04F7445B7__INCLUDED_)
#define AFX_KIRPPOOL_H__25E809A3_B3EF_11D3_9005_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KIrpPool
//
// Provides a pool of preallocated reusable IRPs for intermediate
// drivers using internal device i/o control to a lower device.
// The IRPs allocated from such a pool are supposed to be submitted
// using IoBuildDeviceIoControlRequest() with a completion routine.
//
// Parameters:
//
//	A		-		IRP allocator class.
//	L		-		Lock class
//
// 'A' has the following signature:
//  struct A {
//		PIRP Allocate(PDEVICE_OBJECT,PKEVENT,PIO_STATUS_BLOCK);	// how to get it
//		void Free(PIRP);										// how to return it
//		void Recycle(PIRP);										// how to reinitialize it between uses
//  };
//
//  'L' has the following signature:
//  struct L {
//		void Lock();						// protects pool's list
//		void Unlock();						//
//	};
//
template <class A, class L>
class KIrpPool
{

	//////////////////////////////////////////////////////////////////////
	// _IrpPoolHelper
	struct _IrpUserInfo
	{
#if KNDIS_WDM_STRICT
		NDIS_EVENT		m_Event;
#else
		KEVENT          m_Event;	// event obj needed by IRP allocation.
#endif
		IO_STATUS_BLOCK m_Iosb;		// status block needed by IRP allocation.

		// Constructor
		_IrpUserInfo()
		{
#if KNDIS_WDM_STRICT
			NdisInitializeEvent(&m_Event);
#else
			KeInitializeEvent(&m_Event, NotificationEvent, FALSE);
#endif
			m_Iosb.Status = STATUS_SUCCESS;
			m_Iosb.Information = 0;
		}
	};

	SAFE_DESTRUCTORS;
public:
// constructor (using number of irps)
	KIrpPool(PDEVICE_OBJECT LowerDevice, ULONG NumOfIRPs);
// constructor (empty)
	KIrpPool();
// destructor
	~KIrpPool();

// Allocate an IRP
    PIRP Allocate();
// Free the IRP
    void Free(PIRP irp);

// Get the pool usage balance
	int Usage() { return m_Usage; }

// Wait for all IRPs to be completed (zero-balance)
	NTSTATUS	Wait(ULONG mSec=0);

// Initialize pool (preallocate IRPs)
	void Initialize(PDEVICE_OBJECT LowerDevice, ULONG NumOfIRPs);
// Invalidate pool
	void Invalidate();
// Test the status of the pool
	BOOLEAN IsValid(void) const { return m_pUserInfoPool!=NULL; }

protected:
    LIST_ENTRY      m_head;				// head of the free list
	A				m_Allocator;		// allocator
	L				m_Lock;				// lock
	int				m_Usage;			// how many are allocated/not-freed
	_IrpUserInfo*	m_pUserInfoPool;	// pool of user info structs for IRPs in IrpPool
#if KNDIS_WDM_STRICT
	NDIS_EVENT		m_IdleEvent;
#else
	KEVENT          m_IdleEvent;		// event obj to detect idle list
#endif

// list access
    inline PLIST_ENTRY Entry(PIRP irp)
    { return (PLIST_ENTRY) (PCHAR(irp) + FIELD_OFFSET(IRP, Tail.Overlay.ListEntry)); }
    inline PIRP Entry(PLIST_ENTRY e)
    { return (PIRP) (PCHAR(e) - FIELD_OFFSET(IRP, Tail.Overlay.ListEntry)); }
};

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

template<class A, class L> inline
KIrpPool<A,L>::KIrpPool(PDEVICE_OBJECT LowerDevice, ULONG NumOfIRPs) :
	m_Usage(0),
	m_pUserInfoPool(NULL)
{
    InitializeListHead(&m_head);
#if KNDIS_WDM_STRICT
	NdisInitializeEvent(&m_IdleEvent);
	NdisSetEvent(&m_IdleEvent);
#else
	KeInitializeEvent(&m_IdleEvent, NotificationEvent, TRUE);
#endif
	Initialize(LowerDevice, NumOfIRPs);
}

template<class A, class L> inline
KIrpPool<A,L>::KIrpPool() :
	m_Usage(0),
	m_pUserInfoPool(NULL)
{
    InitializeListHead(&m_head);
#if KNDIS_WDM_STRICT
	NdisInitializeEvent(&m_IdleEvent);
	NdisSetEvent(&m_IdleEvent);
#else
	KeInitializeEvent(&m_IdleEvent, NotificationEvent, TRUE);
#endif
}

template<class A, class L> inline
KIrpPool<A,L>::~KIrpPool()
{
	Invalidate();
}

template<class A, class L> inline void
KIrpPool<A,L>::Initialize(PDEVICE_OBJECT LowerDevice, ULONG NumOfIRPs)
{
	if (IsValid())
		Invalidate();

	m_pUserInfoPool = new _IrpUserInfo[NumOfIRPs];

	if (m_pUserInfoPool)
	{
		for (ULONG i=0; i < NumOfIRPs; i++)
		{
			PIRP irp = m_Allocator.Allocate(
				LowerDevice,
#if KNDIS_WDM_STRICT
				(PKEVENT)&(m_pUserInfoPool[i].m_Event),
#else
				&(m_pUserInfoPool[i].m_Event),
#endif
				&(m_pUserInfoPool[i].m_Iosb)
				);

			if (irp)
			{
				InsertHeadList(&m_head, Entry(irp));
			}
			else
			{
				ASSERT(!"irp"); break;
			}
		}
	}
	else
		ASSERT(!"m_pUserInfoPool");
}

template<class A, class L> inline void
KIrpPool<A,L>::Invalidate()
{
    // TODO: make sure all pending are completed. The container class should do that...
	ASSERT(Usage()==0);

	if (IsValid())
	{
		// Dispose all: remove from the list and return to the system via the allocator
		PIRP irp;
		while ((irp = Allocate()) != NULL)
			m_Allocator.Free(irp);

		delete [] m_pUserInfoPool;
		m_pUserInfoPool = NULL;
		m_Usage = 0;
	}
}


template<class A, class L> inline
PIRP KIrpPool<A,L>::Allocate()
{
    m_Lock.Lock();
	if (IsListEmpty(&m_head))
	{
		m_Lock.Unlock();
		return NULL;	// failed
	}
    PLIST_ENTRY e = RemoveHeadList(&m_head);

	// If the count was zero, clear the idle event, indicating the
	// pool is now in use
	if (!m_Usage++)
#if KNDIS_WDM_STRICT
		NdisResetEvent(&m_IdleEvent);
#else
		KeClearEvent(&m_IdleEvent);
#endif
    m_Lock.Unlock();
	// TDI_TRACE("KIrpPool::Allocate() %X, CurrentLocation=%u\n", Entry(e), Entry(e)->CurrentLocation);
	PIRP irp = Entry(e);
	if(irp->UserEvent)
#if KNDIS_WDM_STRICT
		NdisResetEvent((PNDIS_EVENT)irp->UserEvent);
#else
		KeClearEvent(irp->UserEvent);
#endif

	return irp;
}

template<class A, class L> inline
void KIrpPool<A,L>::Free(PIRP irp)
{
	if(irp->UserEvent)
#if KNDIS_WDM_STRICT
		NdisSetEvent((PNDIS_EVENT)irp->UserEvent);
#else
		KeSetEvent(irp->UserEvent, 0, FALSE);
#endif

	// TDI_TRACE("KIrpPool::Free(%X), CurrentLocation=%u\n", irp, irp->CurrentLocation);
	m_Allocator.Recycle(irp);
    m_Lock.Lock();
    InsertTailList(&m_head, Entry(irp));
	// If the count is becoming zero, set the idle event, indicating the
	// pool is now idle
	if (!--m_Usage)
#if KNDIS_WDM_STRICT
		NdisSetEvent(&m_IdleEvent);
#else
		KeSetEvent(&m_IdleEvent, 0, FALSE);
#endif
	ASSERT(Usage()>=0);
    m_Lock.Unlock();
}


template<class A, class L> inline
NTSTATUS KIrpPool<A,L>::Wait(ULONG TimeoutMs)
{
	NTSTATUS Status=STATUS_SUCCESS;
    ASSERT (KeGetCurrentIrql() == PASSIVE_LEVEL);

#if KNDIS_WDM_STRICT
	if (!NdisWaitEvent(&m_IdleEvent,TimeoutMs))
		Status = STATUS_TIMEOUT;
#else
    LARGE_INTEGER tmo; tmo.QuadPart = -LONGLONG(10000*TimeoutMs);
	Status = KeWaitForSingleObject(&m_IdleEvent, Executive, KernelMode,
		FALSE, (TimeoutMs)?&tmo:NULL);
#endif
	return Status;
}

////////////////////////////////////////////////////////////////////////////
// KGenericIrpBuilder		IRP provider for the generic case
//
// Used as a template parameter for KIrpPool.
//
// NOTE: The generic allocator will use standard DDK IRP allocation and reuse
//		 methods
//
struct KGenericIrpBuilder {
// how to get it
	static inline PIRP Allocate
		(PDEVICE_OBJECT DeviceObject, PKEVENT pEvent, PIO_STATUS_BLOCK pIosb)
	{
		PIRP irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
		if (irp)
		{
			irp->UserIosb = pIosb;
			irp->UserEvent = pEvent;
		}
		return irp;
	}
// how to return it
	static inline void Free(PIRP irp)
	{
		IoFreeIrp(irp);
	}
// how to reinitialize it between uses
	static inline void	Recycle(PIRP irp)
	{
		// We must preserve the event and IO_STATUS_BLOCK associated with the IRP
		// before reinitializing it.  Also, we should clear the event and clean up
		// the iosb.
		PKEVENT pEvent = irp->UserEvent;
		PIO_STATUS_BLOCK pIosb = irp->UserIosb;
		if(pIosb)
		{
			pIosb->Status = STATUS_SUCCESS;
			pIosb->Information = 0;
		}

		// IoInitializeIrp can be used to reuse an IRP in both NT4 DDK and NT5 DDK,
		// if we take steps to preserve the IRP allocation flags.  IoReuseIrp() is
		// only implemented in the NT5 DDK.
		UCHAR AllocationFlags = irp->AllocationFlags;
		IoInitializeIrp(irp, IoSizeOfIrp(irp->StackCount), irp->StackCount);
		irp->AllocationFlags = AllocationFlags;

		// Restore the user event and IO_STATUS_BLOCK
		irp->UserEvent = pEvent;
		irp->UserIosb = pIosb;

		irp->IoStatus.Status = STATUS_SUCCESS;
	}
};

#endif // !defined(AFX_KIRPPOOL_H__25E809A3_B3EF_11D3_9005_00C04F7445B7__INCLUDED_)
