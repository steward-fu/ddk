// Isoc1394Device.cpp
// Implementation of Isoc1394Device class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998-2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//

#pragma warning(disable:4065) // Allow switch statement with no cases
		  
#include <vdw.h>

#include "..\Isoc1394Deviceinterface.h"
#include "Isoc1394.h"
#include "IsocClient.h"

#include "..\Isoc1394types.h"
#include "AsyncCommands.h"
#include "Isoc1394Device.h"

#include "..\Isoc1394ioctl.h"

#pragma hdrstop("Isoc1394.pch")

extern KTrace t;			// Global driver trace object	

//Allows a loopback in progress to be stopped
BOOLEAN g_StopLoopback = FALSE;

GUID Isoc1394Device_Guid = Isoc1394Device_CLASS_GUID;

////////////////////////////////////////////////////////////////////////
// Isoc1394Device Constructor
//
//		This is the constructor for the Functional Device Object, or FDO.
//		It is derived from KPnpDevice, which builds in automatic
//	    dispatching of subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to
//		virtual member functions.
//
// Input
//		Pdo		Physical Device Object - this is a pointer to a system
//				device object that represents the physical device.
//
//		Unit	Unit number - this is a number to append to the device's
//				base device name to form the Logical Device Object's name
//
// Output
//		None   
//
// Notes   
//		The object being constructed contains a data member (m_Lower) of type
//		KPnpLowerDevice. By initializing it, the driver binds the FDO to the
//		PDO and creates an interface to the upper edge of the system class driver.

Isoc1394Device::Isoc1394Device(
	PDEVICE_OBJECT Pdo, 
	ULONG Unit,
	BOOLEAN AllocateResources
	) :
		   KPnpDevice(
				Pdo,
				KUnitizedName(L"Isoc1394Device", Unit), 
				FILE_DEVICE_UNKNOWN,
				&Isoc1394Device_Guid,
				0,
				DO_DIRECT_IO
				),
			m_Lower(),
            m_AllocateResources(AllocateResources),
			m_Unit(Unit)
{
	t << "Entering Isoc1394Device::Isoc1394Device (constructor)\n";

	// Check constructor status
    if ( ! NT_SUCCESS(m_ConstructorStatus) )
	    return;

	// Initialize the lower device
	m_Lower.Initialize(this, Pdo);

    // Inform the base class of the lower edge device object
	SetLowerDevice(&m_Lower);

	// Initialize the PnP Policy settings to the "standard" policy
	SetPnpPolicy();

    // Initialize the bus reset work item object
	m_BusResetWorkItem.Initialize(m_pDeviceObject);
}


////////////////////////////////////////////////////////////////////////
// Isoc1394Device Destructor
//
//		This is the destructor for the Functional Device Object, or FDO.
//
// Input
//		None
//
// Output
//		None
//
// Notes
//		None

Isoc1394Device::~Isoc1394Device()
{
	t << "Entering Isoc1394Device::~Isoc1394Device() (destructor)\n";
}


////////////////////////////////////////////////////////////////////////
// DefaultPnp (member of Isoc1394Device)
//
//		Default handler for IRP_MJ_PNP
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result returned from lower device
//
// Notes   
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_PNP. IRPs that correspond to
//		any virtual members of KpnpDevice that handle minor functions of
//		IRP_MJ_PNP and that are not overridden get passed to this routine.

NTSTATUS Isoc1394Device::DefaultPnp(KIrp I) 
{
	t << "Entering Isoc1394Device::DefaultPnp " << I;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}


////////////////////////////////////////////////////////////////////////
// DefaultPower (member of Isoc1394Device)
//
//		Default handler for IRP_MJ_POWER 
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result returned from lower device
//
// Notes
//		This routine just passes the IRP through to the lower device. It is 
//		the default handler for IRP_MJ_POWER.

NTSTATUS Isoc1394Device::DefaultPower(KIrp I) 
{
	t << "Entering Isoc1394Device::DefaultPower " << I;

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Lower.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////
// SystemControl (member of Isoc1394Device)
//
//		Handler for IRP_MJ_SYSTEM_CONTROL
//
// Input
//		I			Current IRP
//
// Output   
//		NTSTATUS	Result code
//
// Notes   
//

NTSTATUS Isoc1394Device::SystemControl(KIrp I)
{
	t << "Entering Isoc1394Device::SystemControl " << I;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////
// OnStartDevice (member of Isoc1394Device)
//
//		Handler for IRP_MJ_PNP subfcn IRP_MN_START_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		Initialize the physical device.  
//
//  For an isochronous 1394 device, we should allocate any isochronous 
//  resources such as channels and bandwidth here.  We can also 
//  initialize our transfer objects so they will be ready to go 
//  when we get a request.
//

NTSTATUS Isoc1394Device::OnStartDevice(KIrp I)
{
	t << "Entering Isoc1394Device::OnStartDevice\n";

	I.Information() = 0;

	NTSTATUS status = m_Lower.RegisterBusResetNotification(LinkTo(BusReset),this);

	// Get the speed at which we may transmit to the device
	ULONG Speed = 0;
	if (NT_SUCCESS(status)) 
		status = m_Lower.GetSpeedBetweenDevices(
			1, 
			&m_pDeviceObject, 
			USE_LOCAL_NODE,
			&Speed
			);

	if (NT_SUCCESS(status)) 
	{
		// Allocate the resources if the user requested it.
		// The resources for the two 1394 PC's should only be allocated by
		//  the driver on a single PC since the channel allocation would 
		//  fail on the second driver to attempt it.
		// A more typical device would always allocate these resources.
		if (m_AllocateResources)
		{
			status = m_Lower.Allocate(m_Bandwidth, Speed, 256);
			if (NT_SUCCESS(status))
				status = m_Lower.Allocate(m_Channel, 2);
		}
	}

	// Initialize the write transfer
	if (NT_SUCCESS(status))
		status = m_WriteTransfer.Initialize(TRUE, &m_Lower, this, Speed);

	// Initialize the read transfer.
	if (NT_SUCCESS(status))
		status = m_ReadTransfer.Initialize(FALSE, &m_Lower, this, Speed);

	// Start the loopback listener thread
	if (NT_SUCCESS(status))
	{
		m_AsyncCommands.Initialize(&m_Lower);

		m_StopListenerThread = FALSE;
		m_ListenerThread.Start(LinkTo(LoopbackListenerThread), this);
	}

	// The base class will handle completion of the IRP
	return status;
}


////////////////////////////////////////////////////////////////////////
// OnStopDevice (member of Isoc1394Device)
//
//		Handler for IRP_MJ_PNP subfcn IRP_MN_STOP_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		The system calls this when the device is stopped.
//
//		The base class passes the irp to the lower device.
//
//   For an isochronous 1394 device, we should stop our transfers,
//   and free the resources we allocated in OnStartDevice.
// 

NTSTATUS Isoc1394Device::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering Isoc1394Device::OnStopDevice\n";

	//Stop our transfers
	m_WriteTransfer.Stop();
	m_ReadTransfer.Stop();

	//Free the resources we allocated
	m_Lower.Free(m_Bandwidth);
	m_Lower.Free(m_Channel);

	//Stop our loopback listener thread
	m_StopListenerThread = TRUE;
	g_StopLoopback = TRUE;
	m_AsyncCommands.StartListener();
	m_ListenerThread.Wait();

	m_AsyncCommands.Invalidate();

	status = m_Lower.DeregisterBusResetNotification(LinkTo(BusReset),this);

	return status;
}

////////////////////////////////////////////////////////////////////////
// OnRemoveDevice (member of Isoc1394Device)
//
//		Handler for IRP_MJ_PNP subfcn IRP_MN_REMOVE_DEVICE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//		The system calls this when the device is removed.
//		Our PnP policy will take care of 
//			(1) giving the IRP to the lower device
//			(2) detaching the PDO
//			(3) deleting the device object

NTSTATUS Isoc1394Device::OnRemoveDevice(KIrp I)
{
	t << "Entering Isoc1394Device::OnRemoveDevice\n";

	//Sometimes OnStopDevice is not called, so make sure the clean up
	 // code is called if OnRemoveDevice is Called without calling OnStopDevice.
	if(m_State.m_Started)
		OnStopDevice(I);

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
// BusReset (member of Isoc1394Device)
//
//		Handler for callback of 1394 Bus Reset Notification
//
// Input
//		None
//
// Output
//		None
//
// Notes
//		The system calls this when the underlying bus resets.
//		Isochronous resources need to be reallocated if a bus reset 
//		occurs. 
//

void Isoc1394Device::BusReset()
{
	t << "Entering Isoc1394Device::BusReset\n";

	// Increment the i/o count to prevent removal while
	// the work item callback is pending.  This prevents
	// the class from being delete'd from memory before
	// the callback executes
	IncrementOutstandingRequestCount();

	// schedule a work item to continue processing at PASSIVE_LEVEL
	m_BusResetWorkItem.Queue(LinkTo(BusResetWorkItemCallback),this);
}

////////////////////////////////////////////////////////////////////////
// BusResetWorkItemCallback (member of Isoc1394Device)
//
//		Handler for callback of 1394 Bus Reset Notification
//
// Input
//		None
//
// Output
//		None
//
// Notes
//		Isochronous resources need to be reallocated if a bus reset 
//		occurs. 
//
//      For simplicity, this driver does not deal with the case when 
//      the resources cannot be reallocated.  If the resources are no
//      longer available, a driver should stop any transfers, 
//      invalidate any K1394IsochResource classes, and, possibly, try
//      to reinitialize these classes with other, available, resources.
//

void Isoc1394Device::BusResetWorkItemCallback()
{
	t << "Entering Isoc1394Device::BusResetWorkItemCallback\n";

	// Reallocate our isochronous resources
	if (m_AllocateResources)
	{
		NTSTATUS status = m_Lower.Reallocate(m_Channel);
		ASSERT(NT_SUCCESS(status));

		status = m_Lower.Reallocate(m_Bandwidth);
		ASSERT(NT_SUCCESS(status));
	}

	// Reset the generation count for our asynchronous commands
	m_AsyncCommands.ResetGenerationCount();

    // Decrement the I/O count
	DecrementOutstandingRequestCount();
}

////////////////////////////////////////////////////////////////////////
// Cancel (member of Isoc1394Device)
//
//		This routine is called when an IRP is cancelled.
//
// Input
//		I			IRP being cancelled
//
// Output	
//		None
//
// Notes	
//
// TODO:	You may want to write additional cancel routines
//			to use during different stages of IRP processing.

VOID Isoc1394Device::Cancel(KIrp I)
{
	t << "IRP canceled " << I << EOL;

    // If cancelling the the current I/O operations requires
    // interaction with the physical device, do so here.

    I.Information() = 0;
	if ( (PIRP)I == CurrentIrp() )
	{
		CurrentIrp() = NULL;
		CancelSpinLock::Release(I.CancelIrql());
		I.Status() = STATUS_CANCELLED;
		NextIrp(I);
	}
	else
	{
		KDeviceQueue(DeviceQueue()).RemoveSpecificEntry(I);
		CancelSpinLock::Release(I.CancelIrql());
		I.PnpComplete(this, STATUS_CANCELLED);
	}
}

////////////////////////////////////////////////////////////////////////
// StartIo (member of Isoc1394Device)
//
//		StartIo is called when an IRP is taken off the device queue
//		used by the	system for serializing I/O.  StartIo is called at
//		dispatch level.
//
// Input
//		I			IRP removed from queue
//
// Output
//		None
//
// Notes	
//

VOID Isoc1394Device::StartIo(KIrp I)
{
	t << "Entering StartIo, " << I << EOL;

    // Check to see if the Irp has been canceled.
	CancelSpinLock::Acquire();
	if (I.WasCanceled())
	{
		CancelSpinLock::Release();
		return;
	}
	else
	{
		// Setting the cancel routine to NULL prevents the IRP
		// from being cancelled.  You may want to set a different
		// cancel routine here, or at other points within this function.
		I.SetCancelRoutine(NULL);
		CancelSpinLock::Release();
	}

	// Start processing request.

	// Switch on the IRP's function:
	switch (I.MajorFunction())
	{
		case IRP_MJ_READ:
			SerialRead(I);
			break;
		case IRP_MJ_WRITE:
			SerialWrite(I);
			break;
		case IRP_MJ_DEVICE_CONTROL:
			SerialDeviceControl(I);
			break;
		default:
			// The driver queued an Irp that isn't handled
			// by StartIo.  This shouldn't happen.
			ASSERT(FALSE);
			PnpNextIrp(I);
			break;
	}
}

////////////////////////////////////////////////////////////////////////
// Create (member of Isoc1394Device)
//
//		Handler for IRP_MJ_CREATE
//
// Input
//		I			Current IRP
//
// Output   
//		NTSTATUS	Result code
//
// Notes   
//

NTSTATUS Isoc1394Device::Create(KIrp I)
{ 
	t << "Entering Isoc1394Device::Create, " << I << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	NTSTATUS status = m_Lower.PnpCall(this, I);

	t << "Isoc1394Device::Create Status " << (ULONG)status << EOL;

	return status;
}


////////////////////////////////////////////////////////////////////////
// Close (member of Isoc1394Device)
//
//		Handler for IRP_MJ_CLOSE
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//

NTSTATUS Isoc1394Device::Close(KIrp I)
{
	t << "Entering Isoc1394Device::Close, " << I << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	NTSTATUS status = m_Lower.PnpCall(this, I);

	t << "Isoc1394Device::Close Status " << (ULONG)status << EOL;

    return status;
}

////////////////////////////////////////////////////////////////////////
// Cleanup (member of Isoc1394Device)
//
//		Handler for IRP_MJ_CLEANUP	
//
// Input
//		I			Current IRP
//
// Output	
//		NTSTATUS	Result code
//
// Notes
//

NTSTATUS Isoc1394Device::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I << EOL;

// TODO:	Insert your code to respond to the CLEANUP message
//
	KDeviceQueue dq(DeviceQueue());
	dq.PnpCleanUp(this, I.FileObject());
	return I.PnpComplete(this, STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
// SerialRead (member of Isoc1394Device)
//	
//		Handler for serialized READ
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes	
//		This routine is called when the IRP is removed from the
//		STARTIO queue.  This guarantees that multiple requests are
//		never processed simultaneously.
//
//		This routine is called at dispatch level.	
//
//      In this routine, we do the work of setting up the IRP's MDL 
//      for isochronous I/O in StartSegment.  We then attempt to 
//      start the transfer if it has not been started.  Isochronous 
//      transfers may not be started until at least one buffer has
//      been attached to the transfer object.
//
//      The IRP will be completed when the transfer completes in 
//      Isoc1394Transfer::OnBufferComplete.

void Isoc1394Device::SerialRead(KIrp I)
{
	t << "Entering Isoc1394Device::SerialRead, " << I;
	NTSTATUS status		= STATUS_INSUFFICIENT_RESOURCES;

	// Declare a memory object
	KMemory Mem(I.Mdl());

	I.Information() = 0;
	
	//Allocate a completion context
	PBUFFER_DESCRIPTOR  pContext = new (NonPagedPool) BUFFER_DESCRIPTOR;
	if (pContext) 
	{
		//Set up our context
		pContext->BytesProcessed = 0;
		pContext->pIrp = I;
		pContext->AllocatedMdl = FALSE;
		pContext->pEvent = NULL;

		status = StartSegment(pContext);

		if (!m_ReadTransfer.IsStarted() && NT_SUCCESS(status))
			status = m_ReadTransfer.StartListen();
	}

	//Complete the IRP immediately if we are failing the request
	if (!NT_SUCCESS(status))
	{
		I.Status() = status;
		PnpNextIrp(I);

		if (pContext)
			delete pContext;
	}
}

////////////////////////////////////////////////////////////////////////
// Read (member of Isoc1394Device)
//
//		Handler for IRP_MJ_READ
//
// Input
//		I			Current IRP
//
// Output   
//		NTSTATUS	Result code
//
// Note   
//		This routine handles read requests.
//		The requests are queued to the StartIo	
//		queue.  This enforces serialization so that
//		multiple requests are not processed	
//		simultaneously.
//
//		The KPnpDevice class handles restricting IRP flow
//		if the device is stopping or being removed.

NTSTATUS Isoc1394Device::Read(KIrp I) 
{
	t << "Entering Isoc1394Device::Read, " << I << EOL;
// TODO:	Check the incoming request.  Replace "FALSE" in the following
//			line with a check that returns TRUE if the request is not valid.

    if (FALSE)		// If (Request is invalid)
	{
		// Invalid parameter in the Read request
		I.Information() = 0;
		return I.PnpComplete(this, STATUS_INVALID_PARAMETER);
	}

	// Always ok to read 0 elements.
	if (I.ReadSize() == 0)
	{
		I.Information() = 0;
		return I.PnpComplete(this, STATUS_SUCCESS);
	}
	// Queue the IRP for processing in StartIO
	// The read function is performed in SerialRead
	return QueueIrp(I, LinkTo(Cancel));
}

////////////////////////////////////////////////////////////////////////
// SerialWrite (member of Isoc1394Device)
//	
//		Handler for serialized WRITE
//
// Input	
//		I		Current IRP
//
// Output	
//		None
//
// Notes	
//		This routine is called when the IRP is removed from the
//		STARTIO queue.  This guarantees that multiple requests are
//		never processed simultaneously.
//
//      In this routine, we do the work of setting up the IRP's MDL 
//      for isochronous I/O in StartSegment.  We then attempt to 
//      start the transfer if it has not been started.  Isochronous 
//      transfers may not be started until at least one buffer has
//      been attached to the transfer object.
//      The IRP will be completed when the transfer completes in 
//      Isoc1394Transfer::OnBufferComplete.

void Isoc1394Device::SerialWrite(KIrp I)
{
	t << "Entering Isoc1394Device::SerialWrite, " << I << EOL;
	NTSTATUS status		= STATUS_INSUFFICIENT_RESOURCES;

	// Declare a memory object
	KMemory Mem(I.Mdl());

	I.Information() = 0;

	//Allocate a completion context
	PBUFFER_DESCRIPTOR  pContext = new (NonPagedPool) BUFFER_DESCRIPTOR;
	if (pContext)
	{
		//Set up our context
		pContext->BytesProcessed = 0;
		pContext->pIrp = I;
		pContext->AllocatedMdl = FALSE;
		pContext->pEvent = NULL;

        status = StartSegment(pContext);
		
		if (!m_WriteTransfer.IsStarted() && NT_SUCCESS(status))
			status = m_WriteTransfer.StartTalk();
	}

	//Complete the IRP immediately if we are failing the request
	if (!NT_SUCCESS(status))
	{
		I.Status() = status;
		PnpNextIrp(I);

		if (pContext)
			delete pContext;
	}
}

////////////////////////////////////////////////////////////////////////
// Write (member of Isoc1394Device)
//
//		Handler for IRP_MJ_WRITE
//
// Input
//		I			Current IRP
//
// Output   
//		NTSTATUS	Result code
//
// Note   
//		This routine handles write requests.
//		The requests are queued to the StartIo	
//		queue.  This enforces serialization so that
//		multiple requests are not processed	
//		simultaneously.
//
//		The KPnpDevice class handles restricting IRP flow
//		if the device is stopping or being removed.

NTSTATUS Isoc1394Device::Write(KIrp I) 
{
	t << "Entering Isoc1394Device::Write, " << I << EOL;
// TODO:	Check the incoming request.  Replace "FALSE" in the following
//			line with a check that returns TRUE if the request is not valid.
    if (FALSE)
	{
		// Invalid parameter in the Write request
		I.Information() = 0;
		return I.PnpComplete(this, STATUS_INVALID_PARAMETER);
	}

	// Always ok to write 0 elements.
	if (I.WriteSize() == 0)
	{
		I.Information() = 0;
		return I.PnpComplete(this, STATUS_SUCCESS);
	}
	// Queue the IRP for processing in StartIO
	// The write function is performed in SerialWrite
	return QueueIrp(I, LinkTo(Cancel));
}


////////////////////////////////////////////////////////////////////
// Isoc1394Deviace::StartSegment
//
//	Starts the next segment of a buffer
//
// Parameters
//	pContext - The buffer context.  Provides the bytes processed so far,
//		and the IRP being worked on. 
//
// Notes
//	StartSegment:
//		1. Gets buffer from IRP and calculates the number of buffers needed
//		2. Allocates a buffer list
//		3. Fills in the buffer list with MDL's describing peices of the buffer in
//			the MDL described by the IRP
//		4. Submits the buffer to the transfer object
//
//	Isoc1394Transfer::OnBufferComplete will be called when the buffer list completes.
//    It will complete the IRP if the transfer is done, or clean up the buffer list
//		and call StartSegment again to transmit the next portion of the buffer.
//
//   StartSegment breaks up MDL's into lists of MDL's no bigger than the maximum
//    segment size specified when the resource handle was allocated.  This routine
//    is necessary  because experimentation has shown that the 1394 bus driver 
//    does not break up large buffers correctly. 
//
NTSTATUS Isoc1394Device::StartSegment(PBUFFER_DESCRIPTOR pContext)
{
	KIrp					I(pContext->pIrp);
	KMemory					Mdl(I.Mdl());
	PUCHAR					pBuffer		= (PUCHAR) Mdl.VirtualAddress();
	K1394IsochBufferList*	pBufferList = NULL;
	Isoc1394Transfer*		pTransfer	= NULL;

	if (I.MajorFunction() == IRP_MJ_READ)
	{
		pTransfer = &m_ReadTransfer;
	}
	else
	{
		pTransfer = &m_WriteTransfer;
	}
	
	//See how many MDL's we need for the remaining buffer
	ULONG nCount = (Mdl.Size() - pContext->BytesProcessed) / MAX_BYTES_PER_FRAME;
	if ( (Mdl.Size() - pContext->BytesProcessed) % MAX_BYTES_PER_FRAME)
	{
		nCount++;
	}

	//Keep track of the buffers we are going to attach
	pTransfer->m_BufferSpinlock.Lock();
	
	//See if enough descriptors are available:
	if ((MAX_BUFFER_COUNT - pTransfer->m_BuffersAttached) < nCount)
	{
		nCount = MAX_BUFFER_COUNT - pTransfer->m_BuffersAttached;
	}

	if (nCount == 0 || !(pBufferList = new (nCount) K1394IsochBufferList))
	{
		pTransfer->m_BufferSpinlock.Unlock();
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//Update the buffers attached count
	pTransfer->m_BuffersAttached += nCount;
	pTransfer->m_BufferSpinlock.Unlock();

	//Put a pointer to the context in the buffer list
	pBufferList->CompletionContext() = pContext;

	//Fill in the buffer list
	for (ULONG next = 0; next < nCount; next++)
	{
		//Set up the MDL
		if ((nCount == 1) && (pContext->BytesProcessed == 0))
		{
			//In this case, we can use the user's MDL
			pBufferList->SetupBuffer(next, I.Mdl());
			pContext->AllocatedMdl = FALSE;
		}
		else
		{	
			//Get the size of this buffer
			ULONG size = Mdl.Size() - pContext->BytesProcessed - (MAX_BYTES_PER_FRAME*next);
			if(size > MAX_BYTES_PER_FRAME)
				size = MAX_BYTES_PER_FRAME;

			pContext->AllocatedMdl = TRUE;

			PMDL pMdl = IoAllocateMdl(
							pBuffer + pContext->BytesProcessed + (next*MAX_BYTES_PER_FRAME),
							size,
							FALSE,
							FALSE,
							NULL
							);

			if (!pMdl)
			{
				//Unable to allocate MDL.  
				pTransfer->m_BufferSpinlock.Lock();
				pTransfer->m_BuffersAttached -= nCount;
				pTransfer->m_BufferSpinlock.Unlock();

				//Clean up and return
				for (ULONG next2 = 0; next2 < next; next2++)
				{
					IoFreeMdl(pBufferList[next2].Mdl);
				}

				delete pBufferList;
				return STATUS_INSUFFICIENT_RESOURCES;
			}

			//Set up the MDL
			IoBuildPartialMdl(
				I.Mdl(),
				pMdl,
				pBuffer + pContext->BytesProcessed + (next*MAX_BYTES_PER_FRAME),
				size
				);

			//Fill in the buffer descriptor
			pBufferList->SetupBuffer(next, pMdl);
		}
	}

	//Add the buffer list to the transfer.
	return pTransfer->AddBufferList(*pBufferList);
}

////////////////////////////////////////////////////////////////////////
// SerialDeviceControl (member of Isoc1394Device)
//
//		Handler for serialized IRP_MJ_DEVICECONTROL
//
// Input
//		I		Current IRP
//
// Output
//		None
//
// Notes	
//		This routine is called when the IRP is removed from the
//		STARTIO queue.  This guarantees that multiple requests are
//		never processed simultaneously.	

void Isoc1394Device::SerialDeviceControl(KIrp I)
{
	NTSTATUS status = STATUS_INVALID_PARAMETER;

	t << "Entering Isoc1394Device::SerialDeviceControl, " << I << EOL;
    switch (I.IoctlCode())
	{
		default:
			// We queued a request that shouldn't have been queued
			// (should never get here)
			ASSERT(FALSE);
			break;
	}

	I.Status() = status;
	PnpNextIrp(I);
}

////////////////////////////////////////////////////////////////////////
// DeviceControl (member of Isoc1394Device)
//
//		Handler for IRP_MJ_DEVICE_CONTROL
//
// Input
//		I		Current IRP
// 
// Output
//		None
//
// Notes:
//		This routine is the first handler for Device Control requests.
//		Some function codes may be handled immediately, 
//		while others may be serialized through the StartIo routine.
//		
//		The KPnpDevice class handles restricting IRP flow
//		if the device is stopping or being removed.
//
NTSTATUS Isoc1394Device::DeviceControl(KIrp I) 
{
	NTSTATUS status = STATUS_INVALID_PARAMETER;

	t << "Entering Isoc1394Device::Device Control, " << I << EOL;
	switch (I.IoctlCode())
	{
		//Cancel a loopback in progress
		case ISOC1394_IOCTL_800_SETUP:
			status = STATUS_SUCCESS;
			g_StopLoopback = TRUE;
			break;
			
		//Start a loopback test
		case ISOC1394_IOCTL_802:
			if (I.IoctlOutputBufferSize() == sizeof(LOOPBACK_STATISTICS))
			{
				g_StopLoopback = FALSE;
				//Do the loopback test
				status = LoopbackTest((PLOOPBACK_STATISTICS)I.IoctlBuffer());
				
				//Stop the loopback listener
				m_AsyncCommands.SendCommand(COMMAND_STOP_LISTENER);

				I.Information() = sizeof(LOOPBACK_STATISTICS);
			}
			break;

		case ISOC1394_IOCTL_801: //Unsupported IOCTL code
		default:
			// Unrecognized IOCTL request
			break;
	}

	return I.PnpComplete(this, status);
}

///////////////////////////////////////////////////////////////////////////////////////
//Isoc1394Device::LoopbackTransaction
//
// Performs a single loopback transaction, either a read or a write. 
//   1. Submit buffer
//	 2. Start transfer if necessary
//	 2. Wait for buffer to complete
//	 3. If buffer timed out, cancel it.
//	 4. Check the completed buffer's status
//
//	Parameters
//		pTransfer		- The transfer object to submit the buffer on
//		pBufferComplete - Initialized event to be signalled when the buffer completes
//		pBufferList		- The buffer list to be submitted
//		bAllowTimeOut	- 
//
//	Returns:
//		The buffer's status.
//		STATUS_SUCCESS if all was successful
//		STATUS_TIMEOUT if the buffer timed out.
//		Any other status will cause the loopback to exit.
//
NTSTATUS Isoc1394Device::LoopbackTransaction(
	Isoc1394Transfer*		pTransfer,
	KEvent*					pBufferComplete, 
	K1394IsochBufferList*	pBufferList,
	BOOLEAN					bAllowTimeOut
	)
{

	LARGE_INTEGER TimeOut; TimeOut.QuadPart = -10000;
	ULONG TimeoutCount = 0;
	
	//Submit the buffer list
	NTSTATUS status = pTransfer->AddBufferList(*pBufferList);
	if (NT_SUCCESS(status) && !pTransfer->IsStarted())
	{
		status = pTransfer->Start();
	}
	
	if (NT_SUCCESS(status))
	{
		//Wait for the buffer to complete
		status = STATUS_TIMEOUT; 
		while (status == STATUS_TIMEOUT && ! g_StopLoopback && TimeoutCount < 8)
		{
			status = pBufferComplete->Wait(KernelMode, FALSE, &TimeOut);
			if (bAllowTimeOut)
				TimeoutCount++;
		}
		
		//If the buffer timed, cancel the buffer.
		if (status == STATUS_TIMEOUT)
		{
			status = pTransfer->CancelBufferList(*pBufferList);
			//The buffer should complete soon after the cancel request
			if (NT_SUCCESS(status))
			{
				pBufferComplete->Wait(KernelMode, FALSE, &TimeOut);
			}

			status = STATUS_TIMEOUT;
		}
		else 
		{
			if (status == STATUS_SUCCESS)
			{
				status = pTransfer->BufferStatus();
			}
		}
	}

	return status;
}

///////////////////////////////////////////////////////////////
//Isoc1394Device::LoopbackListener 
//
// The Loopback listener function.  Listens for packets and sends them
//   back to the sender.
//		1. Perform a read
//		2. Submit the buffer just read to be written.
//		3. Repeat infinitely.
// Parameters:
//	None
// Returns:
//	None
// Comments:
//	The listener will exit if g_StopLoopback becomes TRUE.  It can be set through 
//	 IOCTL 800, or will be set if a COMMAND_STOP_LISTENER is received.
//

NTSTATUS Isoc1394Device::LoopbackListener()
{
	t << "Entering Isoc1394Device::LoopbackListener!\n";

	LARGE_INTEGER TimeOut; TimeOut.QuadPart = -10;

	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	//Our buffer, and its MDL's
	CHAR		theBuffer[512];
	KMemory		Mdl0(&theBuffer, 256);		Mdl0.SetPageArray();
	KMemory		Mdl1(&theBuffer[256], 256); Mdl1.SetPageArray();

	//Our buffer list
	K1394IsochBufferList *pBufferList = new (2) K1394IsochBufferList; 
	if (pBufferList)
	{
		pBufferList->SetupBuffer(0, Mdl0); 
		pBufferList->SetupBuffer(1, Mdl1); 

		//Event to tell us when the buffers have completed
		KEvent BufferComplete(SynchronizationEvent, FALSE);

		//Set up the read and write contexts
		BUFFER_DESCRIPTOR Context;
		Context.pEvent = &BufferComplete;
		pBufferList->CompletionContext() = &Context;

		//Tell the other side we are ready.
		m_AsyncCommands.SendCommand(COMMAND_LISTENER_READY);

        // initialize the status to success
        status = STATUS_SUCCESS;

		while (!g_StopLoopback && NT_SUCCESS(status))
		{
			//Wait to receive a read
			status = LoopbackTransaction(
							&m_ReadTransfer,
							&BufferComplete,
							pBufferList,
							FALSE
							);
			
			//Wait to make sure the sender is ready for the reply
			BufferComplete.Wait(KernelMode, FALSE, &TimeOut);
				
				//Write the buffer back out
			if (!g_StopLoopback && NT_SUCCESS(status))
			{
				status = LoopbackTransaction(
							&m_WriteTransfer,
							&BufferComplete,
							pBufferList
							);
			}
		}	

		delete pBufferList;
	}

	t << "LoopbackListener Exiting!\n";

	return status;
}

//////////////////////////////////////////////////////////////////////////////////////////
//LoopbackListenerThread
//
// Parameters:
//		None.
//
// Returns:
//		None:
//
// Comments:
//  This is a system background thread that runs constantly in the background.  It waits 
//    for the 'start listener' event to start the loopback listener.
//  
VOID Isoc1394Device::LoopbackListenerThread()
{
	t << "Entering Isoc1394Device::LoopbackListenerThread\n";

	NTSTATUS status = STATUS_SUCCESS;

	while (!m_StopListenerThread)
	{
		m_AsyncCommands.WaitStartListener();
		g_StopLoopback = FALSE;

		if (!m_StopListenerThread)
		{
			status = LoopbackListener();
		}

		if (!(NT_SUCCESS(status)))
		{
			m_StopListenerThread = TRUE;
		}

		m_AsyncCommands.SendCommand(COMMAND_LISTENER_STOPPED);
	}

	t << "Listener thread exiting\n";
	m_ListenerThread.Terminate(status);
}

////////////////////////////////////////////////////////////////
// Isoc1394Device::LoopbackTest
//
//	The active loopback function.
//
//	1. Write out a packet
//	2. Read a packet.
//	3. Wait for completion or timeout
//	4. Compare data in reply packet to data sent.
//	5. Update packet statistics
//
//	Parameters:
//		pStats	-	Will receive the number of successful packets, 
//					timed out packets, and data error packets sent.
//
//	Comments:
//		The loopback transfer can be cancelled by setting g_StopLoopback to TRUE.
//			done in IOCTL 800.
//
//		The loopback transfer currently goes through 10,000 iterations.
//      Average results are > 9990 successful packets
//		Packets sent are 512 bytes.  
//

NTSTATUS Isoc1394Device::LoopbackTest(PLOOPBACK_STATISTICS pStats)
{
	t << "Entering Isoc1394Device::LoopbackTest\n";

	LARGE_INTEGER TimeOut; TimeOut.QuadPart = -100000;

	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	//Our Write buffer, and its MDL's
	CHAR		theBuffer[512];
	KMemory		Mdl0(&theBuffer, 256);		Mdl0.SetPageArray();
	KMemory		Mdl1(&theBuffer[256], 256); Mdl1.SetPageArray();

	//Our Write buffer list
	K1394IsochBufferList* pBufferList = new (2) K1394IsochBufferList; 
	
	if (pBufferList)
	{
		pBufferList->SetupBuffer(0, Mdl0); 
		pBufferList->SetupBuffer(1, Mdl1); 

		//Our Read buffer, and its MDL's
		CHAR		readBuffer[512];
		KMemory		ReadMdl0(&readBuffer, 256);		 ReadMdl0.SetPageArray();
		KMemory		ReadMdl1(&readBuffer[256], 256); ReadMdl1.SetPageArray();

		//Our Read buffer list
		K1394IsochBufferList* pReadBufferList = new (2) K1394IsochBufferList;
		
		if (pReadBufferList)
		{
			pReadBufferList->SetupBuffer(0, ReadMdl0); 
			pReadBufferList->SetupBuffer(1, ReadMdl1); 

			//Event to tell us when the buffers have completed
			KEvent BufferComplete; 
			BufferComplete.Initialize(SynchronizationEvent, FALSE);

			//Set up the context
			BUFFER_DESCRIPTOR Context;
			Context.pEvent = &BufferComplete;

			pBufferList->CompletionContext() = &Context;
			pReadBufferList->CompletionContext() = &Context;

			//Initialize the loopback statistics
			pStats->SuccessfulPackets = 0;
			pStats->TimeOuts = 0;
			pStats->DataErrors = 0;

			//Initialize the write buffer
			for (ULONG next = 0; next < 512; next++)
			{
				theBuffer[next] = (CHAR)('A' + next%26);
			}

			//Start the listener and wait for it to be ready
			m_AsyncCommands.SendCommand(COMMAND_START_LISTENER);
			m_AsyncCommands.WaitListenerReady();

			for (ULONG next2 = 0; next2 < 1000; next2++)
			{
				//Write the buffer out
				status = LoopbackTransaction(
							&m_WriteTransfer,
							&BufferComplete,
							pBufferList
							);

				//Wait for the buffer to come back
				if (NT_SUCCESS(status) && !g_StopLoopback)
				{
					status = LoopbackTransaction(
								&m_ReadTransfer,
								&BufferComplete,
								pReadBufferList
								);
				}

				if (g_StopLoopback)
				{
					status = STATUS_CANCELLED;
				}
				else 
				{
					if (status == STATUS_TIMEOUT)
					{
						pStats->TimeOuts++;

						//Restart the listener to make sure we are in sync with it.
						// This avoids data errors due to one partially received buffer

						m_AsyncCommands.SendCommand(COMMAND_START_LISTENER);
						m_AsyncCommands.WaitListenerReady();
					}
					else
					{
						//Check that the buffer we received was the one we sent
						for (ULONG next = 0; next < 512; next++)
						{
							if (theBuffer[next] != readBuffer[next])
							{
								t << "Error: Returned buffer doesn't match!\n";
								break;
							}
						}

						if (next == 512)
						{
							pStats->SuccessfulPackets++;
						}
						else
						{
							pStats->DataErrors++;
						}
					}
				}
			}

			delete pReadBufferList;
		}
		
		delete pBufferList;
	}
	
	t << "Exiting Isoc1394Device::LoopbackTest\n";

	return status;
}

