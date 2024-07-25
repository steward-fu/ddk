// HwcapPin.cpp: implementation of the HwcapPin class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Hwcap.h"
#include "HwcapFormats.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HwcapPin::HwcapPin() :
 KsPin(), 
	 m_VideoInfoHeader(0),  m_pHardware(NULL), m_bPending(FALSE), 
	 m_pClock(0)
{

}

HwcapPin::~HwcapPin()
{
	ASSERT(m_pHardware==NULL);
	delete m_pHardware;
	delete m_VideoInfoHeader;
	if (m_pClock) m_pClock->Release();
}



///////////////////////////////////////////////////////////////////////
// HwcapPin::OnCreate
//
// Parameters:
// 	I                    - IRP_MJ_CREATE 
// Returns:
// 	STATUS_SUCCESS or STATUS_PENDING
// Comments:
// 	See AVStrMiniPinCreate
NTSTATUS HwcapPin::OnCreate(KIrp I)
{
    NTSTATUS status;

	// Get KS objects:

	PKSPIN Pin		 = PKSPIN(*this);
	PKSFILTER Filter = KsPin::Parent();

	g_Trace << "HwcapPin::OnCreate, id=" << Pin->Id << EOL;

    //
    // Indicate the extended header size.
    //
    Pin->StreamHeaderSize = sizeof(KSSTREAM_HEADER) + sizeof(KS_FRAME_INFO);

	// cache the header info

	m_VideoInfoHeader = new (Pin) KsVideoInfoHeader;

	if (m_VideoInfoHeader==NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	// lock framing descriptor for editing

    if ((status = KsEdit(Pin, &Pin->Descriptor, 'deks')) == STATUS_SUCCESS) 
		if ((status = KsEdit(Pin, &Pin->Descriptor->AllocatorFraming, 'deks')) == STATUS_SUCCESS) {

        PKSALLOCATOR_FRAMING_EX framing = const_cast<PKSALLOCATOR_FRAMING_EX>(Pin->Descriptor->AllocatorFraming);

        framing->FramingItem[0].Frames = 2;
        framing->FramingItem[0].PhysicalRange.MinFrameSize = 
        framing->FramingItem[0].PhysicalRange.MaxFrameSize = 
        framing->FramingItem[0].FramingRange.Range.MinFrameSize = 
        framing->FramingItem[0].FramingRange.Range.MaxFrameSize = (*m_VideoInfoHeader)->bmiHeader.biSizeImage;
        framing->FramingItem[0].PhysicalRange.Stepping = 
        framing->FramingItem[0].FramingRange.Range.Stepping = 0;
	}
                    
    return status;
}


///////////////////////////////////////////////////////////////////////
// HwcapPin::OnSetDeviceState
//
// Parameters:
// 	From                 - The target KSSTATE after receipt of the IRP
// 	To                   - The previous KSSTATE
// Returns:
// 	STATUS_SUCCESS or the error code
// Comments:
// 	Pins that use the standard transport mechanism receive 
//  filtered (single step) state changes through the owning pipe through 
//  this routine.
//  Non-standard transports receive non-filtered (1+ step) state changes
//  through this routine. See AVStrMiniPinSetDeviceState
//
NTSTATUS HwcapPin::OnSetDeviceState(KSSTATE To, KSSTATE From)
{
	g_Trace << "HwcapPin::OnSetDeviceState, id=" << PKSPIN(*this)->Id 
		    << " from " << From << " to " << To << EOL;

    NTSTATUS  Status = STATUS_SUCCESS;

	// Get parent device object to access hardware

	HwcapDevice* device = reinterpret_cast<HwcapDevice*> (KsPin::GetDevice());

	ASSERT(device);
    
    switch (To) 
    {
        case KSSTATE_STOP:
			//
			// stop hardware, release clocks (if any) and release resources
			//
			if (m_pHardware) {
				if (m_pHardware->IsOn()) {
					m_pHardware->Stop();
					m_pHardware->WaitForStop();
				}

				device->m_HardwareResources.Release(m_pHardware);
				m_pHardware = NULL;
			}

			// Release clocks
			if (m_pClock) {
				m_pClock->Release();
				m_pClock = NULL;
			}

			DeleteClones();

            break;
            
        case KSSTATE_ACQUIRE:
            //
            // This is a KS only state, that has no correspondence in DirectShow
			// Acquire hardware resources at this point.
			// If successful, configure it, too
            // 

			if (From == KSSTATE_STOP) {
				ASSERT(m_pHardware==0);
				m_pHardware = device->m_HardwareResources.Acquire(*this);
				if (m_pHardware) {
					Status = m_pHardware->Configure(*m_VideoInfoHeader, this);
					ASSERT(NT_SUCCESS(Status));
					
				}
				else {
					ASSERT(!"KSSTATE_ACQUIRE");
					Status = STATUS_INSUFFICIENT_RESOURCES;
				}

				// Get clock interface
				m_pClock = KsPin::GetReferenceClock();
				if (m_pClock) {
					g_Trace.Trace(TraceInfo, "Assigned clocks %p\n", m_pClock);
				}

			} else {
				// this sample supports only ordered +/- transitions => XP only
				g_Trace << "Non standard state transition" << EOL;
			}


            break;
            
        case KSSTATE_PAUSE:
			// Pause toggles
			if (m_pHardware == NULL)
				break;
			if (From == KSSTATE_RUN) { 
				m_pHardware->Stop();
				m_pHardware->WaitForStop();
				break;
			}
            else; // fall through: pause toggles!
			//break;
            
        case KSSTATE_RUN:
			// start hardware simulation
			//
			ASSERT(m_pHardware);
			if (!m_pHardware->IsOn())
				m_pHardware->Start();
            break;
    }
    
	return Status;
}


///////////////////////////////////////////////////////////////////////
// HwcapPin::OnProcess
//
// Parameters:
//	none
// Returns:
// 	STATUS_SUCCESS to continue processing. Return STATUS_PENDING to stop 
//  processing until the next triggering 
// Comments:
// 	Called when new data arrives into a previously empty queue. Alternatively,
//  it can be called every time a data frame arrives, regardless of whether 
//  the queue was previously empty or not. To do this, set 
//  KSPIN_FLAG_INITIATE_PROCESSING_ON_EVERY_ARRIVAL. Or, to indicate that 
//  this routine should never be called except in the event of a minidriver 
//  call to KsPinAttemptProcessing, set KSPIN_FLAG_DO_NOT_INITIATE_PROCESSING.
//
NTSTATUS  HwcapPin::OnProcess()
{
	g_Trace << "HwcapPin::OnProcess()" << EOL;

	NTSTATUS Status=STATUS_SUCCESS;

	// What's going on over here:
	//
	// 1) Get the leading edge stream pointer. It's locked by default;
	// 2) Clone the leading edge pointer into the "current" stream pointer;
	// 3) Program the "DMA Hardware" with the current pointer mappings. In essense,
	//    this would queue on-the-fly some "dma buffers" to-be-filled onto the hardware;
	// 4) Advance the leading pointer by the number of mappings queued to the hardware.
	//    Note that the current frame is "locked" by the current stream pointer referencing it;
	// 5) Return. Note that no data transfer happens yet; the generated image data are moved
	//    into the queued "dma buffers" on the faked interrupt. At that time the current
	//    stream pointer gets unlocked, which in turn, frees the pointed frame and let it
	//    move by AVStream off the pin to the next node in the graph.

	KsStreamLeadingPointer	Leading(*this);
	KsStreamPointer			Clone;
	StreamPointerContext_t*	pContext;

	if (Leading.IsValid()) {
		
		if (m_Current.IsValid()) {
			
			// still processing previous frame
			
			Clone = m_Current;
			pContext = m_Current.Context(this);
			
			// g_Trace.Trace(TraceInfo, "Processing PREVIOUS clone %p\n", PKSSTREAM_POINTER (Clone));
		}
		
		else {
			
			// New frame: get new clone for the leading
			
			Clone = Leading.Clone(this);
			
			if (Clone.IsValid()) {
				
				// g_Trace.Trace(TraceInfo, "Processing NEW clone %p\n", PKSSTREAM_POINTER (Clone));

				// Store the context. 
				
				pContext = Clone.Context(this);
				pContext->VirtualBuffer = (PUCHAR) PKSSTREAM_HEADER(Clone)->Data;
				PKSSTREAM_HEADER(Clone)->DataUsed = 0;
			}
			else {
				g_Trace.Trace(TraceWarning, "HwcapPin::OnProcess - can't clone\n");
				Leading.Unlock();
				ReturnPending();
			}
		}

		// Program hardware. If we didn't use all the mappings, save the current
		// stream pointer for the next OnProcess():

		KSSTREAM_POINTER_OFFSET& Mapping = PKSSTREAM_POINTER(Leading)->OffsetOut;

		if (Mapping.Remaining) {

			ULONG MappingsUsed = m_pHardware->SetupDma(&pContext->VirtualBuffer, Mapping);

			m_Current = (MappingsUsed == Mapping.Remaining) ? (PKSSTREAM_POINTER) NULL : Clone;

			// If we added mappings, then the h/w still has resources => advance the leading edge. 
			// Otherwise pend until h/w release some resources on interrupt

			Status = (MappingsUsed) ? Leading.AdvanceOut(MappingsUsed) : STATUS_PENDING;
		}
		else { 
			g_Trace.Trace(TraceWarning, "HwcapPin::OnProcess - no mappings in the stream\n");
			Status = STATUS_PENDING;
		}

		// Unlock the leading so that more data can be added.
		// STATUS_DEVICE_NOT_READY indicates we ran off the end of the queue

		if (NT_SUCCESS (Status))
			Leading.Unlock();
		else if (Status == STATUS_DEVICE_NOT_READY)
			Status = STATUS_SUCCESS;
	}
	else {
		g_Trace.Trace(TraceWarning, "HwcapPin::OnProcess - leading edge not avail.\n");
		ReturnPending();
	}


	if (!NT_SUCCESS (Status) || Status == STATUS_PENDING)
        m_bPending = TRUE;

	return Status;
}

///////////////////////////////////////////////////////////////////////
// HwcapPin::OnSetDataFormat
//
// Parameters:
//  Pin				 -	   Pin to be created
// 	DataRange            - A range out of our list of data ranges which was 
//                         determined to be at least a partial match for Pin -> ConnectionFormat.
// 	AttributeRange       - The attribute range
// Returns:
// 	STATUS_SUCCESS if Pin's ConnectionFormat member matches the range 
//  that was passed to this routine.
//  STATUS_NO_MATCH if ConnectionFormat does not match the passed range. 
// Comments:
//  Static method called on format verification BEFORE the object is created.
//
NTSTATUS  HwcapPin::OnSetDataFormat(
				PKSPIN Pin,
				const KSDATARANGE* DataRange, 
				const KSATTRIBUTE_LIST* AttributeRange)
{
	g_Trace << "HwcapPin::OnSetDataFormat VERIFY, id=" << Pin->Id << EOL;

	const GUID VideoInfoSpecifier = 
        {STATICGUIDOF(KSDATAFORMAT_SPECIFIER_VIDEOINFO)};

    if (IsEqualGUID (Pin -> ConnectionFormat -> Specifier, VideoInfoSpecifier)) {

        PKS_DATAFORMAT_VIDEOINFOHEADER ConnectionFormat =
            reinterpret_cast <PKS_DATAFORMAT_VIDEOINFOHEADER> 
                (Pin -> ConnectionFormat);

        //
        // DataRange comes out of OUR data range list, so it should be OK 
        //
        const KS_DATARANGE_VIDEO *VIRange =
            reinterpret_cast <const KS_DATARANGE_VIDEO *>
                (DataRange);

        //
        // Check that the format is a match for the selected range. 
        //
        if (
            (ConnectionFormat -> VideoInfoHeader.bmiHeader.biWidth !=
                VIRange -> VideoInfoHeader.bmiHeader.biWidth) ||

            (ConnectionFormat -> VideoInfoHeader.bmiHeader.biHeight !=
                VIRange -> VideoInfoHeader.bmiHeader.biHeight) ||

            (ConnectionFormat -> VideoInfoHeader.bmiHeader.biCompression !=
                VIRange -> VideoInfoHeader.bmiHeader.biCompression) 

            ) {

            return STATUS_NO_MATCH;

        } 
	}

	return STATUS_SUCCESS;
};

///////////////////////////////////////////////////////////////////////
// HwcapPin::OnSetDataFormat
//
// Parameters:
// 	OldFormat            - The previous format used on this pin. 
// 	OldAttributeList     - The old attribute list for the prior format
// 	DataRange            - A range out of our list of data ranges which was 
//                         determined to be at least a partial match for Pin -> ConnectionFormat.
// 	AttributeRange       - The attribute range
// Returns:
// 	STATUS_SUCCESS if Pin's ConnectionFormat member matches the range 
//  that was passed to this routine.
//  STATUS_NO_MATCH if ConnectionFormat does not match the passed range. 
// Comments:
//  Called on format change.
//
NTSTATUS  HwcapPin::OnSetDataFormat(
				PKSDATAFORMAT OldFormat, 
				PKSMULTIPLE_ITEM OldAttributeList, 
				const KSDATARANGE* DataRange, 
				const KSATTRIBUTE_LIST* AttributeRange)
{
	ASSERT (OldFormat); // would be a Framework bug

	g_Trace << "HwcapPin::OnSetDataFormat CHANGE, id=" << PKSPIN(*this)->Id << EOL;

	PKSPIN Pin = PKSPIN(*this);

	// verify if the format is acceptable

	if ( OnSetDataFormat(Pin, DataRange, AttributeRange) == STATUS_SUCCESS ) {

		// OK. Set the new format by caching the new video header
		// Don't accept dynamic changes

        if (Pin -> DeviceState == KSSTATE_STOP)  {
			if (m_VideoInfoHeader)
				delete m_VideoInfoHeader;
			m_VideoInfoHeader = new (Pin) KsVideoInfoHeader;
			if (m_VideoInfoHeader==NULL)
				return STATUS_INSUFFICIENT_RESOURCES;
		}
		else {
			ASSERT(!"Should be here");
			return STATUS_INVALID_DEVICE_STATE;
		}

	}
	else
		return STATUS_NO_MATCH;

	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// HwcapPin::OnIntersection
//
// Parameters:
// 	Irp                  - IRP with the request
// 	CallerDataRange      - Ptr to one of the data ranges supplied by the client
// 	DescriptorDataRange  - Ptr to one of the data ranges from the pin descriptor
// 	BufferSize           - Size in bytes of the buffer pointed to by the Data
// 	Data                 - Pointer to the buffer to contain the data format
//					       structure representing the best format in the intersection of the
//						   two data ranges. Can be NULL.
// 	DataSize             - Resulting buffer size copied
// Returns:
// 	STATUS_SUCCESS if there is an intersection and it fits in the supplied
//  buffer, STATUS_BUFFER_OVERFLOW for successful size queries, STATUS_NO_MATCH
//  if the intersection is empty, or STATUS_BUFFER_TOO_SMALL if the supplied
//  buffer is too small.
// Comments:
// 	none
NTSTATUS HwcapPin::OnIntersection (KIrp /*Irp*/, 
			PKSDATARANGE CallerDataRange, 
			PKSDATARANGE DescriptorDataRange, 
			ULONG BufferSize, 
			PVOID Data, 
			PULONG DataSize)
{
	g_Trace << "HwcapPin::OnIntersection, BufSize=" << BufferSize << " " << EOL;

    PKS_DATAFORMAT_VIDEOINFOHEADER FormatVideoInfoHeader = PKS_DATAFORMAT_VIDEOINFOHEADER(Data);
    ULONG						   DataFormatSize;

    const GUID VideoInfoSpecifier = 
        {STATICGUIDOF(KSDATAFORMAT_SPECIFIER_VIDEOINFO)};
    

    // Make sure this is the videa data range

    if (!IsEqualGUID(CallerDataRange->Specifier, VideoInfoSpecifier ))  {
		    return STATUS_NO_MATCH;
	}

	
    PKS_DATARANGE_VIDEO callerDataRange = PKS_DATARANGE_VIDEO(CallerDataRange);
    PKS_DATARANGE_VIDEO descriptorDataRange = PKS_DATARANGE_VIDEO(DescriptorDataRange);

    //
    // Check that the other fields match
    //
    if ((callerDataRange->bFixedSizeSamples != 
            descriptorDataRange->bFixedSizeSamples) ||
        (callerDataRange->bTemporalCompression != 
            descriptorDataRange->bTemporalCompression) ||
        (callerDataRange->StreamDescriptionFlags != 
            descriptorDataRange->StreamDescriptionFlags) ||
        (callerDataRange->MemoryAllocationFlags != 
            descriptorDataRange->MemoryAllocationFlags) ||
        (RtlCompareMemory (&callerDataRange->ConfigCaps,
                &callerDataRange->ConfigCaps,
                sizeof (KS_VIDEO_STREAM_CONFIG_CAPS)) != 
                sizeof (KS_VIDEO_STREAM_CONFIG_CAPS))) 
    {
		g_Trace.Trace(TraceWarning, "Field mismatch OnIntersection()");
        return STATUS_NO_MATCH;
    }
    
	// get required size:

    DataFormatSize = sizeof( KSDATAFORMAT ) + KS_SIZE_VIDEOHEADER( &callerDataRange->VideoInfoHeader );

	// for size query, return the required size
        
    if (BufferSize == 0) 
    {
        *DataSize = DataFormatSize;
        g_Trace.Trace(TraceWarning, "Required size OnIntersection() = %d bytes\n", DataFormatSize);
        return STATUS_BUFFER_OVERFLOW;            
    } else if (BufferSize < DataFormatSize)  {

        return STATUS_BUFFER_TOO_SMALL;	// error

	}
    

    // Buffer is OK. Copy over the KSDATAFORMAT, followed by the actual VideoInfoHeader
        
    *DataSize = DataFormatSize;
        
    // Copy over the KSDATAFORMAT 
    
    memcpy(&FormatVideoInfoHeader->DataFormat, DescriptorDataRange, sizeof( KSDATARANGE ));
    FormatVideoInfoHeader->DataFormat.FormatSize = DataFormatSize;

    // Copy over the caller's requested VIDEOINFOHEADER

    memcpy(&FormatVideoInfoHeader->VideoInfoHeader, &callerDataRange->VideoInfoHeader,
        KS_SIZE_VIDEOHEADER( &callerDataRange->VideoInfoHeader ) );

    // Calculate biSizeImage for this request, and put the result in both
    // the biSizeImage field of the bmiHeader AND in the SampleSize field
    // of the DataFormat.
	//
	// Adjust for compression if any

    FormatVideoInfoHeader->VideoInfoHeader.bmiHeader.biSizeImage =
        FormatVideoInfoHeader->DataFormat.SampleSize = 
        KS_DIBSIZE( FormatVideoInfoHeader->VideoInfoHeader.bmiHeader );

    return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// HwcapPin::DeleteClones
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
void HwcapPin::DeleteClones()
{
	KsStreamPointer Clone = KsPin::GetFirstClone();

	while (Clone.IsValid()) {
			KsStreamPointer ThisClone = Clone;
			Clone = Clone.GetNextClone();
			ThisClone.Release();
	}
}

///////////////////////////////////////////////////////////////////////
// HwcapPin::DPC
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Handle the "interrupt". 
//  Simulate image transfer into DMA buffers and satisfy pending cloned
//  stream pointers.
//
void HwcapPin::DPC()
{

	// Generate image into an internal frame store & fill the dma buffers 
	// with the image. 

	ULONG MappingsRemaining = m_pHardware->CompleteDma();

	// Walk over the list of clones and delete
	// the ones for each the mappings have been filled

	KsStreamPointer Clone = KsPin::GetFirstClone();

	while (MappingsRemaining && Clone.IsValid()) {


		// Update the stream header associated with given clone

		ULONG MappingsCompletedInThisClone = (MappingsRemaining > Clone.OffsetOut().Remaining) ?
			Clone.OffsetOut().Remaining : MappingsRemaining;

		for (ULONG i=0; i<MappingsCompletedInThisClone; i++)
			Clone.Header()->DataUsed += Clone.OffsetOut().Mappings[i].ByteCount;

		if (MappingsRemaining >= Clone.OffsetOut().Remaining) {

			// This clone is satisfied, fill the header 

			Clone.Header()->Duration = 
				PKS_VIDEOINFOHEADER(*m_VideoInfoHeader)->AvgTimePerFrame;
			Clone.Header()->PresentationTime.Numerator = 
				Clone.Header()->PresentationTime.Denominator = 1;
				PKS_VIDEOINFOHEADER(*m_VideoInfoHeader)->AvgTimePerFrame;

			// Timestamp it if we got the clocks assigned

			if (m_pClock) {

                Clone.Header()->PresentationTime.Time = m_pClock->GetTime();
                Clone.Header()->OptionsFlags = KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
											   KSSTREAM_HEADER_OPTIONSF_DURATIONVALID;
			}
			else {
				Clone.Header()->PresentationTime.Time = 0;
			}

			// Delete this one and goto the next clone

			// g_Trace.Trace(TraceInfo, "HwcapPin::DPC() deleting clone %p\n", PKSSTREAM_POINTER(Clone));

			KsStreamPointer ThisClone = Clone;
			Clone = Clone.GetNextClone();
			ThisClone.Release();

			MappingsRemaining -= MappingsCompletedInThisClone;

		} else {

			// This clone has some more room to go: advance it and exit
			// g_Trace.Trace(TraceInfo, "HwcapPin::DPC() advancing clone %p\n", PKSSTREAM_POINTER(Clone));

			Clone.AdvanceOut(MappingsRemaining);

			break;
		}


	};

	if (m_bPending) {
		m_bPending = FALSE;
		::KsPinAttemptProcessing(*this, TRUE);
	}
}

