// KNdisPacketPool.h: interface for the KNdisPacketPool class.
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

#if !defined(AFX_KNDISPACKETPOOL_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPACKETPOOL_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

//////////////////////////////////////////////////////////////////////
// KNdisPacketPool
//
// Abstracts NDIS Packet Pool.  Acts as a factory of KNdisPacket objects.
//
// NDIS drivers use KNdisPacketPool to Allocate() and Free() packet
// descriptors. Usually, drivers create KNdisPacketPool(s) during driver
// initialization and allocate packet descriptors (KNdisPacket) and
// buffer descriptors (KNdisBuffer) chained to the packet descroptors as needed.
//
// Generally, NDIS protects access to packet pools with spinlocks. Since
// acquiring a spinlock for each Allocate()/Free() could get fairly expensive,
// NDIS allows 2 alternative ways for packet pool access. AllocateDpr()/FreeDpr()
// is a faster way of using the pool; these functions are called by the driver
// if it is already at DISPATCH_LEVEL. The most effiecient way of using a
// packet pool is via AllocateDprNonInterlocked()/AllocateDprNonInterlocked(),
// when NDIS relies on the external protection provided by the driver itself.
//
class KNdisPacketPool : public KObject<NDIS_HANDLE, 'knpp'>
{
	SAFE_DESTRUCTORS
public:
	inline KNdisPacketPool(
		IN UINT NumberOfDescriptors,
		IN UINT ProtocolReservedLength=PROTOCOL_RESERVED_SIZE_IN_PACKET
		);
	inline KNdisPacketPool(
		IN UINT NumberOfDescriptors,
		IN UINT NumberOfOverflowDescriptors,
		IN UINT ProtocolReservedLength
		);
	inline KNdisPacketPool();

	inline ~KNdisPacketPool();

	inline NDIS_STATUS Initialize(
		IN UINT NumberOfDescriptors,
		IN UINT ProtocolReservedLength=PROTOCOL_RESERVED_SIZE_IN_PACKET
		);
	inline NDIS_STATUS InitializeEx(
		IN UINT NumberOfDescriptors,
		IN UINT NumberOfOverflowDescriptors,
		IN UINT ProtocolReservedLength=PROTOCOL_RESERVED_SIZE_IN_PACKET
		);

	inline void Invalidate();

// Allocating

	inline KNdisPacket Allocate();		// NDIS-sponsored lock (slowest but robust)
	inline KNdisPacket AllocateDpr();	// Faster version assuming we're at DPC
	inline KNdisPacket AllocateDprNonInterlocked();  // Even Faster version assuming external protecting lock

// Freeing. Note the underlying NDIS service doesn't need 'this'
#if DBG
#define _STATIC_FREE
#else
#define _STATIC_FREE	static
#endif
	_STATIC_FREE inline void Free(PNDIS_PACKET Packet);
	_STATIC_FREE inline void FreeDpr(PNDIS_PACKET Packet);
	_STATIC_FREE inline void FreeDprNonInterlocked(PNDIS_PACKET Packet);
#undef 	_STATIC_FREE

// Usage count. NOTE: for release builds available only in NDIS 5 DDK
	inline UINT Usage();

// Helpers
	inline BOOLEAN IsFrom(const PNDIS_PACKET Packet) const;

// Simulate usage count for checked builds
// NOTE: the internal counter is not thread-safe.
//		 Method Usage() is guaranteed to return correct value only for NDIS 5+ targets.
#if DBG
	protected:  KNdisCounter m_nTaken;
#endif
};

/////////////////////////////////////////////////////////////////////
// Implementations


///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	NumberOfDescriptors    - the number of packet descriptors the pool should contain
// 	ProtocolReservedLength - the number of bytes to be allocated for the ProtocolReserved array
//	                         of each packet descriptor
// Returns:
// 	none
// Comments:
// 	The caller should always check the result of construction with IsValid().
KNdisPacketPool::KNdisPacketPool(
	IN UINT NumberOfDescriptors,
	IN UINT ProtocolReservedLength
	)
{
	NDIS_STATUS Status = Initialize(NumberOfDescriptors, ProtocolReservedLength);
	if (!NT_SUCCESS(Status))
	{
		Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////
// Constructor with overflow descriptors
//
// Parameters:
// 	NumberOfDescriptors    - the number of packet descriptors the pool should contain
// 	NumberOfOverflowDescriptors - Specifies the number of packet descriptors reserved for
//                                an overflow situation (that is, when all the descriptors specified by NumberOfDescriptors are in use).
// 	ProtocolReservedLength - the number of bytes to be allocated for the ProtocolReserved array
//	                         of each packet descriptor
// Returns:
// 	none
// Comments:
// 	The caller should always check the result of construction with IsValid().
//	This form is supported for NDIS 5 drivers only.
KNdisPacketPool::KNdisPacketPool(
	IN UINT NumberOfDescriptors,
	IN UINT NumberOfOverflowDescriptors,
	IN UINT ProtocolReservedLength
	)
{
	NDIS_STATUS Status = InitializeEx(NumberOfDescriptors,
									NumberOfOverflowDescriptors,
									ProtocolReservedLength);
	if (!NT_SUCCESS(Status))
	{
		Invalidate();
	}
}


///////////////////////////////////////////////////////////////////////
// Constructor (unininalized - Initialize() is required)
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisPacketPool::KNdisPacketPool() {}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
KNdisPacketPool::~KNdisPacketPool()
{
	Invalidate();
}

///////////////////////////////////////////////////////////////////////
// Initialize the pool
//
// Parameters:
// 	NumberOfDescriptors    - the number of packet descriptors the pool should contain
// 	ProtocolReservedLength - the number of bytes to be allocated for the ProtocolReserved array
//	                         of each packet descriptor
// Returns:
// 	NDIS_STATUS_SUCCESS if the pool has been successfully created; an error code otherwise
// Comments:
//	none
NDIS_STATUS
KNdisPacketPool::Initialize(
	IN UINT NumberOfDescriptors,
	IN UINT ProtocolReservedLength
	)
{
	NDIS_STATUS Status;

	// Initialize the handle to 0 since NdisAllocatePacketPool uses this value
	Assign(0);

	NdisAllocatePacketPool(&Status,
		&m_Handle,
		NumberOfDescriptors,
		ProtocolReservedLength);

	// If NdisAllocatePacketPool failed, set the handle back to -1
	if (NDIS_STATUS_SUCCESS != Status)
		KObject<NDIS_HANDLE, 'knpp'>::Invalidate();

	return Status;
}

///////////////////////////////////////////////////////////////////////
// Initialize the pool with overflow descriptors
//
// Parameters:
// 	NumberOfDescriptors    - the number of packet descriptors the pool should contain
// 	NumberOfOverflowDescriptors - Specifies the number of packet descriptors reserved for
//                                an overflow situation (that is, when all the descriptors specified by NumberOfDescriptors are in use).
// 	ProtocolReservedLength - the number of bytes to be allocated for the ProtocolReserved array
//	                         of each packet descriptor
// Returns:
// 	NDIS_STATUS_SUCCESS if the pool has been successfully created; an error code otherwise
// Comments:
//	This form is supported for NDIS 5 drivers only.
NDIS_STATUS
KNdisPacketPool::InitializeEx(
	IN UINT NumberOfDescriptors,
	IN UINT NumberOfOverflowDescriptors,
	IN UINT ProtocolReservedLength
	)
{
#if (KNDIS_DDK_VERSION >= 5)
	NDIS_STATUS Status;

	// Initialize the handle to 0 since NdisAllocatePacketPoolEx uses this value
	Assign(0);

	NdisAllocatePacketPoolEx(&Status,
	   &m_Handle,
	   NumberOfDescriptors,
	   NumberOfOverflowDescriptors,
	   ProtocolReservedLength);

	// If NdisAllocatePacketPoolEx failed, set the handle back to -1
	if (NDIS_STATUS_SUCCESS != Status)
		KObject<NDIS_HANDLE, 'knpp'>::Invalidate();

	return Status;
#else // for NDIS 4, map to normal
	UNREFERENCED_PARAMETER(NumberOfOverflowDescriptors);
	return Initialize(NumberOfDescriptors, ProtocolReservedLength);
#endif
}

///////////////////////////////////////////////////////////////////////
// Invalidate (free) the pool
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
void KNdisPacketPool::Invalidate()
{
	if (IsValid())
	{
		ASSERT(Usage()==0);
		NdisFreePacketPool(m_Handle);
		KObject<NDIS_HANDLE, 'knpp'>::Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////
// Allocate a packet descriptor
//
// Parameters:
// 	none
// Returns:
// 	Fresh packet descriptor (KNdisPacket)
// Comments:
// 	The caller should always check the returned buffer by KNdisPacket::IsValid().
//	If there is no buffers chained for the packet KNdisPacket::IsValid() returns FALSE.
KNdisPacket
KNdisPacketPool::Allocate()
{
	NDIS_STATUS Status;
	KNdisPacket pkt;
	NdisAllocatePacket(&Status, &pkt.m_packet, m_Handle);
#if DBG
	if (NT_SUCCESS(Status))
	{
		++m_nTaken;
	}
#endif
	return pkt;
}

///////////////////////////////////////////////////////////////////////
// Allocate a packet descriptor from DISPATCH_LEVEL
//
// Parameters:
// 	none
// Returns:
// 	Fresh packet descriptor (KNdisPacket)
// Comments:
// 	The caller should always check the returned buffer by KNdisPacket::IsValid().
//	If there is no buffers chained for the packet KNdisPacket::IsValid() returns FALSE.
KNdisPacket
KNdisPacketPool::AllocateDpr()
{
	NDIS_STATUS Status;
	KNdisPacket pkt;
	NdisDprAllocatePacket(&Status, &pkt.m_packet, m_Handle);
#if DBG
	if (NT_SUCCESS(Status))
	{
		++m_nTaken;
	}
#endif
	return pkt;
}

///////////////////////////////////////////////////////////////////////
// Allocate a packet descriptor from DISPATCH_LEVEL without locking
//
// Parameters:
// 	none
// Returns:
// 	Fresh packet descriptor (KNdisPacket)
// Comments:
//	This is the fastest way to allocate the descriptor.
// 	The caller should always check the returned buffer by KNdisPacket::IsValid().
//	If there is no buffers chained for the packet KNdisPacket::IsValid() returns FALSE.
KNdisPacket
KNdisPacketPool::AllocateDprNonInterlocked()
{
	NDIS_STATUS Status;
	KNdisPacket pkt;
	NdisDprAllocatePacketNonInterlocked(&Status, &pkt.m_packet, m_Handle);
#if DBG
	if (NT_SUCCESS(Status))
	{
		++m_nTaken;
	}
#endif
	return pkt;
}


///////////////////////////////////////////////////////////////////////
// Free the packet back to the pool
//
// Parameters:
// 	Packet               - packet (KNdisPacket) to free
// Returns:
// 	none
// Comments:
// 	none
void
KNdisPacketPool::Free(PNDIS_PACKET Packet)
{
	NdisFreePacket(Packet);
#if DBG
	--m_nTaken;
#endif
}

///////////////////////////////////////////////////////////////////////
// Free the packet back to the pool from DISPATCH_LEVEL
//
// Parameters:
// 	Packet               - packet (KNdisPacket) to free
// Returns:
// 	none
// Comments:
// 	none
void
KNdisPacketPool::FreeDpr(PNDIS_PACKET  Packet)
{
	NdisDprFreePacket(Packet);
#if DBG
	--m_nTaken;
#endif
}

///////////////////////////////////////////////////////////////////////
// Free the packet back to the pool from DISPATCH_LEVEL without locking
//
// Parameters:
// 	Packet               - packet (KNdisPacket) to free
// Returns:
// 	none
// Comments:
// 	none
void
KNdisPacketPool::FreeDprNonInterlocked(PNDIS_PACKET  Packet)
{
	NdisDprFreePacketNonInterlocked(Packet);
#if DBG
	--m_nTaken;
#endif
}


///////////////////////////////////////////////////////////////////////
// Get usage count
//
// Parameters:
// 	none
// Returns:
// 	Number of descriptors allocated so far
// Comments:
// 	The method is implemented in NDIS 5 only. NDIS 4 debug builds
//  simulate the usage counter in the class; free builds always return 0.
UINT
KNdisPacketPool::Usage()
{
#if (KNDIS_DDK_VERSION >= 5) && (defined(NDIS50) || defined(NDIS51) || defined(NDIS50_MINIPORT) || defined(NDIS51_MINIPORT))
	return NdisPacketPoolUsage(m_Handle);
#else
 #if DBG
	return (UINT) ULONG(m_nTaken);
 #else
// #pragma message("WARNING: KNdisPacketPool::Usage() not supported for NDIS 4 free")
	return 0;
 #endif // DBG
#endif
}


///////////////////////////////////////////////////////////////////////
// Test if the packet belongs to the pool
//
// Parameters:
// 	Packet               -
// Returns:
// 	none
// Comments:
// 	none
BOOLEAN
KNdisPacketPool::IsFrom(const PNDIS_PACKET Packet) const
{
	return Packet->Private.Pool == m_Handle;
}

#endif // !defined(AFX_KNDISPACKETPOOL_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
