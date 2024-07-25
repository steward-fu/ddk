// karray.h - a simple vector template
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

#if !defined(__KARRAY__)
#define __KARRAY__

////////////////////////////////////////////////////////
// Helper classes for locking
//
// class KArray takes a "lock class" as a parameter. The
// requirements for the lock class are:
//
//  1. It has a default constructor.
//  2. It has members
//			void Lock(void)
//			void Unlock(void)
//
//  See kgenlock.h for classes that meet these criteria.
//

//////////////////////////////////////////////////////////////
template <class T, class L=KGenericLockNop> class KArray
{
//  data members

protected:
	ULONG 		m_CurrentElementCount;
	ULONG 		m_GrowByElementCount;
	POOL_TYPE 	m_Pool;
	int			m_MaxInserted;
	T* 			m_Data;
	NTSTATUS 	m_Status;
	L			m_Lock;

public:

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::KArray
//		Ctor for template class KArray
//
//	Parameters:
//		InitialElementCount
//			Initial size of array to allocate
//
//		GrowByCount
//			Number of elements to grow when grow required
//
//		pool
//			Pool to use for allocation. Default is PagedPool
//
//	Comments:
//		Use member function Status to determine if the construction
//		was successful.
//
	KArray(
		ULONG InitialElementCount,
		ULONG GrowByCount,
		POOL_TYPE pool = PagedPool
		) :

		m_CurrentElementCount(0),
		m_GrowByElementCount(GrowByCount),
		m_Pool(pool),
		m_MaxInserted(-1),
		m_Data((T*) new (pool) CHAR[InitialElementCount*sizeof(T)]),
		m_Status(STATUS_SUCCESS)
	{
		if ( (m_Data != NULL) || (InitialElementCount == 0) )
		{
			m_CurrentElementCount = InitialElementCount;
			RtlZeroMemory( m_Data, InitialElementCount*sizeof(T));
		}
		else
		{
			m_Status = STATUS_INSUFFICIENT_RESOURCES;
			m_GrowByElementCount = 0;
		}
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::KArray
//		Default ctor
//
//	Comments:
//		Call member function Initialize before using the array.
//
	KArray(void) :
		m_CurrentElementCount(0),
		m_GrowByElementCount(0),
		m_Pool(PagedPool),
		m_MaxInserted(-1),
		m_Data(NULL),
		m_Status(STATUS_UNSUCCESSFUL)
	{
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::Initialize
//		Initialize template class KArray
//
//	Parameters:
//		InitialElementCount
//			Initial size of array to allocate
//
//		GrowByCount
//			Number of elements to grow when grow required
//
//		pool
//			Pool to use for allocation. Default is PagedPool
//
//	Returns
//		Returns status.
//
//	Comments:
//		Assumes that the default constructor or Invalidate was run.
//
	NTSTATUS Initialize(
		ULONG InitialElementCount,
		ULONG GrowByCount,
		POOL_TYPE pool = PagedPool
		)
	{
		m_CurrentElementCount = 0;
		m_GrowByElementCount = GrowByCount;
		m_Pool = pool;
		m_MaxInserted = -1;
		m_Status = STATUS_SUCCESS;

		m_Data = (T*) new (pool) CHAR[InitialElementCount*sizeof(T)];

		if ( (m_Data != NULL)  || (InitialElementCount == 0) )
		{
			m_CurrentElementCount = InitialElementCount;
			RtlZeroMemory( m_Data, InitialElementCount*sizeof(T));
		}
		else
		{
			m_Status = STATUS_INSUFFICIENT_RESOURCES;
			m_GrowByElementCount = 0;
		}

		return m_Status;
	}
///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::~KArray
//		Destructor for class KArray
//
//	Comments:
//		Invokes member function Invalidate.
//
	~KArray(void)
	{
		if (IsValid())
		{
			Invalidate();
		}
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::Invalidate
//		Destructor for class KArray
//
//	Comments:
//		Releases storage and zeroes the counts.
//
	VOID Invalidate(void)
	{
		m_Lock.Lock();
		delete [] m_Data;
		m_Data = NULL;
		m_GrowByElementCount = 0;
		m_CurrentElementCount = 0;
		m_Lock.Unlock();
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::IsValid
//		Tests if the object has been initialized
//
//	Comments:
//		The test is on the data array and the 'grow by' count.
//
	BOOLEAN IsValid(void)
	{
		return (m_Data != NULL) || (m_GrowByElementCount != 0);
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::Resize
//		Resize to accommodate given index
//
//	Parameters:
//		index
//			Zero based index that the array must accommodate
//
//		DoLock
//			TRUE if the function should take the lock before performing the operation.
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		If the array is not currently large enough, the function attempts to
//		allocate new storage, to which it then copies the existing data. The
//		copy operation is coded to enable invocation of copy ctors, if they
//		exist. The old storage is then deleted.
//
	NTSTATUS Resize(int index, BOOLEAN DoLock=TRUE)
	{
		if (DoLock) m_Lock.Lock();

		m_Status = STATUS_SUCCESS;

		if ( ULONG(index) >= m_CurrentElementCount )
		{
			// compute how many more array elements are needed
			ULONG nRequired = (index+1) - m_CurrentElementCount;

			// round up the delta to multiple of grow-by count
			nRequired = ((nRequired+m_GrowByElementCount-1)/m_GrowByElementCount) * m_GrowByElementCount;

			// sum for the new total element count
			ULONG NewElementCount = m_CurrentElementCount +  nRequired;

			// allocate memory for the new array
			CHAR* temp = new (m_Pool) CHAR[NewElementCount*sizeof(T)];

			// if alloc succeeded, copy the existing data to the new store
			if ( temp != NULL )
			{
				for (int i=0; ULONG(i) < m_CurrentElementCount; i++)
				{
					((T*)temp)[i] = m_Data[i];
				}

				m_CurrentElementCount = NewElementCount;

				// free the old array and update the data pointer
				delete [] m_Data;
				m_Data = (T*)temp;
			}
			else
			{
				m_Status = STATUS_INSUFFICIENT_RESOURCES;
			}
		}

		if (DoLock) m_Lock.Unlock();
		return m_Status;
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::Insert
//		Insert an element into the array
//
//	Parameters:
//		index
//			zero based index at which to insert element
//
//		element
//			element to insert
//
//		DoLock
//			TRUE if the function should take the lock before performing the operation.
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		Assumes that if there is a copy ctor of T, then it will do the right thing.
//
	NTSTATUS Insert(int index, T& element, BOOLEAN DoLock=TRUE)
	{
		if (DoLock) m_Lock.Lock();

		if ( NT_SUCCESS( Resize(index, FALSE) ) )
		{
			m_Data[index] = element;

			if ( index > m_MaxInserted )
				m_MaxInserted = index;
		}

		if (DoLock) m_Lock.Unlock();

		return m_Status;
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::InsertNext
//		Insert an element into the next available slot
//
//	Parameters:
//		element
//			Element to be inserted.
//
//		pIndex
//			Points to variable that receives the index at which the element
//			is stored.
//
//		DoLock
//			TRUE if the function should take the lock before performing the operation.
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		This function inserts the given element into the array at the position
//		following that of the previously inserted element with the greatest index.
//		Doing so may require resizing the allocation.
//
	NTSTATUS InsertNext(T element, PULONG pIndex, BOOLEAN DoLock=TRUE)
	{
		if (DoLock) m_Lock.Lock();

		Insert(m_MaxInserted+1, element, FALSE);
		if ( pIndex != NULL )
		{
			if ( NT_SUCCESS(m_Status) )
			{
				*pIndex = m_MaxInserted;
			}
			else
			{
				*pIndex = ULONG(-1);
			}
		}

		if (DoLock) m_Lock.Unlock();

		return m_Status;
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::MaxInserted
//		Determine the highest index that has been inserted
//
//	Return Value:
//		Returns the maximum of all indices of elements that have been
//		inserted since the last initialization of the array.
//
//	Comments:
//		Simple accessor. Caller may need to use Lock and Unlock around this
//		in order for it to be useful.
//
	int MaxInserted(void)
	{
		return m_MaxInserted;
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::Status
//		Retrieve status of last operation (including ctor)
//
//	Return Value:
//		Returns status of last operation.
//
//	Comments:
//		Simple accessor. Caller may need to use Lock and Unlock around this.
//
	NTSTATUS Status(void)
	{
		return m_Status;
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::operator []
//		Array element operator override
//
//	Parameters
//		index
//			zero based index of element to access
//
//	Return Value:
//		Returns reference to array element[index]
//
//	Comments:
//		This allows you to this:
//
//		KArray<mytype> MyArray(100,10);
//		. . .
//		mytype m = MyArray[i];
//
//		Note that you must use Insert or InsertNext for writing the elements
//
	T& operator [](int index)
	{
		m_Lock.Lock();

		ASSERT ( (index >= 0) && (index <= m_MaxInserted) );

		T& temp = m_Data[index];

		m_Lock.Unlock();

		return temp;
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::Data
//		Get a pointer to the actual array.
//
//	Return Value:
//		Returns a pointer to the first element of the array.
//
	T* Data(void) { return m_Data; }

///////////////////////////////////////////////////////////////////////////////
//
//	KArray<T,L>::Lock
//	KArray<T,L>::Unlock
//		Allow client to lock and unlock independently.
//
//	Comments:
//		See kgenlock.h for lock implementations.
//
	void Lock(void) 	{ m_Lock.Lock(); }
	void Unlock(void) 	{ m_Lock.Unlock(); }



///////////////////////////////////////////////////////////////////////////////
// STL Support
//
// To enable STL support, add
//	#include <iterator>
// prior to #include <vdw.h>

#ifdef __SGI_STL_ITERATOR
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef reverse_iterator<const_iterator, value_type, const_reference, difference_type>  const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type, reference, difference_type> reverse_iterator;

	iterator begin() { return m_Data; }
	const_iterator begin() const { return m_Data; }
	iterator end() { return m_Data+m_MaxInserted+1; }
	const_iterator end() const { return m_Data+m_MaxInserted+1; }

	reverse_iterator rbegin() 	{ return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const 	{ return const_reverse_iterator(end()); }
	reverse_iterator rend() 	{ return reverse_iterator(begin()); }
	const_reverse_iterator rend() const 	{ return const_reverse_iterator(begin()); }

	size_type size() const 	{ return size_type(end() - begin()); }
	size_type max_size() const 	{ return size_type(-1) / sizeof(T); }
	size_type capacity() const 	{ return size_type( (m_Data + m_CurrentElementCount) - begin()); }
	bool empty() const { return begin() == end(); }

	void push_back(const T& element) { InsertNext(element, NULL, FALSE); }
#endif // __SGI_STL_ITERATOR
};

#endif // __KARRAY__
