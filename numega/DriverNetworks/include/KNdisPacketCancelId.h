// KNdisPacketCancelId.h: interface for the KNdisPacketCancelId class.
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

#if !defined(AFX_KNDISPACKETCANCELID_H__EA9DE798_D539_41E7_8BD8_03A1A99F99F6__INCLUDED_)
#define AFX_KNDISPACKETCANCELID_H__EA9DE798_D539_41E7_8BD8_03A1A99F99F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if KNDIS_DDK_VERSION <= 5
#error	"Should be compiled with XP DDK (target)"
#endif

//=============================================================================
// KNdisPacketCancelId
//
// This is a NDIS packet cancel ID generator (NDIS 5.1+)
//
// Include an instance of it as data member in one of the main classes of
// NDIS Protocol or IM driver and Initialize() it.
//
// Successive calls to Next() return "cancel id", which you can pass to
// KNdisPacket::CANCEL_ID() to label packets being sent to bound miniport adapters.
//
// Each Next() generates a number with the highest
// byte being an id returned by NdisGeneratePartialCancelId called by Initialize();
// the lowest bytes represent an incremental value.
//
// Cancelling packet(s) is typically a result of cancelling pending IRP(s).
// A driver would typically save a pointer to the "labelled" packet
// in the IRP associated with the packet(s) and call NdisCancelSendPackets
// when the IRP is being cancelled.
//
//
class KNdisPacketCancelId {
public:
	KNdisPacketCancelId();
	inline ~KNdisPacketCancelId() {}

	void Initialize();
	void Invalidate();

	// Get and Advance
	PVOID Next();

	// Get current. Can be used for a group of packets
	operator PVOID() const;

private:
	ULONG_PTR	m_seed;
	LONG		m_count;
};

// Implementations
inline
KNdisPacketCancelId::KNdisPacketCancelId()
{
	m_seed=m_count=0;
}

inline void
KNdisPacketCancelId::Initialize()
{
	m_seed = (ULONG_PTR) NdisGeneratePartialCancelId();
	m_seed <<= ((sizeof(PVOID) - 1) * 8);
	m_count=0;
}

inline KNdisPacketCancelId::operator PVOID() const
{
	return (PVOID)
	((m_count & (ULONG_PTR(-1) >> 8)) | m_seed);
}

inline void
KNdisPacketCancelId::Invalidate()
{
	m_seed=m_count=0;
}

inline PVOID
KNdisPacketCancelId::Next()
{
	return (PVOID)
	(((ULONG_PTR)NdisInterlockedIncrement(&m_count) & (ULONG_PTR(-1) >> 8)) | m_seed);
}

#endif // !defined(AFX_KNDISPACKETCANCELID_H__EA9DE798_D539_41E7_8BD8_03A1A99F99F6__INCLUDED_)
