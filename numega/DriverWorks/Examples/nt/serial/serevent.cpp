// serevent.cpp - event wait support for serial device
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

// A key part of the serial interface is event waiting. This facility
// enables an application to pend a request on the occurrence of 
// various events related to the serial device. For example, an
// application can wait for a particular character to be read. 
// 
// Each subclass must set data member m_SupportedEvents to a bit
// encoded value representing the events that it supports. The bit
// flags are:
//

// SERIAL_EV_RXCHAR            Any Character received
// SERIAL_EV_RXFLAG            Received certain character
// SERIAL_EV_TXEMPTY           Transmit Queue Empty
// SERIAL_EV_CTS               CTS changed state
// SERIAL_EV_DSR               DSR changed state
// SERIAL_EV_RLSD              RLSD changed state
// SERIAL_EV_BREAK             BREAK received
// SERIAL_EV_ERR               Line status error occurred
// SERIAL_EV_RING              Ring signal detected
// SERIAL_EV_PERR              Printer error occured
// SERIAL_EV_RX80FULL          Receive buffer is 80 percent full
// SERIAL_EV_EVENT1            Provider specific event 1
// SERIAL_EV_EVENT2            Provider specific event 2

// When the subclass detects an event, it queues DPC data member
// m_EventDpc, supplying the mask indicating the event(s) that
// occurred. This invokes member function EventDetected, which
// takes the appropriate action.

// The subclass normally overrides member function SynchronizeEventAccess,
// which the base class uses to call its functions that manipulate the
// event state.

#include <vdw.h>
#include "serdev.h"
#include "glbtrace.h"

/////////////////////////////////////////////////////////////////////////////
struct _test_mask
{
	SerialDevice* device;
	ULONG mask;
	KIrp waiter;
	BOOLEAN set_mask;
	BOOLEAN set_waiter;
};

/////////////////////////////////////////////////////////////////////////////
// StartSetMask
//
// Start a set event wait mask operation
//
VOID SerialDevice::StartSetMask(ULONG mask)
{
	_test_mask tm;
	tm.device = this;
	tm.mask = mask;
	tm.waiter = KIrp(0);
	tm.set_mask = TRUE;
	tm.set_waiter = FALSE;
													 
	SynchronizeEventAccess(LinkTo(TestAndClearEventState), &tm);
}

/////////////////////////////////////////////////////////////////////////////
// StartWaitMask
//
// Start a set event wait mask operation. The CancelSpinLock is held when
// this is called.
//
VOID SerialDevice::StartWaitMask(void)
{
	// Only one IRP at at a time may wait

	if (m_Waiter.m_Irp)
	{
		CancelSpinLock::Release();
		m_WaitIrpQueue.CompleteCurrent(STATUS_INVALID_PARAMETER);
		return;
	}

	KIrp I(m_WaitIrpQueue.m_CurrentIrp);

	GTRACE( (TLEVEL, "Start wait for mask=%x\n", m_WaitMask));

	_test_mask tm;
	tm.device = this;
	tm.mask = 0;
	tm.waiter = I;
	tm.set_mask = FALSE;
	tm.set_waiter = TRUE;

	SynchronizeEventAccess(LinkTo(TestAndClearEventState), &tm);

	if (tm.mask == 0)
	{
		I.SetCancelRoutine(LinkTo(CancelWait));
		I.MarkPending();
	}

	CancelSpinLock::Release();

	// Passing STATUS_PENDING to CompleteCurrent tells that routine to get 
	// the next entry in the wait queue. If the wait was immediately satisfied,
	// then TestAndClearEventState has already 

	m_WaitIrpQueue.CompleteCurrent(STATUS_PENDING);
}

/////////////////////////////////////////////////////////////////////////////
// Synchronize access to event mask 
//
// Most subclasses will override this member function. The function is 
// designed to be compatible with subclasses that need interrupt level
// synchronization, although a subclass could equally well implement
// this function by acquiring a spin lock, making the call, and releasing
// the spin lock. The base class just calls the function.

BOOLEAN SerialDevice::SynchronizeEventAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		)
{
	return func(context);
}


/////////////////////////////////////////////////////////////////////////
// TestAndClearEventState
//
// This is a general purpose routine for manipulating the event state.
// It can set either the mask or the waiter, or both, or neither. It
// always clears m_EventState, which reflects the currently recordedd
// events.
//
// This is used by both StartSetMask, StartWaitMask, CancelCurrentEventWait,
// and CancelWait.
//
VOID SerialDevice::TestAndClearEventState(
	ULONG& mask, 
	KIrp waiter,
	BOOLEAN set_mask,
	BOOLEAN set_waiter
	)
{
	if (set_waiter)
		m_Waiter = waiter;

	if (set_mask)
		m_WaitMask = mask;

	if (mask != CANCEL_EVENT_MASK)
		mask = m_EventState & m_WaitMask;

	m_EventState = 0;	// reset state to "no events seen"

	// if the new mask was zero or satisfies a wait, queue the DPC

 	if ((m_Waiter.m_Irp != NULL) && ((mask != 0) || (m_WaitMask == 0)))
	{
		waiter.m_Irp = m_Waiter.m_Irp;
		m_Waiter.m_Irp = 0;
		m_EventDpc.Request(waiter.m_Irp, (PVOID)mask);
	}
}

////////////////////////////////////////////////////////////////////////////
// CancelCurrentEventWait
//
// Cancels the current wait via the synchronized method.
//
VOID SerialDevice::CancelCurrentEventWait(void)
{
	GTRACE ( (TLEVEL, "Canceling current wait\n"));

	_test_mask tm;
	tm.device = this;
	tm.mask = CANCEL_EVENT_MASK;
	tm.waiter = KIrp(0);
	tm.set_mask = TRUE;
	tm.set_waiter = FALSE;

	SynchronizeEventAccess(LinkTo(TestAndClearEventState), &tm);
}

/////////////////////////////////////////////////////////////////////////////
// CancelWait
//
// This is an IRP Cancel routine called by the I/O Manager.
//
VOID SerialDevice::CancelWait(KIrp I)
{
	_test_mask tm;
	tm.device = this;
	tm.waiter = KIrp(0);
	tm.set_mask = FALSE;
	tm.set_waiter = TRUE;

	SynchronizeEventAccess(LinkTo(TestAndClearEventState), &tm);
	CancelSpinLock::Release();

	I.Information() = 0;
	I.Complete(STATUS_CANCELLED);
}

/////////////////////////////////////////////////////////////////////////////
// Event detected
//
// Handle detection of event (this is a DPC)
//
// Arg1 is the waiter IRP  (or NULL) and arg2 is the event state
//
VOID SerialDevice::EventDetected(PVOID arg1, PVOID arg2)
{
	ASSERT (arg1 != NULL) ;

	KIrp I( (PIRP)arg1 );
	NTSTATUS status;

	GTRACE( (TLEVEL, "Event detected: %p\n", arg2) );
	
	CancelSpinLock::Acquire();
	if (I.WasCanceled())
	{
		CancelSpinLock::Release();
		return;
	}
	else
	{
		I.SetCancelRoutine(NULL);
		CancelSpinLock::Release();
	}

	if ((ULONG)arg2 == CANCEL_EVENT_MASK)
	{
	   	status = STATUS_CANCELLED;
	   	I.Information() = 0;
	}
	else
	{
	   	status = STATUS_SUCCESS;
	   	I.Information() = sizeof(ULONG);
	   	*(ULONG*)I.IoctlBuffer() = (ULONG)arg2;
	}			

   	I.Complete(status);
}

////////////////////////////////////////////////////////////////////
// Linkage routine
//
BOOLEAN SerialDevice::LinkTo(TestAndClearEventState)(PVOID Context)
{
	_test_mask* p = (_test_mask*)Context;

	p->device->TestAndClearEventState(
		p->mask,
		p->waiter,
		p->set_mask,
		p->set_waiter
		);

	return TRUE;
}
