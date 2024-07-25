// readq.cpp - implementation of read queue class for serial driver
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

// The serial driver uses three driver managed queues: one for reads,
// one for write and flush, and one for waits. This class implements
// the read queue. The StartIo routine passes the request to the
// StartRead member function of SerialDevice.

#include <vdw.h>
#include "serdev.h"
#include "glbtrace.h"


/////////////////////////////////////////////////////////////////////////////
// MaybeQueueIrp
//
// In order to adequately emulate the system version of the serial driver,
// certain reads may not be queued. Instead, they must be completed immediately,
// and return the current state of the receiver buffer. SerialDevice::NoPendRead
// handles this function.

NTSTATUS SerialReadQueue::MaybeQueueIrp(KIrp I)
{
	if ( IsEmpty() && (m_CurrentIrp==NULL) )
	{
		NTSTATUS status;

		status = m_Device->NoPendRead(I);

		if ( status != STATUS_MORE_PROCESSING_REQUIRED )
			return status;
	}

	return QueueIrp(I);
}


/////////////////////////////////////////////////////////////////////////////
// StartIo
//
VOID SerialReadQueue::StartIo(KIrp I)
{
//	GTRACE((TLEVEL, "ReadQueue::StartIo\n"));

	CancelSpinLock::Acquire();
	if (I.WasCanceled())
	{
		GTRACE((TLEVEL, "ReadQueue: Irp was canceled\n"));
		CancelSpinLock::Release();
		return;
	}
	else
	{
		I.SetCancelRoutine(NULL);
		CancelSpinLock::Release();
	}

// Pass the request to the device

	m_Device->StartRead(
		(PUCHAR)I.BufferedReadDest(),
		I.ReadSize()
		);
}

/////////////////////////////////////////////////////////////////////////////
// Cancel
//
VOID SerialReadQueue::Cancel(KIrp I)
{
	if ( (PIRP)I == (PIRP)m_CurrentIrp )
		m_Device->CancelCurrentRead();

	KDriverManagedQueueEx::Cancel(I);
}	

/////////////////////////////////////////////////////////////////////////////
// CompleteCurrent
//
// Complete the current request.
//
// Input:
//	status	 	The completion status: success, timeout, or canceled
//	nCharsLeft	Number of characters in the original request that were not read
//
VOID SerialReadQueue::CompleteCurrent(NTSTATUS status, ULONG nCharsLeft)
{
	if ( (PIRP)m_CurrentIrp == NULL ) 
		return;

// If not pending, set the count of bytes read 

	if ( ((status == STATUS_SUCCESS) || (status == STATUS_TIMEOUT)) && (nCharsLeft != COMPLETE_ZERO) )
		m_CurrentIrp.Information() = m_CurrentIrp.ReadSize() - nCharsLeft;
	else
		m_CurrentIrp.Information() = 0;
		
	m_CurrentIrp.Status() = status;

	NextIrp(m_CurrentIrp, IO_SERIAL_INCREMENT);
}

/////////////////////////////////////////////////////////////////////////////
// CancelCurrent
//
VOID SerialReadQueue::CancelCurrent(void)
{
	if (m_CurrentIrp.m_Irp != NULL)
		Cancel(m_CurrentIrp);
	else
		CancelSpinLock::Release();
}
