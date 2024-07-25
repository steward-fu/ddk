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

// OVRLAPIO.CPP - main module for VxD OVRLAPIO
// Copyright (c) 1996, Compuware Corporation

// Example of overlapped I/O

#define DEVICE_MAIN
#include "ovrlapio.h"
Declare_Virtual_Device(OVRLAPIO)
#undef DEVICE_MAIN

// This demonstration works in conjunction with the application OVLPAPP.EXE,
// which is built from OVLPAPP.C and OVLPAPP.MAK.

// The application calls the VxD via DeviceIoControl to perform some operation.
// The VxD simulates a delayed I/O operation by returning -1 to signal "I/O 
// pending". The VxD schedules a global time out to simulate the event that
// corresponds to hardware completion of the I/O operation. The handler for
// the global timeout calls VWIN32_DIOCCompletionRoutine, which signals the
// ring 3 event associated with the I/O wait.
// 

// The VxD has one control message handler (W32_DEVICEIOCONTROL)

DWORD OvrlapioDevice::OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams)
{
	SimulateIOTimeOut* pTO;
	PCHAR pageBase;

// Branch on function

	switch (pDIOCParams->dioc_IOCtlCode)
	{

// Return success on open/close so that CreateFile interface works correctly

	case DIOC_OPEN:
	case DIOC_CLOSEHANDLE:
		return DEVIOCTL_NOERROR;

// This case handles the I/O simulation operation

	case SIMULATE_IO:
		if (pDIOCParams->dioc_ovrlp)
		{

// Allocate a time out
			pTO = new SimulateIOTimeOut();

// Lock the pages of the client's overlapped structure because we will access
// them later when they may not be in memory or in the current context

			pageBase = (PCHAR)LinPageLock(PAGENUM(pDIOCParams->dioc_ovrlp), 
					   	_NPAGES_(pDIOCParams->dioc_ovrlp, sizeof(OVERLAPPED)), 
					   	PAGEMAPGLOBAL);
// Compute the global address of the overlapped structure, and store in 
// time out object
			pTO->m_pOvp = (OVERLAPPED*)(pageBase+((ULONG)pDIOCParams->dioc_ovrlp & 0xfff));

// Set the time out. This simulates "I/O in progress". 
			pTO->Set();

// Signal i/o "pending" to the calling application.
			return -1; 
		}
		break;
	}

	return 0;
}

// Constructor for time out

SimulateIOTimeOut::SimulateIOTimeOut() : VGlobalTimeOut(DELAY_MSEC) {}

// Handler for time out

VOID SimulateIOTimeOut::handler(VMHANDLE hVM, THREADHANDLE hThread, 
				PCLIENT_STRUCT pRegs, DWORD lagTime)
{
// Set status value in overlapped structure
	m_pOvp->O_InternalHigh = 0x1234;

// Signal the ring 3 event
	VWIN32_DIOCCompletionRoutine(m_pOvp->O_Internal);

// Release the lock on the memory
	LinPageUnLock(PAGENUM(m_pOvp), _NPAGES_(m_pOvp, sizeof(OVERLAPPED)), 
							PAGEMAPGLOBAL);

// Flush this time out
	delete this;						
}

