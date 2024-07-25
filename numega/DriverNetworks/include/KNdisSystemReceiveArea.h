// KNdisSystemReceiveArea.h: interface for the KNdisSystemReceiveArea class.
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

#if !defined(AFX_KNDISSYSTEMRECEIVEAREA_H__3A4EA9D4_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
#define AFX_KNDISSYSTEMRECEIVEAREA_H__3A4EA9D4_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<kndis.h>


/////////////////////////////////////////////////////////////////////////////
// KNdisSystemReceiveArea
//
// This derivation provides a "frame" area in the system heap. This could be
// used for non-bus master (PIO) NICs and simulations.
// The client of this class is required to provide 2 template parameters.
//
// The 1st template parameter is the client's KNdisMiniAdapter derived class.
// This class must provide method signatures that KNdisSystemReceiveArea will
// call to manipulate the h/w descriptors at appropriate times - setup,
// complete, and reclaim.  Whenever the NIC "completes" h/w descriptor(s),
// Complete() is to be called to advance the engine and return the packet, which
// becomes ready. This packet can then be NdisMReceiveIndicate-d.
// When NDIS is done with the packet (e.g. on MiniportReturnPacket), the driver
// calls Reclaim() to return the packet (and appropriate descriptor) to the "free list".
// The following are the required signatures:
//		void DescriptorInitialize(D* pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
//			This is called so that the client can initialize a h/w desc.
//			Usually called multiple times to initialize an entire group of descriptors
//		void DescriptorInvalidate(D* pHwDesc);
//			This is called so that the client can perform any acctions associated with
//			de-initializing a h/w desc.	Usually called multiple times to de-initialize
//			an entire group of descriptors
//		void DescriptorReclaim(D* pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
//			This is called during reclaim processing so the client can update
//			the h/w descriptor
//		void DescriptorComplete(D* pHwDesc, PNDIS_PHYSICAL_ADDRESS pPhysAddr);
//			This is called during complete processing so the client can update
//			the h/w descriptor
//
// The 2nd template parameter is the Hardware RFD structure (client defined).
// This structure must define the following static methods:
//		ULONG GetBufferLength(void);
//			BufferLength for the RFD used by NdisAllocateBuffer which could
//			be different than sizeof(D)
//		ULONG GetBufferOffset(void);
//			Offset from beginning of D (the h/w RFD) to the beginning of the
//			Buffer.  This is used so that the receive area can calculate the address
//			of the buffer.
// It is assumed that the buffer is contained within D.
//
template<class A, typename D>
class KNdisSystemReceiveArea : public KNdisReceiveArea
{
public:
	SAFE_DESTRUCTORS;

	KNdisSystemReceiveArea(
		A& Adapter,
		UINT NumOfDescriptors
		);

	~KNdisSystemReceiveArea();

	// Initialize descriptors
	void Initialize();

	// Packet previously removed by Complete() gets back
	void Reclaim(PNDIS_PACKET pkt);

	// Next receive is completed.
	PNDIS_PACKET Complete(D* pHwDesc=NULL);

protected:
	// frame descriptor area allocated from the heap
	D*	m_pHwDesc;

	// Pointer to client's derived Adapter class
	A&	m_Adapter;
};



///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pAdapter		 - Adapter object pointer (usually, 'this')
//	NumOfDescriptors - number of Receive Frame Descriptors(RFD)
// Returns:
// 	none
// Comments:
// 	none
template<class A, typename D>
KNdisSystemReceiveArea<A,D>::KNdisSystemReceiveArea(
	A& Adapter,
	UINT NumOfDescriptors
	) : KNdisReceiveArea(
			&Adapter,
			NumOfDescriptors,
			NumOfDescriptors,
			0
			),
		m_Adapter(Adapter)
{
	if ( ! m_Adapter.IsValid() )
	{
		ASSERT(!"m_Adapter.IsValid()\n");
		return;
	}

	// allocate descriptor area:
	m_pHwDesc = (D*) new UCHAR [NumOfDescriptors*sizeof(D)];
	if (m_pHwDesc==NULL)
	{
		ASSERT(!"m_pHwDesc");
		return;
	}
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
//	none
// Comments:
//	none
template<class A, typename D>
KNdisSystemReceiveArea<A,D>::~KNdisSystemReceiveArea()
{
	// Unchain all of the packets and call the "Manipulator" to further
	// handle the invalidation of the Hw descriptors
	// Can't call this while holding a spin lock
	D* pHwDesc;
	while (pHwDesc = reinterpret_cast<D*>(Unchain()))
	{
		m_Adapter.DescriptorInvalidate(pHwDesc);
	}
	// deallocate the descriptors
	delete m_pHwDesc;
}


///////////////////////////////////////////////////////////////////////
// Initialize descriptors
//
// Returns:
// 	NDIS_STATUS
template<class A, typename D>
void
KNdisSystemReceiveArea<A,D>::Initialize()
{
	UINT NumOfDescriptors = GetNumberDescriptors();
	ULONG dwBufLen = D::GetBufferLength();
	ULONG dwBufOffset = D::GetBufferOffset();

	// chain the buffers
	for (UINT i=0; i<NumOfDescriptors; i++)
	{
		PNDIS_PACKET p = KNdisReceiveArea::Chain( PUCHAR(m_pHwDesc+i) + dwBufOffset, dwBufLen );
		ASSERT(p);
		Desc_t* pSwDesc = *reinterpret_cast<Desc_t**>(p->MiniportReserved);
		pSwDesc->pHwDesc = m_pHwDesc+i;
		pSwDesc->HwDescPhys.HighPart = NULL;
		pSwDesc->HwDescPhys.LowPart = NULL;

        NDIS_SET_PACKET_HEADER_SIZE(p, KNdisAdapterTraits<A>::HeaderSize());

		m_Adapter.DescriptorInitialize(m_pHwDesc+i, &pSwDesc->HwDescPhys);
	}
}


///////////////////////////////////////////////////////////////////////
// Next receive is completed.
//
// Parameters:
// 	none
// Returns:
// 	PNDIS_PACKET taken off the head of the free list, NULL if empty list
// Comments:
// 	Sets the status of the packet to NDIS_STATUS_RESOURCES if a low
//	resource condition has been reached.  If async memory support is
//	required, reports use of the memory to the Allocator class.
//	Usually called by the client from the RxDPC.
template<class A, typename D>
PNDIS_PACKET
KNdisSystemReceiveArea<A,D>::Complete(D* pHwDesc)
{
	PNDIS_PACKET pkt = KNdisReceiveArea::Complete(pHwDesc);
	m_Adapter.DescriptorComplete(reinterpret_cast<D*>(GetDescriptor(pkt)), GetPhysicalAddress(pkt));
	return pkt;
}


///////////////////////////////////////////////////////////////////////
// Packet previously removed by Complete() gets back
//
// Parameters:
// 	pkt		- PNDIS_PACKET to put back on the free list
// Returns:
// 	none
// Comments:
//	If async memory support is required, reports use of the memory
//	to the Allocator class.  Usually called by the client from the
//	ReturnPackets handler and from the RxDPC to reclaim failed packets.
template<class A, typename D>
void
KNdisSystemReceiveArea<A,D>::Reclaim(PNDIS_PACKET pkt)
{
	m_Adapter.DescriptorReclaim(reinterpret_cast<D*>(GetDescriptor(pkt)), GetPhysicalAddress(pkt));
	KNdisReceiveArea::Reclaim(pkt);
}

#endif // !defined(AFX_KNDISSYSTEMRECEIVEAREA_H__3A4EA9D4_0AFF_11D4_AFEE_00C04F6A5DA9__INCLUDED_)
