// kheap.h - include file for heap classes based on lookaside lists
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

#ifndef __KHEAP__
#define __KHEAP__

#if defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
// Exposed Globals
namespace _KNDIS {
	extern ULONG MemTag;	// Tag for NDIS allocations. Can be set to override default 'dnw_'
	// extern bool bW98;
};
#endif // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)

///////////////////////////////////////////////////////////////////////////////
// KHeap
//
template <class T> class KHeap
{
public:
	struct HeapBlock
	{
		KHeap<T>* m_pList;
		UCHAR m_t[sizeof(T)];
	};

// Construction
public:
	SAFE_DESTRUCTORS
	KHeap(void)
	{
		RtlZeroMemory(&m_Lal, sizeof(m_Lal));
	}

#if defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	KHeap(USHORT MaxItems, ULONG Tag=_KNDIS::MemTag)
#else // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	KHeap(USHORT MaxItems, ULONG Tag=DefaultPoolTag)
#endif // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	{
		RtlZeroMemory(&m_Lal, sizeof(m_Lal));
		ExInitializeNPagedLookasideList(
			&m_Lal,
			NULL,
			NULL,
			0,
			sizeof(HeapBlock),
			Tag,
			MaxItems
			);
	}

#if defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	VOID Initialize(USHORT MaxItems, ULONG Tag=_KNDIS::MemTag)
#else // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	VOID Initialize(USHORT MaxItems, ULONG Tag=DefaultPoolTag)
#endif // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	{
		if (IsValid())
		{
			ASSERT(m_Lal.L.Size == 0);
			Invalidate();
		}
		ExInitializeNPagedLookasideList(
			&m_Lal,
			NULL,
			NULL,
			0,
			sizeof(HeapBlock),
			Tag,
			MaxItems
			);
	}

	~KHeap(void){ if (IsValid()) Invalidate(); }

	VOID Invalidate(void)
	{
		if (m_Lal.L.Size != 0)
		{
			ExDeleteNPagedLookasideList(&m_Lal);
			RtlZeroMemory(&m_Lal, sizeof(m_Lal));
		}
	}

	BOOLEAN IsValid(void){ return (m_Lal.L.Size != 0); }

// Methods
public:
	PVOID alloc(void)
	{
		return ExAllocateFromNPagedLookasideList(&m_Lal);
	}
	VOID free(PVOID p)
	{
		ExFreeToNPagedLookasideList(&m_Lal, p);
	}

// Data
protected:
	NPAGED_LOOKASIDE_LIST m_Lal;
};

///////////////////////////////////////////////////////////////////////////////
// KHeapClient
//
template <class T> class KHeapClient
{
public:
	struct HeapBlock
	{
		KHeap<T>* m_pList;
		UCHAR m_t[1];
	};

// Construction
public:
	SAFE_DESTRUCTORS
#if defined(_IA64_) || defined(_AMD64_)
	void * __cdecl operator new(size_t nSize, KHeap<T>* pList)
#else
	void * __cdecl operator new(unsigned int nSize, KHeap<T>* pList)
#endif
	{
		HeapBlock* pBlock = (HeapBlock*)pList->alloc();
		if (pBlock)
		{
			pBlock->m_pList = pList;
			return pBlock->m_t;
		}
		else
			return NULL;
	}

	void __cdecl operator delete(void* p)
	{
		HeapBlock* q = (HeapBlock*)((PUCHAR)p - sizeof(KHeap<T>*));
		ASSERT ( (void*)q->m_t == p);

		q->m_pList->free(q);
	}
};

///////////////////////////////////////////////////////////////////////////////
// KPagedHeap
//

template <class T> class KPagedHeap
{
public:
	struct HeapBlock
	{
		KPagedHeap<T>* m_pList;
		UCHAR m_t[sizeof(T)];
	};

// Construction
public:
	SAFE_DESTRUCTORS
	KPagedHeap(void)
	{
		RtlZeroMemory(&m_Lal, sizeof(m_Lal));
	}

#if defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	KPagedHeap(USHORT MaxItems, ULONG Tag=_KNDIS::MemTag)
#else // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	KPagedHeap(USHORT MaxItems, ULONG Tag=DefaultPoolTag)
#endif // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	{
		RtlZeroMemory(&m_Lal, sizeof(m_Lal));
		ExInitializePagedLookasideList(
			&m_Lal,
			NULL,
			NULL,
			0,
			sizeof(HeapBlock),
			Tag,
			MaxItems
			);
	}

#if defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	VOID Initialize(USHORT MaxItems, ULONG Tag=_KNDIS::MemTag)
#else // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
    VOID Initialize(USHORT MaxItems, ULONG Tag=DefaultPoolTag)
#endif // defined(KNDIS_WDM_LIBBLD) || defined(NDIS_WDM)
	{
		if (IsValid())
		{
			ASSERT(m_Lal.L.Size == 0);
			Invalidate();
		}
		ExInitializePagedLookasideList(
			&m_Lal,
			NULL,
			NULL,
			0,
			sizeof(HeapBlock),
			Tag,
			MaxItems
			);
	}

	~KPagedHeap(void){ if (IsValid()) Invalidate(); }

	VOID Invalidate(void)
	{
		if (m_Lal.L.Size != 0)
		{
			ExDeletePagedLookasideList(&m_Lal);
			RtlZeroMemory(&m_Lal, sizeof(m_Lal));
		}
	}

	BOOLEAN IsValid(void){ return (m_Lal.L.Size != 0); }

// Methods
public:
	PVOID alloc(void)
	{
		return ExAllocateFromPagedLookasideList(&m_Lal);
	}

	VOID free(PVOID p)
	{
		ExFreeToPagedLookasideList(&m_Lal, p);
	}

// Data
protected:
	PAGED_LOOKASIDE_LIST m_Lal;
};


///////////////////////////////////////////////////////////////////////////////
// KPagedHeapClient
//

template <class T> class KPagedHeapClient
{
public:
	struct HeapBlock
	{
		KPagedHeap<T>* m_pList;
		UCHAR m_t[1];
	};

// Construction
public:
	SAFE_DESTRUCTORS
#if defined(_IA64_) || defined(_AMD64_)
	void * __cdecl operator new(size_t nSize, KPagedHeap<T>* pList)
#else
	void * __cdecl operator new(unsigned int nSize, KPagedHeap<T>* pList)
#endif
	{
		HeapBlock* pBlock = (HeapBlock*)pList->alloc();
		if (pBlock)
		{
			pBlock->m_pList = pList;
			return pBlock->m_t;
		}
		else
			return NULL;
	}

	void __cdecl operator delete(void* p)
	{
		HeapBlock* q = (HeapBlock*)((PUCHAR)p - sizeof(KPagedHeap<T>*));
		ASSERT ( (void*)q->m_t == p);

		q->m_pList->free(q);
	}
};

#endif // __KHEAP__
