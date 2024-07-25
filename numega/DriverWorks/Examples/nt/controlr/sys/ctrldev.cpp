// ctrldev.cpp - Example code for class KController
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
#include "ctrldev.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////
// Begin INIT time code
//#pragma code_seg ("INIT")
/////////////////////////////////////////////////////////////////////
// Constructor for class ControllerTestDevice
//

ControllerTestDevice::ControllerTestDevice(ULONG unit, KController& Controller) :
	KDevice( 
		KUnitizedName(L"CTRLTST", unit),
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"CTRLTST", unit)
		),
	m_Controller(Controller),
	m_CancelSyncLock(),
	m_tcb(),
	m_unit(unit),
	m_WaitForCancel(),
	m_CancelFinished(SynchronizationEvent)
{
	m_WaitForCancel.Initialize(*this);
}
#pragma code_seg()
// End INIT time code
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Dispatch handler for Create and Close
//
// Complete the IRP and return success
//
NTSTATUS ControllerTestDevice::CreateClose(KIrp I)
{
	I.Information()=0;
	return I.Complete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////////////////////
// Dispatch handler for DeviceIoControl
//
// Queues all valid IRPs to StartIo
//
NTSTATUS ControllerTestDevice::DeviceControl(KIrp I)
{
	switch (I.IoctlCode())
	{
	case IOCTL_CTRLTEST_EASY_REQUEST:
		I.Information() = 0;
		return I.Complete(STATUS_SUCCESS);

	case IOCTL_CTRLTEST_NEEDS_DEVICE_SERIALIZATION:
	case IOCTL_CTRLTEST_NEEDS_CONTROLLER:
		return QueueIrp(I, LinkTo(CancelQueuedIrp));

	default:
		DbgPrint("CTRLTEST: Bad request on unit\n", m_unit); 
		I.Information() = 0;
		return I.Complete(STATUS_INVALID_PARAMETER);
	}	

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// StartIo
//
// If the request needs the controller, synch it.
// Otherwise, complete the request here
//
VOID ControllerTestDevice::StartIo(KIrp I)
{

	// Before processing the IRP, we need to check to see if it has been
	// cancelled and set it to a non-cancellable state.  When performing
	// these operations, it is necessary to hold the global cancel spin
	// lock and take special precautions to ensure the IRP is still valid.
	// This is accomplished using the routine KIrp::TestAndSetCancelRoutine().
	if ( !I.TestAndSetCancelRoutine(
		LinkTo(CancelQueuedIrp),
		NULL,
		CurrentIrp()) )
	{
		
		// The IRP has been cancelled we stop processing and exit.  Since
		// it was in a cancellable state previously, it will be completed by
		// the cancel routine that had been set on it.
		return;
	}

	// process request according to type
	switch (I.IoctlCode())
	{
	case IOCTL_CTRLTEST_NEEDS_DEVICE_SERIALIZATION:
	// Complete now
		I.Information() = 0;
		I.Status() = STATUS_SUCCESS;
		NextIrp(I);
		break;

	case IOCTL_CTRLTEST_NEEDS_CONTROLLER:

		// At this point the IRP is in a noncancelable state.  We could leave
		// it this way and simply check to see if the IRP has been canceled
		// once our callback routine is called.  This would eliminate the need
		// for the m_CancelSyncLock spinlock, and the CancelInProgressIrp cancel
		// routine.  It would also simplify the overall logic.  The cost of
		// this approach is that the IRP cannot be completed or canceled
		// until the controller dequeues our request and calls our callback.
		// If this will be a short time, there is no problem.  If, however,
		// it could be a long time, the approach shown here could be used
		// to allow the IRP to be canceled while queued on the controller
		// object.

		// Lock the spinlock so the cancel routine we are about to set
		// can't execute before we queue our device on the controller.
		m_CancelSyncLock.Lock();

		// Use KIrp::TestAndSetCancelRoutine to set the new cancel routine on
		// the IRP and find out if it has been cancelled.
		if ( !I.TestAndSetCancelRoutine(
			NULL,
			LinkTo(CancelInProgressIrp),
			CurrentIrp()) )
		{
			// If the IRP has been cancelled we must complete it here with
			// STATUS_CANCELLED, since previously the IRP was in a
			// non-cancellable state, and no cancel routine could have been
			// called when it was cancelled.  We must also start the next
			// IRP in the queue.
			m_CancelSyncLock.Unlock();
			I.Information() = 0;
			I.Status() = STATUS_CANCELLED;
			NextIrp(I);
			return;
		}

		// Queue for controller, CurrentIrp remains cancelable. We pass
		// the current IRP as the context pointer for use in the controller
		// sync routine.
		// NOTE: We are effectively calling out of our driver at this point
		// while holding a spinlock.  To avoid deadlock, we must thoroughly
		// understand what will happen as a result of this call.  Specifically,
		// since this call may result in our callback routine being called inline,
		// we must be certain not to try and acquire the spinlock again in our
		// callback routine.
		SynchronizeController(&m_Controller, LinkTo(ControllerSynched), CurrentIrp());

		// Unlock the spinlock.
		m_CancelSyncLock.Unlock();
		break;

	default:
		// Should never get here.
		ASSERT(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////
// Controller Synch Routine
//
// (When called, the device owns the controller.)
//
// Schedule a callback (simulating a hardware interrupt scenario) on which to
// complete the IRP.
//
IO_ALLOCATION_ACTION ControllerTestDevice::ControllerSynched(KIrp I, PVOID Context)
{
	// WARNING:  The m_CancelSyncLock may be held at this point, so don't
	// try to lock it again, or a deadlock will result.
	
	// Before continuing the processing of the IRP, we need to check to
	// see if it has been cancelled and set it to a non-cancellable state.
	// It is extremely important that we test our original IRP, since otherwise
	// the test is meaningless. The IRP passed in by the system is taken from
	// the device object's current IRP field just before the call to the sync routine.
	// Since by design, there is no chance that we have advanced to the next IRP yet,
	// we can use this IRP since it is our original one.
	if ( !I.TestAndSetCancelRoutine(
		LinkTo(CancelInProgressIrp),
		NULL,
		CurrentIrp()) )
	{
		
		// The IRP has been cancelled.  Normally if this happens, the cancel routine
		// will have removed our device from the controller object and called NextIrp,
		// so that we never end up here with a cancelled IRP.  There is a window,
		// however, where the cancel routine was unable to remove the device since it
		// was already dequeued.  We will complete it here after cleaning up
		// our device state.  Since we want all processing to be done, we must wait
		// to make sure the cancel routine has run, otherwise it might try to remove
		// the device from the controller object's queue for the next IRP.
		// In order to make sure the cancel routine has run, we will wait for
		// an event to be signaled in the cancel routine.  We may be running at DIRQL,
		// so to accomplish this, we will schedule a work item to call NextIrp
		// once the wait has been satisfied.
		m_WaitForCancel.Queue(LinkTo(WaitForCancel), this, CriticalWorkQueue);
		
		// Since we will not be using the controller object deallocate it
		// and return.
		return DeallocateObject;
	}

	// Simulate processing to interrupt.
	m_tcb.Set( (LONG) -1000000, LinkTo(TimerCallback), (PVOID)this ) ; // 100 msec
	return KeepObject;
}

/////////////////////////////////////////////////////////////////////
// Timed Callback Member
//
// Release the controller.
// Complete the IRP.
//
VOID ControllerTestDevice::TimerCallback(PVOID arg1, PVOID arg2)
{
	KIrp I = CurrentIrp();

	ASSERT (I.IoctlCode() == IOCTL_CTRLTEST_NEEDS_CONTROLLER);
	ASSERT (I.IoctlInputBufferSize() == m_unit); // by arrangement with test app

	ReleaseController(&m_Controller);

	I.Status() = STATUS_SUCCESS;
	I.Information() = 0;
	NextIrp(I);
}

/////////////////////////////////////////////////////////////////////
// Cancel IRP Handler for IRP while queued
//
VOID ControllerTestDevice::CancelQueuedIrp(KIrp I)
{
	KDeviceQueue dq(DeviceQueue());

	if ( (PIRP)I == CurrentIrp() )
	{
		CurrentIrp() = NULL;
		CancelSpinLock::Release(I.CancelIrql());
		I.Information() = 0;
		I.Status() = STATUS_CANCELLED;
		NextIrp(I, IO_NO_INCREMENT, TRUE);
	}
	else if (dq.RemoveSpecificEntry(I))
	{
		CancelSpinLock::Release(I.CancelIrql());
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
	}
	else
	{
		CancelSpinLock::Release(I.CancelIrql());
	}
}

/////////////////////////////////////////////////////////////////////
// Cancel IRP Handler for IRP while in progress
//
VOID ControllerTestDevice::CancelInProgressIrp(KIrp I)
{
	ASSERT( (PIRP)I == CurrentIrp() );
	
	// Release the global spinlock.
	CancelSpinLock::Release(I.CancelIrql());

	// Acquire the m_CancelSyncLock spinlock and then drop it.  This just
	// prevents us from continuing until the device is queued on the
	// controller.  If we don't do this, there would be a window after
	// the cancel routine is set, and before the request is queued in which
	// our cancel routine could run.  This would result in our inability to remove
	// the request from the queue, and subsequently (when we call NextIrp() below),
	// our device becoming queued twice on the controller object, which would
	// corrupt the queue.
	m_CancelSyncLock.Lock();
	m_CancelSyncLock.Unlock();

	// Try to remove the queued request from the controller object.
	// Although there is no documentation on how to do this, analysis
	// of the IoAllocateController disassembly and related structures,
	// shows that what the system is doing is queueing to a device queue
	// contained within the controller object structure, a device queue
	// entry located in the WAIT_CONTEXT_BLOCK of our device object
	// structure.  We can try to remove this entry using
	// KeRemoveEntryDeviceQueue.  This function will try to remove the entry
	// atomically, and will return a BOOLEAN indicating whether or not
	// the operation succeeded.
	if ( KeRemoveEntryDeviceQueue(
		&(static_cast<PCONTROLLER_OBJECT>(m_Controller)->DeviceWaitQueue), 
		&(m_pDeviceObject->Queue.Wcb.WaitQueueEntry)) )
	{

		// We successfully removed our device object from the controller's
		// queue.  This means our callback will not be called, and that our
		// device queue entry is free to be requeued (if necessary) when we
		// start another IRP on the device.  We can now safely cancel this
		// IRP here and call NextIrp().
		I.Information() = 0;
		I.Status() = STATUS_CANCELLED;
		NextIrp(I, IO_NO_INCREMENT, TRUE);
	}
	else
	{

		// We did not succeed in removing our device from the controller's
		// queue.  This means our callback may have already been called or will
		// be soon.  Since our device state is not resolved we will have to
		// wait for the callback to run.  The IRP will be completed there.
		// We will signal an event here to let the callback routine know we
		// have run.
		m_CancelFinished.Set();
	}
}

/////////////////////////////////////////////////////////////////////
// Cancel IRP Handler for IRP while in progress
//
VOID ControllerTestDevice::WaitForCancel()
{

	// Wait for cancel to signal that it is finished.
	m_CancelFinished.Wait();

	// Now that the cancel routine has run, we can complete this IRP and
	// start the next one on the device.  Our IRP to complete is still the
	// current IRP, so use CurrentIrp() to get it.
	KIrp I(CurrentIrp());
	I.Information() = 0;
	I.Status() = STATUS_CANCELLED;
	NextIrp(I, IO_NO_INCREMENT, TRUE);
}

/////////////////////////////////////////////////////////////////////
// CleanUp Routine
//
NTSTATUS ControllerTestDevice::CleanUp(KIrp I)
{
	PFILE_OBJECT pFile = I.FileObject();

	KDeviceQueue dq(DeviceQueue());
	dq.CleanUp(pFile);

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}
