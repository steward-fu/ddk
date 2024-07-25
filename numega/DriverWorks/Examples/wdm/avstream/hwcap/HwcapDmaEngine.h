// HwcapDmaEngine.h: interface for the HwcapDmaEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWCAPDMAENGINE_H__7E9DB63C_381A_4027_BD65_DC8DC0AFD776__INCLUDED_)
#define AFX_HWCAPDMAENGINE_H__7E9DB63C_381A_4027_BD65_DC8DC0AFD776__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
// HwcapDmaEngine
//
// Scatter/gather DMA Simulation.
//
// Maintains a list of active DMA "scatter/gather" entries associated
// with pending "empty" stream frames.
//
// On each pin's OnProcess() request a list of new available mappings is
// added by calling AddMappings(). On each interrupt, these mappings get
// filled in with image data by calling CompleteMappings() from the DPC.
// As the mappings get filled in, the DPC advances the stream pointer
// accordingly.
//
class HwcapDmaEngine 
{
public:
	HwcapDmaEngine(ULONG MaxScatterGatherEntries=128);
	~HwcapDmaEngine();

	ULONG AddMappings(
		PUCHAR* VirtualAddress, 
		KSSTREAM_POINTER_OFFSET& Mapping);

	ULONG CompleteMappings(PUCHAR Image, ULONG ImageSize);

	void DiscardMappings();

protected:

	// "DMA entry" descriptor for the "hardware"
	struct ScatterGatherEntry : public KHeapClient<ScatterGatherEntry> {
		LIST_ENTRY m_ListEntry;
		PUCHAR Address;		/// virtual
		ULONG ByteCount;

		ScatterGatherEntry(PUCHAR Addr, ULONG Count) :
			Address(Addr),ByteCount(Count) {}  
	};

	KInterlockedList<ScatterGatherEntry>	m_List;
	KHeap<ScatterGatherEntry>				m_Heap;
	ULONG									m_MaxScatterGatherEntries; // limit for growth
	ULONG									m_EntriesQueued;
	ULONG									m_BytesQueued;
};

#endif // !defined(AFX_HWCAPDMAENGINE_H__7E9DB63C_381A_4027_BD65_DC8DC0AFD776__INCLUDED_)
