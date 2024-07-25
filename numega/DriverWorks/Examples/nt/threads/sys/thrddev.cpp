// thrddev.cpp - thread example device class
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
#include <common.h>
#include "thrddev.h"

///////////////////////////////////////////////////////////////////////////////
// Static data

BOOLEAN bUnloading;		// true when driver is unloading

///////////////////////////////////////////////////////////////////////////////
// The device class implements a simple demonstration of system threads.
// The device constructor starts two threads which represent two processing
// stages. The thread functions are members of the device class. Each thread
// has a mailbox (implemented as a semaphore) on which it waits for work.
// Each thread also has a queue, implemented as list of IRPs. 
// 
// When a thread's mailbox is signaled, it takes an IRP from the head its queue.
// Stage 1 reversess the order of bytes in the IOCTL buffer. Stage 2 converts
// from upper case to lower case and completes the IRP.
//
///////////////////////////////////////////////////////////////////////////////
// Begin INIT section code
//
#pragma code_seg("INIT")

///////////////////////////////////////////////////////////////////////////////
// Constructor
//
// The constructor starts the threads.
//
ThreadDevice::ThreadDevice(void) :
	KDevice(L"Threads0", FILE_DEVICE_UNKNOWN, L"Threads0")
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
		return;

	m_Stage1.Start(LinkTo(Stage1), this);
	m_Stage2.Start(LinkTo(Stage2), this);
}
#pragma code_seg()

// End INIT section code
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ~ThreadDevice

ThreadDevice::~ThreadDevice(void)
{
	KIrp I = KIrp::Allocate(1);

	IoSetNextIrpStackLocation(I);
	I.MajorFunction() = IRP_MJ_CLEANUP;

	m_Stage1.Post(I);

	m_Stage1.Wait();
	m_Stage2.Wait();


	KIrp::Deallocate(I);
}

///////////////////////////////////////////////////////////////////////////////
// Create
//
// This is included to enable an app to open the device.
//
NTSTATUS ThreadDevice::Create(KIrp I)
{
	BOOLEAN UnloadState = bUnloading;
	KDriver::DriverInstance()->DisableUnload();

	if ( UnloadState )
		return I.Complete(STATUS_DELETE_PENDING);
	else
	{
		++m_OpenCounter;
		return DefaultDispatch(I);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Close
//
NTSTATUS ThreadDevice::Close(KIrp I)
{
	NTSTATUS status;

	status = DefaultDispatch(I);

	if (--m_OpenCounter == 0)
		KDriver::DriverInstance()->EnableUnload();

	return status;
}

///////////////////////////////////////////////////////////////////////////////
// DeviceControl
//
// DeviceControl accepts input in the form of a string. This routine passes
// the IRP to the first thread, Stage 1.
//
NTSTATUS ThreadDevice::DeviceControl(KIrp I)
{
	I.Information() = 0;

	switch (I.IoctlCode())
	{
	case IOCTL_TEST_THREAD:
		I.MarkPending();
		m_Stage1.Post(I);
		return STATUS_PENDING;

	default:
		return I.Complete(STATUS_INVALID_PARAMETER);
	}	
}

///////////////////////////////////////////////////////////////////////////////
// Stage1
//
// The first thread waits for a message, and when received reverses the
// order of the string in the IoctlBuffer. It then passes the IRP to the
// Stage2 thread.
//
VOID ThreadDevice::Stage1(void)
{
	PCHAR p;
	int len, i;
	CHAR x;

	while (TRUE)
	{
		m_Stage1.m_Mailbox.Wait();

		KIrp I = m_Stage1.m_Queue.RemoveHead();

		// if the MajorFunction is cleanup, the driver is unloading

		if ( I.MajorFunction() == IRP_MJ_CLEANUP )
		{
			m_Stage2.Post(I);
			m_Stage1.Terminate(STATUS_SUCCESS);
		}

		else
		{

			p = (PCHAR)I.IoctlBuffer();
			len = strlen(p);

			for (i=0; i < len/2; i++)
			{
				x = p[i];
				p[i] = p[len-1-i];
				p[len-1-i] = x;
			}

			m_Stage2.Post(I);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Stage2
// 
// When the Stage2 thread receives an IRP, it converts the string to lower
// case and then  completes it with STATUS_SUCCESS.
//
VOID ThreadDevice::Stage2(void)
{
	PCHAR p;

	while (TRUE)
	{
		m_Stage2.m_Mailbox.Wait();
		
		KIrp I = m_Stage2.m_Queue.RemoveHead();

		if ( I.MajorFunction() == IRP_MJ_CLEANUP )
			m_Stage2.Terminate(STATUS_SUCCESS);

		else
		{														   
			p = (PCHAR)I.IoctlBuffer();

			while (*p)
			{
				*p = CHAR(tolower(*p));
				p++;
			}

			I.Information() = strlen((PCHAR)I.IoctlBuffer())+1;
			I.Complete(STATUS_SUCCESS);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Constructor for TestThread
//
TestThread::TestThread(void) : m_Mailbox((LONG)0, MAX_IRP) {}

///////////////////////////////////////////////////////////////////////////////
// Post
//
// Posts an IRP to a TestThread. This simply adds the IRP to the queue
// and signals the semaphore that the thread waits on.
//
VOID TestThread::Post(KIrp I)
{
	m_Queue.InsertTail(I);
	m_Mailbox.Signal();
}

