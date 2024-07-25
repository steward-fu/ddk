// control.cpp - control device class for keyboard filter driver
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
#include <vdw.h>

#include "common.h"
#include "keyfifo.h"
#include "kbfltdev.h"
#include "control.h"

// The control device provides the interface between the test app
// and the keyboard filter device. The control device can be opened
// by only one instance of the test app at a time. The test app
// sends an IOCTL to tell the control device to hook the keyboard.
// In order to hook the keyboard, the control device creates an 
// instance of the keyboard filter device.
//
// The test app provides the handle of an event to be signalled when
// there is something to read. The control device sets the event when
// the embedded FIFO object (see keyfifo.h) detects that the filter
// device has written into the empty FIFO.
//
// When the test app is notified, it performs a ReadFile on the control
// device. The control device responds by supplying data read currently
// in the FIFO.

VOID OnDataAvailable(PVOID);	// forward

////////////////////////////////////////////////////////////////////////////
// Begin INIT section code
#pragma code_seg("INIT")

///////////////////////////////////////////////////////////////////////////
// Constructor
//
FilterControlDevice::FilterControlDevice(BOOLEAN bAutoHook) :
	KDevice(L"FilterTest", FILE_DEVICE_UNKNOWN,L"FilterTest"),
	m_Fifo(::OnDataAvailable, FIFOSIZE),
	m_HookOnLoad(bAutoHook),
	m_FilterDevice(NULL),
	m_DeviceBusy(FALSE),
	m_AppNotificationEvent(NULL)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		DbgPrint("FILTER: Failed to create device: %x\n", m_ConstructorStatus);
		return;
	}
	m_Fifo.m_ControlContext = this;

	//If we're hooking at load time, create our filter device now.
	if(m_HookOnLoad)
	{
		m_FilterDevice = new (NonPagedPool) KeyboardFilterDevice(&m_Fifo, TRUE);
		if(!m_FilterDevice)
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
	}
}
#pragma code_seg()

// End INIT section code
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Destructor
//
FilterControlDevice::~FilterControlDevice(void)
{
	Unhook();

	//Try to delete the filter device.  The call will fail if m_HookOnLoad is TRUE.
	if (m_FilterDevice)
	{
		if(m_FilterDevice->GetUseCount() > 0)
			return;
		delete m_FilterDevice;
		m_FilterDevice = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// Hooking/unhooking
NTSTATUS FilterControlDevice::Hook(HANDLE hUserEvent)
{
	if (m_AppNotificationEvent)
		delete m_AppNotificationEvent;
	if (hUserEvent)
		m_AppNotificationEvent = new (NonPagedPool) KEvent(hUserEvent);
	else
		m_AppNotificationEvent = NULL;

	m_Fifo.Reset();

	if(!m_FilterDevice)
	{
		m_FilterDevice = new (NonPagedPool) KeyboardFilterDevice(&m_Fifo, FALSE);
		if(!m_FilterDevice)
			return STATUS_UNSUCCESSFUL;
	}

	m_FilterDevice->HookCompletions(TRUE);
	return STATUS_SUCCESS;
}

NTSTATUS FilterControlDevice::Unhook()
{

	if (m_FilterDevice)
	{
		m_FilterDevice->HookCompletions(FALSE); 

		if(!m_HookOnLoad)
		{
		if(m_FilterDevice->GetUseCount() > 0)
			return STATUS_PENDING;
		delete m_FilterDevice;
		m_FilterDevice = NULL;
		}
	}

	delete m_AppNotificationEvent;
	m_AppNotificationEvent = NULL;

	return STATUS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////
// DeviceControl
//
// There are two functions: HOOK, which creates a keyboard filter device,
// and UNHOOK, which destroys it.
//
NTSTATUS FilterControlDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;

	I.Information() = 0;

	switch (I.IoctlCode())
	{
	case IOCTL_HOOK:
		status = Hook (*(HANDLE*)I.IoctlBuffer());
		return I.Complete(status);

	case IOCTL_UNHOOK:
			Unhook();
		return I.Complete(STATUS_SUCCESS);

	default:
		return I.Complete(STATUS_INVALID_PARAMETER);
	}
}

////////////////////////////////////////////////////////////////////////////
// This non-member function is called by the FIFO object when there is
// data available. It dispatches to a member function of the control device
//
VOID OnDataAvailable(PVOID Context)
{
	((FilterControlDevice*)Context)->OnDataAvailable();
}

////////////////////////////////////////////////////////////////////////////
// FilterControlDevice::OnDataAvailable
//
// This is called by the FIFO object (via the dispatcher) when there is
// data available in the FIFO. Tell the test app by setting the event.
//
VOID FilterControlDevice::OnDataAvailable() 
{
	if(m_AppNotificationEvent)
		m_AppNotificationEvent->Set();
}

////////////////////////////////////////////////////////////////////////////
// Read
//
// This is called when the test app collects the intercepted keyboard data.
//
NTSTATUS FilterControlDevice::Read(KIrp I) 			
{ 
	ULONG MaxItemsToRead = I.ReadSize() / sizeof(KEYBOARD_INPUT_DATA);
	ULONG itemsRead;

	itemsRead = m_Fifo.ControlRead((KEYBOARD_INPUT_DATA*)I.IoctlBuffer(), MaxItemsToRead);

	I.Information() = itemsRead * sizeof(KEYBOARD_INPUT_DATA);	

	return I.Complete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////
// Create
//
// This is called when the test app opens the control device. It ensures
// that only one process opens the device at a time.
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

////////////////////////////////////////////////////////////////////////////
// Close
//
// Called when the test app closes the device or the process is termianted.
// This will kill the filter device if it exists.
//
NTSTATUS FilterControlDevice::Close(KIrp I)
{
	Unhook();

	m_DeviceBusy = FALSE;

	return DefaultDispatch(I);
}

NTSTATUS FilterControlDevice::FlushBuffers(KIrp I) 	{ return DefaultDispatch(I); }
NTSTATUS FilterControlDevice::CleanUp(KIrp I) 		{ return DefaultDispatch(I); }
