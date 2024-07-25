// HwcapDmaEngine.cpp: implementation of the HwcapDmaEngine class.
//
//////////////////////////////////////////////////////////////////////
#include	<vdw.h>
#include	<kavstream.h>

#include "Hwcap.h"
#include "HwcapDmaEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HwcapDmaEngine::HwcapDmaEngine(ULONG MaxScatterGatherEntries):
	m_EntriesQueued(0), m_BytesQueued(0)
{
	m_Heap.Initialize((USHORT) (m_MaxScatterGatherEntries=MaxScatterGatherEntries),
		DefaultPoolTag);
	if (!m_Heap.IsValid()) {
		ASSERT(!"No heap");
	}
}

HwcapDmaEngine::~HwcapDmaEngine()
{
	// there should be no entries on destruction
	DiscardMappings();
	ASSERT(m_List.Count()==0);
}



///////////////////////////////////////////////////////////////////////
// AddMappings
//
// Parameters:
// 	VirtualAddress       - Virtual address of the buffer
// 	Mapping              - Mappings for this address
// Returns:
// 	Number of mappings used in the supplied Mapping descriptor
// Comments:
// 	Program DMA hardware for given VirtualAddress described by given
//  physical Mapping.
//
ULONG HwcapDmaEngine::AddMappings(
		PUCHAR* VirtualAddress, 
		KSSTREAM_POINTER_OFFSET& Mapping)
{
	// Simply populate the m_List with s/g entries to simulate
	// that we actually do something

	ULONG MappingsCount = Mapping.Remaining;
	PKSMAPPING Mappings = Mapping.Mappings;
	ULONG MappingStride = KsDmaAdapter::MappingStride((HwcapDevice*)0);

    m_List.Lock();

	ULONG EntriesBefore = m_EntriesQueued;

    for (ULONG i = 0; i < MappingsCount && m_EntriesQueued < m_MaxScatterGatherEntries; i++)
	{
		// alloc and init s/g entry. This is fake: in a real h/w you'd deal with
		// Mappings->PhysicalAddress-s instead

		ScatterGatherEntry* entry = 
			new (&m_Heap) ScatterGatherEntry (*VirtualAddress, Mappings->ByteCount);

        if (!entry) 
            break;

		// Queue it

		// g_Trace.Trace(TraceInfo, "Inserting entry %p: %p %u\n", entry, entry->Address, entry->ByteCount);

		m_List.InsertTailNoLock(entry);
		m_EntriesQueued++;
		m_BytesQueued += entry->ByteCount;


		// advance the buffer and mapping pointers

        *VirtualAddress += Mappings->ByteCount;
        Mappings = reinterpret_cast <PKSMAPPING> (
				(reinterpret_cast <PUCHAR> (Mappings) + MappingStride) );

    }

	ULONG Queued = m_EntriesQueued - EntriesBefore;

    m_List.Unlock();

	g_Trace << "HwcapDmaEngine::AddMappings() " << Queued << EOL;

    return Queued;

}


///////////////////////////////////////////////////////////////////////
// HwcapDmaEngine::CompleteMappings
//
// Parameters:
// 	nEntries             - Entries completed
// Returns:
// 	Number of actually completed entries
// Comments:
//  Called when the Image is ready to be chunked into the DMA buffers.
// 	Remove entries for which the DMA has been completed
ULONG HwcapDmaEngine::CompleteMappings(PUCHAR Image, ULONG ImageSize)
{
	// Walk over the list the waiting entries, fill them in and dispose

    PUCHAR Buffer = Image;
    ULONG BufferRemaining = ImageSize;

	m_List.Lock();

	ULONG EntriesBefore = m_EntriesQueued;

	while (BufferRemaining && 
		   m_EntriesQueued && 
		   m_BytesQueued >= BufferRemaining) {

		ScatterGatherEntry* entry = m_List.RemoveHeadNoLock();
		ASSERT(entry);

		ULONG BytesToCopy = (BufferRemaining < entry->ByteCount) ?
            BufferRemaining : entry->ByteCount;

		// g_Trace.Trace(TraceInfo, "Copying entry %p: %p %u\n", entry, entry->Address, entry->ByteCount);

		memcpy(entry->Address, Buffer, BytesToCopy);

        BufferRemaining -= BytesToCopy;
        Buffer += BytesToCopy;
        m_EntriesQueued--;
        m_BytesQueued -= entry->ByteCount;

		delete entry;

	};
	
	ULONG Dequeued = EntriesBefore - m_EntriesQueued ;

	m_List.Unlock();

	if (Dequeued)
		g_Trace << "HwcapDmaEngine::CompleteMappings() " << Dequeued << EOL;

	return Dequeued;

}


///////////////////////////////////////////////////////////////////////
// HwcapDmaEngine::DiscardMappings
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Free pending mappings
//
void HwcapDmaEngine::DiscardMappings()
{
	ScatterGatherEntry* entry;
	ULONG Dequeued = 0;

	while (entry = m_List.RemoveHead()) {
		delete entry;
		Dequeued++;
	}

	g_Trace << "HwcapDmaEngine::DiscardMappings() " << Dequeued << EOL;
}
