// kcsq.h - Cancel-Safe Queue class declaration
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

#ifndef __KCSQ_H__
#define __KCSQ_H__

#if ((NTDDI_VERSION >= 0x05000000) || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10)))))
#if ((NTDDI_VERSION && (NTDDI_VERSION < 0x05010000)) || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION < 0x20)))
extern "C"
{
#include <csq.h>
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////
//
// KCancelSafeQueue
//		Wrapper class for Cancel-Safe Queue
//
template <class T>
class KCancelSafeQueue : public IO_CSQ
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	KCancelSafeQueue()
	{
		IoCsqInitialize(this,
						T::_OnInsertIrp,
						T::_OnRemoveIrp,
						T::_OnPeekNextIrp,
						T::_OnAcquireLock,
						T::_OnReleaseLock,
						T::_OnCompleteCanceledIrp);
	}

	~KCancelSafeQueue() {}

public:
	VOID InsertIrp(PIRP Irp, PIO_CSQ_IRP_CONTEXT pContext = NULL) { IoCsqInsertIrp(this, Irp, pContext); }
	PIRP RemoveIrp(PIO_CSQ_IRP_CONTEXT pContext) { return IoCsqRemoveIrp(this, pContext); }
	PIRP RemoveNextIrp(PVOID pContext = NULL) { return IoCsqRemoveNextIrp(this, pContext); }
};

// static to member function conversion
#define CANCEL_SAFE_HANDLERS(_type_)																												\
	static VOID __stdcall _OnInsertIrp(PIO_CSQ Csq, PIRP Irp) { static_cast<_type_*>(Csq)->OnInsertIrp(Irp); }										\
	static VOID __stdcall _OnRemoveIrp(PIO_CSQ Csq, PIRP Irp) { static_cast<_type_*>(Csq)->OnRemoveIrp(Irp); }										\
	static VOID __stdcall _OnAcquireLock(PIO_CSQ Csq, PKIRQL Irql) { static_cast<_type_*>(Csq)->OnAcquireLock(Irql); }								\
	static VOID __stdcall _OnReleaseLock(PIO_CSQ Csq, KIRQL Irql) { static_cast<_type_*>(Csq)->OnReleaseLock(Irql); }								\
	static VOID __stdcall _OnCompleteCanceledIrp(PIO_CSQ Csq, PIRP Irp) { static_cast<_type_*>(Csq)->OnCompleteCanceledIrp(Irp); }					\
	static PIRP __stdcall _OnPeekNextIrp(PIO_CSQ Csq, PIRP Irp, PVOID Context) { return static_cast<_type_*>(Csq)->OnPeekNextIrp(Irp, Context); }

////////////////////////////////////////////////////////////////////////////////////////////////
//
// KCancelSafeList
//		This class provides cancel-safe irp list
//
template <class T>
class KCancelSafeListBase : public KCancelSafeQueue<T>
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	KCancelSafeListBase()
	{
		KeInitializeSpinLock(&m_Lock);
		InitializeListHead(&m_ListHead);
	}

	~KCancelSafeListBase() {}

	// Dequeues all the irps from the list
	VOID CleanUp(PVOID Context);

protected:
	VOID OnInsertIrp(KIrp I) { InsertTailList(&m_ListHead, &I->Tail.Overlay.ListEntry); }
	VOID OnRemoveIrp(KIrp I) { RemoveEntryList(&I->Tail.Overlay.ListEntry); }
	VOID OnAcquireLock(PKIRQL Irql) { KeAcquireSpinLock(&m_Lock, Irql); }
	VOID OnReleaseLock(KIRQL Irql) { KeReleaseSpinLock(&m_Lock, Irql); }
	VOID OnCompleteCanceledIrp(KIrp I) { I.Information() = 0;
#if _WDM_
										I.PnpComplete(STATUS_CANCELLED); }
#else
										I.Complete(STATUS_CANCELLED); }
#endif
	PIRP OnPeekNextIrp(KIrp I, PVOID Context);

public:
	CANCEL_SAFE_HANDLERS(KCancelSafeList)

protected:
    KSPIN_LOCK	m_Lock;				// the queue is protected by the SpinLock
	LIST_ENTRY  m_ListHead;			// IRPs are stored in a list
};

class KCancelSafeList : public KCancelSafeListBase<KCancelSafeList>
{
public:
	SAFE_DESTRUCTORS;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//  KCancelSafeListBase::OnPeekNextIrp
//
//	Routine Description:
//		The system uses this function to get a pointer to the next IRP
//		matching Context in the queue
//
//	Parameters:
//		[in] I
//			starting Irp
//
//		[in] Context
//			Context to match
//
//	Return Value:
//		PIRP - found Irp
//
//	Comments:
//		This function is called by IoCsqRemoveNextIrp, which passes the
//		Context value it receives from the caller
//
template <class T>
PIRP KCancelSafeListBase<T>::OnPeekNextIrp(KIrp I, PVOID Context)
{
	PLIST_ENTRY next;
	KIrp		NextIrp = NULL;

	if ( I.IsNull() )
		next = m_ListHead.Flink;
	else
		next = I->Tail.Overlay.ListEntry.Flink;

	while ( next != &m_ListHead )
	{
		NextIrp = CONTAINING_RECORD(next, IRP, Tail.Overlay.ListEntry);

		if ( !Context || (NextIrp.FileObject() == (PFILE_OBJECT)Context) )
			break;

		NextIrp = NULL;
		next = next->Flink;
	}

	return NextIrp;
}

////////////////////////////////////////////////////////////////////////
//  KCancelSafeListBase::CleanUp
//
//	Routine Description:
//		This routine removes all irps from the queue and completes them
//
//	Parameters:
//		[in] Context
//			Context to match
//
//	Return Value:
//		None
//
//	Comments:
//		None
//
template <class T>
VOID KCancelSafeListBase<T>::CleanUp(PVOID Context)
{
	PIRP Irp;

	while ( Irp = RemoveNextIrp(Context) )
	{
		OnCompleteCanceledIrp(Irp);
	}
}

#endif // NTDDI_VERSION
#endif // __KCSQ_H__
