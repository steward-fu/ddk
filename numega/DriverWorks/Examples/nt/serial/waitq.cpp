// waitq.cpp - implementation of class SerialWaitQueue
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

// Class SerialWaitQueue is derived from KDriverManagedQueueEx.
// It is used to serialize processing of IOCTLs related to
// event waits.

// Note that when there is a active wait-on-mask operation, the
// IRP for that wait is NOT the current IRP of this queue. The
// interface supports changing the wait mask while a wait is
// progress, and a wait mask of zero terminates a wait.

#include <vdw.h>
#include "serdev.h"
#include "glbtrace.h"

/////////////////////////////////////////////////////////////////////////////
// StartIo
//
// Process next IRP in wait queue.
//
VOID SerialWaitQueue::StartIo(KIrp I)
{
	GTRACE((TLEVEL,"WaitQueue::StartIo I=%p\n", I.m_Irp));

	ULONG Op = I.IoctlCode();

	CancelSpinLock::Acquire();
	if (I.WasCanceled())
	{
		GTRACE((TLEVEL,"WaitQueue: Irp was canceled\n"));
		CancelSpinLock::Release();
		return;
	}
	else
	{
		switch (Op)
		{
		case IOCTL_SERIAL_SET_WAIT_MASK:
		{
			ULONG Mask = *(ULONG*)I.IoctlBuffer();

			I.SetCancelRoutine(NULL);
			CancelSpinLock::Release();

			m_Device->StartSetMask(Mask);
			I.Status() = STATUS_SUCCESS;
			I.Information() = 0;

			NextIrp(I);
			break;
		}
		case IOCTL_SERIAL_WAIT_ON_MASK:
			m_Device->StartWaitMask();
			break;

		default:
			ASSERT(FALSE);
			I.SetCancelRoutine(NULL);
			CancelSpinLock::Release();
			I.Information()=0;
			I.Status() = STATUS_INVALID_PARAMETER;
			NextIrp(I);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CompleteCurrent
//
// Complete the current IRP with the given status
//
VOID SerialWaitQueue::CompleteCurrent(NTSTATUS status)
{
#if DBG && ! defined(DISABLE_TRACE)
	if(!m_CurrentIrp.IsNull())
		GTRACE((TLEVEL,"WaitCompleteCurrent enter: %p status=%x\n",m_CurrentIrp.m_Irp, status));
#endif

	if (status == STATUS_PENDING)
		NextIrp( KIrp(0) );		// new waiter
	else
	{
		ASSERT (status != STATUS_SUCCESS);

		m_CurrentIrp.Information() = 0;
		m_CurrentIrp.Status() = status;

		NextIrp(m_CurrentIrp);
	}

#if DBG && !defined(DISABLE_TRACE)
	if(!m_CurrentIrp.IsNull())
		GTRACE((TLEVEL,"WaitCompleteCurrent leave: %p\n",m_CurrentIrp.m_Irp));
#endif
}



//////////////////////////////////////////////////////////////////////
// Cancel
//
VOID SerialWaitQueue::Cancel(KIrp I)
{
GTRACE((TLEVEL,"WaitCancel enter: %p \n",m_CurrentIrp.m_Irp));
	GTRACE( (TLEVEL, "WaitQueue: Canceling: %p\n",I.m_Irp));
	KDriverManagedQueueEx::Cancel(I);
GTRACE((TLEVEL,"WaitCancel leave: %p\n",m_CurrentIrp.m_Irp));
}
