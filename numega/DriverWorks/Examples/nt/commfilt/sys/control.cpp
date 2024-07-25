// control.cpp - Code for control device class for comm filter driver
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

//
//	FilterControlDevice
//
//	This device provides the interface between the filter device and the
//	test application. It embeds an instance of class CommDataFifo, which
//	is used to pipe comm read/write/control records to the GUI. 
//	
//	The DeviceControl handler accepts requests to hook and unhook 
//	comm ports. To hook the device creats an instance of the
//	filter device class, CommFilterDevice.  To unhook, it sends a
//	Terminate message to the CommFilterDevice device, which in turn
//	calls the TerminateCallback method when it has finished terminating.
//	Termination may take some time due to outstanding requests which
//	must complete prior to termination.
//
//	The DeviceControl handler also accepts requests to modify the data
//	transfered across over the comm port.  The ModifyData method
//	is invoked by the filtering devices to determine whether the
//	data should be modified.  


// Driver::Works include file
#include <vdw.h>

// Includes for this device
#include "common.h"
#include "commfifo.h"
#include "device.h"
#include "control.h"

// Forward
VOID OnDataAvailable(PVOID);

// Begin initialization time code
//
#pragma code_seg("INIT")

///////////////////////////////////////////////////////////////////////////
// Constructor
//
FilterControlDevice::FilterControlDevice(void) :
	KDevice( L"CommFilter", FILE_DEVICE_UNKNOWN, L"CommFilter" ),
	m_Fifo(::OnDataAvailable, FIFOSIZE),
	m_AppNotificationEvent(NULL),
	m_CleanupIrp(NULL),
	m_ModifyData(FALSE)
{
	ULONG	comm;

// Make sure the base class construction was successful
	
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
#if DBG
		DbgPrint("FILTER: Failed to create device: %x\n",
			 m_ConstructorStatus);
#endif
		return;
	}
	m_Fifo.m_ControlContext = this;
	m_DeviceBusy = 0;
	m_CommCount = 0;
	for ( comm = 0; comm < MAX_COMM; ++comm ) {
		m_Filter[comm].Device = NULL;
		m_Filter[comm].Terminated = TRUE;
	}
}
#pragma code_seg()

#define COMM_DEVICE_NAME	L"\\DosDevices\\COM"

///////////////////////////////////////////////////////////////////////////
// DeviceControl
// 
// The test application makes two IOCTL requests, one to hook comm 
// input and one to unhook it.
//
NTSTATUS FilterControlDevice::DeviceControl(KIrp I)
{
	NTSTATUS		status;
//	WCHAR			CommName[sizeof COMM_DEVICE_NAME / 2];
	struct commfilter *	Comm;
	KLowerDevice	*	CommObj;
	
	I.Information() = 0;

	switch (I.IoctlCode())
	{
	case IOCTL_HOOK:
	{

// The hook request includes an event handle as input. The test application
// waits on this event so that it knows when to issue a read request. This
// could be implemented equally well with overlapped I/O. To handle the 
// hook request, we create an instance of the CommFilterDevice.

		struct HOOK_PARMS * parms = (struct HOOK_PARMS *) I.IoctlBuffer();
		Comm = & m_Filter[ parms->Comm - 1 ];

		if ( parms->Comm-1 >= MAX_COMM ) {
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		if ( Comm->Device )  {
			// already hooked
			status = STATUS_SUCCESS;
			break;
		}

		if ( m_CommCount == 0 ) {
			m_AppNotificationEvent = new (NonPagedPool)
						    KEvent(parms->hEvent);
		}
		if ( m_AppNotificationEvent == NULL ) {
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		KUnitizedName CommUnitName(COMM_DEVICE_NAME,parms->Comm);

		CommObj = new (NonPagedPool) 
				KLowerDevice(
					(PCWSTR)CommUnitName,
				   	FILE_ALL_ACCESS
				   	);

		if ( CommObj == NULL  ||
		     !NT_SUCCESS(CommObj->ConstructorStatus()) )
		{
			// Comm port is opened by someone else.
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		// Release comm device for others.
		delete CommObj;


		// Create filter device
		Comm->Device = new (NonPagedPool) 
			CommFilterDevice(CommUnitName, &m_Fifo, this);

		if (!Comm->Device || 
		    !NT_SUCCESS(Comm->Device->ConstructorStatus())) 
		{
			Comm->Device = NULL;
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		Comm->Terminated = FALSE;
		++m_CommCount;
		status = STATUS_SUCCESS;
		m_Fifo.Reset();

		break;
	}
	case IOCTL_UNHOOK:

// The unhook request causes the filter device to be destroyed. 

		Comm = & m_Filter[ *(ULONG *) I.IoctlBuffer() - 1 ];

		if ( Comm->Device && !Comm->Terminated )
		{
			Comm->Device->Terminate();
			Comm->Terminated = TRUE;
			--m_CommCount;
		}

		if ( m_CommCount == 0  &&  m_AppNotificationEvent )
		{
			delete m_AppNotificationEvent;
			m_AppNotificationEvent = NULL;
		}		

		status = STATUS_SUCCESS;
		break;

	case IOCTL_MODIFY:

// The modify request causes the filter device to modify data as it
// passes through.

		m_ModifyData = * (BOOLEAN *) I.IoctlBuffer();
		status = STATUS_SUCCESS;
		break;

	default:
		status = STATUS_INVALID_PARAMETER;
	}

	return I.Complete(status);
}

BOOLEAN FilterControlDevice::ModifyData(void)
{
	return m_ModifyData;
}


///////////////////////////////////////////////////////////////////////////
// Fifo notification
//
// The fifo calls this function when data is available to be read. The
// context parameter is a pointer to the FilterControlDevice, enabling
// this function to call the data notification member function of the
// device object.
//
VOID OnDataAvailable(PVOID Context)
{
	((FilterControlDevice*)Context)->OnDataAvailable();
}

///////////////////////////////////////////////////////////////////////////
// Fifo notification member function.
//
// When the filter device writes data into the fifo, the fifo calls 
// ::OnDataAvailable with a pointer to the control device, which in turn
// calls this member function. This function sets the event passed to
// the device by the application when the application sent the IOCTL to
// hook the comm input. Setting this event causes the app to read
// the device.
// 
VOID FilterControlDevice::OnDataAvailable() 
{
	m_AppNotificationEvent->Set();
}

///////////////////////////////////////////////////////////////////////////
// Read
//
// The test application invokes this member function to read the comm
// data trapped by the filter device and fed into the pipe under the control
// of the control device.
//
// This routine just reads the data out of the fifo into the client's buffer.
//
NTSTATUS FilterControlDevice::Read(KIrp I) 			
{ 
	ULONG itemsRead;

	itemsRead = m_Fifo.ControlRead((RECORD *)I.IoctlBuffer(), 
				       I.ReadSize());

	I.Information() = itemsRead * sizeof(RECORD);

	return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////
// Create
//
// This function is called when the test app opens the control device (with 
// CreateFile). It keeps a busy flag so that only one client can have a handle 
// for the control device at any given time.
//
NTSTATUS FilterControlDevice::Create(KIrp I)
{
	NTSTATUS status;

	m_ThisLock.Lock();

	status = m_DeviceBusy ? STATUS_UNSUCCESSFUL : STATUS_SUCCESS;
	m_DeviceBusy = TRUE;

	m_ThisLock.Unlock();

	I.Information() = 0;
	return I.Complete(status);
}

///////////////////////////////////////////////////////////////////////////
// CleanUp
//
// This function is invoked just prior to Close.  It requests that
// all filter devices detach themselves and terminate.  If all filters
// are detached immediately, it completes with success.  Otherwise,
// it saves a pointer to the IRP and marks it as pending.  The 
// TerminateCallback function completes the IRP in that case.
//
NTSTATUS FilterControlDevice::CleanUp(KIrp I)
{
	ULONG comm;

	for ( comm = 0; comm < MAX_COMM; ++comm )
		if (!m_Filter[comm].Terminated)
		{
			m_Filter[comm].Terminated = TRUE;
			m_Filter[comm].Device->Terminate();
		}

	if (m_AppNotificationEvent)
	{
		delete m_AppNotificationEvent;
		m_AppNotificationEvent = NULL;
	}

	// Ensure all filters have terminated.
	for ( comm = 0; comm < MAX_COMM; ++comm )
		if ( m_Filter[comm].Device != NULL ) {
			m_CleanupIrp = I;
			I.MarkPending();
			return STATUS_PENDING;
		}

	m_DeviceBusy = FALSE;

	return I.Complete(STATUS_SUCCESS);
}


///////////////////////////////////////////////////////////////////////////
// TerminateCallback
//
// This function is invoked by filter devices when they have completed
// a request to terminate.  If it is called by the last filter device
// to terminate and a request to close is pending, then it completes 
// the pending IRP.
//
VOID FilterControlDevice::TerminateCallback( CommFilterDevice * terminatee )
{
	ULONG comm;
	ULONG open = 0;

	for ( comm = 0; comm < MAX_COMM; ++comm ) {
		struct commfilter * Comm = &m_Filter[ comm ];  
		if ( Comm->Device == terminatee )
			Comm->Device = NULL;
		if ( Comm->Device )
			++open;
	}
	if ( m_CleanupIrp  &&  open == 0 ) {
		m_CleanupIrp.Complete(STATUS_SUCCESS);
		m_CleanupIrp = KIrp(NULL);
		m_DeviceBusy = FALSE;
	}
}


///////////////////////////////////////////////////////////////////////////
// Close
//
// This is called when the control device is closing.  The CleanUp
// function takes care of cleanup duties, so this function just
// completes the IRP with success.
//
NTSTATUS FilterControlDevice::Close(KIrp I)
{
	return I.Complete(STATUS_SUCCESS); 
}


///////////////////////////////////////////////////////////////////////////
// Other dispatchers
//
// These are not called by the test app, but they are necessary because
// all device types in the driver must support processing of the union of 
// IRP types processed by all devices. Since the filtered device (the 
// system comm device) handles these IRPs, so must the filter device, 
// and, consequently, the control device.

NTSTATUS FilterControlDevice::FlushBuffers(KIrp I)
{
	return DefaultDispatch(I);
}

NTSTATUS FilterControlDevice::QueryInformation(KIrp I)
{
	return DefaultDispatch(I);
}

NTSTATUS FilterControlDevice::SetInformation(KIrp I)
{
	return DefaultDispatch(I);
}

NTSTATUS FilterControlDevice::Write(KIrp I)
{
	return DefaultDispatch(I);
}
