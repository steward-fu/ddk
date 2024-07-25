// HwcapSimulator.cpp: implementation of the HwcapSimulator class.
//
//////////////////////////////////////////////////////////////////////

#include	<vdw.h>
#include	<kavstream.h>

#include "Hwcap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HwcapSimulator::HwcapSimulator() :
 KTimedCallback(), m_Ticks(0), m_Sink(0), m_bOn(FALSE),
	 m_pImage(0), m_FrameBuffer(NULL), m_FrameBufferSize(0), m_Interval(0)
{
	m_StopEvent.Initialize(SynchronizationEvent);
}

HwcapSimulator::~HwcapSimulator()
{
	Stop();
	delete m_pImage;
	delete m_FrameBuffer;
}


///////////////////////////////////////////////////////////////////////
// HwcapSimulator::Configure
//
// Parameters:
// 	pHeader              - Image specification
// 	Sink                 - Interface to call DPC() on "interrupt"
// Returns:
// 	none
// Comments:
// 	Instaniate an image generator (RGB or YUV) based on video parameters
NTSTATUS HwcapSimulator::Configure (PKS_VIDEOINFOHEADER pHeader, IInterrupt* Sink)
{
	ASSERT(pHeader && Sink);

	// Instantiate image generator

	if (m_pImage) delete m_pImage; m_pImage=NULL;

    if (pHeader->bmiHeader.biBitCount == 24 &&
        pHeader->bmiHeader.biCompression == KS_BI_RGB) 

		m_pImage = new HwcapImageRgb24(pHeader);

	else if (pHeader->bmiHeader.biBitCount == 16 &&
            pHeader->bmiHeader.biCompression == FOURCC_YUV422)

		m_pImage = new HwcapImageYUV(pHeader);

	else
		return STATUS_INVALID_PARAMETER;

	// Create scratch buffer

	if (m_FrameBuffer) delete m_FrameBuffer; 
	
	m_FrameBuffer = new UCHAR [m_FrameBufferSize = pHeader->bmiHeader.biSizeImage];

	if (m_FrameBuffer == NULL || m_pImage == NULL) {
		delete m_FrameBuffer; delete m_pImage;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
		
	m_Sink = Sink;
	m_Interval = pHeader->AvgTimePerFrame;

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// HwcapSimulator::Start
//
// Parameters:
// Returns:
// 	none
// Comments:
// 	Starts the timer. 
//
void HwcapSimulator::Start()
{
	m_Ticks = 0;

	KeQuerySystemTime(&m_StartTime);

	REFERENCE_TIME NextTime = m_StartTime.QuadPart + m_Interval;

	InterlockedExchange(&m_bOn, TRUE);
	m_StopEvent.Reset();

	KTimedCallback::Set(NextTime, LinkTo(TimerCallback), this);

}


///////////////////////////////////////////////////////////////////////
// HwcapSimulator::Stop
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Stops timer
void HwcapSimulator::Stop()
{
	if (InterlockedExchange(&m_bOn, FALSE))
		if ( KTimedCallback::Cancel() )
			m_StopEvent.Set();			
}



///////////////////////////////////////////////////////////////////////
// HwcapSimulator::TimerCallback
//
// Parameters:
//	none
// Returns:
// 	none
// Comments:
// 	Reschedule callback and trigger KS processing
//
void HwcapSimulator::TimerCallback(PVOID, PVOID)
{
//	g_Trace.Trace(TraceInfo, "HwcapSimulator::TimerCallback %u\n", ULONG(m_Ticks));

	if (!m_bOn) {
		m_StopEvent.Set();			
		return;
	}
			  
	REFERENCE_TIME NextTime = m_StartTime.QuadPart + (++m_Ticks*m_Interval);

	KTimedCallback::Set(NextTime, LinkTo(TimerCallback), this);

	if (m_Sink)
		m_Sink->DPC();
}



///////////////////////////////////////////////////////////////////////
// HwcapSimulator::SetupDma
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
ULONG HwcapSimulator::SetupDma(
		PUCHAR* VirtualAddress, 
		KSSTREAM_POINTER_OFFSET& Mapping)
{
	return m_DmaEngine.AddMappings(VirtualAddress, Mapping);
}


///////////////////////////////////////////////////////////////////////
// CompleteDma
//
// Parameters:
//	none
// Returns:
// 	Number of mappings completed
// Comments:
// 	Called to simulate "the end of dma" interrupt.
//  We generate the image into the scratch buffer and cause
//  some pending DMA mapping entries to be filled in.
//  If the number of pending mapping is not sufficient to
//  accomodate the image we simply ignore the rest of
//  the image. This might cause tearing. A double buffering approach
//  couldv'e fixed this.
//
ULONG HwcapSimulator::CompleteDma()
{
	ASSERT(m_pImage && m_FrameBuffer);

	ULONG ActualSize = m_pImage->GetImage(m_FrameBuffer, m_FrameBufferSize);

	return m_DmaEngine.CompleteMappings(m_FrameBuffer, ActualSize);
}
