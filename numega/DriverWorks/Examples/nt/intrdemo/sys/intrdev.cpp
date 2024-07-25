// intrdev.cpp - implementation of interrupt demo device class 
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
#include "intrdev.h"

// Class InterruptDemoDevice hooks IRQ 6, which is the floppy
// interrupt on Intel platforms. The device does not hook the
// interrupt until the test application opens it, and member
// Create is called. The ISR takes a time stamp, writes a record to
// a FIFO, and queues a DPC. The DPC notifies the test app that an
// interrupt has occurred by setting an event whose handle had
// been previously sent to the device using DeviceIoControl. The
// DPC does not set the event if the app has not responded to a
// previous event. The app retrieves the records written to the
// FIFO by the ISR using another DeviceIoControl call.

KDebugOnlyTrace T("IntrDemo");

//////////////////////////////////////////////////////////////////
// Device class constructor
//
InterruptDemoDevice::~InterruptDemoDevice()
{
	T << "Deleting device\n";

	if (m_pEvent)
		delete m_pEvent;

	if ((PKINTERRUPT)m_Interrupt)
		m_Interrupt.Disconnect();
}

//////////////////////////////////////////////////////////////////
// Begin INIT section code
#pragma code_seg("INIT")

//////////////////////////////////////////////////////////////////
// Device class constructor
//
InterruptDemoDevice::InterruptDemoDevice() :

// Base class
	KDevice(
		L"IntrDemo0", 
		FILE_DEVICE_UNKNOWN, 
		L"IntrDemo0", 
		DO_BUFFERED_IO
		),

// Interrupt object
	m_Interrupt(
		Isa,
		0,
		FLOPPY_IRQ,
		FLOPPY_IRQ,
		Latched,
		TRUE,
		FALSE
	),

// Output fifo
	m_TimeStampFifo(FIFOSIZE, NonPagedPool)

{
	m_File = 0;
	m_pEvent = NULL;
	m_bNotifyApp = TRUE;
	m_Dpc.Setup(LinkTo(Dpc), this);
	m_InterruptCount = 0;
}
#pragma code_seg()

//////////////////////////////////////////////////////////////////
// Create 
//
// This is called when the test app issues a CreateFile
//
NTSTATUS InterruptDemoDevice::Create(KIrp I)
{
	NTSTATUS status;

	if (m_File == 0)
	{
		m_File = I.FileObject();
		status = m_Interrupt.Connect(LinkTo(Isr), this);
	}
	else
		status = STATUS_UNSUCCESSFUL;

	m_pEvent = NULL;

	T << "Create status = " << ULONG(status) << "\n";
	
	I.Information() = 0;
	return I.Complete(status);
}

//////////////////////////////////////////////////////////////////
// Close
//
// This is called when the test app closes the handle to the device
//
NTSTATUS InterruptDemoDevice::Close(KIrp I)
{
	NTSTATUS status;

	I.Information() = 0;

	if (m_File == I.FileObject())
	{
		T << "Disconnecting interrupt\n";

		m_Interrupt.Disconnect();

		m_File = 0;
		if (m_pEvent) delete m_pEvent;
		m_pEvent = NULL;

		status = STATUS_SUCCESS;
	}
	else
		status = STATUS_UNSUCCESSFUL;

	return I.Complete(status);
}

//////////////////////////////////////////////////////////////////
// DeviceControl
//
// The test app uses a DeviceIoControl call for two purposes:
//
// 1. It passes an event handle to the driver. The driver sets
//    the event when there is something for the app to do, i.e.,
//    when an interrupt occurs, and the app fetches the data.
//
// 2. It fetches the timestamp data for the interrupts.
//
NTSTATUS InterruptDemoDevice::DeviceControl(KIrp I)
{
	T << I;
	
	I.Information() = 0;

	switch (I.IoctlCode())
	{
	case IOCTL_SET_NOTIFICATION_EVENT:
	{
		HANDLE hEvent = *(HANDLE*)I.IoctlBuffer();

		if (m_pEvent)
			delete m_pEvent;

		m_pEvent = new (NonPagedPool) KEvent(hEvent, OBJECT_TYPE_ALL_ACCESS);

		return I.Complete(STATUS_SUCCESS);
	}

	case IOCTL_GET_TIMESTAMP_DATA:
		if (SynchronizeInterrupt(&m_Interrupt, LinkTo(ReadTimeFifo), PIRP(I)) )
			return I.Complete(STATUS_SUCCESS);

		else
			return I.Complete(STATUS_UNSUCCESSFUL);

		break;

	default:
		T << "Bad Request: " << I.IoctlCode() << "\n";

		return I.Complete(STATUS_INVALID_PARAMETER);	
	}
}

//////////////////////////////////////////////////////////////////
// Isr
//
// This is the interrupt service routine. Since we always return
// FALSE (to indicate that we did not service the interrupt), there
// is no interaction with the hardware required. The system will
// call the actual floppy interrupt handler. This routine justs
// counts the interrupt (m_InterruptCount), grabs a timestamp,
// and requests queues the DPC object.
//
BOOLEAN InterruptDemoDevice::Isr(void)
{
	T << "In the ISR\n";

	TIMESTAMP ts;

	ts.ts_interrupt_count = ++m_InterruptCount;
	KeQueryTickCount(&ts.ts_time);

	m_TimeStampFifo.Write(&ts, 1);

	m_Dpc.Request();
	return FALSE;
}

//////////////////////////////////////////////////////////////////
// Dpc
// 
// This the callback associated with the device's DPC object. The
// ISR queues the dpc object, resulting in a call to this routine.
// It invokes the synch crit section member function TestAndClearNotifyApp,
// which determines if it is necessary to notify the app of the 
// interrupt. The idea is that we only notify the app when there
// is no outstanding interrupt to which the app has not responded.
//
VOID InterruptDemoDevice::Dpc(PVOID Arg1, PVOID Arg2)
{
	BOOLEAN Notify;
	BOOLEAN SynchStatus;

	if (m_pEvent) 
	{
		SynchStatus = SynchronizeInterrupt(
			&m_Interrupt, 
			LinkTo(TestAndClearNotifyApp), 
			&Notify
			);

		if (SynchStatus)
		{
			T << "DPC, App notify=" << ULONG(Notify) << "\n";
			if (Notify) 
				m_pEvent->Set();
		}
		else
			T << "Dpc error synchronizing\n";
	}
}

//////////////////////////////////////////////////////////////////
// ReadTimeFifo
// 
// This is a synch critical section member function, called when
// the app requests data from the fifo. (Note: this could be done
// with a KInterruptSafeFifo object.)
//
BOOLEAN InterruptDemoDevice::ReadTimeFifo(PVOID pIrp)
{
	KIrp I = (PIRP)pIrp;
	ULONG MaxReadItems = I.IoctlOutputBufferSize() / sizeof(TIMESTAMP);
	ULONG nItemsRead;

	nItemsRead = m_TimeStampFifo.Read((TIMESTAMP*)I.IoctlBuffer(), MaxReadItems);
	// guaranteed to empty the buffer, because app's read buffer is sizeof the fifo

	I.Information() = nItemsRead * sizeof(TIMESTAMP);

	m_bNotifyApp = TRUE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// TestAndClearNotifyApp
//
// This is a synch critical section member function, called from
// the DPC to determine if the app needs to be notified. The
// parameter is assumed to be a pointer to a boolean that supplies
// the output status.
//
BOOLEAN InterruptDemoDevice::TestAndClearNotifyApp(PVOID p)
{
	*(BOOLEAN*)p = m_bNotifyApp;
	m_bNotifyApp = FALSE;

	T << "TestAndClearNotifyApp previous state was=" << ULONG(m_bNotifyApp) << "\n";

	return TRUE;
}
