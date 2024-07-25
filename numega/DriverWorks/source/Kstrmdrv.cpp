// kstrmdrv.cpp - KStreamMinidriver implementation
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
	//	BEGIN -- Overrideable methods
	//
	///////////////////////////////////////////////////////////////////////

#ifdef __STREAM_TRACEON__
	KTrace TKS("Stream MiniDriver Classes");
#endif

///////////////////////////////////////////////////////////////////////
//	KStreamMinidriver::OnDestroyAdapter -- Called when the class driver
//				instructs the minidriver to cease operation.
//
// Parameters
//	pDevice		Points to the adapter object to be destroyed
//  pSrb		Points to the stream request block for SRB_UNINITIALIZE_DEVICE
//
// Returns
//	Returns STATUS_SUCCESS
//
// Comments
//

NTSTATUS KStreamMinidriver::OnDestroyAdapter(
	KStreamAdapter* pDevice,
	PHW_STREAM_REQUEST_BLOCK pSrb
	)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::OnDestroyAdapter -- Start\n";
	#endif

	pDevice->_self_delete();	// virtual dtor from KDevice

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::OnDestroyAdapter -- End\n";
	#endif

	return STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(pSrb);
}

///////////////////////////////////////////////////////////////////////
//	KStreamMiniDriver::OnPagingOutDriver -- Called when the driver
//				needs to be paged out.  Override this method if you
//				have work to do here.
//

NTSTATUS KStreamMinidriver::OnPagingOutDriver(void)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::OnPagingOutDriver -- Default Implementation\n";
	#endif
	return STATUS_SUCCESS;
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

////////////////////////////////////////////////////////////////////////////////////
//	KStreamMinidriver::Register -- Called by the DriverEntry routine to register a
//									minidriver with the stream class driver.
//
// Parameters
//
//	Context2			Context2 passed in through driver entry and used in the
//						actual registration of the driver with the stream class
//						driver.
//
//	IsBusmaster			TRUE if the adapter controlled by this driver is a bus
//						master
//
//	IsDma24Bit			TRUE if the adapter controlled by this driver can decode
//						at most 24 address bits for DMA transfers
//
//	SerializeRequests	TRUE if the driver requires the class driver to
//						serialize requests coming into the driver. If TRUE,
//						the driver does not have to deal with reentrancy
//						issues, although throughput could be affected.
//
//	BufferAlignment		Specifies the alignment size for DMA buffers. The value
//						specifies in bytes the size of the smallest unit addressable
//						by DMA cycles. For example, for ULONG alignment, use 4.
//						Note: Hardware should be designed to accommodate USHORT
//						or (preferably) UCHAR alignment.
//
//	DmaBufferSize		Size in bytes of the DMA buffer to allocate for the
//						corresponding adapter. For each adapter, the class driver
//						provides a physically contiguous buffer of the requested
//						size.
//
//	MaxAdapterObjectSize
//						Set this parameter to sizeof(T), where T is the class
//						derived from KStreamAdapter to represent the hardware.
//
//	PerRequestExtensionSize
//						The class driver allows the minidriver to reserve scratch
//						space in each Stream Request Block (SRB). This parameter
//						specifies the size of the workspace that the driver requires.
//						The driver accesses the scratch space using pSrb->SRBExtension.
//
//	PerAdapaterInstanceExtensionSize
//						This parameter applies only to adapters that are capable
//						of implementing multiple instances of their streams.
//						Normally this zero. If non-zero, it specifies the size of
//						the workspace that the driver requires for each instance
//						of a stream. The driver accesses this buffer with
//						pSrb->HwInstanceExtension.
//
// Returns
//		Returns STATUS_SUCCESS if successful.
//
// Commnents
//	If SerializeRequests is TRUE and the adapter in question has an interrupt,
//	then interrupts are effectively disabled while the driver is executing.
//	In addition, if the driver uses KStream::CallAtNewPriority or
//	KStreamAdapter::CallAtNewPriority to execute code at a lower IRQL, then
//	any interrupt that occurs during the execution of that call is lost. To
//	prevent this, the driver should disable the adapter's interrupt before
//	requesting a call at lower IRQL.

NTSTATUS KStreamMinidriver::Register(
		void*	pContext2,
		BOOLEAN bIsBusMaster,
		BOOLEAN bIsDma24Bit,
		BOOLEAN bSerializeRequests,
		BOOLEAN bHasHardwareInterrupt,
		ULONG   uBufferAlignment,
		ULONG   uDmaBufferSize,
		ULONG   uMaxAdapterObjectSize,
		ULONG   uPerRequestExtensionSize,
		ULONG   uPerAdapterInstanceExtensionSize
		)

{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::Register -- Start\n";
	#endif

	HW_INITIALIZATION_DATA hwi;
    ULONG ReturnValue = 0;

	m_bSerializes = bSerializeRequests;

    //
    // -- Set the Adapter entry points for the driver --
    //
    RtlZeroMemory(&hwi, sizeof(HW_INITIALIZATION_DATA));

	hwi.HwInitializationDataSize = sizeof(HW_INITIALIZATION_DATA);

	// -- we ONLY register the hardware interrupt if we've got one.
	//		turns out that the Streaming class driver doesn't initialize
	//		your device correctly if you register one, but don't have one --
	if (bHasHardwareInterrupt)
		hwi.HwInterrupt = InterruptHandler;
	else
		hwi.HwInterrupt = NULL;

	// -- these methods... ya just gotta have ... --
	hwi.HwReceivePacket = ReceivePacket;
	hwi.HwCancelPacket = CancelRequest;
	hwi.HwRequestTimeoutHandler = TimeOutHandler;

	// -- size of our KStreamAdapter derived object --
	hwi.DeviceExtensionSize = uMaxAdapterObjectSize;

	// -- extension size for requests passed down --
	hwi.PerRequestExtensionSize = uPerRequestExtensionSize;

	// -- size of stream oriented info... for us it's a pointer
	//		because (by convention) we've already encapsulated
	//		the instance of	streams IN our KStreamAdapter derived class --
	hwi.PerStreamExtensionSize = sizeof(KStream*);

	// -- what EXTRA do we need for an adapter instance tracking --
	hwi.FilterInstanceExtensionSize = uPerAdapterInstanceExtensionSize;

	// -- obvious hardware stuff --
	hwi.BusMasterDMA = bIsBusMaster;
	hwi.Dma24BitAddresses = bIsDma24Bit;
	hwi.BufferAlignment = uBufferAlignment;
	hwi.TurnOffSynchronization = !bSerializeRequests;
	hwi.DmaBufferSize = uDmaBufferSize;

	// -- register the adapter with the stream class object --
	{
		void* t_pDriver = DriverObject();

		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStreamMiniDriver::Register -- Registering Adapter object.\n";
		#endif

		ReturnValue = StreamClassRegisterAdapter( t_pDriver, pContext2, &hwi );
	}

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::Register -- End.  Return Value = " << TraceHex << ReturnValue << "\n";
	#endif

	// -- capture the address of the Unload routine inserted by the stream class
	//    driver

	m_StreamClassUnloader = m_pDriverObject->DriverUnload;

	// -- insert special minidriver unloader

	m_pDriverObject->DriverUnload = SpecialUnload;

	return ReturnValue;
}

////////////////////////////////////////////////////////////////////////////////////
//	KStreamMinidriver::SpecialUnload - unload handling for minidriver
//
// Parameters
//
//	pDriver	- pointer to system driver object being unloaded
//
// Comments
//
//	The class driver usurps the unload routine, resulting in a memory leak  if we
//	don't reclaim it. This function replicates the action of KDriver::UnloadDispatch,
//	but also calls the unload routine that the stream class inserts.

VOID KStreamMinidriver::SpecialUnload( PDRIVER_OBJECT pDriver )
{
	KStreamMinidriver* pMd = (KStreamMinidriver*)m_TheDriver;

	PDRIVER_UNLOAD ClassUnload = pMd->m_StreamClassUnloader;

#ifdef DRIVER_FUNCTION_UNLOAD
	m_TheDriver->Unload();
#endif

	pMd->_self_delete();

	BOUNDS_CHECKER(DRIVER_DESTROYED, (m_TheDriver));

	m_TheDriver = NULL;

// call termination handlers
	while (AtExitCall::m_ListHead)
// invoking the destructor causes the termination handler to be called
		delete AtExitCall::m_ListHead;

	if (ClassUnload != NULL)
		ClassUnload(pDriver);
}

////////////////////////////////////////////////////////////////////////////////////
//	KStreamMinidriver::ReceivePacket -- SRB Dispatcher for Stream Miniport Driver
//
//
// Parameters
//
//	pSrb		Request to be dispatched.
//
// Comments
//
//	This routine only handles adapter/driver SRBs. SRBs for streams are handled
//	in class KStream.  You shouldn't need to override this method, but instead
//	should override methods in the adapter/driver classes.
//
void STREAMAPI KStreamMinidriver::ReceivePacket(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::ReceivePacket -- Start\n";
	#endif

	NTSTATUS status = STATUS_SUCCESS;

	KStreamMinidriver* pDriver = (KStreamMinidriver*)KDriver::DriverInstance();
	KStreamAdapter* pDevice = AdapterFromSrb(pSrb, pDevice);

	// -- the adapter object CANNOT be NULL in this instance... the OS would
	//		be broken... soo if it is we report a device error  --
	if (NULL == pDevice)
	{
		#ifdef __STREAM_TRACEON__
		TKS << TraceFatal << "KStreamMiniDriver::ReceivePacket -- Adapter Object in SRB is NULL\n";
		#endif

		ASSERT (NULL != pDevice); // -- gives us a debug assert

		pSrb->Status = STATUS_IO_DEVICE_ERROR;
		StreamClassDeviceNotification( DeviceRequestComplete, NULL,	pSrb );
		return;
	}

	///////////////////////////////////////////////////////////////////
	//	Handles Device Specific SRBs
	///////////////////////////////////////////////////////////////////
 	switch (pSrb->Command)
	{
	case SRB_INITIALIZE_DEVICE:		// initialize the device
		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStreamMiniDriver::ReceivePacket -- SRB_INITIALIZE_DEVICE\n";
		#endif
		status = pDriver->OnCreateAdapter(
							pSrb->CommandData.ConfigInfo,
							pSrb->CommandData.ConfigInfo->HwDeviceExtension,
							pSrb );
		if (STATUS_PENDING != status)
			pDevice->NextSrb(pSrb, status);
		break;

    case SRB_UNINITIALIZE_DEVICE:    // uninitialize the device
		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStreamMiniDriver::ReceivePacket -- SRB_UNINITIALIZE_DEVICE\n";
		#endif
		status = pDriver->OnDestroyAdapter(pDevice, pSrb);
		if (STATUS_PENDING != status)
		{
			pSrb->Status = status;
			StreamClassDeviceNotification( DeviceRequestComplete, pDevice,	pSrb);
		}
		break;

	case SRB_PAGING_OUT_DRIVER:      // indicates that the driver is to be paged out
		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStreamMiniDriver::ReceivePacket -- SRB_PAGING_OUT_DRIVER\n";
		#endif
		status = pDriver->OnPagingOutDriver();
		pDevice->NextSrb(pSrb, status);
		break;

		// -- all other SRBs are passed to the adapter object
		//		for processing--
	default:
		#ifdef __STREAM_TRACEON__
		TKS << TraceInfo << "KStreamMiniDriver::ReceivePacket -- passing SRB to adapter\n";
		#endif
		pDevice->ReceivePacket(pSrb);
		break;
	}

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::ReceivePacket -- End\n";
	#endif

}

////////////////////////////////////////////////////////////////////////////////////
//	KStreamMinidriver::TimeOutHandler -- calls adapter object to handle timeout
//				requests.
//
//	Parameters
//
//	pSrb		Request to be dispatched.
//
void STREAMAPI KStreamMinidriver::TimeOutHandler(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::TimeOutHandler -- Start\n";
	#endif

	KStreamAdapter* pDevice = AdapterFromSrb(pSrb, pDevice);

	// -- the adapter object CANNOT be NULL in this instance... the OS would
	//		be broken... soo if it is we report a device error  --
	if (NULL == pDevice)
	{
		#ifdef __STREAM_TRACEON__
		TKS << TraceFatal << "KStreamMiniDriver::TimeOutHandler -- Adapter Object in SRB is NULL\n";
		#endif

		ASSERT (NULL != pDevice); // -- gives us a debug assert

		pSrb->Status = STATUS_IO_DEVICE_ERROR;
		StreamClassDeviceNotification( DeviceRequestComplete, NULL,	pSrb );
		return;
	}

	pDevice->OnTimeout(pSrb);

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::TimeOutHandler -- End\n";
	#endif

}

////////////////////////////////////////////////////////////////////////////////////
//	KStreamMinidriver::CancelRequest -- calls adapter object to handle cancel
//				requests.
//
//	Parameters
//
//	pSrb		Request to be dispatched.
//
void STREAMAPI KStreamMinidriver::CancelRequest(PHW_STREAM_REQUEST_BLOCK pSrb)
{
	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::CancelRequest -- Start\n";
	#endif

	KStreamAdapter* pDevice = AdapterFromSrb(pSrb, pDevice);

	// -- the adapter object CANNOT be NULL in this instance... the OS would
	//		be broken... soo if it is we report a device error  --
	if (NULL == pDevice)
	{
		#ifdef __STREAM_TRACEON__
		TKS << TraceFatal << "KStreamMiniDriver::CancelRequest -- Adapter Object in SRB is NULL\n";
		#endif

		ASSERT (NULL != pDevice); // -- gives us a debug assert

		pSrb->Status = STATUS_IO_DEVICE_ERROR;
		StreamClassDeviceNotification( DeviceRequestComplete, NULL,	pSrb );
		return;
	}

	pDevice->CancelRequest(pSrb);

	#ifdef __STREAM_TRACEON__
	TKS << TraceInfo << "KStreamMiniDriver::CancelRequest -- End\n";
	#endif

}

////////////////////////////////////////////////////////////////////////////////////
// KStreamMinidriver::InterruptHandler -- calls appropriate adapter object to
//				dispatch an interrupt.
//
//
// If the driver has an interrupt handler, the system calls this routine
// when the interrupt occurs. It dispatches the interrupt to the ISR of
// the indicated adapter.... note we've left tracing out of this to minmize
// interrupt latency.

BOOLEAN STREAMAPI KStreamMinidriver::InterruptHandler(PVOID pDeviceExt)
{
	ASSERT(NULL != pDeviceExt);
	return ((KStreamAdapter*)pDeviceExt)->OnInterrupt();
}

	///////////////////////////////////////////////////////////////////////
	//
	//	END -- Methods implemented in THIS base class.  Do Not override.
	//
	///////////////////////////////////////////////////////////////////////

/* *********************** end of Kstradpt.cpp *************************** */

#endif
#endif // WDM
