// NmOpenContext.cpp
// Implementation of NmOpenContext device class
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
//

#include <vdw.h>

#include "NmProt.h"
#include "NmOpenContext.h"
#include "NmProtIoctl.h"
#include "NmProtBinding.h"

#pragma warning(disable:4065) // Allow switch statement with no cases

extern	KTrace	t;			// Global driver trace object

////////////////////////////////////////////////////////////////////////
// NmOpenContext::~NmOpenContext
//
//	Routine Description:
//		This is the destructor for the NmOpenContext
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		Disconnect and release resources here.
//
//		Although the use of SAFE_DESTRUCTORS in the class definition cures
//		improper emission of the destructor into the INIT section most of the
//		time, certain rare cases can still cause improper behavior.  To avoid
//		these cases, the destructor	must preceed the INIT section, causing it
//		to be referenced first by the default compiler section.
//

NmOpenContext::~NmOpenContext()
{
}


////////////////////////////////////////////////////////////////////////////////
//  NmOpenContext::NmOpenContext
//
//	Routine Description:
//		The device constructor is typically responsible for allocating
//		any physical resources that are associated with the device.
//
//	Parameters:
//		Binding		-	NDIS Binding Object, which this device represents
//
//	Return Value:
//		None
//
//	Comments:
//
NmOpenContext::NmOpenContext(NmProtBinding* Binding, PNDIS_STRING pDeviceName) :
	m_pBinding(Binding),
	m_DeviceName(pDeviceName),
	m_Flags(NULL),
	m_pFileObject(NULL)
{
	NPROT_SET_FLAGS(m_Flags, NUIOO_BIND_FLAGS, NUIOO_BIND_OPENING);

	// Allocate an NDIS packet pool to serve the i/o requests:
	NTSTATUS Status = m_PacketPool.Initialize(NUMBER_OF_PACKET_DESCRIPTORS, m_RcvList.ReservedSize());
	if (!NT_SUCCESS(Status))
	{
		ASSERT(!"Alloc Packet Pool");
	}

#if NDIS51
	// Init Cancel ID generator. We'll use it to label packets-on-the-fly in NmOpenContext::Write()
	m_CancelId.Initialize();
#endif
}

///////////////////////////////////////////////////////////////////////
// Cancel all pending reads
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	This internal routine is called on
//  (1) device cleanup irp
//  (2) unbinding
void NmOpenContext::CancelAllPendingReads()
{
	if (!m_RcvList.IsEmpty())
		t << "Cancelling " << m_RcvList.Count() << " pending read-s" << EOL;

	KNdisPacket packet;

	// In order to make list access atomic, a secondary list is used as a temporary
	// storage for the packet elements.
	PacketList tempList;
	m_RcvList.Lock();
	while ( (packet=m_RcvList.RemoveHeadNoLock()).IsValid() )
		tempList.InsertTail(packet);
	m_RcvList.Unlock();

	while ( (packet=tempList.RemoveHead()).IsValid() )
	{
		KIrp I = PacketList::GetContext(packet)->Irp;
		m_PacketPool.Free(packet);
		I.SetCancelRoutine(NULL);
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
		m_PendingIrps.Decrement();
	}
}


///////////////////////////////////////////////////////////////////////
// Waits for pending IRPs to complete. This includes both read, write
// on oid-s.
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Must be called at PASSIVE_LEVEL
void NmOpenContext::WaitForIrpsToComplete()
{
#ifdef NDIS51
	// In 5.1 let's ensure all the IRPs are completed.
	// NOTE that maintaining m_PendingIrps is reduntant in this case
	//      since we always cancel reads before calling WaitForIrpsToComplete.
	if (m_pBinding)
	{
		ULONG IoCount, loop=30;
		while ((IoCount=m_pBinding->QueryPendingIOCount()) && --loop)
		{
			t << "Waiting for " << IoCount << " IRPs to comlete..." << EOL;
			m_pBinding->Sleep(1000);
		}
		ASSERT(IoCount==0); // or smth got stuck
	}
#endif
	m_PendingIrps.Wait();
}
