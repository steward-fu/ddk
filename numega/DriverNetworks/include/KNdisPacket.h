// KNdisPacket.h: interface for the KNdisPacket class.
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

#if !defined(AFX_KNDISPACKET_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
#define AFX_KNDISPACKET_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>

//////////////////////////////////////////////////////////////////////
// NDIS Packet
//
// Abstracts NDIS_PACKET objects and acts as a "smart" pointer to
// NDIS_PACKET.
//
// NDIS_PACKET is a fundamental NDIS data structure used by all types of
// network drivers to represent data packets travelling between adjecent network interfaces.
// NDIS_PACKET is a higher layer abstract than NDIS_BUFFER. The latter
// describes memory layout mapped for given NDIS_PACKET, and in NT world,
// is an equivalent of a MDL. NDIS_PACKET, on the other hand, describes
// the content of data packets sent/received between the layers. The content
// of a packet is stored in the chain of NDIS_BUFFERs associated with given packet.
//
// Beside the attached buffer chain, each NDIS packet is characterized by its
// flags (NDIS_FLAG_XXX) and, optionally, out-of-band (OOB) parameters.
//
// NDIS_PACKETs are always allocated from NDIS packet pools. In DriverNetworks,
// packets pools are represented by KNdisPacketPool class.
//
//
class KNdisPacket
{
public:
// Constructor (uninitialized)
	KNdisPacket() { m_packet=NULL; }
// Constructor from given PNDIS_PACKET
	KNdisPacket(PNDIS_PACKET pkt) { m_packet=pkt; }

// Assignment (assigns the pointer)
	KNdisPacket& operator=(const KNdisPacket& pkt) {m_packet = PNDIS_PACKET(pkt); return *this;}

// Convertsion to PNDIS_PACKET
	operator PNDIS_PACKET () const {return m_packet;}

// Test if the object has been successfully initialized
	BOOLEAN IsValid() const { return KNDIS_GOOD_PTR(m_packet); }

// Compare (pointer comparison)
	bool operator==(const KNdisPacket& pkt) const { return m_packet==pkt.m_packet; }

// Operations with chained buffers

	inline VOID ChainAtBack(IN PNDIS_BUFFER buf);
	inline VOID ChainAtFront(IN PNDIS_BUFFER buf);

	inline KNdisBuffer UnChainAtBack();
	inline KNdisBuffer UnChainAtFront();

	inline UINT CopyFrom(
		OUT PNDIS_PACKET	dst,
		IN  UINT			DestinationOffset,
		IN  UINT			BytesToCopy,
		IN  UINT			SourceOffset,
		BOOLEAN				bMustSucceed=FALSE);

	UINT Fill(IN PVOID Source, IN UINT BytesToCopy);

// Queries
	inline VOID Query(
		OUT PUINT			PhysicalBufferCount,
		OUT PUINT			BufferCount,
		OUT PNDIS_BUFFER*	FirstBuffer,
		OUT PUINT			TotalPacketLength ) const;
	inline UINT QueryPhysicalBufferCount() const;
	inline UINT QueryBufferCount()  const;
	inline UINT QueryTotalLength()  const;
	inline KNdisBuffer QueryFirstBuffer() const;

	// "Fast" get first: available for NT only; not suitable for miniports
	inline KNdisBuffer GetFirst(
		OUT PVOID			*_FirstBufferVA,
		OUT PUINT			_FirstBufferLength,
		OUT PUINT			_TotalBufferLength,
		BOOLEAN				bMustSucceed=FALSE)  const;

// Cleaning the packet. NOTE: the buffers should be unchained (assert it!)
	inline VOID Reinitialize();
	inline VOID ReinitializeEx(); // Helper: safer Reinitialize()
	static inline VOID Reinitialize(PNDIS_PACKET Packet);

// Copying the packet descriptor: Down-for transmission, Up-for indication:
	inline void CloneDown(const KNdisPacket& src);
	inline void CloneUp(const KNdisPacket& src);

// Locking buffers
	inline BOOLEAN LockBuffers();

#if !defined(VTOOLSD)
// Set packet flags (NDIS_FLAGS_XXXX values)
// Caveat in DDK: #define NdisSetPacketFlags(_Packet, _Flags) (_Packet)->Private.Flags |= (_Flags)
	inline void SetFlags(UINT Flags)	{ m_packet->Private.Flags = Flags; }

// Get packet flags (NDIS_FLAGS_XXXX values)
//
// The returned flags are the same as the Flags parameter passed to Miniport's Send() handler
	inline UINT GetFlags()	const		{ return  NdisGetPacketFlags(m_packet); }

// Reinitializing after manually modifying the buffer chain
	VOID Recalculate() { NdisRecalculatePacketCounts(m_packet); }
#endif // VTOOLSD

// Access to reserved fields
	// Capacity of reserved fields in bytes
	enum {
		#ifndef KNDIS_DESIRIALIZED_MINIPORT
		MiniportReservedSize = sizeof(PNDIS_PACKET(0)->MiniportReserved),	 // 8
		#else
		MiniportReservedSize = sizeof(PNDIS_PACKET(0)->MiniportReservedEx),  // 12
		#endif
		ProtocolReservedSize = PROTOCOL_RESERVED_SIZE_IN_PACKET	// it's (4 * sizeof(PVOID)) defined in DDK.
	};
#ifdef NDIS_MINIPORT_DRIVER
	// Get pointer to miniport-reserved area of the packet
	inline PULONG_PTR MiniportReserved() { return (PULONG_PTR) m_packet->MiniportReserved; }
#endif
	// Get pointer to protocol-reserved area of the packet
	inline PULONG_PTR ProtocolReserved() { return (PULONG_PTR) m_packet->ProtocolReserved; }

	// Get "stack" area pointer (for IM drivers, NDIS 5.1+ only)
	PULONG_PTR GetCurrentStack();

#if !defined(VTOOLSD)
// get OOB parameter
	inline UINT HEADER_SIZE() {return NDIS_GET_PACKET_HEADER_SIZE(m_packet);}
	inline NDIS_STATUS STATUS() { return NDIS_GET_PACKET_STATUS(m_packet);}
	inline ULONGLONG TIME_TO_SEND() { return NDIS_GET_PACKET_TIME_TO_SEND(m_packet);}
	inline ULONGLONG TIME_SENT() { return NDIS_GET_PACKET_TIME_SENT(m_packet); }
	inline ULONGLONG TIME_RECEIVED() { return NDIS_GET_PACKET_TIME_RECEIVED(m_packet);}

#if KNDIS_DDK_VERSION > 5
	inline PVOID CANCEL_ID() { NDIS_GET_PACKET_CANCEL_ID(m_packet); }
#endif

// set OOB parameter
	inline void HEADER_SIZE(UINT Size) { NDIS_SET_PACKET_HEADER_SIZE(m_packet, Size);}
	inline void STATUS(NDIS_STATUS s) { NDIS_SET_PACKET_STATUS(m_packet, s);}
	inline void TIME_TO_SEND(ULONGLONG t) { NDIS_SET_PACKET_TIME_TO_SEND(m_packet, t);}
	inline void TIME_SENT(ULONGLONG t) { NDIS_SET_PACKET_TIME_SENT(m_packet, t);}
	inline void TIME_RECEIVED(ULONGLONG t) { NDIS_SET_PACKET_TIME_RECEIVED(m_packet, t);}
#if KNDIS_DDK_VERSION > 5
	inline void CANCEL_ID(PVOID id) { NDIS_SET_PACKET_CANCEL_ID(m_packet, id); }
#endif

#endif // VTOOLSD

	// Get media-specific info (OOB data)
	template<class T> inline void MEDIA_SPECIFIC_INFO(T** ppInfo, PUINT pSize) {
		NDIS_GET_PACKET_MEDIA_SPECIFIC_INFO(m_packet, ppInfo, pSize); }

	// Set media-specific info (OOB data)
	template<class T> inline void MEDIA_SPECIFIC_INFO(T* pInfo) {
		NDIS_SET_PACKET_MEDIA_SPECIFIC_INFO(m_packet, pInfo, sizeof(T));}

	// Assign from packet pointer
	inline KNdisPacket& operator =(const PNDIS_PACKET buf) { m_packet = buf; return *this;}

	// Get handle to the containing pool
	HANDLE GetPoolHandle() const;

protected:

	PNDIS_PACKET	m_packet;		// wrapped ptr

	friend class KNdisPacketPool;
	friend class KNdisPacketArray;
};

#if !defined(VTOOLSD)
///////////////////////////////////////////////////////////////
// KNdisPacketArray
//
// Intended for use within adapter's SendPacket() handler
//
class KNdisPacketArray
{
public:
// ctor
	inline KNdisPacketArray(PPNDIS_PACKET PArray, UINT Size) :
	m_pArray(PArray), m_uSize(Size) {}

// Subscript operator: returns KNdisPacket at index 'i'
	 KNdisPacket operator [] (UINT i)
	 {
		 KNdisPacket pkt;
		 if (i<m_uSize) pkt.m_packet = * (m_pArray+i);
		 return pkt;
	 }
protected:
	PPNDIS_PACKET	m_pArray;
	UINT			m_uSize;
};
#endif // VTOOLSD

////////////////////////////////////////////////////////////
// KNdisPacketWithContext
//
// A template alias to KNdisPacket with a type-safe
// access to the reserved area.
//
// Parameter T is the type placed on the reserved area
// of the packet. The second parameter, bProtocol, defines
// what reserved fields of NDIS_PACKET are used to store T.
// It defaults to the proper area based on KNDIS_PROTOCOL_DRIVER
// value, which is FALSE for miniport and IM builds, and TRUE
// for protocol builds.
//
template <class T, const BOOLEAN bProtocol=KNDIS_PROTOCOL_DRIVER>
struct KNdisPacketWithContext : public KNdisPacket
{
// ctor
	KNdisPacketWithContext() : KNdisPacket() {}
	KNdisPacketWithContext(PNDIS_PACKET p) : KNdisPacket(p) {}

// Return Context pointer for given packet
	inline T* GetContext()
	{
		ASSERT(bProtocol || (sizeof(T) <= 3*sizeof(PVOID))); // assuming deserialized
		return reinterpret_cast<T*> ((bProtocol) ?
			m_packet->ProtocolReserved : m_packet->MiniportReserved);
	}

// Type accessor.
	typedef T Type;

// Assignment
	KNdisPacketWithContext& operator=(KNdisPacket& p) { m_packet=p; return *this; }

// Required reserved area's size. Used by protocol drivers when allocating packet pools
	static inline ULONG ReservedSize()
	{	return (bProtocol) ? min(PROTOCOL_RESERVED_SIZE_IN_PACKET, sizeof(T)) : min(3*sizeof(PVOID), sizeof(T));	}
};


/////////////////////////////////////////////////////////////////////////////////////
// Implementations


///////////////////////////////////////////////////////////////////////
// Chain buffer at back
//
// Parameters:
// 	buf                  - NDIS buffer (KNdisBuffer) to chain
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisPacket::ChainAtBack(IN PNDIS_BUFFER buf)
{	NdisChainBufferAtBack(m_packet, buf);	}

///////////////////////////////////////////////////////////////////////
// Chain buffer at front
//
// Parameters:
// 	buf                  - NDIS buffer (KNdisBuffer) to chain
// Returns:
// 	none
// Comments:
// 	none
VOID KNdisPacket::ChainAtFront(IN PNDIS_BUFFER buf)
{	NdisChainBufferAtFront(m_packet, buf);	}

///////////////////////////////////////////////////////////////////////
// Unchain last buffer
//
// Parameters:
// 	none
// Returns:
// 	KNdisBuffer for the unchained buffer
// Comments:
// 	The caller should always check the returned buffer by KNdisBuffer::IsValid().
//	If there is no buffers chained for the packet KNdisBuffer::IsValid() returns FALSE.
KNdisBuffer
KNdisPacket::UnChainAtBack()
{
	KNdisBuffer buf;
	NdisUnchainBufferAtBack(m_packet, &buf.m_pBuf);
	return buf;
}

///////////////////////////////////////////////////////////////////////
// Unchain first buffer
//
// Parameters:
// 	none
// Returns:
// 	KNdisBuffer for the first unchained buffer
// Comments:
// 	The caller should always check the returned buffer by KNdisBuffer::IsValid().
//	If there is no buffers chained for the packet KNdisBuffer::IsValid() returns FALSE.
KNdisBuffer
KNdisPacket::UnChainAtFront()
{
	KNdisBuffer buf;
	NdisUnchainBufferAtFront(m_packet, &buf.m_pBuf);
	return buf;
}



///////////////////////////////////////////////////////////////////////
// Copy a specified range of data *from* this packet to another packet
//
// Parameters:
// 	dst                  - packet (KNdisPacket) to copy to
// 	DestinationOffset    - the byte offset within the destination packet at which to begin copying
// 	BytesToCopy          - the number of bytes to copy
// 	SourceOffset         - the byte offset within the source packet at which to begin copying
//  bMustSucceed		 - Defines priority of the "safe" allocation (NDIS 5.1 only)
// Returns:
// 	Number of bytes actually copied. This can be less than BytesToCopy.
// Comments:
// 	The method copies only data buffers as long as the destination has enough space
//	to do so. No OOB data is copied.
UINT
KNdisPacket::CopyFrom(
	OUT PNDIS_PACKET	dst,
	IN  UINT			DestinationOffset,
	IN  UINT			BytesToCopy,
	IN  UINT			SourceOffset,
	BOOLEAN				bMustSucceed)
{
	UINT BytesCopied;
#if KNDIS_DDK_VERSION >= 0x0501
	NdisCopyFromPacketToPacketSafe(	dst,
									DestinationOffset,
									BytesToCopy,
									m_packet,
									SourceOffset,
									&BytesCopied,
									(bMustSucceed)?HighPagePriority:NormalPagePriority);
#else
	NdisCopyFromPacketToPacket(	dst,
								DestinationOffset,
								BytesToCopy,
								m_packet,
								SourceOffset,
								&BytesCopied);
#endif
	UNREFERENCED_PARAMETER(bMustSucceed);
	return BytesCopied;
}

///////////////////////////////////////////////////////////////////////
// Query buffer info of the packet
//
// Parameters:
// 	PhysicalBufferCount  - Points to a caller-supplied variable in which this function
//                         returns the maximum number of physical breaks mapped by the
//						   buffer descriptors chained to the given packet. This parameter can be NULL.
// 	BufferCount          - Points to a caller-supplied variable in which this function returns the number
//                         of buffer descriptors chained to the given packet. This parameter can be NULL.
// 	FirstBuffer          - Points to a caller-supplied variable in which this function returns a pointer
//                         to the initial buffer descriptor chained to the given packet. This parameter can be NULL.
// 	TotalPacketLength    - Points to a caller-supplied variable in which this function returns the number
//                         of bytes of packet data mapped by all chained buffer descriptors. This parameter can be NULL.
// Returns:
// 	none
// Comments:
// 	To query only one of those values use methods
//	 QueryPhysicalBufferCount(), QueryBufferCount(), QueryTotalLength(), QueryFirstBuffer()
//
VOID
KNdisPacket::Query(
	OUT PUINT			PhysicalBufferCount ,
	OUT PUINT			BufferCount ,
	OUT PNDIS_BUFFER*	FirstBuffer ,
	OUT PUINT			TotalPacketLength
	)  const
{
	NdisQueryPacket(m_packet,
					PhysicalBufferCount,
					BufferCount,
					FirstBuffer,
					TotalPacketLength);
}


///////////////////////////////////////////////////////////////////////
// Query Physical Buffers
//
// Parameters:
// 	none
// Returns:
// 	The maximum number of physical breaks mapped by the buffer descriptors chained to the given packet.
// Comments:
// 	none
UINT KNdisPacket::QueryPhysicalBufferCount() const
{	UINT c; Query(&c, 0, 0, 0); return c;	}

///////////////////////////////////////////////////////////////////////
// Query buffer count
//
// Parameters:
// 	none
// Returns:
// 	The number of buffer descriptors chained to the given packet.
// Comments:
// 	none
UINT KNdisPacket::QueryBufferCount()  const
{	UINT c; Query(0, &c, 0, 0); return c;	}

///////////////////////////////////////////////////////////////////////
// Query total length of buffers
//
// Parameters:
// 	none
// Returns:
// 	The number of bytes of packet data mapped by all chained buffer descriptors.
// Comments:
// 	none
UINT KNdisPacket::QueryTotalLength()  const
{	UINT c; Query(0, 0, 0, &c); return c;	}

///////////////////////////////////////////////////////////////////////
// Query first buffer in the chain
//
// Parameters:
// 	none
// Returns:
// 	The initial buffer descriptor (KNdisBuffer) chained to the given packet
// Comments:
// 	The caller should always check the returned buffer by KNdisBuffer::IsValid().
//	If there is no buffers chained for the packet KNdisBuffer::IsValid() returns FALSE.
KNdisBuffer KNdisPacket::QueryFirstBuffer() const
{	KNdisBuffer buf; Query(0, 0, &buf.m_pBuf, 0); return buf;	}

#if !defined(VTOOLSD)

///////////////////////////////////////////////////////////////////////
// Clean the packet descriptor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	The method cleans the fields of the packet descriptor and the
//	attached OOB data block. No buffers should be chained for the packet at the
//  time this function is called.
VOID
KNdisPacket::Reinitialize()
{
	#if DBG
	KNdisBuffer buf = UnChainAtFront(); ASSERT(!buf.IsValid() && "Buffers Chained!");
	#endif
	NdisReinitializePacket(m_packet);
	PNDIS_PACKET_OOB_DATA pOob = NDIS_OOB_DATA_FROM_PACKET(m_packet);
	if (pOob)
		NdisZeroMemory(pOob, sizeof(NDIS_PACKET_OOB_DATA));
}

///////////////////////////////////////////////////////////////////////
// Cleans the packet descriptor and free buffers
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
//	Extended version of Reinitialize(). In addition to Reinitialize(), this
//	this function also frees the chained buffers back to their respective buffuer pools.
VOID
KNdisPacket::ReinitializeEx()
{
	KNdisBuffer buf = UnChainAtFront();
	while (buf.IsValid())
	{
		reinterpret_cast<KNdisBufferPool*>(0)->Free(buf);
		buf = UnChainAtFront();
	};
	Reinitialize();
}

///////////////////////////////////////////////////////////////////////
// Clean the specified packet descriptor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Static version of Reinitialize()
VOID KNdisPacket::Reinitialize(PNDIS_PACKET Packet) // static
{
	NdisReinitializePacket(Packet);
	PNDIS_PACKET_OOB_DATA pOob = NDIS_OOB_DATA_FROM_PACKET(Packet);
	if (pOob)
		NdisZeroMemory(pOob, sizeof(NDIS_PACKET_OOB_DATA));
}

#if !BINARY_COMPATIBLE
///////////////////////////////////////////////////////////////////////
// Get buffer info for the 1st chained buffer
//
// Parameters:
// 	_FirstBufferVA       - Points to a caller-supplied variable in which this macro returns the address of the
//						   initial buffer descriptor chained to the given packet descriptor.
// 	_FirstBufferLength   - Points to a caller-supplied variable in which this macro returns the number of
//                         bytes mapped by the initial buffer descriptor chained to the packet descriptor
// 	_TotalBufferLength   - Points to a caller-supplied variable in which this macro returns the number of bytes
//                         mapped by the initial buffer descriptor chained to the packet descriptor
// bMustSucceed		 - Defines priority of the "safe" allocation (NDIS 5.1 only)
// Returns:
// 	KNdisBuffer for the first buffer
// Comments:
// 	The caller should always check the returned buffer by KNdisBuffer::IsValid().
//	If there is no buffers chained for the packet KNdisBuffer::IsValid() returns FALSE.
//  NOTE this method is not defined for WDM builds. Thus, miniport drivers should use
//  QueryFirstBuffer() instead.
KNdisBuffer
KNdisPacket::GetFirst(
	OUT PVOID			*_FirstBufferVA,
	OUT PUINT			_FirstBufferLength,
	OUT PUINT			_TotalBufferLength,
	BOOLEAN				bMustSucceed)  const
{
	KNdisBuffer buf;
#if KNDIS_DDK_VERSION >= 0x0501
	NdisGetFirstBufferFromPacketSafe(m_packet,
									&buf.m_pBuf,
									_FirstBufferVA,
									_FirstBufferLength,
									_TotalBufferLength,
									(bMustSucceed) ? HighPagePriority : NormalPagePriority);
#else
	NdisGetFirstBufferFromPacket(m_packet,
								&buf.m_pBuf,
								_FirstBufferVA,
								_FirstBufferLength,
								_TotalBufferLength);
	UNREFERENCED_PARAMETER(bMustSucceed);
#endif
	return buf;
}
#endif


///////////////////////////////////////////////////////////////////////
// Clone the specified packet for passing it down
//
// Parameters:
// 	src                  - The source packet to clone
// Returns:
// 	none
// Comments:
// 	CloneDown() performs field-by-field copy of the appropriate fields from
//	the source packet descriptor into this packet descriptor.
//	This method is used to create an "alias" packet descriptor, which is to
//	be passed down the binding. The alias descriptor points to the same
//	buffer chain and the OOB data block (if any) that the original descriptor.
//	CloneDown() is used primarily by intermediate drivers.
void
KNdisPacket::CloneDown(const KNdisPacket& src)
{
	ASSERT(src != *this);	// don't be stupid

	// copy the buffer chain pointers
	m_packet->Private.Head = src.m_packet->Private.Head;
	m_packet->Private.Tail = src.m_packet->Private.Tail;

	// Copy OOB data. They are fixed size located @ var offset
	NdisMoveMemory(NDIS_OOB_DATA_FROM_PACKET(m_packet),
					NDIS_OOB_DATA_FROM_PACKET(src.m_packet),
					sizeof(NDIS_PACKET_OOB_DATA));

#if KNDIS_DDK_VERSION >= 5
	// Copy some packet extension parameters. They are fixed size array, following OOB.
	// Do it manually for the "compatible" mode: a special API is available in W2K only...
 #if KNDIS_W9X_COMPATIBLE || !defined(NDIS_MINIPORT_DRIVER)
	PNDIS_PACKET_EXTENSION srcExt = NDIS_PACKET_EXTENSION_FROM_PACKET(src.m_packet);
	PNDIS_PACKET_EXTENSION dstExt = NDIS_PACKET_EXTENSION_FROM_PACKET(m_packet);
	dstExt->NdisPacketInfo[TcpIpChecksumPacketInfo] = srcExt->NdisPacketInfo[TcpIpChecksumPacketInfo];
	dstExt->NdisPacketInfo[IpSecPacketInfo] = srcExt->NdisPacketInfo[IpSecPacketInfo];
	dstExt->NdisPacketInfo[TcpLargeSendPacketInfo] = srcExt->NdisPacketInfo[TcpLargeSendPacketInfo];
	dstExt->NdisPacketInfo[ClassificationHandlePacketInfo] = srcExt->NdisPacketInfo[ClassificationHandlePacketInfo];
	dstExt->NdisPacketInfo[Ieee8021pPriority] = srcExt->NdisPacketInfo[Ieee8021pPriority];
 #else
	NdisIMCopySendPerPacketInfo(m_packet, src);
 #endif  // KNDIS_W9X_COMPATIBLE
	// Copy media-specific stuff. This is a blob optionally hanging off the descriptor

	PVOID MediaSpecificInfo; ULONG MediaSpecificInfoSize;
	NDIS_GET_PACKET_MEDIA_SPECIFIC_INFO(src.m_packet, &MediaSpecificInfo, &MediaSpecificInfoSize);
	if (MediaSpecificInfo || MediaSpecificInfoSize)
		NDIS_SET_PACKET_MEDIA_SPECIFIC_INFO(m_packet, MediaSpecificInfo, MediaSpecificInfoSize);

#endif // KNDIS_DDK_VERSION
}

///////////////////////////////////////////////////////////////////////
// Clone the specified packet for passing it up
//
// Parameters:
// 	src                  - The source packet to clone
// Returns:
// 	none
// Comments:
// 	CloneUp() performs field-by-field copy of the appropriate fields from
//	the source packet descriptor into this packet descriptor.
//	This method is used to create an "alias" packet descriptor, which is to
//	be passed *up* the binding. The alias descriptor points to the same
//	buffer chain and the OOB data block (if any) that the original descriptor.
//	CloneUp() is used primarily by intermediate drivers.
 void
KNdisPacket::CloneUp(const KNdisPacket& src)
{
	// copy the buffer chain pointers
	m_packet->Private.Head = src.m_packet->Private.Head;
	m_packet->Private.Tail = src.m_packet->Private.Tail;

#if KNDIS_DDK_VERSION >= 5
	// Set the packet chain correctly (for IM drivers)
	NDIS_SET_ORIGINAL_PACKET(m_packet, NDIS_GET_ORIGINAL_PACKET(src.m_packet));
#endif // KNDIS_DDK_VERSION

	// Copy Packet Flags
	SetFlags(NdisGetPacketFlags(src.m_packet));

	// Status and header
	NDIS_SET_PACKET_STATUS(m_packet, NDIS_GET_PACKET_STATUS(src.m_packet));
	NDIS_SET_PACKET_HEADER_SIZE(m_packet, NDIS_GET_PACKET_HEADER_SIZE(src.m_packet));
}


///////////////////////////////////////////////////////////////////////
// Locks packet's buffers in the memory
//
// Parameters:
// 	none
// Returns:
// 	TRUE if the operation succeeded. FALSE under low-memory conditions.
// Comments:
// 	Walks over the list of buffers to lock them. This is done to circumvent
//  NdisCopyFromPacketToPacket's (NDIS 5.0) use of non-safe functions
// to do the copy. Should be called before KNdisPacket::CopyFrom().
// NOTE: This is NOT required in NDIS 5.1 when using 'safe' functions.
BOOLEAN KNdisPacket::LockBuffers()
{
	KNdisBuffer buf = QueryFirstBuffer();
	UINT len; PVOID va=NULL;

	while (buf.IsValid())
	{
		buf.Query(&len, &va); // Query uses NdisQueryBufferSafe
		if (va==NULL) break;
		buf = buf.GetNext();
	}
	return (va!=NULL);
}

inline
HANDLE KNdisPacket::GetPoolHandle() const
{
	ASSERT(IsValid());
#if KNDIS_DDK_VERSION >= 0x0501
	return NdisGetPoolFromPacket(m_packet);
#else
	return (HANDLE) m_packet->Private.Pool;
#endif
}

inline
PULONG_PTR KNdisPacket::GetCurrentStack()
{
#if KNDIS_DDK_VERSION >= 0x0501
	// NOTE: Default # stacks is 2. Stored in variable _ndisPacketStackSize.
	BOOLEAN bAvail;
	PNDIS_PACKET_STACK pStack=NdisIMGetCurrentPacketStack(m_packet, &bAvail);
	return (bAvail) ? pStack->IMReserved : NULL;
#else
	return NULL;		// not supported otherwise
#endif
}


#include	<KNdisPartialPacket.h>

#endif // VTOOLSD

#endif // !defined(AFX_KNDISPACKET_H__A31A8706_3DEC_11D3_8FA3_00C04F7445B7__INCLUDED_)
