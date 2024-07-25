// kstradpt.cpp - member functions for class KStreamAdapter
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

/////////////////////////////////////////////////////////////////////////////
// KStreamAdapter

#include <vdw.h>
#if _WDM_
#if !defined(_DW_INCLUDE_NTIFS_) && !defined(_DW_INCLUDE_NTDDK_)
#include <kstream.h>


	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- Overrideable methods
	//
	///////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnOpenInstance -- SRB_OPEN_DEVICE_INSTANCE
//
void KStreamAdapter::OnOpenInstance(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnOpenInstance -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnCloseInstance -- SRB_CLOSE_DEVICE_INSTANCE
//
void KStreamAdapter::OnCloseInstance(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnCloseInstance -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnGetProperty -- SRB_GET_DEVICE_PROPERTY
//
void KStreamAdapter::OnGetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnGetProperty -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnSetProperty -- SRB_SET_DEVICE_PROPERTY
//
void KStreamAdapter::OnSetProperty(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnSetProperty -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnChangePowerState -- SRB_CHANGE_POWER_STATE
//
void KStreamAdapter::OnChangePowerState(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnChangePowerState -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnUnknownCommand -- SRB_UNKNOWN_DEVICE_COMMAND
//
void KStreamAdapter::OnUnknownCommand(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnUnknownCommand -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnGetDataIntersection -- SRB_GET_DATA_INTERSECTION
//
void KStreamAdapter::OnGetDataIntersection(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnGetDataIntersection -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnInitializationComplete -- SRB_INITIALIZATION_COMPLETE
//
void KStreamAdapter::OnInitializationComplete(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnInitializationComplete -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnSurpriseRemoval -- SRB_SURPRISE_REMOVAL
//
void KStreamAdapter::OnSurpriseRemoval(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnSurpriseRemoval -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	NextSrb(pSrb, STATUS_NOT_IMPLEMENTED);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnTimeout -- called by device PHW_REQUEST_TIMEOUT_HANDLER
//
//	This routing is called when an SRB has timed out.  The Tmeout
void KStreamAdapter::OnTimeout(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnTimeout -- Start\n";
	#endif

	if (pSrb->Flags & SRB_HW_FLAGS_STREAM_REQUEST)
	{
		// -- need to request stream timeout... get the stream --
		KStream* pStream = StreamFromSrb(pSrb, pStream);

		if (NULL != pStream)
		{
			#ifdef __STREAM_TRACEON__
			TKS << TraceAlways << "KStreamAdapter::OnTimeout -- calling KStream::OnTimeout\n";
			#endif
			pStream->OnTimeout(pSrb);
		}
		else
		{
			// -- look, this would be BAD news...a NULL stream pointer and would cause us to
			//		fail miserably, so we just blow away without trying to call our stream
			//		object --
			#ifdef __STREAM_TRACEON__
			TKS << TraceFatal << "KStreamAdapter::OnTimeout -- stream pointer in SRB was NULL!\n";
			#endif

			ASSERT (pStream != NULL); // -- a place to go when debugging --

			StreamClassStreamNotification( StreamRequestComplete, NULL );
		}
	}
	else
	{
		// --- call OnAdapterTimeout, which may be overridden --
		#ifdef __STREAM_TRACEON__
		TKS << TraceAlways << "KStreamAdapter::OnTimeout -- calling KStreamAdapter::OnAdapterTimeout\n";
		#endif
		OnAdapterTimeout(pSrb);
	}
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnTimeout -- End\n";
	#endif
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnAdapterTimeout -- called by KStreamAdapter::OnTimeout
//
//  called from OnTimeout method when the timeout is a Device (i.e. Adapter)
// 	timeout.  Override this to get non-default behavior (which just lets
// 	the timeout happen --
void KStreamAdapter::OnAdapterTimeout(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::OnAdapterTimeout -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	ReadyForNextRequest();
	UNREFERENCED_PARAMETER(pSrb);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::CancelRequest -- called by device PHW_CANCEL_SRB
//
void KStreamAdapter::CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::CancelRequest -- Default STATUS_NOT_IMPLEMENTED\n";
	#endif
	return;
	UNREFERENCED_PARAMETER(pSrb);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnInterrupt -- called by device PHW_INTERRUPT
//
BOOLEAN KStreamAdapter::OnInterrupt(void)
{
	return FALSE;
}

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- Overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//
	//	BEGIN -- Methods implemented in THIS base class.  Do Not override.
	//
	///////////////////////////////////////////////////////////////////////

#if defined(_IA64_) || defined(_AMD64_)
void* __cdecl KStreamAdapter::operator new(size_t nSize, PVOID ProvidedStorage)
#else
void* __cdecl KStreamAdapter::operator new(unsigned int nSize, PVOID ProvidedStorage)
#endif
{
	return ProvidedStorage;
	UNREFERENCED_PARAMETER(nSize);
}

void  __cdecl KStreamAdapter::operator delete(void* p)
{
	// do nothing
	UNREFERENCED_PARAMETER(p);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::KStreamAdapter -- constructs new Adapter given
//		information supplied here.  Called by the KStreamMiniDriver
//		class OnCreateAdapter method.
//
KStreamAdapter::KStreamAdapter(
	KStreamMinidriver* pDriver,
	PORT_CONFIGURATION_INFORMATION* pConfigInfo,
	ULONG uNumberOfStreams,
	ULONG uNumDevPropArrayEntries,
	PKSPROPERTY_SET pDevicePropertiesArray,
	ULONG uNumDevEventArrayEntries,
	PKSEVENT_SET pDeviceEventsArray,
	PKSTOPOLOGY pTopology
	)
{

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::KStreamAdapter -- Start\n";
	#endif

	pConfigInfo->StreamDescriptorSize = sizeof (HW_STREAM_HEADER) +
		(uNumberOfStreams * sizeof (HW_STREAM_INFORMATION));

	m_pDriver = pDriver;
	m_sConfigInfo = *pConfigInfo;
	m_uNumberOfStreams = uNumberOfStreams;
	m_uNumDevPropArrayEntries = uNumDevPropArrayEntries;
	m_pDevicePropertiesArray = pDevicePropertiesArray;
	m_uNumDevEventArrayEntries = uNumDevEventArrayEntries;
	m_pDeviceEventsArray = pDeviceEventsArray;
	m_pTopology = pTopology;

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::KStreamAdapter -- End\n";
	#endif

}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::~KStreamAdapter
//
KStreamAdapter::~KStreamAdapter(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::~KStreamAdapter -- Start\n";
	#endif

	m_pDriver = NULL;
	m_uNumberOfStreams = 0;
	m_uNumDevPropArrayEntries = 0;
	m_pDevicePropertiesArray = NULL;
	m_uNumDevEventArrayEntries = 0;
	m_pDeviceEventsArray = NULL;
	m_pTopology = NULL;

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::~KStreamAdapter -- End\n";
	#endif

}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::ReceivePacket -- command dispatch method that calls the
//		appropriate method based upon the pSrb --
//
void KStreamAdapter::ReceivePacket(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- Start\n";
	#endif

	// Make sure that the adapter has not been destroyed

	if ( Driver() == NULL )
	{
		pSrb->Status = STATUS_IO_DEVICE_ERROR;
		RequestComplete(pSrb);
		return;
	}

	///////////////////////////////////////////////////////////////////
	//	Handles Adapter Specific SRBs
	///////////////////////////////////////////////////////////////////

	switch (pSrb->Command)
	{
	case SRB_GET_STREAM_INFO:
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_GET_STREAM_INFO\n";
			#endif
			OnGetStreamInfo(pSrb);
			break;
	case SRB_OPEN_STREAM:            // open the specified stream
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_OPEN_STREAM\n";
			#endif
			OnOpenStream(pSrb);
			break;
    case SRB_CLOSE_STREAM:           // close the specified stream
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_CLOSE_STREAM\n";
			#endif
			OnCloseStream(pSrb);
			break;
    case SRB_OPEN_DEVICE_INSTANCE:   // open an instance of the device
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_OPEN_DEVICE_INSTANCE\n";
			#endif
			OnOpenInstance(pSrb);
			break;
    case SRB_CLOSE_DEVICE_INSTANCE:  // close an instance of the device
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_CLOSE_DEVICE_INSTANCE\n";
			#endif
			OnCloseInstance(pSrb);
			break;
    case SRB_GET_DEVICE_PROPERTY:    // get a property of the device
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_GET_DEVICE_PROPERTY\n";
			#endif
			OnGetProperty(pSrb);
			break;
    case SRB_SET_DEVICE_PROPERTY:    // set a property for the device
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_SET_DEVICE_PROPERTY\n";
			#endif
			OnSetProperty(pSrb);
			break;
    case SRB_CHANGE_POWER_STATE:     // change power state
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_CHANGE_POWER_STATE\n";
			#endif
			OnChangePowerState(pSrb);
			break;
    case SRB_UNKNOWN_DEVICE_COMMAND: // IRP function is unknown to class driver
			#ifdef __STREAM_TRACEON__
			TKS << TraceAlways << "KStreamAdapter::ReceivePacket -- SRB_UNKNOWN_DEVICE_COMMAND\n";
			#endif
			OnUnknownCommand(pSrb);
			break;
    case SRB_GET_DATA_INTERSECTION:  // returns stream data intersection
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_GET_DATA_INTERSECTION\n";
			#endif
			OnGetDataIntersection(pSrb);
			break;
    case SRB_INITIALIZATION_COMPLETE:// indicates init sequence has completed
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_INITIALIZATION_COMPLETE\n";
			#endif
			OnInitializationComplete(pSrb);
			break;

#if !defined(VXD_COMPATLIB)
	case SRB_SURPRISE_REMOVAL:
			#ifdef __STREAM_TRACEON__
			TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- SRB_SURPRISE_REMOVAL\n";
			#endif
			OnSurpriseRemoval(pSrb);
			break;
#endif

	default:
			#ifdef __STREAM_TRACEON__
			TKS << TraceAlways << "KStreamAdapter::ReceivePacket -- unknown SRB type received\n";
			#endif
			OnUnknownCommand(pSrb);
	}
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::ReceivePacket -- End\n";
	#endif

}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnGetStreamInfo -- SRB_GET_STREAM_INFO
//
void KStreamAdapter::OnGetStreamInfo(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnGetStreamInfo -- Start\n";
	#endif
    //
    // pick up the pointer to header which preceeds the stream info structs
    //

    PHW_STREAM_HEADER pHead =
            (PHW_STREAM_HEADER)&(pSrb->CommandData.StreamBuffer->StreamHeader);

    //
    // pick up the pointer to the array of stream information data structures
    //

    PHW_STREAM_INFORMATION pStreamInfo =
            (PHW_STREAM_INFORMATION)&(pSrb->CommandData.StreamBuffer->StreamInfo);

    //
    // verify that the buffer is large enough to hold our return data
    //

    DEBUG_ASSERT (pSrb->NumberOfBytesToTransfer >=
            sizeof (HW_STREAM_HEADER) +
            sizeof (HW_STREAM_INFORMATION) * m_uNumberOfStreams);

    //
    // Set the header
    //
	pHead->NumberOfStreams = m_uNumberOfStreams;
	pHead->NumDevPropArrayEntries = m_uNumDevPropArrayEntries;
	pHead->DevicePropertiesArray = m_pDevicePropertiesArray;
	pHead->NumDevEventArrayEntries = m_uNumDevEventArrayEntries;
	pHead->DeviceEventsArray = m_pDeviceEventsArray;
	pHead->Topology = m_pTopology;
	pHead->SizeOfHwStreamInformation = sizeof (HW_STREAM_INFORMATION); // future proofing


    //
    // stuff the contents of each HW_STREAM_INFORMATION struct
    //
    for (ULONG j = 0; j < m_uNumberOfStreams; j++)
	{
        GetStream(j)->GetInformation(pStreamInfo);
		pStreamInfo++;
    }

	NextSrb(pSrb);

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnGetStreamInfo -- End\n";
	#endif

}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnOpenStream -- SRB_OPEN_STREAM
//
void KStreamAdapter::OnOpenStream(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnOpenStream -- Start\n";
	#endif

	ULONG i = pSrb->StreamObject->StreamNumber;

	if (i >= m_uNumberOfStreams)
	{
		#ifdef __STREAM_TRACEON__
		TKS << TraceError << "KStreamAdapter::OnOpenStream -- Stream Number " << i << " > number of streams\n";
		#endif

		NextSrb(pSrb, STATUS_INVALID_PARAMETER);
		return;
	}

	KStream* pStream = GetStream(i);

	ASSERT (pStream != NULL);

	// set pointer to class instance in stream extension

	*(KStream**)(pSrb->StreamObject->HwStreamExtension) = pStream;

	// initialize the stream data

	pStream->Initialize(this, pSrb->StreamObject);

	// call the stream's open function
	NextSrb(pSrb, pStream->OnOpen(pSrb));

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnOpenStream -- End\n";
	#endif

}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::OnCloseStream -- SRB_CLOSE_STREAM
//
void KStreamAdapter::OnCloseStream(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnCloseStream -- Start\n";
	#endif

	KStream* pStream = StreamFromSrb(pSrb, pStream);

	if (NULL != pStream)
		NextSrb(pSrb, pStream->OnClose(pSrb));
	else
	{
		// -- look, this can ONLY happen if the class driver is REALLY hosed.
		//		but we should catch it anyway --
		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStreamAdapter::OnCloseStream -- No Stream in SRB\n";
		#endif

		ASSERT (pStream != NULL);

		NextSrb(pSrb, STATUS_INVALID_PARAMETER);
	}

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::OnCloseStream -- End\n";
	#endif
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::RequestComplete -- Calls class driver's request
//		complete entry for specific SRB on this adapter.
//
void KStreamAdapter::RequestComplete(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::RequestComplete\n";
	#endif

	StreamClassDeviceNotification( DeviceRequestComplete, this,	pSrb);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::ReadyForNextRequest -- Calls class driver's
//		ready for next device request for this adapter.
//
void KStreamAdapter::ReadyForNextRequest(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::ReadyForNextRequest\n";
	#endif

	StreamClassDeviceNotification( ReadyForNextDeviceRequest, this );
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::NextSrb -- Calls class driver's
//		ready for next device request for this adapter if we are serialized,
//		and completes the request appropriately.
//
void KStreamAdapter::NextSrb(PHW_STREAM_REQUEST_BLOCK pSrb, NTSTATUS status)
{
	ASSERT (NULL != Driver());

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::NextSrb\n";
	#endif

	if (Driver()->Serializes())
		ReadyForNextRequest();

	pSrb->Status = status;
	RequestComplete(pSrb);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::CallAtNewPriority -- Calls class driver's
//		call at new priority on behalf of this adapter object
//
void KStreamAdapter::CallAtNewPriority(
	STREAM_PRIORITY Priority,
	PHW_PRIORITY_ROUTINE func,
	PVOID Context
	)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::CallAtNewPriority\n";
	#endif

	StreamClassCallAtNewPriority(
		NULL,
		this,
		Priority,
		func,
		Context);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::AbortOutstandingRequests -- Calls class driver's
//		AbortOutstandingRequests on behalf of this adapter object
//
void KStreamAdapter::AbortOutstandingRequests(NTSTATUS status)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceAlways << "KStreamAdapter::AbortOutstandingRequests\n";
	#endif

	StreamClassAbortOutstandingRequests(this, NULL, status);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::GetDmaBuffer --
//
PVOID KStreamAdapter::GetDmaBuffer(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::GetDmaBuffer\n";
	#endif

	return StreamClassGetDmaBuffer(this);
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::GetPhysicalAddress --
//
STREAM_PHYSICAL_ADDRESS KStreamAdapter::GetPhysicalAddress(
    PVOID VirtualAddress,
    STREAM_BUFFER_TYPE Type,
    ULONG *Length,
	PHW_STREAM_REQUEST_BLOCK HwSRB
	)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamAdapter::GetPhysicalAddress\n";
	#endif

	return StreamClassGetPhysicalAddress (
    	this,
    	HwSRB,
    	VirtualAddress,
    	Type,
    	Length );
}

/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::Driver --  returns pointer to driver for this adapter
//
KStreamMinidriver* KStreamAdapter::Driver(void)
{
	return m_pDriver;
}


/////////////////////////////////////////////////////////////////////////////
//	KStreamAdapter::PDO --  returns pointer to physical device object for
//		this adapter
//
PDEVICE_OBJECT KStreamAdapter::PDO(void)
{
	return m_sConfigInfo.PhysicalDeviceObject;
}


	///////////////////////////////////////////////////////////////////////
	//
	//	END -- Methods implemented in THIS base class.  Do Not override.
	//
	///////////////////////////////////////////////////////////////////////

/* ************************* end of Kstradpt.cpp *************************** */

#endif
#endif // WDM
