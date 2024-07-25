// KNdisStats.h: interface for the KNdisStats class.
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

#if !defined(AFX_KNDISSTATS_H__6C3A3814_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
#define AFX_KNDISSTATS_H__6C3A3814_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// KNdisStats		Common template for miniport statistics
//
// NDIS Statistics are respresented by the "statistics" OID sent to
// miniports. The statistics can be general (OID_GEN_...) and medium-specific.
// The statistics could also be mandatory and optional.
//
// The framework provides 3 classes to provide unform treatment of statistics:
//
//	KNdisStatsGen		- Mandatory general (GEN) statistics
//	KNdisStatsGenEx		- Optional	general (GEN) statistics
//	KNdisStatsEx<M>		- Medium-specific statistics, where M is an NdisMediumXxx value.
//
// KNdisStats is a base class for all the above. It defines  a single statistics set
// specified by the template parameter 'S', where 'S' represents a structure
// of NDIS OID statistics items (usually, counters of ULONG or ULONGLONG types).
//
// The statistics items are accessed using the dereference operator (->)
// and reset to zeroes using Reset().
//
// The KNdisStats objects are typically built into KNdisMiniAdapter-derived
// class. The driver maintains the counters in appropriate miniport handlers.
//
// NOTE The QuickMp wizard app automatically populates the OID handlers
// for the statistcics OID. The basic design assumes the statistic object
// is cached and built into the adapter class. The driver writer might change
// that if the hardware maintains the required counters.
//
// <<Add the counters by the medium descriptions>>
//
template<typename S>
class KNdisStats {
public:
	S	m_Counters;

	// Constructor: resets counters
	inline KNdisStats() { Reset(); }

	// Access statistics item ("counter").
	inline S* operator->() { return &m_Counters; }

	// Reset statistics
	inline void Reset() { NdisZeroMemory(&m_Counters, sizeof(m_Counters)); }

};

/////////////////////////////////////////////////////////////////////
// Data structures for GENERAL stats types

//  GEN MANDATORY
typedef struct {
		ULONG	xmit_ok;		// OID_GEN_XMIT_OK
		ULONG	rcv_ok;			// OID_GEN_RCV_OK
		ULONG	xmit_error;		// OID_GEN_XMIT_ERROR
		ULONG	rcv_error;		// OID_GEN_RCV_ERROR
		ULONG	rcv_no_buffer;	// OID_GEN_RCV_NO_BUFFER
} GEN_STATISTICS;

//  GEN OPTIONAL
typedef struct {				// optional
		ULONGLONG	dir_bytes_xmit;	// OID_GEN_DIRECTED_BYTES_XMIT
		ULONG		dir_frames_xmit;// OID_GEN_DIRECTED_FRAMES_XMIT
		ULONGLONG	mcast_bytes_xmit;// OID_GEN_MULTICAST_BYTES_XMIT
		ULONG		mcast_frames_xmit;// OID_GEN_MULTICAST_FRAMES_XMIT
		ULONGLONG	bcast_bytes_xmit;// OID_GEN_BROADCAST_BYTES_XMIT
		ULONG		bcast_frames_xmit;// OID_GEN_BROADCAST_FRAMES_XMIT
		ULONGLONG	dir_bytes_rcv; // OID_GEN_DIRECTED_BYTES_RCV
		ULONG		dir_frames_rcv; // OID_GEN_DIRECTED_FRAMES_RCV
		ULONGLONG	mcast_bytes_rcv; // OID_GEN_MULTICAST_BYTES_RCV
		ULONG		mcast_frames_rcv; // OID_GEN_MULTICAST_FRAMES_RCV
		ULONGLONG	bcast_bytes_rcv; // OID_GEN_BROADCAST_BYTES_RCV
		ULONG		bcast_frames_rcv; // OID_GEN_BROADCAST_FRAMES_RCV
		ULONG		rcv_crc_error; // OID_GEN_RCV_CRC_ERROR
		ULONG		xmit_que_length; // OID_GEN_TRANSMIT_QUEUE_LENGTH
} GEN_STATISTICS_OPT;

// Shorthands for GEN statistics
typedef KNdisStats<GEN_STATISTICS>		KNdisStatsGen;
typedef KNdisStats<GEN_STATISTICS_OPT>	KNdisStatsGenEx;

//////////////////////////////////////////////////////////////////////
// Generic template for medium-specific statistics (internal class)
//
// Specialization of KNdisStatsStruct define a medium
// specific structure of statistics requested by
// NDIS statistics OIDs, e.g. OID_802_XMIT_LATE_COLLISIONS, etc.
//
template<const NDIS_MEDIUM M> struct KNdisStatsStruct {};


//////////////////////////////////////////////////////////////////////
// Medium-specific statistics
//
// KNdisStatsEx defines medium 'M'-specific statistics.
// KNdisStatsEx is derived from a KNdisStats<> that uses the
// medium 'M' specific structure for the statistics items.
//
// E.g. KNdisStatsEx<NdisMedium802_3> would define the statistics
// set for the Ethernet medium.
//
template<const NDIS_MEDIUM M>
class KNdisStatsEx : public KNdisStats < KNdisStatsStruct<M> > {};


///////////////////////////////////////////////////////
// 802_3 Statistics
//
template<> struct KNdisStatsStruct<NdisMedium802_3> {
	ULONG rcv_err_align;		// OID_802_3_RCV_ERROR_ALIGNMENT
	ULONG xmit_one_collision;  // OID_802_3_XMIT_ONE_COLLISION
	ULONG xmit_more_collision; // OID_802_3_XMIT_MORE_COLLISIONS
	ULONG xmit_deferred;		// OID_802_3_XMIT_DEFERRED
	ULONG xmit_max_collisions;	// OID_802_3_XMIT_MAX_COLLISIONS
	ULONG rcv_overrun;			// OID_802_3_RCV_OVERRUN
	ULONG xmt_underrun;			// OID_802_3_XMIT_UNDERRUN
	ULONG xmt_heartbeat_failure;	// OID_802_3_XMIT_HEARTBEAT_FAILURE
	ULONG xmt_times_crs_lost;		// OID_802_3_XMIT_TIMES_CRS_LOST
	ULONG xmt_late_collisions;		// OID_802_3_XMIT_LATE_COLLISIONS
};

///////////////////////////////////////////////////////
// 802_5 Statistics
//
template<> struct KNdisStatsStruct<NdisMedium802_5> {
	ULONG line_errors;	// OID_802_5_LINE_ERRORS
	ULONG lost_frames;// OID_802_5_LOST_FRAMES
	ULONG burst_errors;//  OID_802_5_BURST_ERRORS
	ULONG ac_errors;//  OID_802_5_AC_ERRORS
	ULONG abort_delimiterss;//  OID_802_5_ABORT_DELIMETERS
	ULONG frame_copied_errors;//  OID_802_5_FRAME_COPIED_ERRORS
	ULONG frequency_errors;//  OID_802_5_FREQUENCY_ERRORS
	ULONG token_errors;//  OID_802_5_TOKEN_ERRORS
	ULONG internal_errors;//  OID_802_5_INTERNAL_ERRORS

};

/////////////////////////////////////////////////////
// TODO: Specializations for other mediums




////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Ethernet phys address
//
// ETHERNET_ADDRESS is a simpl wrapper around a 6-byte array.
// The conversions are defined to satisfy the OID cracker.
struct ETHERNET_ADDRESS {
	UCHAR  m_bytes[6];

	operator PVOID() { return (PVOID) m_bytes; }
	operator ULONG() { return * PULONG(m_bytes); }
	operator ULONGLONG() { return * PULONGLONG(m_bytes); }

	// Assignment
	ETHERNET_ADDRESS& operator=(const ETHERNET_ADDRESS& a)
			{ NdisMoveMemory(m_bytes, (void *)a.m_bytes, sizeof(m_bytes)); return *this; }
	//
	void Invalidate() { NdisZeroMemory(m_bytes, sizeof(m_bytes)); }
	// Check if it was not invalidated
	BOOLEAN IsValid() const { return *PULONG(m_bytes)!=0 && *PULONG(m_bytes+2)!=0; }

	// Comparison
	bool operator==(const ETHERNET_ADDRESS& a)
			{ return memcmp(m_bytes, a.m_bytes, sizeof(m_bytes))==0;}
};
typedef ETHERNET_ADDRESS * PETHERNET_ADDRESS;

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Token Ring Address - alias to 6-byte ETHERNET_ADDRESS
typedef ETHERNET_ADDRESS TOKENRING_ADDRESS;
typedef TOKENRING_ADDRESS * PTOKENRING_ADDRESS;

#endif // !defined(AFX_KNDISSTATS_H__6C3A3814_2B15_11D3_8F9E_00C04F7445B7__INCLUDED_)
