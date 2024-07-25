// KNdisPacketList.h: interface for the KNdisPacketList class.
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

#if !defined(AFX_KNDISPACKETLIST_H__3DD3F97D_C2B9_40B1_B2E4_7A93F137CD91__INCLUDED_)
#define AFX_KNDISPACKETLIST_H__3DD3F97D_C2B9_40B1_B2E4_7A93F137CD91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<KNdisList.h>

#if defined (VTOOLSD)
#define	MiniportReserved	WidgetReserved	// different NDIS_PACKET definition
#endif

///////////////////////////////////////////////////////////////////
// List of NDIS_PACKET descriptors (no protection)
//
// KNdisPacketList is derived from KList<NDIS_PACKET>. The only tricky
// issue about it that NDIS_PACKET structure does not have a field
// (like m_ListEntry) intended for linking descriptors in lists.
// Basic design of KList needs such a field, and KList constructor needs to know
// the offset to this field in the structures to be linked.
//
// KNdisPacketList has to make use of one of the "reserved" areas in NDIS_PACKET.
// There are 2 areas, ProtocolReserved and MiniportReserved. Packets allocated by
// protocol drivers can use the former while miniport drivers do the latter.
// Intermediate drivers, on the other hand, operate in 2 modes (protocol and miniport),
// so there should be run time differentiation as far as what kind of packet
// list is required.
//
// The default constructor takes BOOLEAN parameter that tells what mode
// the packet list is for (default: miniport). By default, KNdisPacketList uses
// 8 first bytes (the sizeof(LIST_ENTRY))  for linking purposes at NDIS_PACKET's
// MiniportReserved or ProtocolReserved area. The driver writer must not use these 8
// bytes in packet descriptors intended for linking in KList-s.
//
// The second form of constructor allows to circumwent this restriction. In this case,
// the driver writer passes the offset to a field within NDIS_PACKET structure
// that is to be used for linking purposes (8 bytes should be reserved at least).
//
class KNdisPacketList : public KList<NDIS_PACKET> {
public:
	// Returns offset used for list entry in the packet descriptor
	static inline ULONG DefaultEntryOffset(BOOLEAN bProtocol=FALSE) { return (bProtocol) ?
			FIELD_OFFSET(NDIS_PACKET,ProtocolReserved) :
			FIELD_OFFSET(NDIS_PACKET,MiniportReserved);
		}

	// Default Constructor
	inline KNdisPacketList(BOOLEAN bProtocol=FALSE) :
	KList<NDIS_PACKET> (DefaultEntryOffset(bProtocol)) {}

	// Custom list entry offset constructor
	inline KNdisPacketList(ULONG ListOffset) :
	KList<NDIS_PACKET> (ListOffset) {}
};

/////////////////////////////////////////////////////////
// List of NDIS_PACKET descriptors (spinlock protected)
//
// See description for KNdisPacketList for more details.
//
class KNdisInterlockedPacketList : public KInterlockedList<NDIS_PACKET> {
public:
	// Default Constructor
	inline KNdisInterlockedPacketList(BOOLEAN bProtocol=FALSE) :
	KInterlockedList<NDIS_PACKET> ( KNdisPacketList::DefaultEntryOffset(bProtocol) ) {}

	// Custom list entry offset constructor
	inline KNdisInterlockedPacketList(ULONG ListOffset) :
	KInterlockedList<NDIS_PACKET> (ListOffset) {}

};

///////////////////////////////////////////////////////////
// Interrupt safe packet list
//
// No "mode" issues here; so it is simply an alias to
// KInterruptSafeList<NDIS_PACKET>. Thus, we have to define
// default list entry determinator function
//
#ifdef NDIS_MINIPORT_DRIVER

typedef KInterruptSafeList<NDIS_PACKET>	KNdisInterruptSafePacketList;

// Delault list entry offset for miniports
inline ULONG ListEntryOffset(NDIS_PACKET*)
{
	return FIELD_OFFSET(NDIS_PACKET, MiniportReserved);
}

#endif


//////////////////////////////////////////////////////////////
// KNdisPacketListWithContext
//
// Provides the same functionality as KNdisPacketList with a
// type-safe access to the reserved fields of NDIS_PACKET descriptor.
//
// Parameter T is the type placed on the reserved area
// of the packet following LIST_ENTRY used for linking.
// Second parameter, L, defines the type of the list (interlocked by default)
// The 3rd parameter, bProtocol, defines what reserved fields of NDIS_PACKET
// are used to store T. It defaults to the proper area based on KNDIS_PROTOCOL_DRIVER
// value, which is FALSE for miniport and IM builds, and TRUE for protocol builds.
//
template <class T,
 class L=KNdisInterlockedPacketList, const BOOLEAN bProtocol=KNDIS_PROTOCOL_DRIVER>
  struct KNdisPacketListWithContext : public L
{
// ctor
	KNdisPacketListWithContext() : L(bProtocol) {}

// Return Context pointer for given packet
	static inline T* GetContext(PNDIS_PACKET packet)  { return
		reinterpret_cast<T*>(PCHAR(packet)+KNdisPacketList::DefaultEntryOffset(bProtocol)+sizeof(LIST_ENTRY));
	}

// Type accessor.
	typedef T Type;

// Required reserved area's size. Used by protocol drivers when allocating packet pools
	static inline ULONG ReservedSize() { return sizeof(T)+sizeof(LIST_ENTRY); }
};

#endif // !defined(AFX_KNDISPACKETLIST_H__3DD3F97D_C2B9_40B1_B2E4_7A93F137CD91__INCLUDED_)
