// kdmqex.cpp - implementation of class KDriverManagedQueueEx
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

// KDriverManagedQueueEx's dependant KWorkItem is deprecated since Win2k.
// For WHQL compliance, new drivers should use KDriverManagedQueueCsq instead.
// This class is maintained for non-WDM code compatibility only.
#if (NTVERSION == 400)
#include <vdw.h>

///////////////////////////////////////////////////////////////////////////////
//
//	KDriverManagedQueueEx(
//		BOOLEAN DispatchIrqlStartIo,
//		BOOLEAN QueueCancelable )
//
//	Routine Description:
//
//		Constructor for class KDriverManagedQueueEx
//
//	Parameters:
//
//		DispatchIrqlStartIo - TRUE if the StartIo function should be called at
//			IRQL DISPATCH_LEVEL. FALSE if the StartIo function should be called
//			at IRQL < DISPATCH_LEVEL. Default is TRUE.
//
//		QueueCancelable - TRUE if the IRP should be queued in a cancelable state.
//			Default is TRUE.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This is a subclass of KInterlockedList<IRP>. It is also an abstract class,
//		due to member StartIo.
//
KDriverManagedQueueEx::KDriverManagedQueueEx(
	BOOLEAN DispatchIrqlStartIo,
	BOOLEAN QueueCancelable,
	IRP_NOTIFICATION_FCN IrpCompleteNotificationFunction
	) :
	m_CurrentIrp(0),
	m_bBusy(FALSE),
	m_DispatchIrqlStartIo(DispatchIrqlStartIo),
	m_QueueCancelable(QueueCancelable),
	m_CallStartIo(),
	m_IrpNotify(IrpCompleteNotificationFunction)
{

	// Initialize the work queue item
	m_CallStartIo.Initialize(LinkTo(CallStartIo), this);
}

///////////////////////////////////////////////////////////////////////////////
//
//	NTSTATUS
//	QueueIrp(
//		KIrp I )
//
//	Routine Description:
//
//		Queues an IRP to the StartIo routine. If the queue is empty, calls
//		StartIo synchronously.
//
//	Parameters:
//
//		I - The IRP to queue (or process).
//
//	Return Value:
//
//		Always returns STATUS_PENDING.
//
//	Comments:
//
//		If the queue is not busy, this member calls member function StartIo
//		with the IRP to be queued. If the queue is busy, the IRP is put on the
//		tail of the queue. Queuing an IRP when the queue is not busy causes the
//		queue to become busy. It remains busy until there is an attempt to
//		dequeue an IRP from the queue when it is empty. Member function NextIrp
//		dequeues IRPs.
//
//		Based on the class settings (see KDriverManagedQueueEx or
//		SetDispatchIrqlStartIo), IRQL will be raised to DISPATCH_LEVEL before
//		StartIo is called (the default behavior), or StartIo will be called at
//		IRQL < DISPATCH_LEVEL from an arbitrary thread context.
//
//		If the class settings call for queuing IRPs in a cancelable state
//		(see KDriverManagedQueueEx or SetQueueCancelable), IRPs queued by this
//		member function are cancelable. QueueIrp sets the cancel routine in the
//		IRP such that overridable virtual member function Cancel is called if a
//		higher level driver cancels the IRP.
//
//		The default behavior for the class is to call StartIo at DISPATCH_LEVEL
//		and queue IRPs in a cancelable state.
//
NTSTATUS KDriverManagedQueueEx::QueueIrp( KIrp I )
{
	PDRIVER_CANCEL pLastCancel;

	// Acquire our private spinlock.
	Lock();

	// Test to see if we are making the IRP cancelable while on the queue.
	if (m_QueueCancelable)
	{

	// The IRP is to be queued in a cancelable state.

	// Check if the queue is busy
		if(m_bBusy)
		{

	// If the queue is busy, mark the IRP pending and set the DriverConext[3] field to the this pointer
	// so we have class instance information for the CancelDispatch routine.
	// The system calls the cancel routine with only the device object and
	// the IRP, so the driver has no way of knowing which of its private
	// queues the IRP is in. To overcome this, this routine stores the
	// address of the KDriverManagedQueueEx object in the last entry (index 3)
	// of the DriverContext array of PVOIDs in the Overlay structure of the IRP's
	// Tail union, where it can be retrieved by the cancel routine.
			I.MarkPending();
			I->Tail.Overlay.DriverContext[3] = this;

	// Insert the IRP into the queue and set the cancel routine.  We put the
	// IRP into the queue first so that if the cancel routine gets called,
	// the IRP will be found on the queue.  ASSERT if the cancel routine set on
	// the IRP is not NULL, since this affects our basic assumption that
	// the IRP was in a non-cancelable state when the user tried to queue it.
			InsertTailNoLock(I);
			pLastCancel = I.SetCancelRoutine(CancelDispatch);
			ASSERT(pLastCancel == NULL);

	// Test to see if the IRP was cancelled.
			if (I.WasCanceled())
			{

	// Clear out the cancel routine set on the IRP, and save the
	// pointer to the cancel routine passed back.
				pLastCancel = I.SetCancelRoutine(NULL);

	// Check the cancel routine that was set on the IRP. This is the cancel
	// routine that was in effect on the IRP when we attempted to
	// clear the cancel routine.
				if (pLastCancel == CancelDispatch)
				{

	// If the cancel routine we set previously is still set on the IRP,
	// it hasn't been called.  Unlock, and then call the cancel routine
	// ourselves.  Note we must acquire the global cancel spinlock before
	// calling the cancel routine, since this must be held when the
	// cancel routine is called.  Also, set the IRP's cancel IRQL to the
	// IRQL returned by CancelSpinLock::Acquire, so it can be restored
	// correctly by the cancel routine.
					Unlock();
					I.CancelIrql() = CancelSpinLock::Acquire();
					Cancel(I);
				}
				else
				{

	// If the cancel routine has changed (has been set to NULL by the
	// system), the cancel routine has been called and will run when
	// we release the spinlock. Unlock.
					ASSERT(pLastCancel==NULL);
					Unlock();
				}
			}
			else
			{

	// Simply unlock.  The IRP is already queued.
				Unlock();
			}
		}
		else
		{

	// If the queue is not busy, we will process the IRP with StartIo(),
	// so long as it isn't already canceled.

	// Making this IRP the current IRP, and mark the queue as busy.
			m_CurrentIrp = I;
			m_bBusy = TRUE;

	// Do some sanity checks. The queue should be empty, and there should
	// be no cancel routine set on the IRP, since that would invalidate
	// our assumption that the IRP was handed to us in a non-cancelable state.
			ASSERT(IsEmptyNoLock());
			pLastCancel = m_CurrentIrp.SetCancelRoutine(NULL);
			ASSERT(pLastCancel == NULL);

	// Mark the IRP pending
			I.MarkPending();

	// Test to see if the IRP was cancelled.  While not strictly required,
	// since we will be calling StartIo shortly and that should deal with
	// this IRP in a timely manner, this is the same behavior as
	// IoStartNextPacket().
			if (m_CurrentIrp.WasCanceled())
			{

	// The IRP is canceled.  Unlock then call the cancel routine
	// ourselves.  Before doing this acquire the global cancel
	// spinlock, which must be held when Cancel() is called.  Also,
	// set the IRP's cancel IRQL to the IRQL returned by
	// CancelSpinLock::Acquire, so it can be restored
	// correctly by the cancel routine.
				Unlock();
				m_CurrentIrp.CancelIrql() = CancelSpinLock::Acquire();
				Cancel(m_CurrentIrp);
			}
			else
			{

	// The IRP is not canceled as of this time.

	// Check to see if the StartIo routine should be called at
	// DISPATCH_LEVEL or not.
				if (m_DispatchIrqlStartIo)
				{

	// Unlock the spinlock but leave ourselves at DISPATCH_LEVEL
	// and call the StartIo routine with this IRP. When done,
	// return the IRQL to the original level.  Note that the IRP
	// is in a noncancelable state when StartIo() is called.
					KIRQL Irql = OldIrql();
					OldIrql() = DISPATCH_LEVEL;
					Unlock();
					StartIo(m_CurrentIrp);
					KeLowerIrql(Irql);
				}
				else
				{
					Unlock();

	// Check current IRQL.  If it is below DISPATCH_LEVEL, call
	// the StartIo routine.  Otherwise schedule a work queue item
	// to call StartIo, since we need to call the user at lower IRQL.
					if (KeGetCurrentIrql() < DISPATCH_LEVEL)
					{
						StartIo(m_CurrentIrp);
					}
					else
					{
						m_CallStartIo.Queue();
					}
				}
			}
		}
	}
	else
	{

	// The IRP is not to be queued in a cancelable state.

	// There should be no cancel routine set on the IRP, since that would
	// invalidate our assumption that the IRP was handed to us in a non-cancelable
	// state.
		pLastCancel = I.SetCancelRoutine(NULL);
		ASSERT(pLastCancel == NULL);

	// Mark the IRP pending.
		I.MarkPending();

	// Check if the queue is busy
		if(m_bBusy)
		{
	// Insert the IRP into the queue
			InsertTailNoLock(I);
			Unlock();
		}
		else
		{

	// If the queue is not busy, we will process the IRP with StartIo().

	// Make the IRP the current IRP, and mark the queue as busy.
			m_CurrentIrp = I;
			m_bBusy = TRUE;

	// Check to see if the StartIo routine should be called at
	// DISPATCH_LEVEL or not.
			if (m_DispatchIrqlStartIo)
			{

	// Unlock the spinlock but leave ourselves at DISPATCH_LEVEL
	// and call the StartIo routine with this IRP. When done,
	// return the IRQL to the original level.  Note that the IRP
	// is in a noncancelable state when StartIo() is called.
				KIRQL Irql = OldIrql();
				OldIrql() = DISPATCH_LEVEL;
				Unlock();
				StartIo(m_CurrentIrp);
				KeLowerIrql(Irql);
			}
			else
			{

	// Unlock and restore the old IRQL
				Unlock();

	// Check current IRQL.  If it is below DISPATCH_LEVEL, call
	// the StartIo routine.  Otherwise schedule a work queue item
	// to call StartIo, since we need to call the user at lower IRQL.
				if (KeGetCurrentIrql() < DISPATCH_LEVEL)
				{
					StartIo(m_CurrentIrp);
				}
				else
				{
					m_CallStartIo.Queue();
				}
			}
		}
	}

	return STATUS_PENDING;
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	NextIrp(
//		KIrp I,
//		CCHAR boost )
//
//	Routine Description:
//
//		Complete the specified IRP and start processing the next IRP, if present,
//		in the queue.
//
//	Parameters:
//
//		I - IRP to be completed.
//
//		boost - The increment in priority to be granted to the thread waiting for
//			the IRP to complete. The default is IO_NO_INCREMENT.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		For WDM drivers that track outstanding requests, use PnpNextIrp rather
//		than this function.
//
//		When a driver is ready to complete the current IRP and dequeue the next,
//		this member is called to perform both those operations. Before calling
//		this function, the IRP to be completed should be in a non-cancelable
//		state. The IRP is delivered to the StartIo routine in such a state, but
//		if the IRP has been made cancelable again, steps must be taken to make
//		it non-cancelable. The caller must also set the Status and Information
//		fields of the IRP being completed before calling this function. A
//		typical fragment of code looks like:
//
//		I.Status() = STATUS_SUCCESS;
//		I.Information() = nBytesReturned;
//		NextIrp(I);
//
//		To only start processing the next IRP, without completing any IRP, pass
//		the first parameter as KIrp(0).
//
//		NextIrp attempts to dequeue an IRP. If it succeeds, data member
//		m_CurrentIrp is set to the dequeued IRP, and pure virtual member
//		function StartIo is called. Otherwise, m_CurrentIrp is set to NULL.
//
//		Based on the class settings (see KDriverManagedQueueEx or
//		SetDispatchIrqlStartIo), IRQL will be raised to DISPATCH_LEVEL before
//		StartIo is called (the default behavior), or StartIo will be called at
//		IRQL < DISPATCH_LEVEL from an arbitrary thread context.
//
VOID KDriverManagedQueueEx::NextIrp( KIrp I, CCHAR boost)
{

	// Acquire our private spinlock.
	Lock();

	// Test to see if IRPs are being queued in a cancelable state.
	if (m_QueueCancelable)
	{

	// IRPs are being queued in a cancelable state.

	// A sanity check
		ASSERT(m_bBusy == TRUE);

	// Dequeue the next IRP into m_CurrentIrp
		m_CurrentIrp = RemoveHeadNoLock();

	// Check if the IRP is NULL
		if (m_CurrentIrp.IsNull())
		{

	// If so, declare the queue not busy and unlock.
			m_bBusy = FALSE;
			Unlock();
		}
		else
		{

	// If not, start to process it.  First we must make it noncancelable.
	// Clear out the cancel routine set on the IRP, and save the pointer
	// to the cancel routine passed back.
			PDRIVER_CANCEL pLastCancel;
			pLastCancel = m_CurrentIrp.SetCancelRoutine(NULL);

	// Test to see if the IRP was cancelled.
			if (m_CurrentIrp.WasCanceled())
			{

	// If the IRP was canceled, it is now the current IRP.  We hold the
	// lock so the cancel routine cannot have started yet (otherwise the
	// IRP wouldn't have been in the queue).  When we release the lock,
	// the Cancel() routine will run, and NextIrp() will be called again.

	// Check the cancel routine that was set on the IRP when we
	// cleared the cancel routine.
				if (pLastCancel == CancelDispatch)
				{

	// If the cancel routine we set was still set on the IRP,
	// it hasn't been called.  Unlock then call the cancel routine
	// ourselves.  Before doing this acquire the global cancel
	// spinlock, which must be held when Cancel() is called.
					Unlock();
					m_CurrentIrp.CancelIrql() = CancelSpinLock::Acquire();
					Cancel(m_CurrentIrp);
				}
				else
				{

	// If the cancel routine has changed (has been set to NULL by the
	// system), the cancel routine has been called and will run when
	// we release the spinlock.
					ASSERT(pLastCancel==NULL);
					Unlock();
				}
			}
			else
			{

	// We found an IRP that wasn't NULL and wasn't canceled, call our
	// StartIo() routine with it.

	// Check to see if the StartIo routine should be called at
	// DISPATCH_LEVEL or not.
				if (m_DispatchIrqlStartIo)
				{

	// Unlock the spinlock but leave ourselves at DISPATCH_LEVEL
	// and call the StartIo routine with this IRP. When done,
	// return the IRQL to the original level.  Note that the IRP
	// is in a noncancelable state when StartIo() is called.
					KIRQL Irql = OldIrql();
					OldIrql() = DISPATCH_LEVEL;
					Unlock();
					StartIo(m_CurrentIrp);
					KeLowerIrql(Irql);
				}
				else
				{
					Unlock();

	// Check current IRQL.  If it is below DISPATCH_LEVEL, call
	// the StartIo routine.  Otherwise schedule a work queue item
	// to call StartIo, since we need to call the user at lower IRQL.
					if (KeGetCurrentIrql() < DISPATCH_LEVEL)
					{
						StartIo(m_CurrentIrp);
					}
					else
					{
						m_CallStartIo.Queue();
					}
				}
			}
		}
	}
	else
	{

	// IRPs are not being queued in a cancelable state.

	// A sanity check
		ASSERT(m_bBusy == TRUE);

	// Dequeue the next IRP into m_CurrentIrp
		m_CurrentIrp = RemoveHeadNoLock();

	// Check if the IRP is NULL
		if (m_CurrentIrp.IsNull())
		{

	// If so, declare the queue not busy and unlock.
			m_bBusy = FALSE;
			Unlock();
		}
		else
		{

	// We found an IRP that wasn't NULL, call our StartIo() routine with it.

	// Check to see if the StartIo routine should be called at
	// DISPATCH_LEVEL or not.
			if (m_DispatchIrqlStartIo)
			{

	// Unlock the spinlock but leave ourselves at DISPATCH_LEVEL
	// and call the StartIo routine with this IRP. When done,
	// return the IRQL to the original level.  Note that the IRP
	// is in a noncancelable state when StartIo() is called.
				KIRQL Irql = OldIrql();
				OldIrql() = DISPATCH_LEVEL;
				Unlock();
				StartIo(m_CurrentIrp);
				KeLowerIrql(Irql);
			}
			else
			{

	// Unlock and restore the old IRQL
				Unlock();

	// Check current IRQL.  If it is below DISPATCH_LEVEL, call
	// the StartIo routine.  Otherwise schedule a work queue item
	// to call StartIo, since we need to call the user at lower IRQL.
				if (KeGetCurrentIrql() < DISPATCH_LEVEL)
				{
					StartIo(m_CurrentIrp);
				}
				else
				{
					m_CallStartIo.Queue();
				}
			}
		}
	}

	// Complete the supplied IRP
	if ( !I.IsNull() )
	{

#if DBG
	// Clear the cancel routine.  The user should have already cleared the
	// cancel routine.  ASSERT if they didn't.
		PDRIVER_CANCEL pLastCancel;
		pLastCancel = I.SetCancelRoutine(NULL);
		ASSERT(pLastCancel == NULL);
#endif
		I.Complete(I.Status(), boost);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	CallStartIo(
//		KWorkItem* pItem )
//
//	Routine Description:
//
//		Calls StartIo from the context of a worker thread.
//
//	Parameters:
//
//		pItem - Pointer to the work item
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This function is used when the user has specified that StartIo should
//		not be called at DISPATCH_LEVEL.  In this case if QueueIrp or NextIrp
//		is called from DISPATCH_LEVEL, they cannot directly call StartIo, and
//		use this work queue item to do it for them instead.
//
VOID KDriverManagedQueueEx::CallStartIo( KWorkItem* pItem )
{

	// Call StartIo
	StartIo(m_CurrentIrp);
	UNREFERENCED_PARAMETER(pItem);
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	Cancel(
//		KIrp I)
//
//	Routine Description:
//
//		Overridable function called by the system to cancel a request.
//
//	Parameters:
//
//		I - The IRP to be canceled.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		The class always sets up this routine as the cancel routine.
//		When called, the system wide cancel spin lock is held, but it is
//		dropped immediately in the default class handler implementation,
//		since it is not needed.
//
//		If the driver has special cancellation requirements, it can override
//		this function. The subclass may invoke the base class handler.
//
//		Important: A WDM driver needs to needs to take into account the
//		PnP aspects of IRP management, such as IRP reference counting.
//		Use PnpNextIrp instead NextIrp of, and KIrp::PnpComplete instead of
//		KIrp::Complete. Refer to the default implementation as an example
//		if overriding this function.
//
VOID KDriverManagedQueueEx::Cancel( KIrp I )
{

	// Release the global cancel spinlock and acquire our private lock
	CancelSpinLock::Release(I.CancelIrql());
	Lock();

#if _WDM_
	// Cast the device object pointer passed in to a KDevice pointer
	// to find out if the device object is a Pnp device or not.  If it
	// is a Pnp device, we will call the appropriate Pnp functions below
	// to maintain proper IRP reference counting.
	PDEVICE_OBJECT pDev = I.DeviceObject();
	ASSERT(pDev);
	KPnpDevice * pPnpDevice = NULL;

	if ( m_IrpNotify == NULL )
	{
		KDevice * pDevice = KDevicePTR(pDev);
		if (pDevice->IsPnpDevice())
		{
			pPnpDevice = static_cast <KPnpDevice *> (pDevice);
		}
	}
#endif // _WDM_

	// Check if the IRP is the current IRP
	if ( (PIRP)I == CurrentIrp() )
	{

	// If so, NULL it out, release the private spinlock, then
	// complete it as canceled and start the next IRP.
		CurrentIrp() = NULL;
		Unlock();

	// Setup the information and status fields in the IRP and then try
	// to start the next IRP, which will also complete this IRP.
		I.Information() = 0;
		I.Status() = STATUS_CANCELLED;

#if _WDM_
		if (m_IrpNotify == NULL)
		{
			if (pPnpDevice == NULL)
			{
				NextIrp(I, IO_NO_INCREMENT);
			}
			else
			{
				PnpNextIrp(pPnpDevice, I, IO_NO_INCREMENT);
			}
		}
		else
		{
			NextIrp(I, IO_NO_INCREMENT);
			m_IrpNotify(pDev);
		}

#else // _WDM_
		NextIrp(I, IO_NO_INCREMENT);
#endif // _WDM_

	}
	// Check if the IRP is in our list
	else if (RemoveSpecificEntry(I, FALSE))
	{

	// If so, release the private spinlock and complete it
	// as canceled.
		Unlock();

	// Setup the information and status fields in the IRP and complete it.
		I.Information() = 0;

#if _WDM_
		if ( m_IrpNotify == NULL )
		{
			if (pPnpDevice == NULL)
			{
				I.Complete(STATUS_CANCELLED);
			}
			else
			{
				I.PnpComplete(pPnpDevice, STATUS_CANCELLED);
			}
		}
		else
		{
			I.Complete(STATUS_CANCELLED);
			m_IrpNotify(pDev);
		}
#else // _WDM_
		I.Complete(STATUS_CANCELLED);
#endif // _WDM_

	}
	else
	{
	// If we got here the IRP wasn't the current IRP, and wasn't in the
	// queue.  This really should never happen, and is considered an error
	// condition, since we can't be sure that the IRP is still around.  If
	// it has already been completed, accessing the cancel IRQL above was
	// not safe.  ASSERT and then simply release the private spinlock.
		ASSERT(FALSE);
		Unlock();
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	CancelDispatch(
//		PDEVICE_OBJECT pDev,
//		PIRP pIrp )
//
//	Routine Description:
//
//		Static to dispatch to virtual member function.
//
//	Parameters:
//
//		pDev - pointer to device object
//		pIrp - IRP to be canceled.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		QueueIrp stores the queue address in the last entry (index 3)
//		of the DriverContext array of PVOIDs in the Overlay structure of
//		the IRP's Tail union.
//
VOID KDriverManagedQueueEx::CancelDispatch(	PDEVICE_OBJECT pDev, PIRP pIrp )
{
	KIrp I(pIrp);
	KDriverManagedQueueEx* pQ =
		(KDriverManagedQueueEx*)I->Tail.Overlay.DriverContext[3];

	pQ->Cancel(I);
	UNREFERENCED_PARAMETER(pDev);
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	CleanUp(
//		PFILE_OBJECT pFile )
//
//	Routine Description:
//
//		Completes with STATUS_CANCELLED any queued IRPs associated with the
//		specified file object.
//
//	Parameters:
//
//		pFile - Any IRP in the queue whose file object matches this parameter is
//			removed and canceled, or NULL to remove all IRPs
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		In WDM drivers that track outstanding requests, use PnpCleanUp instead
//		of this function.
//
//		This function walks the queue, checking the file object of each IRP.
//		Any IRP whose file object matches parameter FileObject is removed from
//		the queue and canceled.
//
//		If pFile is NULL, all IRPs in the queue are removed.
//
VOID KDriverManagedQueueEx::CleanUp( PFILE_OBJECT pFile )
{
	PDRIVER_CANCEL pLastCancel;
	KList<IRP> IrpsToCancel;

	// Acquire the private spinlock.
	Lock();

	// Get the first IRP in the queue
	KIrp I(HeadNoLock());
	KIrp INext;

	// Loop through the queue looking for specified file object
	while ( !I.IsNull() )
	{
		INext = NextNoLock(I);
		if ((pFile == NULL) ||
			( (I->Tail.Overlay.CurrentStackLocation != NULL) &&
			(I.FileObject() == pFile) ))
		{

	// Found a match

	// Make the IRP noncancelable and save the last cancel routine
	// set on the IRP.
			pLastCancel = I.SetCancelRoutine(NULL);

	// If we are queueing IRPs in a noncancelable state, or the cancel
	// routine has not been called (and we are queuing IRPs in a cancelable
	// state), we will clean up the IRP.  If the cancel routine has
	// been called, it will dispose of the IRP.  This is evidenced by
	// the last cancel routine being NULL.
			if ( !m_QueueCancelable || (pLastCancel != NULL) )
			{

	// If we set a cancel routine, it should be the one we set.
				ASSERT(!m_QueueCancelable || (pLastCancel == CancelDispatch));

	// Remove the IRP and place in our list to be canceled
				RemoveNoLock(I);
				IrpsToCancel.InsertTail(I);
			}
		}
		I = INext;
	}

	// Release the private spinlock.
	Unlock();

	// Cancel the IRPs in our list.  They are all already in a noncancelable state.
	I = IrpsToCancel.RemoveHead();
	while ( !I.IsNull() )
	{
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
		I = IrpsToCancel.RemoveHead();
	}
}

#if _WDM_

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	PnpCleanUp(
//		KPnpDevice* pDevice,
//		PFILE_OBJECT pFile )
//
//	Routine Description:
//
//		Completes with STATUS_CANCELLED any queued IRPs associated with the
//		specified file object. Use this rather than CleanUp in WDM drivers
//		whose PnP Policies are configured to track outstanding requests.
//
//	Parameters:
//
//		pDevice - Pointer to the device object to which the IRPs in the
//			queue were dispatched.
//
//		pFile - Any IRP in the queue whose file object matches this parameter
//			is removed and canceled, or NULL to remove all IRPs.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This function walks the queue, checking the file object of each IRP.
//		Any IRP whose file object matches parameter FileObject is removed
//		from the queue and canceled.
//
//		If pFile is NULL, all IRPs in the queue are removed.
//
VOID KDriverManagedQueueEx::PnpCleanUp( KPnpDevice* pDevice, PFILE_OBJECT pFile )
{
	PDRIVER_CANCEL pLastCancel;
	KList<IRP> IrpsToCancel;

	// Acquire the private spinlock.
	Lock();

	// Get the first IRP in the queue
	KIrp I(HeadNoLock());
	KIrp INext;

	// Loop through the queue looking for specified file object
	while ( !I.IsNull() )
	{
		INext = NextNoLock(I);
		if ((pFile == NULL) ||
			( (I->Tail.Overlay.CurrentStackLocation != NULL) &&
			(I.FileObject() == pFile) ))
		{

	// Found a match

	// Make the IRP noncancelable and save the last cancel routine
	// set on the IRP.
			pLastCancel = I.SetCancelRoutine(NULL);

	// If we are queueing IRPs in a noncancelable state, or the cancel
	// routine has not been called (and we are queuing IRPs in a cancelable
	// state), we will clean up the IRP.  If the cancel routine has
	// been called, it will dispose of the IRP.  This is evidenced by
	// the last cancel routine being NULL.
			if ( !m_QueueCancelable || (pLastCancel != NULL) )
			{

	// If we set a cancel routine, it should be the one we set.
				ASSERT(!m_QueueCancelable || (pLastCancel == CancelDispatch));

	// Remove the IRP and place in our list to be canceled
				RemoveNoLock(I);
				IrpsToCancel.InsertTail(I);
			}
		}
		I = INext;
	}

	// Release the private spinlock.
	Unlock();

	// Cancel the IRPs in our list.  They are all already in a noncancelable state.
	I = IrpsToCancel.RemoveHead();
	while ( !I.IsNull() )
	{
		I.Information() = 0;
		if ( m_IrpNotify == NULL )
		{
			I.PnpComplete(pDevice, STATUS_CANCELLED);
		}
		else
		{
			PDEVICE_OBJECT pSystemDevice = I.DeviceObject();
			I.Complete(STATUS_CANCELLED);
			m_IrpNotify(pSystemDevice);
		}
		I = IrpsToCancel.RemoveHead();
	}
}

#endif
#endif