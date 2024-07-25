// serrdwr.cpp - read/write support for serial driver
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

// Class SerialDevice handles most of the setup for reading and
// writing to or from the serial device.

// For read operations, the requirements of the subclass are as follows:
//
// 1. It must override member SynchronizeReceiverAccess. This member
//    function is used to serialize access to state information that 
//    controls a read operation. This may be implemented with a spin
//    lock or via synchronization with an interrupt object.
//
// 2. When a character is received, the subclass must check 
//    m_ReadBuffer for non-zero to see if a read is in progress. If
//    so, it must store the character in the buffer and decrement
//    m_ReadCount.
//
// 3. When a read is complete, the subclass must request the 
//    read complete DPC, m_ReadCompleteDpc, passing to it the
//    status (STATUS_SUCCESS) and the count of bytes remaining
//    (0).
// 
// 4. If the subclass provides buffering of data received while no
//    read is in progress, it must override member CopyReceivedData.
//
// For write operations, the requirements of the subclass are as follows:
//
// 1. It must override member SynchronizeTransmitterAccess. This member 
//    is used to serialize access to state information that controls
//    a write operation.
//
// 2. It must override member StartWrite, which initiates a write
//    operation. The base class member, which the override may
//    invoke, sets up m_WriteBuffer and m_WriteCount.
//
// 3. It must test m_WriteBuffer for zero to check if the write has
//    timed out or has been canceled.
//
// 4. When a write is complete, it must set m_WriteBuffer to zero, and
//    request the write complete DPC, m_WriteCompleteDpc.

#include <vdw.h>
#include "serdev.h"
#include "glbtrace.h"

///////////////// Functions for Read ////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
struct _read_start
{
	SerialDevice* device;
	PUCHAR buffer;
	ULONG count;
};

/////////////////////////////////////////////////////////////////////////////
// Start a read operation
//
// This is called from the SerialReadQueue::StartIo.
//
// Input:
//	buffer		Buffer to receive data
//	count		Maximum number of bytes to read
//
// This routine uses the receiver synchronization member functon of the
// subclass to invoke SynchReadStart.
//
VOID SerialDevice::StartRead(PUCHAR buffer, ULONG count)
{
// Call SynchReadStart, allowing subclasses to apply whatever
// synchronization is required.

	struct _read_start rs;

//	GTRACE( (TLEVEL,"Start Read, count=%d\n", count));

	rs.device = this;
	rs.buffer = buffer;
	rs.count  = count;

	SynchronizeReceiverAccess(LinkTo(SynchReadStart), &rs);
}

/////////////////////////////////////////////////////////////////////////////
// SynchReadStart
//
// Set up the state information to handle the read operation.
// This is called via SynchronizeReceiverAccess.
//
BOOLEAN SerialDevice::SynchReadStart(PUCHAR buffer, ULONG count)
{
	ASSERT (m_ReadBuffer == NULL);

	// See if there is any received data in the buffer.

	ULONG nReceived = CopyReceivedData(buffer, count);

	// Adjust count and buffer for received data.

	count -= nReceived;
	buffer += nReceived;

	// Special semantics when IntervalTimeout == MAXULONG

	if (m_Timeouts.ReadIntervalTimeout == MAXULONG)
	{

	// if both multiplier and constant are zero, return immediately,
	// even if there are no characters available

		if 	( (m_Timeouts.ReadTotalTimeoutMultiplier == 0) &&
		      (m_Timeouts.ReadTotalTimeoutConstant == 0) )
		{
			m_ReadCompleteDpc.Request((PVOID)STATUS_SUCCESS, (PVOID)count);
			return TRUE;
		}

	// otherwise, if neither multipler nor constant is MAXULONG, then
	// return immediately only if at least one character is available

		else if ( (m_Timeouts.ReadTotalTimeoutMultiplier != MAXULONG) &&
		          (m_Timeouts.ReadTotalTimeoutConstant   != MAXULONG) &&
				  (nReceived != 0) )
		{
			m_ReadCompleteDpc.Request((PVOID)STATUS_SUCCESS, (PVOID)count);
			return TRUE;
		}
	}

	// If no more data is needed to satisfy the request, complete it

	if (count == 0)
	{
		m_ReadCompleteDpc.Request((PVOID)STATUS_SUCCESS, 0);
		return TRUE;
	}

	// Otherwise, set up the read state

	else
	{
		m_ReadBuffer = buffer;
		m_ReadCount = count;

	// Queue the DPC to handle total read and interval read timeouts
	// if the m_Timeouts requires doing so.

		if ( ((m_Timeouts.ReadTotalTimeoutMultiplier != 0) ||
		      (m_Timeouts.ReadTotalTimeoutConstant != 0))
							    ||
		     ((m_Timeouts.ReadIntervalTimeout != 0) &&
		      (m_Timeouts.ReadIntervalTimeout != MAXULONG))
		   )

			m_StartReadTimerDpc.Request( (PVOID)m_ReadCount);
					
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ReadComplete
//
// Finish a write operation (this is a DPC)
//
// arg1 is status
// arg2 is number of bytes that remained to be read when stopped
//
// The subclass must queue data member m_ReadCompleteDpc when a
// read completes. The constructor for SerialDevice initializes
// the DPC to this member.
//
VOID SerialDevice::ReadComplete(PVOID arg1, PVOID arg2)
{
	NTSTATUS status = (NTSTATUS)arg1;

	// Cancel a total read timeout if one is still active

	if ((status != STATUS_TIMEOUT) && (m_ReadTimeout != 0))
	{
		m_ReadTimeout = 0;
		m_ReadExpiredCallback.Cancel();
	}

	m_ReadIrpQueue.CompleteCurrent(status, (ULONG)arg2);
}

/////////////////////////////////////////////////////////////////////////////
// CancelCurrentRead
//
// Cancel the current read operation. Synch up to SynchCancelRead 
// using receiver synchronization method. This is called from
// SerialReadQueue::Cancel.
//
VOID SerialDevice::CancelCurrentRead(void)
{
	SynchronizeReceiverAccess(LinkTo(SynchCancelRead), this);
}

/////////////////////////////////////////////////////////////////////////////
// SynchCancelRead
//
// Kill the current read. This is used by the timeout handlers and
// the cancel routines. This is called via SynchronizeReceiverAccess.
//
BOOLEAN SerialDevice::SynchCancelRead(void)
{
	BOOLEAN status = (m_ReadBuffer != 0);

	m_FinalReadCount = m_ReadCount;
	m_ReadBuffer = 0;
	m_ReadCount = 0;
	m_ReadCountLastInterval = 0;

	return status;
}

/////////////////////////////////////////////////////////////////////////////
// StartReadTimer
//
// Start timing a read. This is a DPC. arg1 is the character count.
//
VOID SerialDevice::StartReadTimer(PVOID arg1, PVOID arg2)
{
	LONGLONG Count = (LONGLONG)(LONG)arg1;
	LARGE_INTEGER l;

// Set up the timer for the total read. The total interval is 
// computed by adding the constant to the product of the
// multiplier and the character count.

	if (m_Timeouts.ReadTotalTimeoutMultiplier || 
	    m_Timeouts.ReadTotalTimeoutConstant)
	{
		m_ReadTimeout =
			m_Timeouts.ReadTotalTimeoutConstant + 
			(Count*m_Timeouts.ReadTotalTimeoutMultiplier);

		l.QuadPart = 10000 * (ULONGLONG)(-m_ReadTimeout);

	// Set up a callback to ReadExpired 

		m_ReadExpiredCallback.Set(l, LinkTo(ReadExpired), this);
	}

// Set up the interval timer. 

	if ((m_Timeouts.ReadIntervalTimeout != 0) &&
	    (m_Timeouts.ReadIntervalTimeout != MAXULONG))

	{
		LARGE_INTEGER Large;
		KeQuerySystemTime(&Large);

		m_ReadCountLastInterval = m_ReadCount;
		m_TimeLastInterval = Large.QuadPart;

		Large.QuadPart = m_IntervalTimerPeriod;

		m_ReadIntervalCallback.Set(
			Large,
			LinkTo(ReadIntervalExpired),
			this
			);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ReadExpired
//
// Handle read total timeout. This is a DPC.
//
VOID SerialDevice::ReadExpired(PVOID arg1, PVOID arg2)
{
	GTRACE((TLEVEL,"Total read timeout\n"));
//
// Stop the current read. If it was still in progress, complete it
// 
	if (SynchronizeReceiverAccess(LinkTo(SynchCancelRead), this))
		ReadComplete( (PVOID)STATUS_TIMEOUT, (PVOID)m_FinalReadCount );
}

/////////////////////////////////////////////////////////////////////////////
// ReadIntervalExpired
//
// Handle read interval timeout
//
VOID SerialDevice::ReadIntervalExpired(PVOID arg1, PVOID arg2)
{
	if (SynchronizeReceiverAccess(LinkTo(SynchReadIntervalExpired), this))
	{
		LARGE_INTEGER Large; 
		Large.QuadPart = m_IntervalTimerPeriod;
		m_ReadIntervalCallback.Set(
			Large,
			LinkTo(ReadIntervalExpired),
			this
			);
	}
}

/////////////////////////////////////////////////////////////////////////////
// SynchReadIntervalExpired
//
// Synchronized handling of interval timer
//
BOOLEAN SerialDevice::SynchReadIntervalExpired(void)
{
// if the read is done, there is nothing to do

	if (m_ReadCount == 0)
		return FALSE;

	LARGE_INTEGER Now;
	KeQuerySystemTime(&Now);

// if no characters have arrived since the last call, then check the
// elapsed time

	if ( (m_ReadCount == m_ReadCountLastInterval) && (m_ReadCount != 0) )
	{
		LONGLONG Elapsed = Now.QuadPart - m_TimeLastInterval;
		if (Elapsed > m_MaxCharacterInterval)
		{
			m_ReadCompleteDpc.Request((PVOID)STATUS_TIMEOUT, (PVOID)m_ReadCount);
			m_ReadCount = 0;
			m_ReadBuffer = 0;
			m_ReadCountLastInterval = 0;
			return FALSE;
		}
	}

	m_ReadCountLastInterval = m_ReadCount;
	m_TimeLastInterval = Now.QuadPart;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Static linkage members
//
// These static members pass control between SynchronizeReceiverAccess
// and various non-static members.
//
BOOLEAN SerialDevice::LinkTo(SynchCancelRead)(PVOID Context)
{
	return ((SerialDevice*)Context)->SynchCancelRead();
}

BOOLEAN SerialDevice::LinkTo(SynchReadIntervalExpired)(PVOID Context)
{
	return ((SerialDevice*)Context)->SynchReadIntervalExpired();
}

BOOLEAN SerialDevice::LinkTo(SynchReadStart)(PVOID Context)
{
	_read_start* prs = (_read_start*)Context;

	return prs->device->SynchReadStart(prs->buffer, prs->count);
}

/////////////////////////////////////////////////////////////////////////////
// Synchronize access to receiver data
//
// Most subclasses will override this member function. The function is 
// designed to be compatible with subclasses that need interrupt level
// synchronization, although a subclass could equally well implement
// this function by acquiring a spin lock, making the call, and releasing
// the spin lock. The base class just calls the function.

BOOLEAN SerialDevice::SynchronizeReceiverAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		)
{
	return func(context);
}

/////////////////////////////////////////////////////////////////////////////
// CopyReceivedData
//
// Copy any unsolicited data which has been buffered while no read was
// pending. The base class has none. Most subclasses will override
// this member.
//
ULONG SerialDevice::CopyReceivedData(PUCHAR buffer, ULONG count)
{
	return 0;
}


struct CopyReceivedDataArgs
{
	SerialDevice* device;
	PUCHAR buf;
	ULONG count;
};


/////////////////////////////////////////////////////////////////////////////
// NoPendRead
//
// Non-blocking read for special RAS support
//
NTSTATUS SerialDevice::NoPendRead(KIrp I)
{
	CopyReceivedDataArgs crsargs;
	crsargs.device = this;
	crsargs.buf = (PUCHAR)I.BufferedReadDest();
	crsargs.count = I.ReadSize();

	SynchronizeReceiverAccess(LinkTo(CopyReceivedData), &crsargs);

	if ( ((m_Timeouts.ReadIntervalTimeout == MAXULONG) &&
	      (m_Timeouts.ReadTotalTimeoutMultiplier == 0) &&
	      (m_Timeouts.ReadTotalTimeoutConstant == 0) 
		 ) || (crsargs.count > 0)
	   )
	{
		I.Information() = crsargs.count;
		I.Complete(STATUS_SUCCESS);
		return STATUS_SUCCESS;
	}
	else 
		return STATUS_MORE_PROCESSING_REQUIRED;
}

///////////////// Functions for Write //////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CancelCurrentWrite
//
// Cancel the current write operation
//
VOID SerialDevice::CancelCurrentWrite(void)
{
	SynchronizeTransmitterAccess(LinkTo(SynchCancelWrite), this);
}

/////////////////////////////////////////////////////////////////////////////
// SynchCancelWrite
//
// Cancel the current write operation
//
BOOLEAN SerialDevice::SynchCancelWrite(void)
{
	BOOLEAN status = (m_WriteBuffer != 0);

	m_FinalWriteCount = m_WriteCount;
	m_WriteBuffer = 0;
	m_WriteCount = 0;

	return status;
}

/////////////////////////////////////////////////////////////////////////////
// Start a write operation
//
// Subclasses should override this member if additional functionality
// is required, as will most often be the case. 
//
VOID SerialDevice::StartWrite(PUCHAR buffer, ULONG count)
{
	ASSERT (m_WriteBuffer == NULL);

// Handle a write of zero characters

	if (count == 0)
	{
		m_WriteIrpQueue.CompleteCurrent(STATUS_SUCCESS, 0);
		return;
	}

	GTRACE( (TLEVEL, "Start write: |%s|\n", buffer));


// Set up write state data

	m_WriteBuffer = buffer;
	m_WriteCount = count;

// If the write has a timeout, start it

	if (m_Timeouts.WriteTotalTimeoutMultiplier ||
	    m_Timeouts.WriteTotalTimeoutConstant )

		m_StartWriteTimerDpc.Request((PVOID)count);
}

/////////////////////////////////////////////////////////////////////////////
// WriteComplete
//
// Finish a write operation (this is a DPC). Status is arg1.
// The subclass must request DPC m_WriteCompleteDpc when it
// detects that a write is complete. The constructor for
// SerialDevice sets up the DPC to call this function.
//
VOID SerialDevice::WriteComplete(PVOID arg1, PVOID arg2)
{
	NTSTATUS status = (NTSTATUS)arg1;

// Cancel a write timeout if one is active	

	if ((status != STATUS_TIMEOUT) && (m_WriteTimeout != 0))
	{
		m_WriteTimeout = 0;
		m_WriteExpiredCallback.Cancel();
	}

	m_WriteIrpQueue.CompleteCurrent(status, m_FinalWriteCount);
}

/////////////////////////////////////////////////////////////////////////////
// StartWriteTimer
//
// Start timing a write. This is a DPC. arg1 is the character count.
//
VOID SerialDevice::StartWriteTimer(PVOID arg1, PVOID arg2)
{
	LONGLONG Count = (LONGLONG)(LONG)arg1;
	LARGE_INTEGER l;

	m_WriteTimeout =
		m_Timeouts.WriteTotalTimeoutConstant + 
		(Count*m_Timeouts.WriteTotalTimeoutMultiplier);

	l.QuadPart = 10000 * (ULONGLONG)(-m_WriteTimeout);

	m_WriteExpiredCallback.Set(l, LinkTo(WriteExpired), this);
}

/////////////////////////////////////////////////////////////////////////////
// WriteExpired
//
// Handle Write timeout
//
VOID SerialDevice::WriteExpired(PVOID arg1, PVOID arg2)
{
	if (SynchronizeTransmitterAccess(LinkTo(SynchCancelWrite), this))
		WriteComplete( (PVOID)STATUS_TIMEOUT, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// Synchronize transmitter access
//
// Most subclasses will override this member function. The function is 
// designed to be compatible with subclasses that need interrupt level
// synchronization, although a subclass could equally well implement
// this function by acquiring a spin lock, making the call, and releasing
// the spin lock. The base class just calls the function.

BOOLEAN SerialDevice::SynchronizeTransmitterAccess(
		PKSYNCHRONIZE_ROUTINE func,
		PVOID context
		)
{
	return func(context);
}

/////////////////////////////////////////////////////////////////////////////
// Static linkage members
//
// These static members pass control between SynchronizeReceiverAccess
// and various non-static members.
//
BOOLEAN SerialDevice::LinkTo(SynchCancelWrite)(PVOID Context)
{
	return ((SerialDevice*)Context)->SynchCancelWrite();
}

BOOLEAN SerialDevice::LinkTo(CopyReceivedData)(PVOID Context)
{
	CopyReceivedDataArgs* pcrs = (CopyReceivedDataArgs*)Context;
	
	pcrs->count = pcrs->device->CopyReceivedData(pcrs->buf, pcrs->count);

	return TRUE;
}


