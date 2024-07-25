// writeq.cpp - implementation of class SerialWriteQueue
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
#include "serdev.h"
#include "glbtrace.h"

// The serial driver uses three driver managed queues: one for reads,
// one for write and flush, and one for waits. This class implements
// the write queue. The StartIo routine passes the request to the
// StartWrite member function of SerialDevice (or its subclass).

/////////////////////////////////////////////////////////////////////////////
// StartIo
//
VOID SerialWriteQueue::StartIo(KIrp I)
{
	GTRACE((TLEVEL,"WriteQueue::StartIo\n"));

	CancelSpinLock::Acquire();
	if (I.WasCanceled())
	{
		GTRACE((TLEVEL, "WriteQueue: Irp was canceled\n"));
		CancelSpinLock::Release();
		return;
	}
	else
	{
		I.SetCancelRoutine(NULL);
		CancelSpinLock::Release();
	}

	if (I.MajorFunction() == IRP_MJ_FLUSH_BUFFERS)
	{
		I.Information() = 0;
		I.Status() = STATUS_SUCCESS;
		NextIrp(I, IO_SERIAL_INCREMENT);
	}
	else
		m_Device->StartWrite(
			(PUCHAR)I.BufferedWriteSource(),
			I.WriteSize()
			);
}

/////////////////////////////////////////////////////////////////////////////
// Cancel
//
VOID SerialWriteQueue::Cancel(KIrp I)
{
	GTRACE((TLEVEL,"WriteQueue: Cancel IRP\n"));

	if ( (PIRP)I == (PIRP)m_CurrentIrp)
		m_Device->CancelCurrentWrite();

	KDriverManagedQueueEx::Cancel(I);
}	

/////////////////////////////////////////////////////////////////////////////
// CompleteCurrent
//
VOID SerialWriteQueue::CompleteCurrent(NTSTATUS status, ULONG nLeft)
{
	ASSERT ( (PIRP)m_CurrentIrp != NULL);

	m_CurrentIrp.Information() = m_CurrentIrp.WriteSize() - nLeft;
	m_CurrentIrp.Status() = status;

	NextIrp(m_CurrentIrp, IO_SERIAL_INCREMENT);
}
