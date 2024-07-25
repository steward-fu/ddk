// kstream.cpp - implementation of class KStream
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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#include <vdw.h>
#if _WDM_
#if !defined(_DW_INCLUDE_NTIFS_) && !defined(_DW_INCLUDE_NTDDK_)
#include <kstream.h>

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStream overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//	KStream::OnRead -- SRB_READ_DATA
//		Called by DispatchData to read data from hardware.
//
void KStream::OnRead(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnRead Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnWrite -- SRB_WRITE_DATA
//		Called by DispatchData to Write data to hardware.
//
void KStream::OnWrite(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnWrite Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnGetState -- SRB_GET_STREAM_STATE
//		Called by DispatchControl to retrieve current state of this
//		stream.
//
void KStream::OnGetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnGetState Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnSetStateRead -- SRB_SET_STREAM_STATE
//		Called by DispatchControl to set the current state of this
//		stream.
//
void KStream::OnSetState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnSetState Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnGetProperty -- SRB_GET_STREAM_PROPERTY
//		Called by DispatchControl to retrieve a property of this
//		stream.
//
void KStream::OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnGetProperty Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnSetProperty -- SRB_SET_STREAM_PROPERTY
//		Called by DispatchControl to set a property of this
//		stream.
//
void KStream::OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnSetProperty Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnOpenMasterClock -- SRB_OPEN_MASTER_CLOCK
//		Called by DispatchControl.
//
void KStream::OnOpenMasterClock(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnOpenMasterClock Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnCloseMasterClock -- SRB_CLOSE_MASTER_CLOCK
//		Called by DispatchControl.
//
void KStream::OnCloseMasterClock(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnCloseMasterClock Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);

	m_hMasterClockHandle = NULL;
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnSetRate -- SRB_SET_STREAM_RATE
//		Called by DispatchControl.
//
void KStream::OnSetRate(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnSetRate Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnProposeDataFormat -- SRB_PROPOSE_DATA_FORMAT
//		Called by DispatchControl. Proposes new data format but doesn't
//		change it.
//
void KStream::OnProposeDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnProposeDataFormat Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnProposeRate -- SRB_PROPOSE_STREAM_RATE
//		Called by DispatchControl.  Proposes new rate but doesn't
//		change it.
//
void KStream::OnProposeRate(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnProposeRate Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnSetDataFormat -- SRB_SET_DATA_FORMA
//		Called by DispatchControl.
//
void KStream::OnSetDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnSetDataFormat Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnGetDataFormat -- SRB_GET_DATA_FORMA
//		Called by DispatchControl.
//
void KStream::OnGetDataFormat(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnGetDataFormat Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnBeginFlush -- SRB_BEGIN_FLUSH
//		Called by DispatchControl.
//
void KStream::OnBeginFlush(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnBeginFlush Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnEndFlush -- SRB_END_FLUSH
//		Called by DispatchControl.
//
void KStream::OnEndFlush(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnEndFlush Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnUnknownCommand -- SRB_UNKNOWN_STREAM_COMMAND
//		Called by DispatchControl.
//
void KStream::OnUnknownCommand(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnUnknownCommand Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::DispatchData -- Called by ReceiveData which is called by
//		the class driver to read/write data.  This default method
//		doesn't do queueing... override this if you need to implement
//		request queueing.
//
void KStream::DispatchData(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::DispatchData -- Start\n";
	#endif

	switch (pSrb->Command)
	{
    case SRB_READ_DATA:              // read data from hardware
		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchData -- SRB_READ_DATA\n";
		#endif
		OnRead(pSrb);
		break;

    case SRB_WRITE_DATA:             // write data to the hardware
		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchData -- SRB_WRITE_DATA\n";
		#endif
		OnWrite(pSrb);
		break;

	default:
		#ifdef __STREAM_TRACEON__
		TKS << TraceWarning << "KStream::DispatchData -- Unknown Data SRB!\n";
		#endif
		ASSERT(FALSE);				// what is it?
	}

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::DispatchData -- End\n";
	#endif

}

///////////////////////////////////////////////////////////////////////
//	KStream::DispatchControl -- Called by ReceiveControl which is called by
//		the class driver to pass in control SRBs.  This default method
//		doesn't do queueing... override this if you need to implement
//		request queueing.
//
void KStream::DispatchControl(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::DispatchControl -- Start\n";
	#endif

	switch (pSrb->Command)
	{
    case SRB_SET_STREAM_STATE:       // set the state of the stream

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_SET_STREAM_STATE\n";
		#endif

		OnSetState(pSrb);
		break;

    case SRB_GET_STREAM_STATE:       // get the state of the stream

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_GET_STREAM_STATE\n";
		#endif

		OnGetState(pSrb);
		break;

    case SRB_SET_STREAM_PROPERTY:    // set a property of the stream

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_SET_STREAM_PROPERTY\n";
		#endif

		OnSetProperty(pSrb);
		break;

    case SRB_GET_STREAM_PROPERTY:    // get a property value for the stream

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_GET_STREAM_PROPERTY\n";
		#endif

		OnGetProperty(pSrb);
		break;

    case SRB_OPEN_MASTER_CLOCK:      // indicates that the master clock is on this stream

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_OPEN_MASTER_CLOCK\n";
		#endif

		OnOpenMasterClock(pSrb);
		break;

    case SRB_CLOSE_MASTER_CLOCK:     // indicates that the master clock is closed

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_CLOSE_MASTER_CLOCK\n";
		#endif

		OnCloseMasterClock(pSrb);
		break;

    case SRB_INDICATE_MASTER_CLOCK:  // supplies the handle to the master clock

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_INDICATE_MASTER_CLOCK\n";
		#endif

		OnIndicateMasterClock(pSrb);
		break;

    case SRB_SET_STREAM_RATE:        // set the rate at which the stream should run

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_SET_STREAM_RATE\n";
		#endif

		OnSetRate(pSrb);
		break;

    case SRB_PROPOSE_DATA_FORMAT:    // propose a new format, DOES NOT CHANGE IT!

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_PROPOSE_DATA_FORMAT\n";
		#endif

		OnProposeDataFormat(pSrb);
		break;

    case SRB_PROPOSE_STREAM_RATE:    // propose a new rate, DOES NOT CHANGE IT!

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_PROPOSE_STREAM_RATE\n";
		#endif

		OnProposeRate(pSrb);
		break;

    case SRB_SET_DATA_FORMAT:        // sets a new data format

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_SET_DATA_FORMAT\n";
		#endif

		OnSetDataFormat(pSrb);
		break;

    case SRB_GET_DATA_FORMAT:        // returns the current data format

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_GET_DATA_FORMAT\n";
		#endif

		OnGetDataFormat(pSrb);
		break;

    case SRB_BEGIN_FLUSH:            // beginning flush state

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_BEGIN_FLUSH\n";
		#endif

		OnBeginFlush(pSrb);
		break;

    case SRB_END_FLUSH:              // ending flush state

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStream::DispatchControl -- SRB_END_FLUSH\n";
		#endif

		OnEndFlush(pSrb);
		break;

    case SRB_UNKNOWN_STREAM_COMMAND: // IRP function is unknown to class driver
	default:

		#ifdef __STREAM_TRACEON__
		TKS << TraceWarning << "KStream::DispatchControl -- SRB_UNKNOWN_STREAM_COMMAND or default case!\n";
		#endif

		OnUnknownCommand(pSrb);
	}

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::DispatchControl -- End\n";
	#endif

}

///////////////////////////////////////////////////////////////////////
//	KStream::OnClock -- called by DispatchClock as called by class driver.
//
void KStream::OnClock(PHW_TIME_CONTEXT HwTimeContext)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnClock -- Default does nothing\n";
	#endif

	// -- default does nothing --
	return;
	UNREFERENCED_PARAMETER(HwTimeContext);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnEvent -- called by DispatchEvent as called by class driver
//
NTSTATUS KStream::OnEvent(PHW_EVENT_DESCRIPTOR pEventDesc)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnEvent -- Default does nothing\n";
	#endif

	return STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(pEventDesc);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnTimer -- called by DispatchTimer as called by class driver
//
void KStream::OnTimer(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnTimer -- Default does nothing\n";
	#endif

	// -- default does nothing --
	return;
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnOpen -- called by Adapter at open time.  Override if you
//		need non-default behavior --
//
NTSTATUS KStream::OnOpen(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnOpen -- Default does nothing\n";
	#endif

	return STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(pSrb);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnClose -- called by Adapter when a stream is closed.
//		Override if you	need non-default behavior --
//
NTSTATUS KStream::OnClose(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::OnClose -- Default sets state to stop\n";
	#endif

	m_eState = KSSTATE_STOP;
	m_hMasterClockHandle = NULL;
	return STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(pSrb);
}

///////////////////////////////////////////////////////////////////////
//	KStream::CancelRequest -- called by Adapter when an SRB needs to
//		be cancelled. Override if you	need non-default behavior --
//
void KStream::CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::CancelRequest -- Not Implemented\n";
	#endif

	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnTimeout --
//
void KStream::OnTimeout(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::OnTimeout -- Start\n";
	#endif

	if (pSrb->Flags & SRB_HW_FLAGS_DATA_TRANSFER)
		ReadyForNextDataRequest();
	else
		ReadyForNextControlRequest();

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::OnTimeout -- End\n";
	#endif
}

///////////////////////////////////////////////////////////////////////
//	KStream::GetInformation --
//
void KStream::GetInformation(PHW_STREAM_INFORMATION pInfo)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::GetInformation -- Returning stream info\n";
	#endif

	ASSERT (NULL != pInfo);
	*pInfo = m_sInfo;
}

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStream overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- KStream methods implemented in THIS base class.
	//		Do NOT override.
	//
	///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//	KStream::KStream -- constructor.  Called from the Adapter object
//		This constructor simply sets all values to defaults.  Real work
//		is performed in the Initialize method.
//
KStream::KStream()
	  : m_pClassObject(NULL),
		m_hMasterClockHandle(NULL),
		m_bPio(FALSE),
		m_bDma(FALSE),
		m_uMediaExpansionSize(0),
		m_uHeaderWorkspaceSize(0),
		m_eState(KSSTATE_STOP)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::Kstream -- Setting default values, clearing structs\n";
	#endif
	RtlZeroMemory(&m_sInfo,  sizeof(HW_STREAM_INFORMATION));
	RtlZeroMemory(&m_sClock, sizeof(HW_CLOCK_OBJECT));
}

///////////////////////////////////////////////////////////////////////
//	KStream::InitializeBaseClass -- Called from your Initialize method to
//		REALLY initialize the base object.  You provide the stream
//		information structure and various components that are usually
//		part of the  method to the Adapter object
//		This constructor simply sets all values to defaults.  Real work
//		is performed in the Initialize method.
//
void KStream::InitializeBaseClass(
	const HW_STREAM_INFORMATION&	info,
	const ULONG uClockSupportFlags,				   // -- object
	const BOOL bCanBeMasterClock,				   // -- can this object BE master
	const BOOLEAN bDma,							   // -- object
	const BOOLEAN bPio,							   // -- object
	const ULONG uMediaSpecificHeaderExpansionSize, // -- object
	const ULONG uHeaderWorkspace)				   // -- object
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::InitializeBaseClass -- Start\n";
	#endif

	// -- information associated with the stream object definition itself --
	m_sClock.ClockSupportFlags = uClockSupportFlags;
	if (bCanBeMasterClock)
		m_sClock.HwClockFunction = DispatchClock;
	else
		m_sClock.HwClockFunction = NULL;
	m_bDma = bDma;
	m_bPio = bPio;
	m_uMediaExpansionSize = uMediaSpecificHeaderExpansionSize;
	m_uHeaderWorkspaceSize = uHeaderWorkspace;

	// -- info "characteristics" of the stream object --
	m_sInfo.NumberOfPossibleInstances =		info.NumberOfPossibleInstances;
	m_sInfo.DataFlow =						info.DataFlow;
	m_sInfo.NumberOfFormatArrayEntries =	info.NumberOfFormatArrayEntries;
	m_sInfo.StreamFormatsArray =			info.StreamFormatsArray;
	m_sInfo.NumStreamPropArrayEntries =		info.NumStreamPropArrayEntries;
	m_sInfo.StreamPropertiesArray =			info.StreamPropertiesArray;
	m_sInfo.NumStreamEventArrayEntries =	info.NumStreamEventArrayEntries;
	m_sInfo.StreamEventsArray =				info.StreamEventsArray;
	m_sInfo.Category =						info.Category;
	m_sInfo.Name =							info.Name;
	m_sInfo.MediumsCount =					info.MediumsCount;
	m_sInfo.Mediums =						info.Mediums;
	m_sInfo.BridgeStream =					info.BridgeStream;

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::InitializeBaseClass -- End\n";
	#endif
}

KStream::~KStream(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::~KStream -- Resetting all values to defaults\n";
	#endif

	m_pClassObject = NULL;
	m_hMasterClockHandle = NULL;
	m_eState = KSSTATE_STOP;
	// -- information associated with the stream object definition itself --
	m_sClock.ClockSupportFlags = 0;
	m_sClock.HwClockFunction = NULL;
	m_bDma = FALSE;
	m_bPio = FALSE;
	m_uMediaExpansionSize = 0;
	m_uHeaderWorkspaceSize = 0;

	// -- info "characteristics" of the stream object --
	memset(&m_sInfo, 0, sizeof(HW_STREAM_INFORMATION));
}

///////////////////////////////////////////////////////////////////////
//	KStream::OnIndicateMasterClock -- SRB_INDICATE_MASTER_CLOCK
//		Called by DispatchControl.... not overrideable
//
void KStream::OnIndicateMasterClock(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::OnIndicateMasterClock -- Saving Master Clock Handle\n";
	#endif
	m_hMasterClockHandle = pSrb->CommandData.MasterClockHandle;
	NextSrb(pSrb);
}

///////////////////////////////////////////////////////////////////////
//	KStream::Initialize
//
//
void KStream::Initialize(KStreamAdapter* pDevice, PHW_STREAM_OBJECT pClassObject)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::Initialize -- Start\n";
	#endif

	ASSERT (pClassObject->SizeOfThisPacket >= sizeof(HW_STREAM_OBJECT));

	m_pClassObject = pClassObject;
	m_pClassObject->HwDeviceExtension = pDevice;

	m_pClassObject->ReceiveDataPacket = ReceiveData;
	m_pClassObject->ReceiveControlPacket = ReceiveControl;

	m_pClassObject->HwClockObject.HwClockFunction = m_sClock.HwClockFunction;
	m_pClassObject->HwClockObject.ClockSupportFlags = m_sClock.ClockSupportFlags;

	m_pClassObject->HwEventRoutine = DispatchEvent;

	m_pClassObject->Dma = m_bDma;
	m_pClassObject->Pio = m_bPio;

	// -- the following may need to be more dynamic --
	m_pClassObject->Allocator = FALSE;

	m_pClassObject->StreamHeaderMediaSpecific = m_uMediaExpansionSize;
	m_pClassObject->StreamHeaderWorkspace = m_uHeaderWorkspaceSize;

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::Initialize -- End\n";
	#endif

}

///////////////////////////////////////////////////////////////////////
//	KStream::ReceiveData -- static method called by Class driver object
//
void STREAMAPI KStream::ReceiveData(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::ReceiveData -- Start\n";
	#endif

	KStream* pStream = StreamFromSrb(pSrb, pStream);

	ASSERT (NULL != pStream);
	pStream->DispatchData(pSrb);

	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::ReceiveData -- End\n";
	#endif
}

///////////////////////////////////////////////////////////////////////
//	KStream::ReceiveControl -- static method called by Class driver object
//
void STREAMAPI KStream::ReceiveControl(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::ReceiveControl -- Start\n";
	#endif

	KStream* pStream =	StreamFromSrb(pSrb, pStream);

	ASSERT (NULL != pStream);
	pStream->DispatchControl(pSrb);

	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::ReceiveControl -- End\n";
	#endif
}

///////////////////////////////////////////////////////////////////////
//	KStream::DispatchTimer -- static method called by Class driver object
//
void STREAMAPI KStream::DispatchTimer(PVOID pContext)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::DispatchTimer -- Start\n";
	#endif

	ASSERT (NULL != pContext);
	((KStream*)pContext)->OnTimer();

	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::DispatchTimer -- End\n";
	#endif

}

///////////////////////////////////////////////////////////////////////
//	KStream::DispatchClock -- static method called by Class driver object
//
void STREAMAPI KStream::DispatchClock(PHW_TIME_CONTEXT pHwTimeContext)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::DispatchClock -- Start\n";
	#endif

	ASSERT(NULL != pHwTimeContext);
	ASSERT(NULL != pHwTimeContext->HwStreamObject->HwStreamExtension);

	(*(KStream**)pHwTimeContext->HwStreamObject->HwStreamExtension)->OnClock(pHwTimeContext);

	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::DispatchClock -- End\n";
	#endif

}

///////////////////////////////////////////////////////////////////////
//	KStream::DispatchEvent -- static method called by Class driver object
//
NTSTATUS STREAMAPI KStream::DispatchEvent(PHW_EVENT_DESCRIPTOR pEventDesc)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::DispatchEvent -- Start\n";
	#endif

	ASSERT(NULL != pEventDesc);
	ASSERT(NULL != pEventDesc->StreamObject->HwStreamExtension);
	return (*(KStream**)pEventDesc->StreamObject->HwStreamExtension)->OnEvent(pEventDesc);

	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStream::DispatchEvent -- End\n";
	#endif

}

///////////////////////////////////////////////////////////////////////
//	KStream::RequestComplete -- wrapper method
//
void KStream::RequestComplete(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::RequestComplete\n";
	#endif

	StreamClassStreamNotification(StreamRequestComplete, pSrb->StreamObject, pSrb);

}


///////////////////////////////////////////////////////////////////////
//	KStream::ReadyForNextDataRequest -- wrapper method
//
void KStream::ReadyForNextDataRequest(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::ReadyForNextDataRequest\n";
	#endif

	StreamClassStreamNotification(ReadyForNextStreamDataRequest, m_pClassObject);
}

///////////////////////////////////////////////////////////////////////
//	KStream::ReadyForNextControlRequest -- wrapper method
//
void KStream::ReadyForNextControlRequest(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::ReadyForNextControlRequest\n";
	#endif

	StreamClassStreamNotification(ReadyForNextStreamControlRequest, m_pClassObject);
}

///////////////////////////////////////////////////////////////////////
//	KStream::NextSrb -- completes this SRB and returns
//
void KStream::NextSrb(PHW_STREAM_REQUEST_BLOCK pSrb, NTSTATUS status)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::NextSrb\n";
	#endif

	KStreamAdapter* pAdapter = Adapter();

	if (pAdapter->Driver()->Serializes())
	{
		if (pSrb->Flags & SRB_HW_FLAGS_DATA_TRANSFER)
			ReadyForNextDataRequest();
		else
			ReadyForNextControlRequest();
	}

	pSrb->Status = status;
	RequestComplete(pSrb);
}

///////////////////////////////////////////////////////////////////////
//	KStream::NotifyStarved -- wrapper
//
void KStream::NotifyStarved(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::NotifyStarved\n";
	#endif

	StreamClassStreamNotification(HardwareStarved, m_pClassObject);
}

///////////////////////////////////////////////////////////////////////
//	KStream::ScheduleTimedCallback -- wrapper
//
void KStream::ScheduleTimedCallback(ULONG uMicroseconds)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::ScheduleTimedCallback\n";
	#endif

	StreamClassScheduleTimer(
		m_pClassObject,
		m_pClassObject->HwDeviceExtension,
		uMicroseconds,
		DispatchTimer,
		this
		);
}

///////////////////////////////////////////////////////////////////////
//	KStream::CallAtNewPriority -- wrapper
//
void KStream::CallAtNewPriority(
	STREAM_PRIORITY Priority,
	PHW_PRIORITY_ROUTINE pFunc,
	PVOID pContext
	)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::CallAtNewPriority\n";
	#endif

	ASSERT (NULL != m_pClassObject);
	ASSERT (NULL != pFunc);

	StreamClassCallAtNewPriority(
		m_pClassObject,
		Adapter(),
		Priority,
		pFunc,
		pContext
		);
}

///////////////////////////////////////////////////////////////////////
//	KStream::AbortOutstandingRequests -- wrapper
//
void KStream::AbortOutstandingRequests(NTSTATUS status)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::AbortOutstandingRequests\n";
	#endif

	StreamClassAbortOutstandingRequests(Adapter(), m_pClassObject, status);
}

///////////////////////////////////////////////////////////////////////
//	KStream::QueryMasterClock -- wrapper
//
void KStream::QueryMasterClock(TIME_FUNCTION tf, PHW_QUERY_CLOCK_ROUTINE pFunc)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::QueryMasterClock\n";
	#endif

	ASSERT (NULL != m_hMasterClockHandle);

	StreamClassQueryMasterClock(m_pClassObject, m_hMasterClockHandle, tf, pFunc);
}

///////////////////////////////////////////////////////////////////////
//	KStream::QueryMasterClock -- wrapper... static function
//
void KStream::QueryMasterClockSync(HANDLE hClock, HW_TIME_CONTEXT* pTimeCtx)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStream::QueryMasterClockSync\n";
	#endif

	StreamClassQueryMasterClockSync(hClock, pTimeCtx);
}


///////////////////////////////////////////////////////////////////////
//	KStream::Adapter -- returns pointer to our adapter.
//
KStreamAdapter* KStream::Adapter(void)
{
	return (KStreamAdapter*)m_pClassObject->HwDeviceExtension;
}

///////////////////////////////////////////////////////////////////////
//	KStream::Clock -- returns handle of our clock .
//
HW_CLOCK_OBJECT& KStream::Clock(void)
{
	return m_sClock;
}

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- KStreamAdapter methods implemented in THIS base class.
	//			Do Not override.
	//
	///////////////////////////////////////////////////////////////////////

/* ************************* end of Kstream.cpp *************************** */

#endif
#endif // WDM
