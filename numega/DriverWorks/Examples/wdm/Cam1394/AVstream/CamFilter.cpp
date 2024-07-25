// CamFilter.cpp: implementation of the CamFilter class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

extern KDebugOnlyTrace	t;

#include	"Camera.h"
#include	"CamLowerDevice.h"
#include	"CamDevice.h"
#include	"CamPin.h"

#include	"CamFilter.h"


///////////////////////////////////////////////////////////////////////
// CamFilter::OnCreate
//
// Parameters:
// 	I                    - IRP_MJ_CREATE
// Returns:
// 	NT Status
// Comments:
//	The first call to the filter object that the Framework makes after
//	AVStream has created the underlying KSFILTER object. The framework
//  takes care of storing the 'this' context in the KSFILTER's object
//  bag. Returning status other than SUCCESS or PENDING results in 
//  destruction of the object and failure status to KS. 
//
NTSTATUS CamFilter::OnCreate(KIrp I) 
{
	t << "CamFilter::OnCreate\n";

	// TODO: custom initialization

	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// CamFilter::OnClose
//
// Parameters:
// 	I                    - IRP_MJ_CLOSE
// Returns:
// 	STATUS_SUCCESS
// Comments:
// 	The last call the framework makes before destroying the filter.
//  After return from this method the framework removes the object
//  from the underlying KSFILTER's object bag, and provided there
//  are no more references to it, the 'this' object gets deleted.
//
NTSTATUS CamFilter::OnClose(KIrp I)  
{
	t << "CamFilter::OnClose\n";

	// TODO: custom deinitialization

	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// CamFilter::OnProcess
//
// Parameters:
// 	pIndex               - A pointer to an array of process pin index entries.
//						   Each describes the process state of a specific pin.
//						   The array is ordered according to pin IDs, which is
//						   the 0-based index corresponding to the pin descriptor
//						   appearence order in the filter descriptor (KsFilterDescriptor::Attach() order).
// Returns:
// 	STATUS_SUCCESS to continue processing. Return STATUS_PENDING to stop
//  processing until the next triggering event.
// Comments:
// 	This is a mandatory method for filter-centric minidriver.
//  Called when the filter is meant to process frames: one method
//  for all pins maintained by the filter.
//  NOTE that A processing attempt will not succeed unless frames 
//  are available on every pin that does not specify 
//  KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING (which we do specify).
//
NTSTATUS CamFilter::OnProcess(PKSPROCESSPIN_INDEXENTRY pIndex)
{
	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	// Get the state for our only pin:

	PKSPROCESSPIN processPin = pIndex->Pins[0];

	t.Trace(TraceInfo, "CamFilter::OnProcess, processPin = %p, Data = %p, BytesAvailable = %d\n", 
		processPin,processPin->Data,processPin->BytesAvailable);

	// Get the framework pin object
	CamPin* Pin = reinterpret_cast<CamPin*> ( KsPin::Object(processPin->Pin) );

    // If there is space available on the video frame, 
    // get current video frame data.
	if (processPin->BytesAvailable)
	{

		KMemory* pMem = new KMemory(processPin->Data, processPin->BytesAvailable);
		if (pMem && pMem->IsValid()) {

			KsStreamPointer		SP(processPin->StreamPointer);
			KsStreamPointer		Clone = SP.Clone(Pin);
			Clone->Context = pMem;

			pMem->SetPageArray();

			// Read the frame from the device
			status = Device()->ReadFrame(*pMem, this, processPin);
		}

		if (STATUS_PENDING != status) {

			t.Trace(TraceWarning,"ReadFrame failed with status = %X", status);

			// an error occurred, clean up and attempt to process again
			delete pMem;
			::KsFilterAttemptProcessing(*this,TRUE);
		}
	}
	else 
	{
		// @@TODO maintain count of frames
/*
		Pin->m_ulVideoDroppedFrames++;
		if (Pin->m_ulVideoDroppedFrames % 10 == 0) 
			T.Trace(TraceWarning, "Dropped %ld/%ld, processed %ld video frames\n", 
					 ULONG(Pin->m_ulVideoDroppedFrames), ULONG(Pin->m_Timer.Ticks()), ULONG(Pin->m_PictureNumber));
*/

		// no buffer to process, try again later...
		::KsFilterAttemptProcessing(*this,TRUE);
	}

	// normal case: status == STATUS_PENDING
    return status;
}

	
///////////////////////////////////////////////////////////////////////
// CamFilter::OnReadFrameComplete
//
// Parameters:
// 	Status               - status of reading the video frame from 1394 camera
//	ulLen				 - Number of bytes transferred
//	pCtx				 - Pointer to context structure passed to CamLowerDevice::ReadFrame
//								is actually a pointer to PKSPROCESSPIN structure 
//								- see OnProcess method
// Returns:
// 	None.
// Comments:
// 	The 1394 transfer has been completed to read a frame into the stream buffer.
//	Perform any post-processing.  
//
void CamFilter::OnReadFrameComplete(NTSTATUS Status, ULONG ulLen, PVOID pCtx)
{
	PKSPROCESSPIN processPin = reinterpret_cast<PKSPROCESSPIN>(pCtx);
	ASSERT(processPin);

	t.Trace(TraceInfo, "CamFilter::OnReadFrameComplete, processPin = %p, processPin->Data = %p, Status = %X\n", 
		processPin, processPin->Data, Status);

	// Get the framework pin object
	CamPin* Pin = reinterpret_cast<CamPin*> ( KsPin::Object(processPin->Pin) );
	ASSERT(Pin);

	// Get the cloned stream pointer
	KsStreamPointer Clone = Pin->GetFirstClone();
	ASSERT( Clone.IsValid() );

	// Check status of reading the frame
	if (STATUS_SUCCESS == Status) {

		// successfully read the frame into the buffer
		// complete processing and clean up

		processPin->BytesUsed = ulLen;

		// @@TODO Fill in the stream header; perform any accounting for the data transfer
/*
		pStreamHeader->PresentationTime.Time = m_Timer.TickTime(); // important: relative to the start of the stream!
		pStreamHeader->PresentationTime.Numerator = 1;
		pStreamHeader->PresentationTime.Denominator = 1;
		pStreamHeader->Duration = (*m_VideoInfoHeader)->AvgTimePerFrame;
		pStreamHeader->OptionsFlags |= KSSTREAM_HEADER_OPTIONSF_TIMEVALID | 
										   KSSTREAM_HEADER_OPTIONSF_DURATIONVALID;

*/
	}
	else {

		processPin->BytesUsed = 0;

		// there was an error reading the frame

		// @@TODO update count of dropped frames

	}

	processPin->Terminate = TRUE;

	KMemory* pMem = reinterpret_cast<KMemory*>(Clone->Context);
	delete pMem;

	Clone.Release();

	::KsFilterAttemptProcessing(*this, FALSE);
}
