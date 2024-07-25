// device.cpp - COMM filter device class 
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
#include <kfilter.cpp>

#include "common.h"
#include "commfifo.h"
#include "control.h"
#include "device.h"

// The function of the comm filter device is to intercept read requests 
// to the comm device, pass them through to the comm device, and
// catch the return data in an IRP completion routine. The IRP completion
// routine is a virtual member function of KFilterDevice, and it simply
// writes the data to a fifo, a pointer to which is passed into the 
// constructor.

// The device is complicated by the need to prevent deconstruction while there
// are IRPs pending completion. The device tracks the number of outstanding 
// IRPs in data member m_PendingIrpCount. Clients call member function Terminate
// to instruct the device to stop filtering and destroy itself as soon as
// possible.

/////////////////////////////////////////////////////////
// Constructor
//
CommFilterDevice::CommFilterDevice( WCHAR * CommName, 
				    CommDataFifo* pFifo,
				    FilterControlDevice * ControlDevice ) :

	KFilterDevice(
		CommName,
		FILE_DEVICE_SERIAL_PORT,
		DO_BUFFERED_IO),

	m_pFifo(pFifo),
	m_PendingIrpCount(0),
	m_TerminationRequested(FALSE),
	m_Open(0)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )  {
#if DBG
		DbgPrint("FILTER: Failed to create device: %x\n",
			 m_ConstructorStatus);
#endif
		return;
	}
	m_pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	m_ControlDevice = ControlDevice;
}


/////////////////////////////////////////////////////////
// ProcessRequest
//
// The handler must check to determine if the device has been asked to 
// terminate. The test is done under lock. If termination has been requested, 
// the IRP passes through without setting a completion notification routine 
// (2nd parameter of member function PassThrough). If there are no outstanding
// IRPs, the device deletes itself.
//
// In the normal case, i.e., not pending termination, completion notification 
// is requested, and the pending IRP count is incremented.
//
NTSTATUS CommFilterDevice::ProcessRequest(KIrp I)
{
	NTSTATUS status;
	ULONG_PTR	seq;

	m_Lock.Lock();
	seq = m_Sequence++;

	if ( m_TerminationRequested )
	{
		m_Lock.Unlock();

		if ( I.MajorFunction() == IRP_MJ_CLOSE )
			--m_Open;

		status = PassThrough( I, FALSE, NULL );

		m_Lock.Lock();
		if (m_PendingIrpCount == 0  &&  m_Open == 0)
		{
			m_Lock.Unlock();
			m_ControlDevice->TerminateCallback(this);
			delete this;
		}
		else
		{
			m_Lock.Unlock();
		}
	}
	else
	{
		m_PendingIrpCount++;
		m_Lock.Unlock();
		status = PassThrough( I, TRUE, (PVOID)seq );
	}
	return status;
}


////////////////////////////////////////////////////////////////////////////
// Rot13
//
// Apply a "rotate 13" transformation to a buffer.  It rotates all
// alpha characters in the buffer by 13 ascii positions.  This is
// performed simply as a demonstration that data being filtered
// can be modified.
//
void Rot13( CHAR * buf, ULONG len )
{
	for ( ; len; len--, buf++ ) {
		CHAR ch = *buf;
		if ( ch >= 'a'  &&  ch <= 'z' ) {
			if ( ch > 'm' )	*buf = ch - 13;
			else		*buf = ch + 13;
		} else if ( ch >= 'A'  &&  ch <= 'Z' ) {
			if ( ch > 'M' )	*buf = ch - 13;
			else		*buf = ch + 13;
		}
	}
}


////////////////////////////////////////////////////////////////////////////
// Filtered dispatch routines.
//
// These functions are invoked in place of the comm port's regular dispatch
// functions.  We simply call ProcessRequest, which sets up a completion
// for the request, and then invokes the original comm port dispatch function.
//
// For the Write request, we optionally tweak the data before passing
// it on to the comm port driver.
//
NTSTATUS CommFilterDevice::Read(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::Create(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::Close(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::Write(KIrp I)
{
	if ( m_ControlDevice->ModifyData() ) {
		// Our modification is to perform a rotate 13 on text:
		Rot13( (CHAR *)I.BufferedWriteSource(), I.WriteSize() );
	}
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::QueryInformation(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::SetInformation(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::FlushBuffers(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::DeviceControl(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::Cleanup(KIrp I)
{
	return ProcessRequest(I);
}

NTSTATUS CommFilterDevice::Unload(KIrp I)
{
	return ProcessRequest(I);
}


////////////////////////////////////////////////////////////////////////////
// Irp Complete handler
//
// This is an override of the virtual member function of KFilterDevice. It is
// called by the I/O manager when the comm driver completes a request.
//
// After locking, the routine decrements the pending IRP count. If termination
// has been requested, the function does not write to the fifo. In the normal 
// case, i.e. termination has not been requested, this function writes the 
// comm data to the fifo. In both cases, the function calls the base class 
// handler for proper handling of the IRP.
//
// The function keeps track of the number of times the underlying device
// has been successfully opened and closed.  If we detach while the
// device is open, we can crash the system.
//
// If the operation is a Read, we optionally tweak the data being read.
//
NTSTATUS CommFilterDevice::OnIrpComplete(KIrp I, PVOID Context)
{
	m_Lock.Lock();

	m_PendingIrpCount--;

	// Keep track of whether the underlying device is in use.
	if ( NT_SUCCESS(I.Status()) ) {
		switch ( I.MajorFunction() )  {
			case IRP_MJ_CREATE:
				++m_Open;
				break;
			case IRP_MJ_CLOSE:
				--m_Open;
				break;
		}		
	}

	if (m_TerminationRequested)
	{
			m_Lock.Unlock();
	}
	else
	{
// Normal case. Write the comm data to the fifo.
		RECORD rec;

		m_Lock.Unlock();

		rec.sequence	= (ULONG_PTR) Context;
		rec.function	= I.MajorFunction();
		rec.status	= I.Status();
		rec.information	= (ULONG)I.Information();

		m_pFifo->ControlWrite( &rec, 1 );

		// Modify data if requested.
		if ( I.MajorFunction() == IRP_MJ_READ	&&
		     NT_SUCCESS( I.Status() )		&&
		     m_ControlDevice->ModifyData() ) 
		{
			// Our modification is to perform a rotate 13 on text:
			Rot13( (CHAR *)I.BufferedReadDest(), (ULONG)I.Information() );
		}
	}

// Let the base class handle the IRP and return the appropriate status.

	return KFilterDevice::OnIrpComplete(I, Context);
}

////////////////////////////////////////////////////////////////////////////
// Terminate
//
// Client calls this to tell the device to stop filtering. After this call,
// no more data is written to the fifo. If there are no pending IRPs, and
// the device is not open, the routine destroys this. Otherwise, it sets 
// the "termination requested" flag to disable the filter.
// 

VOID CommFilterDevice::Terminate(void)
{
	m_Lock.Lock();

	if (m_Open == 0  &&  m_PendingIrpCount == 0)
	{
		m_Lock.Unlock();
		m_ControlDevice->TerminateCallback(this);
		delete this;
	}
	else
	{
		m_TerminationRequested = TRUE;
		m_Lock.Unlock();
	}
}
