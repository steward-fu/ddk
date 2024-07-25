// kfifo.h - include file for template class KFifo and KLockableFifo
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

#ifndef __KFIFO__
#define __KFIFO__

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class _fifocore
{

// Construction
protected:
	SAFE_DESTRUCTORS
	_fifocore(void);
	_fifocore(ULONG ItemSize, ULONG MaxItems, POOL_TYPE PoolType);
	_fifocore(ULONG ItemSize, ULONG BufferItems, PVOID SuppliedBuffer);
	VOID _initialize(ULONG ItemSize, ULONG MaxItems, POOL_TYPE PoolType);
	VOID _initialize(ULONG ItemSize, ULONG BufferItems, PVOID SuppliedBuffer);
	~_fifocore(void){ if (IsValid()) Invalidate(); }
private:
	_fifocore(_fifocore&); // do not allow copy
	_fifocore& operator=(_fifocore&); // do not allow assignment
public:
	VOID Invalidate(void);
	BOOLEAN IsValid(void){ return (m_pBuffer != NULL); }

// Methods
protected:
	ULONG	_read(PVOID pBuffer, ULONG count);
	ULONG	_write(PVOID pBuffer, ULONG count);
	VOID	_flush(void);
	ULONG	_numberOfItemsAvailableForRead(void);
	ULONG	_numberOfItemsAvailableForWrite(void);
	BOOLEAN	_isEmpty(void);
	BOOLEAN	_isFull(void);
public:
	ULONG	BufferItemCapacity(void) { return m_bufSize-1; }

// Data
protected:
	BOOLEAN m_Allocated;
	ULONG m_itemSize;
	ULONG m_head;
	ULONG m_tail;
	ULONG m_bufSize;
	PUCHAR m_pBuffer;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template <class T> class KFifo : public _fifocore
{

// Construction
public:
	SAFE_DESTRUCTORS
	KFifo(ULONG MaxItems, POOL_TYPE PoolType);
	KFifo(ULONG BufferItems, PVOID pBuffer);
	KFifo(void);
	VOID Initialize(ULONG MaxItems, POOL_TYPE PoolType);
	VOID Initialize(ULONG BufferItems, PVOID pBuffer);

// Methods
public:
	ULONG	Read(T* pItems, ULONG count);
	ULONG	Write(T* pItems, ULONG count);
	VOID	Flush(void);
	ULONG	NumberOfItemsAvailableForRead(void);
	ULONG	NumberOfItemsAvailableForWrite(void);
	BOOLEAN	IsEmpty(void);
	BOOLEAN	IsFull(void);
private:
	KFifo(KFifo&); // do not allow copy
	KFifo& operator=(KFifo&); // do not allow assignment
};

/////////////////////////////////////////////////////////////////////

template <class T> class KLockableFifo : public KFifo<T>, public KSpinLock
{

// Construction
public:
	SAFE_DESTRUCTORS
	KLockableFifo(ULONG MaxItems, POOL_TYPE PoolType=NonPagedPool);
	KLockableFifo(ULONG BufferItems, PVOID pBuffer);
	KLockableFifo(void);
	VOID Initialize(ULONG MaxItems, POOL_TYPE PoolType);
	VOID Initialize(ULONG BufferItems, PVOID pBuffer);

	VOID Invalidate(void){ KFifo<T>::Invalidate(); }
	BOOLEAN IsValid(void){ return KFifo<T>::IsValid(); }


// Methods
public:
	ULONG LockedRead(T* pItems, ULONG count);
	ULONG LockedWrite(T* pItems, ULONG count);
	VOID  LockedFlush(void);
	ULONG LockedNumberOfItemsAvailableForRead(void);
	ULONG LockedNumberOfItemsAvailableForWrite(void);
private:
	KLockableFifo(KLockableFifo&); // do not allow copy
	KLockableFifo& operator=(KLockableFifo&); // do not allow assignment
};

/////////////////////////////////////////////////////////////////////

class _isafefifo : public _fifocore
{

// Construction
protected:
	SAFE_DESTRUCTORS
	_isafefifo(void);
	_isafefifo(
		ULONG ItemSize,
		ULONG MaxItems,
		PKINTERRUPT pIntr
		);
	_isafefifo(
		ULONG ItemSize,
		ULONG BufferItems,
		PVOID SuppliedBuffer,
		PKINTERRUPT pIntr
		);
private:
	_isafefifo(_isafefifo&); // do not allow copy
	_isafefifo& operator=(_isafefifo&); // do not allow assignment

// Methods
protected:
	static VOID Synch(PKSYNCHRONIZE_ROUTINE pSynchFunc,	...	);
	static BOOLEAN _synchRead(PVOID Context);
	static BOOLEAN _synchWrite(PVOID Context);
	static BOOLEAN _synchFlush(PVOID Context);
	static BOOLEAN _synchNRead(PVOID Context);
	static BOOLEAN _synchNWrite(PVOID Context);
	static BOOLEAN _synchIsEmpty(PVOID Context);
	static BOOLEAN _synchIsFull(PVOID Context);

// Data
protected:
	PKINTERRUPT m_intobj;
};

template <class T> class KInterruptSafeFifo : protected _isafefifo
{

// Construction
public:
	SAFE_DESTRUCTORS
	KInterruptSafeFifo(ULONG MaxItems, PKINTERRUPT pInterrupt);
	KInterruptSafeFifo(ULONG BufferItems, PVOID pBuffer,PKINTERRUPT pInterrupt);
	KInterruptSafeFifo(void);
	VOID Initialize(ULONG MaxItems, PKINTERRUPT pInterrupt);
	VOID Initialize(ULONG BufferItems, PVOID pBuffer, PKINTERRUPT pInterrupt);

// Methods
public:
	ULONG	Read(T* pItems, ULONG count, BOOLEAN AtDirql=FALSE);
	ULONG	Read(T& Item);
	ULONG	Write(T* pItems, ULONG count, BOOLEAN AtDirql=FALSE);
	ULONG	Write(T Item);
	VOID	Flush(BOOLEAN AtDirql=FALSE);
	ULONG	NumberOfItemsAvailableForRead(BOOLEAN AtDirql=FALSE);
	ULONG	NumberOfItemsAvailableForWrite(BOOLEAN AtDirql=FALSE);
	BOOLEAN	IsEmpty(BOOLEAN AtDirql=FALSE);
	BOOLEAN	IsFull(BOOLEAN AtDirql=FALSE);
	BOOLEAN Synchronize(PKSYNCHRONIZE_ROUTINE func, PVOID context);

	BOOLEAN IsValid(void){return (_fifocore::IsValid() && m_intobj);}

private:
	KInterruptSafeFifo(KInterruptSafeFifo&); // do not allow copy
	KInterruptSafeFifo& operator=(KInterruptSafeFifo&); // do not allow assignment
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template <class T>
	inline KFifo<T>::KFifo(ULONG MaxItems, POOL_TYPE PoolType) :
	_fifocore(sizeof(T), MaxItems, PoolType)
{
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline KFifo<T>::KFifo(ULONG BufferItems, PVOID pBuffer) :
	_fifocore(sizeof(T), BufferItems, pBuffer)
{
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline KFifo<T>::KFifo(void) :
	_fifocore()
{
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline VOID KFifo<T>::Initialize(ULONG MaxItems, POOL_TYPE PoolType)
{
	_initialize(sizeof(T), MaxItems, PoolType);
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline VOID KFifo<T>::Initialize(ULONG BufferItems, PVOID pBuffer)
{
	_initialize(sizeof(T), BufferItems, pBuffer);
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline ULONG KFifo<T>::Write(T* pItems, ULONG count)
{
	return _write(pItems, count);
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline ULONG KFifo<T>::Read(T* pItems, ULONG count)
{
	return _read(pItems, count);
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline VOID KFifo<T>::Flush(void)
{
	_flush();
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline ULONG KFifo<T>::NumberOfItemsAvailableForRead(void)
{
	return _numberOfItemsAvailableForRead();
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline ULONG KFifo<T>::NumberOfItemsAvailableForWrite(void)
{
	return _numberOfItemsAvailableForWrite();
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline BOOLEAN KFifo<T>::IsEmpty(void)
{
	return _isEmpty();
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline BOOLEAN KFifo<T>::IsFull(void)
{
	return _isFull();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template <class T>
	KLockableFifo<T>::KLockableFifo(ULONG MaxItems, POOL_TYPE PoolType) :
		KFifo<T>(MaxItems, PoolType)
{
}

/////////////////////////////////////////////////////////////////////
template <class T>
	KLockableFifo<T>::KLockableFifo(ULONG BufferItems, PVOID pBuffer) :
		KFifo<T>(BufferItems, pBuffer)
{
}

/////////////////////////////////////////////////////////////////////
template <class T>
	KLockableFifo<T>::KLockableFifo(void) :
		KFifo<T>()
{
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline VOID KLockableFifo<T>::Initialize(ULONG MaxItems, POOL_TYPE PoolType)
{
	KFifo<T>::Initialize(MaxItems, PoolType);
	KSpinLock::Initialize();
}

/////////////////////////////////////////////////////////////////////
template <class T>
	inline VOID KLockableFifo<T>::Initialize(ULONG BufferItems, PVOID pBuffer)
{
	KFifo<T>::Initialize(BufferItems, pBuffer);
	KSpinLock::Initialize();
}

/////////////////////////////////////////////////////////////////////
template <class T>
	ULONG KLockableFifo<T>::LockedRead(T* pItems, ULONG count)
{
	ULONG result;

	Lock();
	result = Read(pItems, count);
	Unlock();

	return result;
}

template <class T>
	ULONG KLockableFifo<T>::LockedWrite(T* pItems, ULONG count)
{
	ULONG result;

	Lock();
	result = Write(pItems, count);
	Unlock();

	return result;
}

template <class T>
	VOID KLockableFifo<T>::LockedFlush(void)
{
	Lock();
	Flush();
	Unlock();
}

template <class T>
	ULONG KLockableFifo<T>::LockedNumberOfItemsAvailableForRead(void)
{
	ULONG result;

	Lock();
	result = NumberOfItemsAvailableForRead();
	Unlock();

	return result;
}

template <class T>
	ULONG KLockableFifo<T>::LockedNumberOfItemsAvailableForWrite(void)
{
	ULONG result;

	Lock();
	result = NumberOfItemsAvailableForWrite();
	Unlock();

	return result;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
template <class T>
	KInterruptSafeFifo<T>::KInterruptSafeFifo(ULONG MaxItems, PKINTERRUPT pInterrupt) :
		_isafefifo(sizeof(T), MaxItems, pInterrupt)
{
}

template <class T>
	KInterruptSafeFifo<T>::KInterruptSafeFifo(
		ULONG BufferItems,
		PVOID pBuffer,
		PKINTERRUPT pInterrupt) :
	_isafefifo(sizeof(T), BufferItems, pBuffer, pInterrupt)
{
}

template <class T>
	KInterruptSafeFifo<T>::KInterruptSafeFifo(void) :
	_isafefifo()
{
}

template <class T>
	VOID KInterruptSafeFifo<T>::Initialize(ULONG MaxItems,
		PKINTERRUPT pInterrupt)
{
	m_intobj = pInterrupt;
	_initialize(sizeof(T), MaxItems, NonPagedPool);
}

template <class T>
	VOID KInterruptSafeFifo<T>::Initialize(ULONG BufferItems,
		PVOID pBuffer,
		PKINTERRUPT pInterrupt)
{
	m_intobj = pInterrupt;
	_initialize(sizeof(T), BufferItems, pBuffer);
}

template <class T>
	inline BOOLEAN KInterruptSafeFifo<T>::Synchronize(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		)
{
	return KeSynchronizeExecution(m_intobj, func, context);
}

////////////////////////////////////////////////////////////////////////
//
// In the following functions, parameter AtDirql is usually either
// defaulted to constant FALSE, or specified as constant TRUE. In a
// free build, the compiler will optimize out the "if" statement.
//
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

template <class T> inline
	ULONG KInterruptSafeFifo<T>::Read(T* pItems, ULONG count, BOOLEAN AtDirql)
{
	if (AtDirql)
		return _read(pItems, count);
	else
	{
		ULONG ulReturn;
		Synch(_synchRead, this,	pItems, count, &ulReturn);
		return ulReturn;
	}
}

template <class T> inline
	ULONG KInterruptSafeFifo<T>::Read(T& Item)
{
	ASSERT (KeGetCurrentIrql() > DISPATCH_LEVEL);

	if (m_head != m_tail)
	{
		Item = ((T*)m_pBuffer)[m_head];
		m_head++;

		if (m_head == m_bufSize)
			m_head = 0;

		return 1;
	}

	return 0;
}

template <class T> inline
	ULONG KInterruptSafeFifo<T>::Write(T* pItems, ULONG count, BOOLEAN AtDirql)
{
	if (AtDirql)
		return _write(pItems, count);
	else
	{
		ULONG ulReturn;
		Synch(_synchWrite, this, pItems, count, &ulReturn);
		return ulReturn;
	}
}

template <class T> inline
	 ULONG KInterruptSafeFifo<T>::Write(T Item)
{
	ASSERT (KeGetCurrentIrql() > DISPATCH_LEVEL);

	LONG d = (LONG)m_head - (LONG)m_tail;

	if ( (d != 1) && ( (1-d) != (LONG)m_bufSize) )
	{
		((T*)m_pBuffer)[m_tail] = Item;
		m_tail++;

		if (m_tail == m_bufSize)
			m_tail = 0;

		return 1;
	}
	else
		return 0;
}

template <class T> inline
	VOID KInterruptSafeFifo<T>::Flush(BOOLEAN AtDirql)
{
	if (AtDirql)
		_flush();
	else
		Synch(_synchFlush, this);
}

template <class T> inline
	ULONG KInterruptSafeFifo<T>::NumberOfItemsAvailableForRead(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _numberOfItemsAvailableForRead();
	else
	{
		ULONG n;
		Synch(_synchNRead, this, &n);
		return n;
	}
}

template <class T> inline
	ULONG KInterruptSafeFifo<T>::NumberOfItemsAvailableForWrite(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _numberOfItemsAvailableForWrite();
	else
	{
		ULONG n;
		Synch(_synchNWrite, this, &n);
		return n;
	}
}

template <class T> inline
	BOOLEAN KInterruptSafeFifo<T>::IsEmpty(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _isEmpty();
	else
	{
		BOOLEAN bEmpty;
		Synch(_synchIsEmpty, this, &bEmpty);
		return bEmpty;
	}
}

template <class T> inline
	BOOLEAN KInterruptSafeFifo<T>::IsFull(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _isFull();
	else
	{
		BOOLEAN bFull;
		Synch(_synchIsFull, this, &bFull);
		return bFull;
	}
}


inline int _min_(int a, int b)
{
	return (a < b) ? a : b;
}

#endif
