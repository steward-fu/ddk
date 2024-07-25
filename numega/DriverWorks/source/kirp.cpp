// kirp.cpp - implementation of class KIrp
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

//=============================================================================
BOOLEAN KIrp::TestAndSetCancelRoutine(
	PDRIVER_CANCEL OriginalCancelRoutine,
	PDRIVER_CANCEL NewCancelRoutine,
	PIRP& CurrentIrp
	)
/*
	Safely change the IRP's cancel routine, checking for cancellation.

Parameters
	OriginalCancelRoutine	Cancel routine previously set in call to QueueIrp
							or SetCancelRoutine. May be NULL.
	NewCancelRoutine		New cancel routine to be set in the IRP. The default
							is NULL, which makes the IRP not cancelable.
							See comments.
	CurrentIrp				Reference to IRP that is the 'current' IRP. See
							comments.

Returns
	Returns TRUE in the normal case, indicating that the IRP should be
	processed. Otherwise, returns FALSE indicating that the IRP was
	canceled and (in an obscure case) that processing may have already started.

Comments
	If OriginalCancelRoutine is not NULL, then NewCancelRoutine	must
	not equal OriginalCancelRoutine. This is necessary because it gives
	this routine a means to detect if it has already processed the
	IRP. The assumption/requirement is that OriginalCancelRoutine is
	used uniquely at a single stage in the IRP's processing, and never
	reused at a later stage.  The NewCancelRoutine can be either a successive
	cancel routine, or NULL which leaves the IRP in a non-cancelable state
	upon return.

	If the IRP was previously in a non-cancelable state (OriginalCancelRoutine==NULL),
	and the IRP's cancel bit is set, then this function returns FALSE, and the
	caller is responsible for completing the IRP with STATUS_CANCELLED.

	If the IRP was previously in a cancelable state (OriginalCancelRoutine != NULL),
	and the	IRP's cancel bit is found is the set state, then this function returns
	FALSE, but the caller should not complete the IRP, assuming the cancel routine
	unconditionally completes it. Drivers whose cancel routines do not always perform
	the completion must have a mechanism for managing that state information and
	completing the IRP when appropriate.


	Parameter CurrentIrp serves to validate that data member m_Irp is still
	pointing to an IRP owned by the driver after acquisition of the lock. It
	is possible that while waiting for the lock, the IRP was cancelled and
	the cancel routine completed the IRP making it no longer accessible to the
	caller and changed the value at CurrentIrp. Drivers	that serialize processing
	through KDevice::StartIo or KDriverManagedQueue::StartIo can simply use the
	return value of member function CurrentIrp() for this parameter. Drivers
	that do not serialize must invent some other mechanism, which is
	complicated only if there are multiple outstanding unserialized
	IRPs for a single device.

	If OriginalCancelRoutine is NULL, parameter CurrentIrp is ignored. The IRP
	referenced by the 'this' pointer is assumed valid in this case, since there
	is no cancel routine to complete it.

	If the function returns TRUE, then the NewCancelRoutine has been set in the IRP.

*/
{
	BOOLEAN CallerShouldProcessIrp=FALSE;
	KIRQL PreviousIrql;

	// Don't permit reuse of a cancel routine
	ASSERT ( (OriginalCancelRoutine == NULL) || (OriginalCancelRoutine != NewCancelRoutine) );

	// Lock it
	PreviousIrql = CancelSpinLock::Acquire();

	// If the IRP was not cancelable, i.e. OriginalCancelRoutine is NULL,
	// then it is safe to assume we can touch the IRP, because there is no
	// cancel routine that could have completed it already.
	if ( OriginalCancelRoutine == NULL )
		CallerShouldProcessIrp = ! m_Irp->Cancel;

	// Otherwise, the IRP was cancelable. Therefore, we must verify that
	// it is still processable. To do so, we compare to the caller provided
	// IRP reference to verify that the IRP is still current.
	else if ( m_Irp == CurrentIrp )
	{

		// At this point, we have determined that the input IRP pointer
		// was not invalidated while we waited to acquire the lock. Now
		// verify that the IRP is still in the same cancel state as we
		// expect.
		if ( m_Irp->CancelRoutine == OriginalCancelRoutine )
		{
			// We reach this point if the IRP is in the expected state. So we can
			// proceed with testing the cancel bit and setting the new cancel
			// routine. We might not reach here if the IRP was not in the expected
			// state. This could happen in a very obscure case, in which the IRP
			// gets completed and requeued to the same device while we waited to
			// acquire the lock. Effectively, this means that the caller should
			// not process the IRP, because another thread has already taken it
			//through this state transition.
			CallerShouldProcessIrp = ! m_Irp->Cancel;
		}
	}

	if ( CallerShouldProcessIrp )
		SetCancelRoutine(NewCancelRoutine);

	// Unlock it
	CancelSpinLock::Release(PreviousIrql);

	return CallerShouldProcessIrp;
}
