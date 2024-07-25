// KNdisFilterPacketPool.h
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

#if !defined(_KNdisFilterPacketPool_H__78FBA730_2F6C_11d5_903C_00C04F7445B7__INCLUDED_)
#define _KNdisFilterPacketPool_H__78FBA730_2F6C_11d5_903C_00C04F7445B7__INCLUDED_

////////////////////////////////////////////////////////////////////
// KNdisFilterPacketPool.h
//
// Type-safe packet pool for use as private pools in filter drivers
//
// Usage
// 1) Define your reserved type T
//		 struct MyContext { PVOID data; };
//
// 2) Define your pools
//		typedef KNdisFilterPacketPool<MyContext, true>		CTxPool;
//		typedef KNdisFilterPacketPool<MyContext, false>	CRxPool;
//
// 3) Allocating packet and using the reserved area in
//		KNdisPacket p = m_CTxPool.Allocate();	// send path
//		CTxPool::GetContext(p)->data = ...
//		. . .
//		CTxPool::GetContext(p)->data = ...		// receive path
//
template <class T, const bool bSendPath>
class KNdisFilterPacketPool : public KNdisPacketPool
{
public:
// ctor
	inline KNdisFilterPacketPool();

// Initialization
	inline NDIS_STATUS Initialize(IN UINT NumberOfDescriptors);
	inline NDIS_STATUS InitializeEx(IN UINT NumberOfDescriptors, IN UINT NumberOfOverflowDescriptors);

// Return user context pointer for given packet
	static inline T* GetContext(PNDIS_PACKET packet);

// Type accessor.
	typedef T Type;

protected:
// Everything the packet reserved area can be used for by KNdisFilterXxx classes.
// Currently, the framework reserves the first ULONG_PTR in reserved area for
// all packets being ForwardUp and ForwardUp
	union FrameworkReserved {
		PVOID			_adapter;
	};

	static inline UINT ReservedSize() { return max (PROTOCOL_RESERVED_SIZE_IN_PACKET, sizeof(FrameworkReserved) + sizeof(T)); }
};

//////////////////////////////////////////////////////////////////////
// Implementations

template <class T, const bool bSendPath>
KNdisFilterPacketPool<T, bSendPath>::KNdisFilterPacketPool() : KNdisPacketPool()
{
	// For Rx direction can't use more than 2 PVOIDs in MiniportReserved.
	// I suggest to always use a single pointer as a context to be on the safe side.
	ASSERT(bSendPath || sizeof(T) <= (3*sizeof(PVOID)-sizeof(FrameworkReserved)));
}

template <class T, const bool bSendPath> NDIS_STATUS
KNdisFilterPacketPool<T, bSendPath>::Initialize(IN UINT NumberOfDescriptors)
{
	return KNdisPacketPool::Initialize(NumberOfDescriptors, ReservedSize());
}

template <class T, const bool bSendPath> NDIS_STATUS
KNdisFilterPacketPool<T, bSendPath>::InitializeEx(IN UINT NumberOfDescriptors, IN UINT NumberOfOverflowDescriptors)
{
	return KNdisPacketPool::InitializeEx(NumberOfDescriptors, NumberOfOverflowDescriptors, ReservedSize());
}

template <class T, const bool bSendPath>
T* KNdisFilterPacketPool<T, bSendPath>::GetContext(PNDIS_PACKET packet)
{ return (bSendPath) ?
		reinterpret_cast<T*>(PCHAR(packet)+FIELD_OFFSET(NDIS_PACKET,ProtocolReserved)+sizeof(FrameworkReserved))
		:
		reinterpret_cast<T*>(PCHAR(packet)+FIELD_OFFSET(NDIS_PACKET,MiniportReserved)+sizeof(FrameworkReserved));
}

#endif  //(_KNdisFilterPacketPool_H__704DD9E0_2EBD_11d5_903C_00C04F7445B7__INCLUDED_)
