// KNdisList.h: interface for the KNdisList class.
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
// Do not include KList classes if compiled with DriverWorks
//

#if !defined(AFX_KNDISLIST_H__5551B5C7_4457_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISLIST_H__5551B5C7_4457_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// Redefinitions from DriverWorks to DriverNetworks
//////////////////////////////////////////////////////////////////////
#define	KNdisList				KList
#define	KNdisInterlockedList	KInterlockedList
#define	KNdisInterrupSafeList	KInterrupSafeList

// For builds including standard DriverWorks libraries, such as TDI clients, use the
// DriverWorks, definitions which are the same as the KNDIS definitions.
//
// For NDIS_WDM miniport drivers, use the KNDIS definitions since kndiswdm.lib does not
// include any KList definitions
#if !defined(__VDW_VERSION__) || defined(KNDIS_WDM_LIBBLD)

//////////////////////////////////////////////////////////////////////
// _listcore (internal use only) This is a base class for all lists.
//
class _listcore
{

// Construction
protected:
	SAFE_DESTRUCTORS
	_listcore(ULONG OffsetOfListEntry);
public:
	VOID Invalidate(void) {}
	BOOLEAN IsValid(){ return TRUE; }

// Methods
public:
	PVOID 	_head(void);
	PVOID 	_tail(void);
	PVOID 	_next(PVOID Item);
	PVOID 	_prev(PVOID Item);
	VOID 	_insertTail(PVOID Item);
	VOID 	_insertHead(PVOID Item);
	VOID 	_insertBefore(PVOID Item, PVOID BeforeThisItem);
	VOID 	_insertAfter(PVOID Item, PVOID AfterThisItem);
	PVOID 	_remove(PVOID Item);
	PVOID 	_removeTail(void);
	PVOID 	_removeHead(void);
	PVOID 	_find(PVOID Item, PVOID StartItem, BOOLEAN bForward);
	BOOLEAN	_isEmpty(void);
	ULONG 	_count(void);
	VOID 	_reset(void);
	PVOID	Container(PLIST_ENTRY pLE);
	PLIST_ENTRY ListEntryPointer(PVOID p);

// Data
protected:
	LIST_ENTRY m_head;
	ULONG_PTR m_ListEntryOffset;
};

//////////////////////////////////////////////////////////////////////
// KList
//
template <class T> class KList : protected _listcore
{

// Construction
public:
	SAFE_DESTRUCTORS
	KList(void);
	KList(ULONG OffsetOfListEntry);

// Methods
public:
	T*		Head(void);
	T*		HeadNoLock(void);
	T* 		Tail(void);
	T* 		TailNoLock(void);
	T* 		Next(T* Item);
	T* 		NextNoLock(T* Item);
	T*		Prev(T* Item);
	T*		PrevNoLock(T* Item);
	VOID	InsertTail(T* Item);
	VOID	InsertTailNoLock(T* Item);
	VOID	InsertHead(T* Item);
	VOID	InsertHeadNoLock(T* Item);
	VOID 	InsertBefore(T* Item, T* BeforeThisItem);
	VOID 	InsertBeforeNoLock(T* Item, T* BeforeThisItem);
	VOID	InsertAfter(T* Item, T* AfterThisItem);
	VOID	InsertAfterNoLock(T* Item, T* AfterThisItem);
	T* 		Remove(T* Item);
	T* 		RemoveNoLock(T* Item);
	T* 		RemoveTail(void);
	T* 		RemoveTailNoLock(void);
	T* 		RemoveHead(void);
	T* 		RemoveHeadNoLock(void);
	T*		Find(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	T*		FindNoLock(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmpty(void);
	BOOLEAN	IsEmptyNoLock(void);
	ULONG 	Count(void);
	ULONG 	CountNoLock(void);
	VOID 	Reset(void);
	VOID 	ResetNoLock(void);
	virtual VOID Lock(void);
	virtual VOID Unlock(void);

	T*		HeadAtDpc(void);
	T* 		TailAtDpc(void);
	T* 		NextAtDpc(T* Item);
	T*		PrevAtDpc(T* Item);
	VOID	InsertTailAtDpc(T* Item);
	VOID	InsertHeadAtDpc(T* Item);
	VOID 	InsertBeforeAtDpc(T* Item, T* BeforeThisItem);
	VOID	InsertAfterAtDpc(T* Item, T* AfterThisItem);
	T* 		RemoveAtDpc(T* Item);
	T* 		RemoveTailAtDpc(void);
	T* 		RemoveHeadAtDpc(void);
	T*		FindAtDpc(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmptyAtDpc(void);
	ULONG 	CountAtDpc(void);
	VOID 	ResetAtDpc(void);
	virtual VOID LockAtDpc(void);
	virtual VOID UnlockAtDpc(void);

	using _listcore::IsValid;
	using _listcore::Invalidate;
};

//////////////////////////////////////////////////////////////////////
// KInterlockedList
//
template <class T> class KInterlockedList : protected _listcore
{

// Construction
public:
	SAFE_DESTRUCTORS
	KInterlockedList(void);
	KInterlockedList(ULONG OffsetOfListEntry);

// Methods
public:
	T*		Head(void);
	T*		HeadNoLock(void);
	T* 		Tail(void);
	T* 		TailNoLock(void);
	T* 		Next(T* Item);
	T* 		NextNoLock(T* Item);
	T*		Prev(T* Item);
	T*		PrevNoLock(T* Item);
	VOID	InsertTail(T* Item);
	VOID	InsertTailNoLock(T* Item);
	VOID	InsertHead(T* Item);
	VOID	InsertHeadNoLock(T* Item);
	VOID 	InsertBefore(T* Item, T* BeforeThisItem);
	VOID 	InsertBeforeNoLock(T* Item, T* BeforeThisItem);
	VOID	InsertAfter(T* Item, T* AfterThisItem);
	VOID	InsertAfterNoLock(T* Item, T* AfterThisItem);
	T* 		Remove(T* Item);
	T* 		RemoveNoLock(T* Item);
	T* 		RemoveTail(void);
	T* 		RemoveTailNoLock(void);
	T* 		RemoveHead(void);
	T* 		RemoveHeadNoLock(void);
	T*		Find(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	T*		FindNoLock(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmpty(void);
	BOOLEAN	IsEmptyNoLock(void);
	ULONG 	Count(void);
	ULONG 	CountNoLock(void);
	VOID 	Reset(void);
	VOID 	ResetNoLock(void);
	VOID	Lock(void);
	VOID	Unlock(void);

	T*		HeadAtDpc(void);
	T* 		TailAtDpc(void);
	T* 		NextAtDpc(T* Item);
	T*		PrevAtDpc(T* Item);
	VOID	InsertTailAtDpc(T* Item);
	VOID	InsertHeadAtDpc(T* Item);
	VOID 	InsertBeforeAtDpc(T* Item, T* BeforeThisItem);
	VOID	InsertAfterAtDpc(T* Item, T* AfterThisItem);
	T* 		RemoveAtDpc(T* Item);
	T* 		RemoveTailAtDpc(void);
	T* 		RemoveHeadAtDpc(void);
	T*		FindAtDpc(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmptyAtDpc(void);
	ULONG 	CountAtDpc(void);
	VOID 	ResetAtDpc(void);
	VOID	LockAtDpc(void);
	VOID	UnlockAtDpc(void);

	using _listcore::IsValid;
	using _listcore::Invalidate;

// Data
protected:
	KNdisSpinLock	m_lock;
};

#ifdef NDIS_MINIPORT_DRIVER
//////////////////////////////////////////////////////////////////////
// _isafelist (internal use only)
//
class _isafelist : protected _listcore
{

// Construction
protected:
	SAFE_DESTRUCTORS
	_isafelist(KNdisInterrupt* pInterruptObject, ULONG OffsetOfListEntry);
	_isafelist(ULONG OffsetOfListEntry);
public:
	VOID Initialize(KNdisInterrupt* pInterruptObject);
	VOID Invalidate(void){ m_intobj = NULL; }
	BOOLEAN IsValid(void){ return (m_intobj != NULL); }

// Methods
protected:
	static BOOLEAN _synchHead(PVOID Context);
	static BOOLEAN _synchTail(PVOID Context);
	static BOOLEAN _synchNext(PVOID Context);
	static BOOLEAN _synchPrev(PVOID Context);
	static BOOLEAN _synchInsertTail(PVOID Context);
	static BOOLEAN _synchInsertHead(PVOID Context);
	static BOOLEAN _synchInsertBefore(PVOID Context);
	static BOOLEAN _synchInsertAfter(PVOID Context);
	static BOOLEAN _synchRemoveHead(PVOID Context);
	static BOOLEAN _synchRemoveTail(PVOID Context);
	static BOOLEAN _synchRemove(PVOID Context);
	static BOOLEAN _synchFind(PVOID Context);
	static BOOLEAN _synchIsEmpty(PVOID Context);
	static BOOLEAN _synchCount(PVOID Context);
	static BOOLEAN _synchReset(PVOID Context);
	static VOID Synch(PKSYNCHRONIZE_ROUTINE pSynchFunc, ...);

// Data
protected:
	KNdisInterrupt* m_intobj;
};

//////////////////////////////////////////////////////////////////////
// KInterruptSafeList
//
template <class T> class KInterruptSafeList : protected _isafelist
{

// Construction
public:
	SAFE_DESTRUCTORS
	KInterruptSafeList(KNdisInterrupt* pInterruptObject);
	KInterruptSafeList(KNdisInterrupt* pInterruptObject, ULONG OffsetOfListEntry);
	KInterruptSafeList(void);
	KInterruptSafeList(ULONG OffsetOfListEntry);

// Methods
public:

	// Users of the class should set AtDirql to TRUE (constant)
	// when calling from ISRs or synch crit section routines. This
	// enables the compiler to generate optimal code that bypasses
	// the synchronization logic.
	T*		Head(BOOLEAN AtDirql=FALSE);
	T*		Tail(BOOLEAN AtDirql=FALSE);
	T*		Next(T* Item, BOOLEAN AtDirql=FALSE);
	T*		Prev(T* Item, BOOLEAN AtDirql=FALSE);
	VOID	InsertTail(T* Item, BOOLEAN AtDirql=FALSE);
	VOID 	InsertHead(T* Item, BOOLEAN AtDirql=FALSE);
	VOID	InsertBefore(T* Item, T* BeforeThisItem, BOOLEAN AtDirql=FALSE);
	VOID	InsertAfter(T* Item, T* AfterThisItem, BOOLEAN AtDirql=FALSE);
	T* 		RemoveHead(BOOLEAN AtDirql=FALSE);
	T*		RemoveTail(BOOLEAN AtDirql=FALSE);
	T*		Remove(T* Item, BOOLEAN AtDirql=FALSE);
	T*		Find(T* Item, BOOLEAN AtDirql = FALSE, T* StartItem = NULL,
		BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmpty(BOOLEAN AtDirql=FALSE);
	ULONG	Count(BOOLEAN AtDirql=FALSE);
	VOID	Reset(BOOLEAN AtDirql=FALSE);
};
#endif //NDIS_MINIPORT_DRIVER

////////////////////////////////////////////////////////////////////////
// Generic offset determinator for type T
template <class T> inline ULONG_PTR ListEntryOffset(T* p)
{
	return FIELD_OFFSET(T, m_ListEntry);
	UNREFERENCED_PARAMETER(p);
}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Offset determinator for IRP lists
#if NDIS_WDM
inline ULONG ListEntryOffset(IRP* Irp)
{
	return FIELD_OFFSET(IRP, Tail.Overlay.ListEntry);
	UNREFERENCED_PARAMETER(Irp);
}
#endif // NDIS_WDM
////////////////////////////////////////////////////////////////////////

template <class T> KList<T>::KList(ULONG OffsetOfListEntry) :
	_listcore(OffsetOfListEntry)
{
}

template <class T> KList<T>::KList(void) :
	_listcore(0)
{
	m_ListEntryOffset = ListEntryOffset( (T*)0 );
}

////////////////////////////////////////////////////////////////////////
inline PLIST_ENTRY _listcore::ListEntryPointer(PVOID p)
{
	return  (PLIST_ENTRY)((ULONG_PTR)p + m_ListEntryOffset);
}
////////////////////////////////////////////////////////////////////////
inline PVOID _listcore::Container(PLIST_ENTRY pLE)
{
	if (pLE == &m_head)
		return NULL;
	else
		return (PVOID)( (PCHAR)pLE - m_ListEntryOffset );
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Head(void)
{
	Lock();
	T* pT = (T*)_head();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::HeadNoLock(void)
{
	return (T*)_head();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Tail(void)
{
	Lock();
	T* pT = (T*)_tail();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::TailNoLock(void)
{
	return (T*)_tail();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertTail(T* Item)
{
	Lock();
	_insertTail(Item);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertTailNoLock(T* Item)
{
	_insertTail(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertHead(T* Item)
{
	Lock();
	_insertHead(Item);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertHeadNoLock(T* Item)
{
	_insertHead(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertBefore(T* Item, T* BeforeThisItem)
{
	Lock();
	_insertBefore(Item, BeforeThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertBeforeNoLock(T* Item, T* BeforeThisItem)
{
	_insertBefore(Item, BeforeThisItem);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertAfter(T* Item, T* AfterThisItem)
{
	Lock();
	_insertAfter(Item, AfterThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertAfterNoLock(T* Item, T* AfterThisItem)
{
	_insertAfter(Item, AfterThisItem);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Next(T* Item)
{
	Lock();
	T* pT = (T*)_next(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::NextNoLock(T* Item)
{
	return (T*)_next(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Prev(T* Item)
{
	Lock();
	T* pT = (T*)_prev(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::PrevNoLock(T* Item)
{
	return (T*)_prev(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Remove(T* Item)
{
	Lock();
	T* pT = (T*)_remove(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveNoLock(T* Item)
{
	return (T*)_remove(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KList<T>::IsEmpty(void)
{
	Lock();
	BOOLEAN empty = _isEmpty();
	Unlock();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KList<T>::IsEmptyNoLock(void)
{
	return _isEmpty();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveTail(void)
{
	Lock();
	T* pT = (T*)_removeTail();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveTailNoLock(void)
{
	return (T*)_removeTail();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveHead(void)
{
	Lock();
	T* pT = (T*)_removeHead();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveHeadNoLock(void)
{
	return (T*)_removeHead();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Find(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	Lock();
	T* pT = (T*)_find(Item, StartItem, bForward);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::FindNoLock(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	return (T*)_find(Item, StartItem, bForward);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KList<T>::Count(void)
{
	Lock();
	ULONG count = _count();
	Unlock();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KList<T>::CountNoLock(void)
{
	return _count();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::Reset(void)
{
	Lock();
	_reset();
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::ResetNoLock(void)
{
	_reset();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::Lock(void)
{
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::Unlock(void)
{
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::HeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_head();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::TailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_tail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertTailAtDpc(T* Item)
{
	LockAtDpc();
	_insertTail(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertHeadAtDpc(T* Item)
{
	LockAtDpc();
	_insertHead(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertBeforeAtDpc(T* Item, T* BeforeThisItem)
{
	LockAtDpc();
	_insertBefore(Item, BeforeThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertAfterAtDpc(T* Item, T* AfterThisItem)
{
	LockAtDpc();
	_insertAfter(Item, AfterThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::NextAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_next(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::PrevAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_prev(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_remove(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KList<T>::IsEmptyAtDpc(void)
{
	LockAtDpc();
	BOOLEAN empty = _isEmpty();
	UnlockAtDpc();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveTailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeTail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveHeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeHead();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::FindAtDpc(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	LockAtDpc();
	T* pT = (T*)_find(Item, StartItem, bForward);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KList<T>::CountAtDpc(void)
{
	LockAtDpc();
	ULONG count = _count();
	UnlockAtDpc();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::ResetAtDpc(void)
{
	LockAtDpc();
	_reset();
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::LockAtDpc(void)
{
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::UnlockAtDpc(void)
{
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
template <class T> KInterlockedList<T>::KInterlockedList(void) :
	_listcore(0),
	m_lock()
{
	m_ListEntryOffset = ListEntryOffset( (T*)0 );
}
////////////////////////////////////////////////////////////////////////
template <class T> KInterlockedList<T>::KInterlockedList(ULONG OffsetOfListEntry) :
	_listcore(OffsetOfListEntry),
	m_lock()
{
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::Lock(void)
{
	m_lock.Lock();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::Unlock(void)
{
	m_lock.Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Head(void)
{
	Lock();
	T* pT = (T*)_head();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::HeadNoLock(void)
{
	return (T*)_head();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Tail(void)
{
	Lock();
	T* pT = (T*)_tail();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::TailNoLock(void)
{
	return (T*)_tail();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertTail(T* Item)
{
	Lock();
	_insertTail(Item);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertTailNoLock(T* Item)
{
	_insertTail(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertHead(T* Item)
{
	Lock();
	_insertHead(Item);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertHeadNoLock(T* Item)
{
	_insertHead(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertBefore(T* Item, T* BeforeThisItem)
{
	Lock();
	_insertBefore(Item, BeforeThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertBeforeNoLock(T* Item, T* BeforeThisItem)
{
	_insertBefore(Item, BeforeThisItem);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertAfter(T* Item, T* AfterThisItem)
{
	Lock();
	_insertAfter(Item, AfterThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertAfterNoLock(T* Item, T* AfterThisItem)
{
	_insertAfter(Item, AfterThisItem);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Next(T* Item)
{
	Lock();
	T* pT = (T*)_next(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::NextNoLock(T* Item)
{
	return (T*)_next(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Prev(T* Item)
{
	Lock();
	T* pT = (T*)_prev(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::PrevNoLock(T* Item)
{
	return (T*)_prev(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Remove(T* Item)
{
	Lock();
	T* pT = (T*)_remove(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveNoLock(T* Item)
{
	return (T*)_remove(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KInterlockedList<T>::IsEmpty(void)
{
	Lock();
	BOOLEAN empty = _isEmpty();
	Unlock();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KInterlockedList<T>::IsEmptyNoLock(void)
{
	return _isEmpty();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveTail(void)
{
	Lock();
	T* pT = (T*)_removeTail();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveTailNoLock(void)
{
	return (T*)_removeTail();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveHead(void)
{
	Lock();
	T* pT = (T*)_removeHead();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveHeadNoLock(void)
{
	return (T*)_removeHead();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Find(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	Lock();
	T* pT = (T*)_find(Item, StartItem, bForward);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::FindNoLock(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	return (T*)_find(Item, StartItem, bForward);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KInterlockedList<T>::Count(void)
{
	Lock();
	ULONG count = _count();
	Unlock();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KInterlockedList<T>::CountNoLock(void)
{
	return _count();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::Reset(void)
{
	Lock();
	_reset();
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::ResetNoLock(void)
{
	_reset();
}

////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::LockAtDpc(void)
{
	m_lock.LockDpr();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::UnlockAtDpc(void)
{
	m_lock.UnlockDpr();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::HeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_head();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::TailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_tail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertTailAtDpc(T* Item)
{
	LockAtDpc();
	_insertTail(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertHeadAtDpc(T* Item)
{
	LockAtDpc();
	_insertHead(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertBeforeAtDpc(T* Item, T* BeforeThisItem)
{
	LockAtDpc();
	_insertBefore(Item, BeforeThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertAfterAtDpc(T* Item, T* AfterThisItem)
{
	LockAtDpc();
	_insertAfter(Item, AfterThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::NextAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_next(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::PrevAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_prev(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_remove(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KInterlockedList<T>::IsEmptyAtDpc(void)
{
	LockAtDpc();
	BOOLEAN empty = _isEmpty();
	UnlockAtDpc();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveTailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeTail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveHeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeHead();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::FindAtDpc(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	LockAtDpc();
	T* pT = (T*)_find(Item, StartItem, bForward);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KInterlockedList<T>::CountAtDpc(void)
{
	LockAtDpc();
	ULONG count = _count();
	UnlockAtDpc();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::ResetAtDpc(void)
{
	LockAtDpc();
	_reset();
	UnlockAtDpc();
}

#ifdef NDIS_MINIPORT_DRIVER

////////////////////////////////////////////////////////////////////////
template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList(KNdisInterrupt* pInterruptObject) :
	_isafelist(pInterruptObject, ListEntryOffset((T*)0))
{
}

template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList(
		KNdisInterrupt* pInterruptObject,
		ULONG OffsetOfListEntry) :
	_isafelist(pInterruptObject, OffsetOfListEntry)
{
}

template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList(void) :
	_isafelist(ListEntryOffset((T*)0))
{
}

template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList( ULONG OffsetOfListEntry) :
	_isafelist(OffsetOfListEntry)
{
}
////////////////////////////////////////////////////////////////////////
//
// In the following functions, parameter AtDirql is usually either
// defaulted to constant FALSE, or specified as constant TRUE. In a
// free build, the compiler will optimize out the "if" statement.
//
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

template <class T>	inline
	BOOLEAN KInterruptSafeList<T>::IsEmpty(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _isEmpty();
	else
	{
		BOOLEAN bReturn;
		Synch(
			_synchIsEmpty,
			this,
			&bReturn
			);
		return bReturn;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertTail(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertTail(Item);
	else
		Synch(
			_synchInsertTail,
			this,
			Item
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertHead(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertHead(Item);
	else
		Synch(
			_synchInsertHead,
			this,
			Item
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertBefore(T* Item, T* BeforeThisItem, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertBefore(Item, BeforeThisItem);
	else
		Synch(
			_synchInsertBefore,
			this,
			Item,
			BeforeThisItem
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertAfter(T* Item, T* AfterThisItem, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertAfter(Item, AfterThisItem);
	else
		Synch(
			_synchInsertAfter,
			this,
			Item,
			AfterThisItem
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Head(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_head();
	else
	{
		T* h;
		Synch(_synchHead, this,	&h);
		return h;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Tail(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_tail();
	else
	{
		T* t;
		Synch(_synchTail, this, &t);
		return t;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::RemoveHead(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_removeHead();
	else
	{
		T* r;
		Synch(_synchRemoveHead, this, &r);
		return r;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::RemoveTail(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_removeTail();
	else
	{
		T* r;
		Synch(_synchRemoveTail, this, &r);
		return r;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Next(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_next(Item);
	else
	{
		T* n;
		Synch(_synchNext, this, Item, &n);
		return n;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Prev(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_prev(Item);
	else
	{
		T* p;
		Synch(_synchPrev, this, Item, &p);
		return p;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Find(T* Item, BOOLEAN AtDirql, T* StartItem,
	BOOLEAN bForward)
{
	if (AtDirql)
		return (T*)_find(Item, StartItem, bForward);
	else
	{
		T* n;
		Synch(_synchFind, this, Item, StartItem, bForward, &n);
		return n;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Remove(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_remove(Item);
	else
	{
		Synch(_synchRemove, this, Item);
		return Item;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T>	inline
	ULONG KInterruptSafeList<T>::Count(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _count();
	else
	{
		ULONG c;
		Synch(_synchCount, this, &c);
		return c;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::Reset(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _reset();
	else
		Synch(_synchReset, this);
}

#endif // NDIS_MINIPORT_DRIVER

#endif // __VDW_VERSION__
#endif // !defined(AFX_KNDISLIST_H__5551B5C7_4457_11D3_8FA3_00C04F7445B7__INCLUDED_)
