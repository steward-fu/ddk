// VcapFilter.cpp: implementation of the VcapFilter class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Vcap.h"




///////////////////////////////////////////////////////////////////////
// VcapFilter::OnCreate
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
//  destruction of the object and, consequtively, failure status to KS. 
NTSTATUS VcapFilter::OnCreate(KIrp I) 
{
	g_Trace << "VcapFilter::OnCreate" << EOL;

	// TODO: custom initialization

	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// VcapFilter::OnClose
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
NTSTATUS VcapFilter::OnClose(KIrp I)  
{
	g_Trace << "VcapFilter::OnClose" << EOL;

	// TODO: custom deinitialization

	return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// VcapFilter::OnProcess
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
NTSTATUS VcapFilter::OnProcess(PKSPROCESSPIN_INDEXENTRY pIndex)
{
	// Get the state for our only pin:

	PKSPROCESSPIN processPin = pIndex->Pins[0];

	// g_Trace << "VcapFilter::OnProcess, PinId = " << processPin->Pin << EOL;

	// Get the framework pin object:

	VcapPin* Pin = reinterpret_cast<VcapPin*> ( KsPin::Object(processPin->Pin) );

    //
    // If there is space available on the video frame, 
    // generate current video frame data.
    //

	if (processPin->BytesAvailable)
	{
		g_Trace.Trace(TraceInfo, "VcapFilter::OnProcess, BytesAvailable = %d, Pic #%lu\n", 
			processPin->BytesAvailable, ULONG(Pin->m_PictureNumber));

		// Get the synthesized image

		processPin->BytesUsed = Pin->GetImage(
			processPin->StreamPointer->StreamHeader,
			processPin->Data, 
			processPin->BytesAvailable);


		processPin->Terminate = TRUE;
	}
	else 
	{
		Pin->m_ulVideoDroppedFrames++;
		if (Pin->m_ulVideoDroppedFrames % 10 == 0) 
			g_Trace.Trace(TraceWarning, "Dropped %ld/%ld, processed %ld video frames\n", 
					 ULONG(Pin->m_ulVideoDroppedFrames), ULONG(Pin->m_Timer.Ticks()), ULONG(Pin->m_PictureNumber));
	}

    //
    // Pend until the timer triggers further processing
    //


    return STATUS_PENDING;
}