// kdevque.h - include file for class KDeviceQueueR, KDeviceQueue
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

#ifndef __KDEVQUE__
#define __KDEVQUE__

#if _WDM_
class KPnpDevice;
#endif

class KDeviceQueue
{

// Construction
public:
	SAFE_DESTRUCTORS
	KDeviceQueue(PKDEVICE_QUEUE p);
	KDeviceQueue(void);
	KDeviceQueue(KDeviceQueue& Copy);
	VOID Initialize(PKDEVICE_QUEUE p);
	VOID Initialize(void);
	VOID Initialize(KDeviceQueue& Copy);
	~KDeviceQueue(void){ if (IsValid()) Invalidate(); }
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_pdq != NULL); }

// Methods
public:
	BOOLEAN IsEmpty(void);

	PIRP Head(void);
	PIRP Tail(void);
	PIRP Next(PIRP pIrp);

	operator PKDEVICE_QUEUE();

	VOID Lock(BOOLEAN AtDpcLevel=FALSE);
	VOID Unlock(void);
	KIRQL _lock(BOOLEAN AtDpcLevel = FALSE);
	VOID _unlock(KIRQL restoreIrql);

	BOOLEAN Insert(PIRP pIrp);
	BOOLEAN InsertByKey(PIRP pIrp, ULONG SortKey);

	PIRP RemoveHead(void);
	BOOLEAN RemoveSpecificEntry(PIRP pIrp);
	PIRP RemoveByKey(ULONG SortKey);

	VOID CleanUp(PFILE_OBJECT FileObject);
#if _WDM_
	VOID PnpCleanUp(KPnpDevice* pDevice, PFILE_OBJECT FileObject);
#endif

protected:
	PIRP Container(PKDEVICE_QUEUE_ENTRY p);
	PIRP ContainerNoTestEmpty(PKDEVICE_QUEUE_ENTRY p);

// Data
protected:
	PKDEVICE_QUEUE m_pdq;
	BOOLEAN m_ConstructorAllocated;
	KIRQL m_PreviousIrql;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

inline KDeviceQueue::KDeviceQueue(PKDEVICE_QUEUE p) :
	m_pdq(p),
	m_ConstructorAllocated(FALSE),
	m_PreviousIrql(PASSIVE_LEVEL)
{
}

inline KDeviceQueue::KDeviceQueue(void) :
	m_pdq(new(NonPagedPool) KDEVICE_QUEUE),
	m_ConstructorAllocated(FALSE),
	m_PreviousIrql(PASSIVE_LEVEL)
{
	if (m_pdq != NULL)
	{
		KeInitializeDeviceQueue(m_pdq);
	}
	m_ConstructorAllocated = (m_pdq != NULL);
}

inline KDeviceQueue::KDeviceQueue(KDeviceQueue& Copy) :
	m_pdq(new(NonPagedPool) KDEVICE_QUEUE),
	m_ConstructorAllocated(FALSE),
	m_PreviousIrql(PASSIVE_LEVEL)
{
	if (m_pdq != NULL)
	{
		*m_pdq = *Copy.m_pdq;
		m_PreviousIrql = Copy.m_PreviousIrql;
	}
	m_ConstructorAllocated = (m_pdq != NULL);
}

inline VOID KDeviceQueue::Initialize(PKDEVICE_QUEUE p)
{
	if (IsValid())
	{
		ASSERT(m_pdq == NULL);
		Invalidate();
	}
	m_pdq = p;
	m_ConstructorAllocated = FALSE;
	m_PreviousIrql = PASSIVE_LEVEL;
}

inline VOID KDeviceQueue::Initialize(void)
{
	if (IsValid())
	{
		ASSERT(m_pdq == NULL);
		Invalidate();
	}
	m_pdq = new(NonPagedPool) KDEVICE_QUEUE;

	if (m_pdq != NULL)
	{
		KeInitializeDeviceQueue(m_pdq);
		m_PreviousIrql = PASSIVE_LEVEL;
	}
	m_ConstructorAllocated = (m_pdq != NULL);
}

inline VOID KDeviceQueue::Initialize(KDeviceQueue& Copy)
{
	if (IsValid())
	{
		ASSERT(m_pdq == NULL);
		Invalidate();
	}
	m_pdq = new(NonPagedPool) KDEVICE_QUEUE;

	if (m_pdq != NULL)
	{
		*m_pdq = *Copy.m_pdq;
		m_PreviousIrql = Copy.m_PreviousIrql;
	}
	m_ConstructorAllocated = (m_pdq != NULL);
}

inline VOID KDeviceQueue::Invalidate(void)
{
	if (m_ConstructorAllocated)
	{
		ASSERT(m_pdq != NULL);
		delete m_pdq;
	}
	m_pdq = NULL;
}

inline VOID KDeviceQueue::Lock(BOOLEAN AtDpcLevel)
{
	m_PreviousIrql = _lock(AtDpcLevel);
}

inline VOID KDeviceQueue::Unlock(void)
{
	_unlock(m_PreviousIrql);
}

inline BOOLEAN KDeviceQueue::Insert(PIRP pIrp)
{
	BOOLEAN bRaised=FALSE;
	BOOLEAN result;
	KIRQL irql = PASSIVE_LEVEL;

	if (KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		KeRaiseIrql(DISPATCH_LEVEL, &irql);
		bRaised = TRUE;
	}

	result = KeInsertDeviceQueue(
		m_pdq,
		&pIrp->Tail.Overlay.DeviceQueueEntry);

	if (bRaised)
		KeLowerIrql(irql);

	return result;
}

inline BOOLEAN KDeviceQueue::InsertByKey(PIRP pIrp, ULONG SortKey)
{
	BOOLEAN bRaised=FALSE;
	BOOLEAN result;
	KIRQL irql = PASSIVE_LEVEL;

	if (KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		KeRaiseIrql(DISPATCH_LEVEL, &irql);
		bRaised = TRUE;
	}

	result = KeInsertByKeyDeviceQueue(
		m_pdq,
		&pIrp->Tail.Overlay.DeviceQueueEntry,
		SortKey);

	if (bRaised)
		KeLowerIrql(irql);

	return result;
}

inline PIRP KDeviceQueue::RemoveHead(void)
{
	return ContainerNoTestEmpty(KeRemoveDeviceQueue(m_pdq));
}

inline BOOLEAN KDeviceQueue::RemoveSpecificEntry(PIRP pIrp)
{
	return KeRemoveEntryDeviceQueue(
		m_pdq,
		&pIrp->Tail.Overlay.DeviceQueueEntry);
}

inline PIRP KDeviceQueue::RemoveByKey(ULONG SortKey)
{
	return ContainerNoTestEmpty(KeRemoveByKeyDeviceQueue(m_pdq, SortKey));
}

inline KIRQL KDeviceQueue::_lock(BOOLEAN AtDpcLevel)
{
	KSpinLock slock(&m_pdq->Lock);
	return slock._lock(AtDpcLevel);
}

inline VOID KDeviceQueue::_unlock(KIRQL restoreIrql)
{
	KSpinLock slock(&m_pdq->Lock);
	slock._unlock(restoreIrql);
}

inline PIRP KDeviceQueue::Head(void)
{
	return Container((PKDEVICE_QUEUE_ENTRY)m_pdq->DeviceListHead.Flink);
}

inline PIRP KDeviceQueue::Tail(void)
{
	return Container((PKDEVICE_QUEUE_ENTRY)m_pdq->DeviceListHead.Blink);
}

inline PIRP KDeviceQueue::Next(PIRP pIrp)
{
	if ( (pIrp == NULL) ||
	     (pIrp->Tail.Overlay.DeviceQueueEntry.DeviceListEntry.Flink == &m_pdq->DeviceListHead)
	   )
		return Head();

	else
		return Container( (PKDEVICE_QUEUE_ENTRY)pIrp->
			Tail.Overlay.DeviceQueueEntry.DeviceListEntry.Flink);
}

inline BOOLEAN KDeviceQueue::IsEmpty(void)
{
	return m_pdq->DeviceListHead.Flink == &m_pdq->DeviceListHead;
}

inline KDeviceQueue::operator PKDEVICE_QUEUE()
{
	return m_pdq;
}

inline PIRP KDeviceQueue::Container(PKDEVICE_QUEUE_ENTRY p)
{
	if (!p || (m_pdq->DeviceListHead.Flink == &m_pdq->DeviceListHead))
		return NULL;
	else
		return (PIRP)((PCHAR)p-(PCHAR)(&((PIRP)0)->Tail.Overlay.DeviceQueueEntry));
}

inline PIRP KDeviceQueue::ContainerNoTestEmpty(PKDEVICE_QUEUE_ENTRY p)
{
	if (!p)
		return NULL;
	else
		return (PIRP)((PCHAR)p-(PCHAR)(&((PIRP)0)->Tail.Overlay.DeviceQueueEntry));
}

#endif
