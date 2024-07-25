// capstrdrv.cpp - Sample video capture stream minidriver
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#define VDW_MAIN	
#include <vdw.h>

#include "kstream.h"

#include "capstream.h"

KTrace T("TestStreamDriver");

///////////////////////////////////////////////////////////////////
// Begin INIT time code
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(VideoCaptureDriver, NULL)

////////////////////////////////////////////////////////////////
// VideoCaptureDriver::DriverEntry
// 
// This is the first routine in the driver that the system calls.
// For this driver, there is nothing to do except register with
// the stream class driver.  More complex drivers would initialize
// hardware etc.
//
NTSTATUS VideoCaptureDriver::DriverEntry(PUNICODE_STRING RegistryPath)
{
    NTSTATUS   ReturnValue = STATUS_SUCCESS;

    T << TraceInfo << "VideoCaptureDriver::DriverEntry -- Start\n";
     
	//
    // Register this driver 
    //
	ReturnValue = Register(
		(void*) RegistryPath,		// second context used...in registration process
		FALSE,						// BOOLEAN IsBusMaster,
		FALSE,						// BOOLEAN IsDma24Bit,
		FALSE,						// BOOLEAN SerializeRequests,
		FALSE,						// BOOLEAN HasHardwareInterrupt,
		3,							// ULONG   BufferAlignment,
		0,							// ULONG   DmaBufferSize,
		sizeof(VideoCaptureAdapter),// ULONG   MaxAdapter (Device Extension) ObjectSize,
		sizeof(SRB_EXTENSION),		// ULONG   PerRequestExtensionSize 
		0  );						// ULONG   PerAdapter (Filter) InstanceExtensionSize

	T << TraceInfo << TraceHex << "VideoCaptureDriver::DriverEntry -- End. Status is " << (ULONG)ReturnValue << "\n";

    return ReturnValue;
}

// End INIT time code
#pragma code_seg()

////////////////////////////////////////////////////////////////
// VideoCaptureDriver::OnCreateAdapter
// 
// The driver calls this when the class driver creates a new 
// adapter object SRB_INITIALIZE_DEVICE.  The adapter object memory
// is ALREADY allocated	so we construct the object in memory allocated
// by the stream class driver.  We then call the initialization method
// on the Adapter which will initialize internal adapter information
// (including the Adapter object's member streams).
//
NTSTATUS VideoCaptureDriver::OnCreateAdapter(
	PORT_CONFIGURATION_INFORMATION* pConfigInfo,
	PVOID AdapterStorage,
	PHW_STREAM_REQUEST_BLOCK pSrb	
	)
{
	NTSTATUS ReturnValue = STATUS_SUCCESS;

	T << TraceInfo << "VideoCaptureDriver::OnCreateAdapter -- Start\n";

	// -- since our test driver DOESN'T have hardware, this BETTER be 0! --
    if (pConfigInfo->NumberOfAccessRanges != 0)
    {
        T << TraceError << "VideoCaptureDriver::OnCreateAdapter -- Illegal port configuration info\n";
        return (pSrb->Status = STATUS_NO_SUCH_DEVICE);
    }

	// -- construct the adapter using the AdapterStorage memory allocated
	//		by the Class Driver --
	m_pAdapter = new (AdapterStorage) VideoCaptureAdapter(this, pConfigInfo);
	
	// -- initialize the adapter object and its member streams! --
	if (NULL != m_pAdapter)
		m_pAdapter->Initialize();
	else
	{
		T << TraceError << "VideoCaptureDriver::OnCreateAdapter -- Adapter constructor failed!\n";
	    ReturnValue = STATUS_IO_DEVICE_ERROR;	
	}

	T << TraceInfo << TraceHex << "VideoCaptureDriver::OnCreateAdapter -- End. Status is " << (ULONG)ReturnValue << "\n";

	return ReturnValue;
}

// ****************************** end of capstrdrv.cpp ***********************************



