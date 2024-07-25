// kdmqcsq.cpp - implementation of class KDriverManagedQueueCsq
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

#if ((NTDDI_VERSION >= 0x05000000) || (_WDM_ && (WDM_MAJORVERSION > 1 || ((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10)))))

///////////////////////////////////////////////////////////////////////////////
//
//	KDriverManagedQueueCsq(
//		BOOLEAN DispatchIrqlStartIo,
//		IRP_NOTIFICATION_FCN IrpCompleteNotificationFunction )
//
//	Routine Description:
//
//		Constructor for class KDriverManagedQueueCsq
//
//	Parameters:
//
//		DispatchIrqlStartIo - TRUE if the StartIo function should be called at
//			IRQL DISPATCH_LEVEL. FALSE if the StartIo function should be called
//			at IRQL < DISPATCH_LEVEL. Default is TRUE.
//
//		IrpCompleteNotificationFunction - Pointer to function that driver will call
//			after it completes an IRP.
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This is a subclass of KCancelSafeListBase<KCancelSafeList>. It is also an abstract class,
//		due to member StartIo.
//
KDriverManagedQueueCsq::KDriverManagedQueueCsq(
	BOOLEAN DispatchIrqlStartIo,
	IRP_NOTIFICATION_FCN IrpCompleteNotificationFunction
	) :
	m_CurrentIrp(0),
	m_bBusy(FALSE),
	m_DispatchIrqlStartIo(DispatchIrqlStartIo),
	m_CallStartIo(),
	m_IrpNotify(IrpCompleteNotificationFunction)
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	Initialize(
//		PDEVICE_OBJECT pDeviceObject )
//
//	Routine Description:
//
//		Initialize worker thread with device object
//
//	Parameters:
//
//		pDeviceObject - Pointer to the device object
//
//	Return Value:
//
//		None
//
//	Comments:
//
//		This function initialize this worker thread with device object. It adds
//		a reference to device object which ensures that this device object exists
//		while worker thread is still outstanding.
//
NTSTATUS KDriverManagedQueueCsq::Initialize(PDEVICE_OBJECT pDeviceObject)
{
	return m_CallStartIo.Initialize(pDeviceObject);
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
//		Based on the class settings (see KDriverManagedQueueCsq or
//		SetDispatchIrqlStartIo), IRQL will be raised to DISPATCH_LEVEL before
//		StartIo is called (the default behavior), or StartIo will be called at
//		IRQL < DISPATCH_LEVEL from an arbitrary thread context.
//
//		The default behavior for the class is to call StartIo at DISPATCH_LEVEL
//		and queue IRPs in a cancelable state.
//
NTSTATUS KDriverManagedQueueCsq::QueueIrp( KIrp I )
{
	// Check if the queue is busy
	if(m_bBusy)
	{
		// If the queue is busy, mark the IRP pending.
		// DDK:"Note that IoCsqXxx routines use the DriverContext[3] member of the IRP to hold IRP context information.
		// Drivers that use these routines to queue IRPs must leave that member unused."
		I.MarkPending();

		// Insert the IRP into the queue
		InsertIrp(I);
	}
	else
	{
		// If the queue is not busy, we will process the IRP with StartIo(),
		// so long as it isn't already canceled.

		// Making this IRP the current IRP, and mark the queue as busy.
		m_CurrentIrp = I;
		m_bBusy = TRUE;

		// Mark the IRP pending
		I.MarkPending();

		// Check to see if the StartIo routine should be called at
		// DISPATCH_LEVEL or not.
		if (m_DispatchIrqlStartIo)
		{
			StartIo(m_CurrentIrp);
		}
		else
		{
			// Check current IRQL.  If it is below DISPATCH_LEVEL, call
			// the StartIo routine.  Otherwise schedule a work queue item
			// to call StartIo, since we need to call the user at lower IRQL.
			if (KeGetCurrentIrql() < DISPATCH_LEVEL)
			{
				StartIo(m_CurrentIrp);
			}
			else
			{
				m_CallStartIo.Queue(LinkTo(CallStartIo), this);
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
//		Based on the class settings (see KDriverManagedQueueCsq or
//		SetDispatchIrqlStartIo), IRQL will be raised to DISPATCH_LEVEL before
//		StartIo is called (the default behavior), or StartIo will be called at
//		IRQL < DISPATCH_LEVEL from an arbitrary thread context.
//
VOID KDriverManagedQueueCsq::NextIrp( KIrp I, CCHAR boost)
{
	// A sanity check
	ASSERT(m_bBusy == TRUE);

	// Dequeue the next IRP into m_CurrentIrp
	m_CurrentIrp = RemoveNextIrp();

	// Check if the IRP is NULL
	if (m_CurrentIrp.IsNull())
	{
		// If so, declare the queue not busy
		m_bBusy = FALSE;
	}
	else
	{
		// If not, start to process it.

		// Check to see if the StartIo routine should be called at
		// DISPATCH_LEVEL or not.
		if (m_DispatchIrqlStartIo)
		{
			StartIo(m_CurrentIrp);
		}
		else
		{
			// Check current IRQL.  If it is below DISPATCH_LEVEL, call
			// the StartIo routine.  Otherwise schedule a work queue item
			// to call StartIo, since we need to call the user at lower IRQL.
			if (KeGetCurrentIrql() < DISPATCH_LEVEL)
			{
				StartIo(m_CurrentIrp);
			}
			else
			{
				m_CallStartIo.Queue(LinkTo(CallStartIo), this);
			}
		}
	}


	// Complete the supplied IRP
	if ( !I.IsNull() )
	{
		I.Complete(I.Status(), boost);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	VOID
//	CallStartIo()
//
//	Routine Description:
//
//		Calls StartIo from the context of a worker thread.
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
VOID KDriverManagedQueueCsq::CallStartIo()
{
	// Call StartIo
	StartIo(m_CurrentIrp);
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
VOID KDriverManagedQueueCsq::Cancel( KIrp I )
{
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
	else if(RemoveIrp((PIO_CSQ_IRP_CONTEXT)I->Tail.Overlay.DriverContext[3]))
	{
		// If so, setup the information and status fields in the IRP and complete it.
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
		// not safe.
		ASSERT(FALSE);
	}
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
VOID KDriverManagedQueueCsq::CleanUp( PFILE_OBJECT pFile )
{
	KIrp I;
    while(I = RemoveNextIrp(pFile))
    {
        // Cancel the IRP
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
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
VOID KDriverManagedQueueCsq::PnpCleanUp( KPnpDevice* pDevice, PFILE_OBJECT pFile )
{
	KIrp I;
    while(I = RemoveNextIrp(pFile))
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
    }
}

#endif
#endif // NTDDI_VERSION
