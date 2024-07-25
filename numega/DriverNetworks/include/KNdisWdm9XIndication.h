// KNdisWdm9XIndication.h - include file for class KNdisWdm9XIndication
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

#ifndef __KNDISWDM9XINDICATION_h__
#define __KNDISWDM9XINDICATION_h__

#include <kndis.h>

//////////////////////////////////////////////////////////////////////////////
// KNdisWdm9XIndication
//
// Provides a mechanism to indicate an NDIS_PACKET to NDIS in Win9X.
//
// It is required, in Win9X, to indicate a Packet to NDIS within the context
// of NDIS.
//
// A timer is used to re-enter the NDIS context. When the timer expires,
// OnDPCTimeout is called. This member then calls the
// <Adapter>::ProcessReceiveIndicate member.
//
// <Adapter>::ProcessReceiveIndicate is manditory for all miniport drivers that
// use this class and should call IndicateReceive() with the packet.
//
// ----------------------------------------------------------------------------
//
// Typical Usage: Assuming that there is an adapter named "myAdapter".
// 1) The following declarations must be present in the myAdapter class.
//
//	   VOID ProcessReceiveIndicate(PNDIS_PACKET Packet);
//	   KNdisWdm9XIndication<myAdapter>  m_PacketIndicate;
//
// 2) In the myAdapter::Initialize function initialize the object.
//
//     m_PacketIndicate.Initialize(this);
//
// 3) When a packet is ready for indication forward it to this object.
//
//	   m_PacketIndicate.ScheduleReceiveIndicate(Packet);
//
// 4) Provide the following implementation for myAdapter::ProcessReceiveIndicate.
//    This is the minimum needed.
//     VOID myAdapter::ProcessReceiveIndicate(PNDIS_PACKET Packet)
//     {
//           IndicateReceive(Packet);
//     }
//
template<class Adapter> class KNdisWdm9XIndication
{
public:
	// Local copy of the adapter's this pointer
	// Used by the NDIS_HANDLE operator
	Adapter*	a;

public:

	// Declaration of the timer callback routine
	KNDIS_DECLARE_TIMERCALLBACK(KNdisWdm9XIndication, OnDPCTimeout);

	// Timeout routine called by the timer.
	// This routine is responsible to remove a packet and call
	// back to the miniport adapter's ProcessReceiveIndicate
	// member.
	VOID OnDPCTimeout(void)
	{
		ASSERT(a);
		KNdisPacket Packet = m_ScheduleQueue.Head();
		while (Packet)
		{
			a->ProcessReceiveIndicate(Packet);
			m_ScheduleQueue.Remove(Packet);
			Packet = m_ScheduleQueue.Head();
		}
	}

	// ctor
	KNdisWdm9XIndication() : m_ScheduleQueue((BOOLEAN)TRUE),a(NULL) {};

	~KNdisWdm9XIndication(){}

	// Initialization routine
	VOID Initialize(Adapter* t)
	{
		ASSERT(t);
		a=t;
		m_Timer.Initialize(this, KNDIS_MEMBER_CALLBACK(OnDPCTimeout));
	}

	// This member receives the Packet and stores them into the
	// scheduling queue.
	VOID ScheduleReceiveIndicate(PNDIS_PACKET Packet)
	{
		ASSERT(Packet);
		m_ScheduleQueue.InsertTail(Packet);
		m_Timer.Set(0);
	}

	// Convert to system handle
	inline operator NDIS_HANDLE() const { return *a;}

private:
	KNdisTimer	m_Timer;
	KNdisPacketList  m_ScheduleQueue;
};

#endif __KNDISWDM9XINDICATION_h__
