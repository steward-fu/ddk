// KNdisSharedMemory.h: interface for the KNdisSharedMemory class.
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

#if !defined(AFX_KNDISSHAREDMEMORY_H__A00244F9_3B93_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISSHAREDMEMORY_H__A00244F9_3B93_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

//////////////////////////////////////////////////////////////////////
// KNdisSharedMemory
//
// Encapsulates NIC shared memory object
//
class KNdisSharedMemory
{
// Constructors/Destructor
public:
	// Normal allocation  (assumed that Length is total_buffer_length + alignment)
	//						alignment is 4 for DWORD
	inline KNdisSharedMemory(
		KNdisMiniAdapter* pAdapter,
		ULONG Length,
		ULONG Alignment=0,
		BOOLEAN Cached=FALSE
		);

	// Requests asynchronous allocation - Adapter::AllocateComplete() handler
	// will be called.  Client can call KNdisSharedMemory::AllocateComplete()
	// to initialize the pointer data members
	inline KNdisSharedMemory(
		KNdisMiniAdapter* pAdapter,
		ULONG Length,
		BOOLEAN Cached
		);

	// Client calls this method when asynchronous allocation is completed
	// Called by client from Adapter::AllocateComplete() handler
	inline static KNdisSharedMemory* AllocateComplete(
		IN PVOID VirtualAddress,
		IN PNDIS_PHYSICAL_ADDRESS PhysicalAddress,
		IN ULONG Length,
		IN PVOID Context
		);

	// Normal free
	inline ~KNdisSharedMemory();

// Methods
public:
	// Accessors
	inline BOOLEAN IsValid();
	inline NDIS_STATUS AllocationStatus();
	inline ULONG Length();
	inline BOOLEAN Cached();
	inline PVOID VirtualAddress();
	inline PNDIS_PHYSICAL_ADDRESS PhysicalAddress();

// Data
protected:
	KNdisMiniAdapter*		m_pAdapter;			// adapter
	ULONG					m_Length;			// length of the shared memory
    BOOLEAN					m_Cached;			// memory cacheable or not
    PVOID					m_VirtualAddress;	// base virtual address
    NDIS_PHYSICAL_ADDRESS	m_PhysicalAddress;	// base physical address

	PVOID					m_AlignedVirtualAddress;	// aligned base virtual address
	NDIS_PHYSICAL_ADDRESS	m_AlignedPhysicalAddress;	// aligned base physical address
	NDIS_STATUS				m_AllocationStatus;			// Status of Async allocation

	KNdisSharedMemory();
};


///////////////////////////////////////////////////////////////////////
// Constructor for synchronous allocation
//
// Parameters:
// 	pAdapter	- Adapter object pointer (usually, 'this')
//	Length		- Length of the shared memory to allocate
//	Alignment	- Value to align the memory, default = 0 no alignment
//	Cached		- TRUE allocate cached memory, FALSE(default) noncached
// Returns:
// 	none
// Comments:
// 	This form of the constructor will allocate shared memory using
//	NdisMAllocateSharedMemory() and optionally align the memory.
KNdisSharedMemory::KNdisSharedMemory(
	KNdisMiniAdapter* pAdapter,
	ULONG Length,
	ULONG Alignment,
	BOOLEAN Cached
	):	m_Length(Length),
			m_Cached(Cached),
				m_pAdapter(pAdapter),
					m_AllocationStatus(NDIS_STATUS_FAILURE)
{
	ASSERT( pAdapter );
	ASSERT( Length );

	NdisMAllocateSharedMemory(
		IN *pAdapter,
		IN Length,
		IN Cached,
		&m_VirtualAddress,
		&m_PhysicalAddress
		);

	m_AlignedPhysicalAddress = m_PhysicalAddress;
	m_AlignedVirtualAddress = m_VirtualAddress;

	if ( m_VirtualAddress )
	{
		m_AllocationStatus = NDIS_STATUS_SUCCESS;
		NdisZeroMemory( m_VirtualAddress, Length);

		if ( Alignment )
		{
			ULONG dwLowAddr = NdisGetPhysicalAddressLow(m_PhysicalAddress);

			// Check for overflow of low part
			if( dwLowAddr > (dwLowAddr + Alignment-1) )
			{
				NdisSetPhysicalAddressHigh( m_AlignedPhysicalAddress, NdisGetPhysicalAddressHigh(m_AlignedPhysicalAddress) + 1 );
			}

			dwLowAddr += Alignment - 1;
			dwLowAddr &= ~(Alignment - 1);

			NdisSetPhysicalAddressLow( m_AlignedPhysicalAddress, dwLowAddr);

			m_AlignedVirtualAddress = reinterpret_cast<PUCHAR>(m_VirtualAddress) + Alignment - 1;
			m_AlignedVirtualAddress = PVOID(reinterpret_cast<ULONG_PTR>(m_AlignedVirtualAddress) & ~((ULONG_PTR)Alignment - 1));
		}
	}
}


///////////////////////////////////////////////////////////////////////
// Constructor for asynchronous allocation
//
// Parameters:
// 	pAdapter	- Adapter object pointer (usually, 'this')
//	Length		- Length of the shared memory to allocate
//	Cached		- TRUE allocate cached memory, FALSE noncached
// Returns:
// 	none
// Comments:
// 	This form of the constructor requests asynchronous allocation of
//	shared memory.  If successful, the framework will call the
//	Adapter class' AllocateComplete handler.  The client should then
//	call KNdisSharedMemory::AllocateComplete to initialize the
//	virtual and physical address members.
KNdisSharedMemory::KNdisSharedMemory(
	KNdisMiniAdapter* pAdapter,
	ULONG Length,
	BOOLEAN Cached
	):	m_Length(Length),
			m_Cached(Cached),
				m_pAdapter(pAdapter)
{
	ASSERT( pAdapter );
	ASSERT( Length );

	// Request an allocation based on strategy
	m_AllocationStatus = NdisMAllocateSharedMemoryAsync(
		*m_pAdapter,
		m_Length,
		Cached,
		this
		);
}


///////////////////////////////////////////////////////////////////////
// Report that an allocation requested by this object is complete
//
// Parameters:
//	VirtualAddress	- base virtual address of the allocation
//	PhysicalAddress	- base physical address of the allocation
//	Length			- Length of the shared memory allocated
//	Context			- Usually a pointer to KNdisSharedMemory object (this)
// Returns:
//	KNdisSharedMemory* - The data member pointers were initialized
// 	NULL			   - Invalid pointers or length.  The client
//		should check the parameters and if valid, free the shared memory.
// Comments:
//	This method is called by clients that request asynchronous allocation
// 	of shared memory using the form of the constructor for asynchronous
//  allocation.  It is called from Adapter class' AllocateComplete
//	handler.  The client should check the return value for error condition.
KNdisSharedMemory* KNdisSharedMemory::AllocateComplete(
	IN PVOID VirtualAddress,
	IN PNDIS_PHYSICAL_ADDRESS PhysicalAddress,
	IN ULONG Length,
	IN PVOID Context
	)
{
	KNdisSharedMemory* pShMem = reinterpret_cast<KNdisSharedMemory*>(Context);

	if( NULL == pShMem || NULL == VirtualAddress || NULL == PhysicalAddress )
	{
		ASSERT(!"KNdisSharedMemory::AllocateComplete NULL pointer");
		return NULL;
	}

	if( pShMem->Length() != Length )
	{
		ASSERT(!"KNdisSharedMemory::AllocateComplete NULL pointer");
		return NULL;
	}

	pShMem->m_AlignedPhysicalAddress = *PhysicalAddress;
	pShMem->m_PhysicalAddress = *PhysicalAddress;
	pShMem->m_AlignedVirtualAddress = VirtualAddress;
	pShMem->m_VirtualAddress = VirtualAddress;

	return pShMem;
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
//	none
// Comments:
//	NdisMFreeSharedMemory will be called for a valid object.
KNdisSharedMemory::~KNdisSharedMemory()
{
	ASSERT( m_pAdapter );
	ASSERT( m_Length );
	ASSERT( m_VirtualAddress );

	if( m_pAdapter && m_Length && m_VirtualAddress )
	{
		NdisMFreeSharedMemory(
			*m_pAdapter,
			m_Length,
			m_Cached,
			m_VirtualAddress,
			m_PhysicalAddress
			);
	}
}


///////////////////////////////////////////////////////////////////////
// Test validity of the object
//
// Parameters:
// 	none
// Returns:
//	TRUE if the object contains valid pointers, FALSE if NULL pointers
// Comments:
//	If the allocation failed in the call to NdisAllocateSharedMemory,
//  the object would be invalid.
BOOLEAN KNdisSharedMemory::IsValid()
{
	return m_AlignedVirtualAddress != NULL;
}


///////////////////////////////////////////////////////////////////////
// Test allocation status of the object
//
// Parameters:
// 	none
// Returns:
//	TRUE if the object contains valid pointers, FALSE if NULL pointers
// Comments:
//	If the allocation failed in the call to the CTOR that calls
//	NdisMAllocateSharedMemoryAsync internally, the object would have a
//	status other than NDIS_STATUS_PENDING.
NDIS_STATUS KNdisSharedMemory::AllocationStatus()
{
	return m_AllocationStatus;
}


///////////////////////////////////////////////////////////////////////
// Get the length of the shared memory region
//
// Parameters:
// 	none
// Returns:
//	ULONG
// Comments:
//	none
ULONG KNdisSharedMemory::Length()
{
	return m_Length;
}


///////////////////////////////////////////////////////////////////////
// Returns whether the original allocation was cacheable
//
// Parameters:
// 	none
// Returns:
//	TRUE if the memory is cacheable, FALSE is non-cacheable
// Comments:
//	none
BOOLEAN KNdisSharedMemory::Cached()
{
	return m_Cached;
}


///////////////////////////////////////////////////////////////////////
// Get the base virtual address of the shared memory region
//
// Parameters:
// 	none
// Returns:
//	PVOID
// Comments:
//	none
PVOID KNdisSharedMemory::VirtualAddress()
{
	return m_AlignedVirtualAddress;
}


///////////////////////////////////////////////////////////////////////
// Get the base physical address of the shared memory region
//
// Parameters:
// 	none
// Returns:
//	PNDIS_PHYSICAL_ADDRESS
// Comments:
//	none
PNDIS_PHYSICAL_ADDRESS KNdisSharedMemory::PhysicalAddress()
{
	return &m_AlignedPhysicalAddress;
}

#endif // !defined(AFX_KNDISSHAREDMEMORY_H__A00244F9_3B93_11D3_8FA3_00C04F7445B7__INCLUDED_)
