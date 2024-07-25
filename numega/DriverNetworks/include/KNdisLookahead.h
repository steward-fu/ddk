// KNdisLookahead.h: interface for the KNdisLookahead class.
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

#if !defined(AFX_KNDISLOOKAHEAD_H__A1BBC6F3_7AAE_11D3_8FEF_00C04F7445B7__INCLUDED_)
#define AFX_KNDISLOOKAHEAD_H__A1BBC6F3_7AAE_11D3_8FEF_00C04F7445B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <kndis.h>

//////////////////////////////////////////////////////////////////////
// KNdisLookaheadIndication
//
// A template for medium-specific NDIS receive indication.
// The template is declared below and specialized (at the end of the file)
// for the supported medium types. Used internally by KNdisLookahead only.
//
template<const NDIS_MEDIUM M> struct KNdisLookaheadIndication;


//////////////////////////////////////////////////////////////////////
// KNdisLookahead
//
// Abstracts NDIS mechanism used to perform partial packet receives.
//
// The idea behind NDIS partial transfers is that a miniport driver
// lets the protocol(s) first examine the header and a small ("lookahead")
// portion of the incoming frame rather than indicating the whole frame at
// once. This is performed using media-specific NdisMXXXReceiveIndicate() API.
// The protocols then call into MiniportTransferData handler to retrieve
// the remainder of the packet.
//
// Template class KNdisLookahead encapsulates mechanism of partial indications.
// The template uses 2 parameters:
//
//  L	-	Max Length of the Lookahead buffer. The buffer is built into the class.
//	M	-	Medium type. Defines how NDIS is called to indicate frames; 802.3 is the default.
//
// USING THE CLASS
//
// 1. Include KNdisLookahead<> as a data member of your adapter (KNdisMiniAdapter-derived)
//	  class, e.g.
//
//			KNdisLookahead<256>	m_Lookahead;
//
// 2. In your adapter class, provide public method
//
//			UINT CopyDataUp(PVOID FrameBase, PUCHAR Buf, UINT Len, UINT Offset);
//
//	  that synchronously transfers the content of length Len, starting at Offset, of the current frame
//	  into buffer Buf. The frame is identified by an opaque to KNDIS value FrameBase. The driver
//	  writer uses FrameBase in a h/w specific way, e.g., it might indicate an offset in the on-card
//	  ring buffer to the current frame.
//
// 3. In your HandleInterrupt() handler, after figuring the length of the received data to be
//	  indicated and the "FrameBase" of the frame in the on-card memory, call
//
//			m_Lookahead.Indicate(this, header_len, lookah_len, packet_len, FrameBase);
//
//	  This causes the lookahed portion of the frame be cached in the KNdisLookahed instance,
//	  and a subsequent call to NDIS to indicate that lookahead. NDIS, in turn, exposes this
//	  lookahead to the bound protocol(s). If the protocol(s) are interested in the packet, the control
//	  is then passed to adapter's TransferData() handler. See step 5 below.
//	  The FrameBase value is is stored in the class and passed back to CopyDataUp() whenever it is called.
//	  Repeat as many times as many received frames found in the h/w this time around.
//
// 4. Before leaving your HandleInterrupt() handler (or, its "receive" part), call
//
//			m_Lookahead.IndicateComplete(this);
//
//	  at least once. This calls NDIS to indicate the end of the current receive burst.
//
// 5. In your TransferData() handler, call
//
//			m_Lookahead.Transfer(this, Packet, BytesToCopy, ByteOffset);
//
//	  This transfers the protocol-requested portion BytesToCopy of the current frame
//	  starting from ByteOffset into the protocol-provided Packet.
//
// NOTE
//	Between invokations of m_Lookahead.Indicate() and Adapter::TransferData() serialized
//  miniports do not get called (except, possibly, the ISR at DIRQL). The entire transaction
//  takes place at DISPATCH_LEVEL, and the interrupts are disabled on the card.
//  When TransferData() is called, the current frame (identified by FrameBase) is partially
//  cached in m_LookaheadData, and partially still stored on the card. The KNdisLookahead::Transfer()
//  method has smarts to figure where to transfer the requested data from. If the protocol(s) are not interested in the
//	frame the TransferData() handler might not get called at all, and the lookahead cache
//	will be overriden by the next received frame.
//
template <const UINT L, const NDIS_MEDIUM M=NdisMedium802_3>
class KNdisLookahead : protected KNdisLookaheadIndication<M>
{
public:
// Constructor
	KNdisLookahead() : m_uIndications(0) {}

///////////////////////////////////////////////////////////////////////
// Cache the lookahead from the card and indicate it to NDIS
//
// Parameters:
//	pAdapter	-	adapter indicating (usually, 'this') of type A.
//	HeaderSize	-	Frame header size (usually, a const like 14 for Ethernet)
//	LookaheadSize -	Total size being indicated (including the header)
//	PacketSize	-	Packet size (not including the header)
//	FrameBase	-	On-card "location" of the frame; opaque to the class.
// Returns:
// 	none
// Comments
//	Class A has to provide a public method
//			UINT A::CopyDataUp(PVOID FrameBase, PUCHAR Buf, UINT Len, UINT Offset);
//	which synchronously moves data from the card to the specified buffer at the
//	specified offset (relative to the FrameBase).
//	When CopyDataUp() is called by KNdisLookahead, the first parameter will be the
//  FrameBase value passed to Indicate().
//	Both type F and value FrameBase are opaque to KNdisLookahead. Usually,
//	FrameBase is a h/w-specific pointer identifying the currently processed received frame on the card.
// 	Indicate() is normally called one or more times during the receive frame processing
//	from adapter's A::HandleInterrupt() handler.
	template <class A, typename F>
	void Indicate(A* pAdapter,
				  UINT HeaderSize, UINT LookaheadSize, UINT PacketSize, F FrameBase)
	{
		ASSERT(LookaheadSize <= sizeof(m_LookaheadData));	// check a mismatch in parameters
		m_uIndicatedLength = min (LookaheadSize, sizeof(m_LookaheadData));
		m_FrameBase = (PVOID) FrameBase;
		pAdapter->CopyDataUp(m_FrameBase, m_LookaheadData, m_uIndicatedLength, 0);
		m_uIndications++;
		IndicateReceive(*pAdapter, this,
			(PCHAR) m_LookaheadData, HeaderSize,
			m_LookaheadData+HeaderSize, m_uIndicatedLength-HeaderSize,
			PacketSize);
	}

///////////////////////////////////////////////////////////////////////
// Indicate the end of the receive burst to NDIS
//
// Parameters:
// 	pAdapter             - Adapter indicating (usually, 'this') of type A
// Returns:
// 	none
// Comments:
// 	This function is normally called once during the receive frame processing
//	from adapter's A::HandleInterrupt() handler.
	template <class A>
	void IndicateComplete(A* pAdapter)
	{
		IndicateReceiveComplete(*pAdapter);
	}

///////////////////////////////////////////////////////////////////////
// Copy the (remainder of the) packet from the card.
//
// Parameters:
//	Adapter		-	Adapter pointer (usually, this) of type A
//  packet		-	Protocol-provided packet to copy the data into
//  BytesToCopy -	Number of bytes to copy
//	ByteOffset	-	Offset from the end of the start of the frame to copy from.
// Returns:
// 	Number of bytes actually copied
// Comments:
// 	Transfer() is normally called from adapter's A::TransferData() handler.
//	This method might call A::CopyDataUp() passing the FrameBase value passed
//	to the instance of KNdisLookahead earlier on Indicate().
	template <class A>
	UINT Transfer(A* Adapter, PNDIS_PACKET packet, UINT BytesToCopy, UINT ByteOffset)
	{
		UINT LenToCopy, copied, BytesCopied, BufLenUsed;
		KNdisPacket pkt(packet);
		KNdisBuffer buf = pkt.QueryFirstBuffer();

		BytesCopied = BufLenUsed = 0;

		while (BytesToCopy && buf.IsValid()) {
			// check where we need to move the portion from
			if (ByteOffset < m_uIndicatedLength) { // from the cache
				LenToCopy = min (m_uIndicatedLength-ByteOffset, BytesToCopy);
				copied = min (buf.Length()-BufLenUsed, LenToCopy);
				NdisMoveMemory
					(PCHAR(buf.Address())+BufLenUsed, m_LookaheadData+ByteOffset, copied);
			}
			else { // from the card
				LenToCopy = min (buf.Length()-BufLenUsed, BytesToCopy);
				copied = Adapter->CopyDataUp
					(m_FrameBase, PUCHAR(buf.Address())+BufLenUsed, LenToCopy, ByteOffset);
				ASSERT(copied <= LenToCopy);
			}
			BytesCopied += copied;
			ByteOffset += copied;
			BufLenUsed += copied;
			BytesToCopy -= copied;
			ASSERT(BufLenUsed <= buf.Length());
			if (BytesToCopy > 0 && BufLenUsed==buf.Length()) {
				buf = buf.GetNext(); // this buf was short, need next one
				BufLenUsed=0;
			}
		}
		return BytesCopied;
	}

// Get Frame base (opaque to the KNdisLookahead)
	PVOID GetFrameBase() const { return m_FrameBase; }
// Get ponter to the lookahead data
	operator PCHAR() const { return m_LookaheadData; }

protected:
	UINT		m_uIndicatedLength;		// lookahead actually being indicated
	UINT		m_uIndications;			// number of indications passed up
	PVOID		m_FrameBase;			// current frame "pointer" in the h/w, opaque to the class
	UCHAR		m_LookaheadData[L];		// cached header+lookahead
};



//////////////////////////////////////////////////////////////////////////////////////////
// KNdisLookaheadIndication Implementations
//
// NOTE
//
// Filter-based receive indications are, in fact, macro defined in ndis.h
// In ND 4 DDK, they were implemented via  function pointers (EthDB, TrDB, etc.)
// built into miniport structure directly.
// In W2K DDK RC2, the set of pointers has been replaced with an unnamed structure
// encapsulating those pointers.
// This breaks in C++. In RC3, the structure has been fixed, but the NdisMXXXIndicateReceive
// macro using it are still broken. The defs below are the tmp workaround:
//
#if (KNDIS_DDK_VERSION >= 5)
#define TrDB	FilterDbs.TrDB
#define FddiDB	FilterDbs.FddiDB
#define ArcDB	FilterDbs.ArcDB
#define EthDB	FilterDbs.EthDB
#endif

// NdisMedium802_3
template<> struct KNdisLookaheadIndication<NdisMedium802_3>
{
	void IndicateReceive(
			NDIS_HANDLE  MiniportAdapterHandle, PVOID MiniportReceiveContext,
			PVOID HeaderBuffer, UINT HeaderBufferSize,
			PVOID LookaheadBuffer, UINT LookaheadBufferSize, UINT PacketSize) {
				NdisMEthIndicateReceive(MiniportAdapterHandle, MiniportReceiveContext,
					(PCHAR)HeaderBuffer, HeaderBufferSize, LookaheadBuffer, LookaheadBufferSize,
					PacketSize);
	}
	void IndicateReceiveComplete(NDIS_HANDLE  MiniportAdapterHandle) {
			NdisMEthIndicateReceiveComplete(MiniportAdapterHandle);
	}
};

// NdisMedium802_5
template<> struct KNdisLookaheadIndication<NdisMedium802_5>
{
	void IndicateReceive(
			NDIS_HANDLE  MiniportAdapterHandle, PVOID MiniportReceiveContext,
			PVOID HeaderBuffer, UINT HeaderBufferSize,
			PVOID LookaheadBuffer, UINT LookaheadBufferSize, UINT PacketSize) {
				NdisMTrIndicateReceive(MiniportAdapterHandle, MiniportReceiveContext,
					HeaderBuffer, HeaderBufferSize, LookaheadBuffer, LookaheadBufferSize,
					PacketSize);
	}
	void IndicateReceiveComplete(NDIS_HANDLE  MiniportAdapterHandle) {
			NdisMTrIndicateReceiveComplete(MiniportAdapterHandle);
	}
};

// NdisMediumFddi
template<> struct KNdisLookaheadIndication<NdisMediumFddi>
{
	void IndicateReceive(
			NDIS_HANDLE  MiniportAdapterHandle, PVOID MiniportReceiveContext,
			PVOID HeaderBuffer, UINT HeaderBufferSize,
			PVOID LookaheadBuffer, UINT LookaheadBufferSize, UINT PacketSize) {
	// NOTE: NdisMFddiIndicateReceive macro is broken in DDK for C++ builds
	//       (type mismatch PCHAR vs PUCHAR) :(
#if KNDIS_DDK_VERSION >= 5
		(*((PNDIS_MINIPORT_BLOCK)(MiniportAdapterHandle))->FddiRxIndicateHandler)(	// NDIS 5 way
#else
		FddiFilterDprIndicateReceive(							// NDIS 4 way
#endif
			((PNDIS_MINIPORT_BLOCK)(MiniportAdapterHandle))->FddiDB,								\
			MiniportReceiveContext,
			PCHAR(HeaderBuffer) + 1,
			((((PUCHAR)HeaderBuffer)[0] & 0x40) ? FDDI_LENGTH_OF_LONG_ADDRESS
							: FDDI_LENGTH_OF_SHORT_ADDRESS),
			HeaderBuffer,
			HeaderBufferSize,
			LookaheadBuffer,
			LookaheadBufferSize,
			PacketSize
	);
	}
	void IndicateReceiveComplete(NDIS_HANDLE  MiniportAdapterHandle) {
			NdisMFddiIndicateReceiveComplete(MiniportAdapterHandle);
	}
};

#if ARCNET || (KNDIS_DDK_VERSION <= 5)
// NOTE: For NDIS 5.1, you need to #define ARCNET for this to compile
// NdisMediumArcnetRaw
template<> struct KNdisLookaheadIndication<NdisMediumArcnetRaw>
{
	void IndicateReceive(
			NDIS_HANDLE  MiniportAdapterHandle, PVOID MiniportReceiveContext,
			PVOID HeaderBuffer, UINT HeaderBufferSize,
			PVOID LookaheadBuffer, UINT LookaheadBufferSize, UINT PacketSize) {
				NdisMArcIndicateReceive(MiniportAdapterHandle,
				(PUCHAR) HeaderBuffer, (PUCHAR) LookaheadBuffer, PacketSize);
				UNREFERENCED_PARAMETERS3
					(HeaderBufferSize, LookaheadBufferSize, MiniportReceiveContext);
	}
	void IndicateReceiveComplete(NDIS_HANDLE  MiniportAdapterHandle) {
			NdisMArcIndicateReceiveComplete(MiniportAdapterHandle);
	}
};
#endif // KNDIS_DDK_VERSION <= 5


#endif // !defined(AFX_KNDISLOOKAHEAD_H__A1BBC6F3_7AAE_11D3_8FEF_00C04F7445B7__INCLUDED_)
