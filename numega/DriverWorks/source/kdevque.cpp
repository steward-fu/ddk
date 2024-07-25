// kdevque.cpp - implementation for class KDeviceQueue
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

/////////////////////////////////////////////////////////////////////
VOID KDeviceQueue::CleanUp(PFILE_OBJECT pFile)
/**
KDeviceQueue::CleanUp
	Removes and cancels all IRPs in the queue for a specific file object.

Parameters
	FileObject	Any IRP in the queue whose file object matches this
				parameter is removed and cancelled.
Comments
	This function walks the queue, checking the file object of each IRP.
	Any IRP whose file object matches parameter FileObject is removed from
	the queue and canceled.
*/
{
	KIRQL PreviousIrql = _lock();

	KIrp I(Head());
	KIrp T(Tail());

	// bail out if the queue is empty
	if ( I.IsNull() )
	{
		_unlock(PreviousIrql);
		return;
	}

	// loop through the queue looking for specified file object
	do
	{
		if ((pFile == NULL) ||
		    ( (I->Tail.Overlay.CurrentStackLocation != NULL) &&
		      ( I.FileObject() == pFile )
		    )
		   )
		{
			_unlock(PreviousIrql);

	// found it; try to remove and cancel
			if (RemoveSpecificEntry(I))
			{
				CancelSpinLock::Acquire();
				I.SetCancelRoutine(NULL);
				CancelSpinLock::Release();
				I.Information() = 0;
				I.Complete(STATUS_CANCELLED);

			}
			PreviousIrql = _lock();

	// reset and start over (with IRP removed)
			I = Head();
			T = Tail();
			continue;
		}
		else
		{
			I = Next(I);
			if ((PIRP)I == (PIRP)Head())
				break;
		}
	}
	while ( !I.IsNull() );

	_unlock(PreviousIrql);
}

#if _WDM_

///////////////////////////////////////////////////////////////////////
VOID KDeviceQueue::PnpCleanUp(KPnpDevice* pDevice, PFILE_OBJECT pFile)
/**
KDeviceQueue::PnpCleanUp
	Same as CleanUp, but does PnpComplete

Parameters
	FileObject	Any IRP in the queue whose file object matches this
				parameter is removed and cancelled.
Comments
	This function walks the queue, checking the file object of each IRP.
	Any IRP whose file object matches parameter FileObject is removed from
	the queue and canceled.
*/
{
	KIRQL PreviousIrql = _lock();

	KIrp I(Head());
	KIrp T(Tail());

	// bail out if the queue is empty
	if ( I.IsNull() )
	{
		_unlock(PreviousIrql);
		return;
	}

	// loop through the queue looking for specified file object
	do
	{
		if ((pFile == NULL) ||
		    ( (I->Tail.Overlay.CurrentStackLocation != NULL) &&
		      ( I.FileObject() == pFile )
		    )
		   )
		{
			_unlock(PreviousIrql);

	// found it; try to remove and cancel
			if (RemoveSpecificEntry(I))
			{
				CancelSpinLock::Acquire();
				I.SetCancelRoutine(NULL);
				CancelSpinLock::Release();
				I.Information() = 0;
				I.PnpComplete(pDevice, STATUS_CANCELLED);
			}
			PreviousIrql = _lock();

	// reset and start over (with IRP removed)
			I = Head();
			T = Tail();
			continue;
		}
		else
		{
			I = Next(I);
			if ((PIRP)I == (PIRP)Head())
				break;
		}
	}
	while ( !I.IsNull() );

	_unlock(PreviousIrql);
}

#endif
