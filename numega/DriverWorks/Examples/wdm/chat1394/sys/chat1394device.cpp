// Chat1394Device.cpp
// Implementation of Chat1394Device device class
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
//

#pragma warning(disable:4065) // Allow switch statement with no cases
		  
#include <vdw.h>

#include "..\Chat1394Deviceinterface.h"

#include "Chat1394.h"
#include "ChatClient.h"
#include "Chat1394Device.h"
#include "..\Chat1394ioctl.h"

#pragma hdrstop("Chat1394.pch")

extern KTrace t;			// Global driver trace object	

GUID Chat1394Device_Guid = Chat1394Device_CLASS_GUID;

////////////////////////////////////////////////////////////////////////
// Chat1394Device Constructor
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

Chat1394Device::Chat1394Device(PDEVICE_OBJECT Pdo, ULONG Unit) :

   KPnpDevice(
	    Pdo,
		KUnitizedName(L"Chat1394Device", Unit), 
		FILE_DEVICE_UNKNOWN,
		&Chat1394Device_Guid,
		0,
		DO_DIRECT_IO
		),
	m_Lower(),
	m_ReadFifo(MAX_READ_FIFO_SIZE, NonPagedPool),
	m_bReadPended(FALSE),
	m_bReadDataLoss(FALSE)
{
	t << "Entering Chat1394Device::Chat1394Device (constructor)\n";


	// Check constructor status
    if ( ! NT_SUCCESS(m_ConstructorStatus) )
	    return;

	// Remember our unit number
	m_Unit = Unit;

	// Initialize the lower device
	m_Lower.Initialize(this, Pdo);

    // Inform the base class of the lower edge device object
	SetLowerDevice(&m_Lower);

	// Initialize the PnP Policy settings to the "standard" policy
	SetPnpPolicy();
}


////////////////////////////////////////////////////////////////////////
// Chat1394Device Destructor
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

Chat1394Device::~Chat1394Device()
{
	t << "Entering Chat1394Device::~Chat1394Device() (destructor)\n";
}

////////////////////////////////////////////////////////////////////////
// PNPMinorFunctionName
//
//		Return a string describing the Plug and Play minor function	
//
// Input	
//		mn		Minor function code
//
// Output	
//		char *	Ascii name of minor function
//
// Notes	
//		This function is used for tracing the IRPs.  Remove the function,
//		or conditionalize it for debug-only builds, if you want to save
//		space in the driver image.
	
char *PNPMinorFunctionName(ULONG mn)
{
	static char* minors[] = {
		"IRP_MN_START_DEVICE",
		"IRP_MN_QUERY_REMOVE_DEVICE",
		"IRP_MN_REMOVE_DEVICE",
		"IRP_MN_CANCEL_REMOVE_DEVICE",
		"IRP_MN_STOP_DEVICE",
		"IRP_MN_QUERY_STOP_DEVICE",
		"IRP_MN_CANCEL_STOP_DEVICE",
		"IRP_MN_QUERY_DEVICE_RELATIONS",
		"IRP_MN_QUERY_INTERFACE",
		"IRP_MN_QUERY_CAPABILITIES",
		"IRP_MN_QUERY_RESOURCES",
		"IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
		"IRP_MN_QUERY_DEVICE_TEXT",
		"IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
		"IRP_MN_READ_CONFIG",
		"IRP_MN_WRITE_CONFIG",
		"IRP_MN_EJECT",
		"IRP_MN_SET_LOCK",
		"IRP_MN_QUERY_ID",
		"IRP_MN_QUERY_PNP_DEVICE_STATE",
		"IRP_MN_QUERY_BUS_INFORMATION",
		"IRP_MN_DEVICE_USAGE_NOTIFICATION",
		"IRP_MN_SURPRISE_REMOVAL"
	};

	if (mn > IRP_MN_SURPRISE_REMOVAL) 
		return "<unknown minor function>";
	else
		return minors[mn];
}

////////////////////////////////////////////////////////////////////////
// DefaultPnp (member of Chat1394Device)
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

NTSTATUS Chat1394Device::DefaultPnp(KIrp I) 
{
	t << "Entering Chat1394Device::DefaultPnp with IRP minor function="
	  << PNPMinorFunctionName(I.MinorFunction()) << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}


////////////////////////////////////////////////////////////////////////
// DefaultPower (member of Chat1394Device)
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

NTSTATUS Chat1394Device::DefaultPower(KIrp I) 
{
	t << "Entering Chat1394Device::DefaultPower\n";

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Lower.PnpPowerCall(this, I);
}


////////////////////////////////////////////////////////////////////////
// OnStartDevice (member of Chat1394Device)
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
//      For an asynchronous 1394 device, we must get the generation count,
//   necessary for any asynchronous requests.
//
//      The Chat device also uses an address range class, ChatClient, to
//   receive notification when the device writes to its 1394 address space.
//   This address range class is initialized here as well.
//

NTSTATUS Chat1394Device::OnStartDevice(KIrp I)
{

	t << "Entering Chat1394Device::OnStartDevice\n";

    NTSTATUS status = STATUS_SUCCESS;

	I.Information() = 0;

	// Initialize our address range class
	status = m_ChatClient.Initialize(&m_Lower, this);

	if (NT_SUCCESS(status)) 
	{
		// Get the generation count
		status = m_GenerationCount.Initialize(&m_Lower);
		if ( NT_SUCCESS(status) )
		{
			t << "Generation Count: " << m_GenerationCount << EOL;
		}
	}

	// The base class will handle completion

	return status;
}

////////////////////////////////////////////////////////////////////////
// OnStopDevice (member of Chat1394Device)
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
//		The driver should release any hardware resources
//		in this routine.
//
//		The base class passes the irp to the lower device.

NTSTATUS Chat1394Device::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering Chat1394Device::OnStopDevice\n";

	status = m_GenerationCount.Deinitialize();

	return status;
}

////////////////////////////////////////////////////////////////////////
// OnRemoveDevice (member of Chat1394Device)
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
//
//    In this case, we need to shut down our address range class, and
//  cancel any queued read IRPs.
//

NTSTATUS Chat1394Device::OnRemoveDevice(KIrp I)
{
	t << "Entering Chat1394Device::OnRemoveDevice\n";

	NTSTATUS status = STATUS_SUCCESS;
	NTSTATUS LastErrorStatus = STATUS_SUCCESS;

	//Shut down our address range class
	m_ChatClient.Invalidate();

	// Cancel all outstanding read Irps that may be hanging out
	// in the StartIO queue
	KDeviceQueue dq(DeviceQueue());
	dq.PnpCleanUp(this, NULL);

	// Acquire the spin lock for the read fifo
	m_ReadFifo.Lock();

	// If there is a current Irp, we only need to cancel it if it is pended,
	// otherwise it is already completing on its own in the SerialRead function.
	// We must flush the read fifo
	if ( CurrentIrp() != NULL && m_bReadPended )
	{
		// Clear the pended flag so the NotificationCallback function can't 
		// complete the Irp
		m_bReadPended = FALSE;

		// In this case fifo should be empty, but flush it anyway and release
		// the read spin lock
		m_ReadFifo.Flush();
		m_ReadFifo.Unlock();

		KIrp CurIrp = CurrentIrp();

		CancelSpinLock::Acquire();
		CurIrp.SetCancelRoutine(NULL);
		CancelSpinLock::Release();

		CurIrp.Information() = 0;
		CurIrp.Status() = STATUS_CANCELLED;

		CurrentIrp() = NULL;
		PnpNextIrp(CurIrp);
	}
	else
	{
		// Flush any unused data in the read fifo and release the read spin lock
		m_ReadFifo.Flush();
		m_ReadFifo.Unlock();
	}

	return LastErrorStatus;
}


////////////////////////////////////////////////////////////////////////
//  Chat1394Device::OnDeviceSleep
//
//	Routine Description:
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to a low power state from a high power state
//
//	Parameters:
//		I - IRP containing POWER request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		This routine implements the OnDeviceSleep function.
//		This function was called by the framework from the IRP_MJ_POWER
//		dispatch handler in KPnpDevice prior to forwarding to the PDO.
//		The hardware has yet to be powered down and this driver can now
//		access the hardware device.
//		This routine runs at passive level.
//

NTSTATUS Chat1394Device::OnDeviceSleep(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering Chat1394Device::OnDeviceSleep" << EOL;

	status = m_GenerationCount.Deinitialize();

	return status;
	UNREFERENCED_PARAMETER(I);
}


////////////////////////////////////////////////////////////////////////
//  Chat1394Device::OnDevicePowerUp
//
//	Routine Description:
//		Handler for IRP_MJ_POWER with minor function IRP_MN_SET_POWER
//		for a request to go to power on state from low power state
//
//	Parameters:
//		I - IRP containing POWER request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		This routine implements the OnDevicePowerUp function.
//		This function was called by the framework from the completion
//		routine of the IRP_MJ_POWER dispatch handler in KPnpDevice.
//		The bus driver has completed the IRP and this driver can now
//		access the hardware device.
//		This routine runs at dispatch level.
//

NTSTATUS Chat1394Device::OnDevicePowerUp(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering Chat1394Device::OnDevicePowerUp" << EOL;

	status = m_GenerationCount.Initialize(&m_Lower);

	return status;
	UNREFERENCED_PARAMETER(I);
}


////////////////////////////////////////////////////////////////////////
// Cancel (member of Chat1394Device)
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

VOID Chat1394Device::Cancel(KIrp I)
{
	t << "IRP canceled " << I << EOL;

    I.Information() = 0;

	if ( (PIRP)I == CurrentIrp() )
	{

		// Cancellation for the current Irp is only enabled if the Irp was
		// pended by SerialRead().  This means the m_bReadPended flag is
		// set.  Since we are cancelling the Irp, clear this flag.  We don't
		// use the ReadFifo spin lock since setting the flag is atomic anyway,
		// and besides we will only read this flag in the NotificationCallback()
		// routine unless we have already made the Irp not cancellable.
		m_bReadPended = FALSE;

		CurrentIrp() = NULL;
		CancelSpinLock::Release(I.CancelIrql());
		I.Status() = STATUS_CANCELLED;
		PnpNextIrp(I);
	}
	else
	{
		KDeviceQueue(DeviceQueue()).RemoveSpecificEntry(I);
		CancelSpinLock::Release(I.CancelIrql());
		I.PnpComplete(this, STATUS_CANCELLED);
	}
}

////////////////////////////////////////////////////////////////////////
// StartIo (member of Chat1394Device)
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
//		In this driver, we use StartIo to exclusively handle the
//		IRP_MJ_READ Irp.  This Irp when it is not immediately servicable
//		(there is no data in the read fifo) will pend until it becomes
//		servicable.  If our driver was more complicated, we could have
//		dedicated an independent driver managed queue for read Irps.
//

VOID Chat1394Device::StartIo(KIrp I)
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
		I.SetCancelRoutine(NULL);
		CancelSpinLock::Release();
	}

	// Start processing request.  We only process IRP_MJ_READ.  All other
	// Irps are an error

	// Switch on the IRP's function:
	switch (I.MajorFunction())
	{
		case IRP_MJ_READ:
			SerialRead(I);
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
// Create (member of Chat1394Device)
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

NTSTATUS Chat1394Device::Create(KIrp I)
{ 
	t << "Entering Chat1394Device::Create, " << I << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	NTSTATUS status = m_Lower.PnpCall(this, I);

	t << "Chat1394Device::Create Status " << (ULONG)status << EOL;

	return status;
}


////////////////////////////////////////////////////////////////////////
// Close (member of Chat1394Device)
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

NTSTATUS Chat1394Device::Close(KIrp I)
{
	t << "Entering Chat1394Device::Close, " << I << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	NTSTATUS status = m_Lower.PnpCall(this, I);

	t << "Chat1394Device::Close Status " << (ULONG)status << EOL;

    return status;
}

////////////////////////////////////////////////////////////////////////
// Cleanup (member of Chat1394Device)
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

NTSTATUS Chat1394Device::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I << EOL;

	KDeviceQueue dq(DeviceQueue());
	dq.PnpCleanUp(this, I.FileObject());

	// Acquire the spin lock for the read fifo so we can read/modify the read
	// pending flag
	m_ReadFifo.Lock();

	// If there is a current pended Irp, complete it with STATUS_CANCELLED
	if (m_bReadPended)
	{
		// Before we can complete the Irp, check to see if the Irp was cancelled.
		// If so, the cancel routine will 
		CancelSpinLock::Acquire();
		KIrp Current = CurrentIrp();
		if (Current.WasCanceled())
		{
			CancelSpinLock::Release();
			m_ReadFifo.Unlock();
		}
		else
		{
			// Setting the cancel routine to NULL prevents the IRP
			// from being cancelled.
			Current.SetCancelRoutine(NULL);
			CancelSpinLock::Release();

			// Clear the pended flag preventing Notification routine from processing
			// this Irp
			m_bReadPended = FALSE;

			Current.Information() = 0;
			Current.Status() = STATUS_CANCELLED;

			CurrentIrp() = NULL;

			m_ReadFifo.Unlock();
			PnpNextIrp(Current);
		}
	}
	else
	{
		// Release the spin lock for the read fifo
		m_ReadFifo.Unlock();
	}
	
	return I.PnpComplete(this, STATUS_SUCCESS);
}



////////////////////////////////////////////////////////////////////////
// SerialRead (member of Chat1394Device)
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

void Chat1394Device::SerialRead(KIrp I)
{
	t << "Entering Chat1394Device::SerialRead, " << I << EOL;

	// Get a pointer to the caller's buffer
	KMemory IMem(I.Mdl());
	PUCHAR	pBuffer = (PUCHAR) IMem.MapToSystemSpace();

	// Acquire the spin lock for the read fifo
	m_ReadFifo.Lock();

	// Do a read of the read fifo to fill what we can of this Irp
	ULONG BytesRead = m_ReadFifo.Read(pBuffer, I.ReadSize(CURRENT));

	// If there was anything to read, complete the Irp, otherwise the Irp
	// will be completed the next time a write is done to our address range
	if (BytesRead)
	{
		// Store the number of bytes read in the Irp's information field
		I.Information() = BytesRead;

		// If we had a read failure, complete this Irp with the failure status
		// and clear the failure, otherwise return STATUS_SUCCESS.
		if (m_bReadDataLoss)
		{
			m_bReadDataLoss = FALSE;
			I.Status() = STATUS_DATA_OVERRUN;
		}
		else
			I.Status() = STATUS_SUCCESS;

		// Release the spin lock before calling PnpNextIrp!
		m_ReadFifo.Unlock();
		PnpNextIrp(I);
	}
	else
	{
		// Set the pended flag to TRUE
		m_bReadPended = TRUE;

		// Since this might not complete for a while, make the Irp cancellable
		// again.
		CancelSpinLock::Acquire();
		I.SetCancelRoutine(LinkTo(Cancel));
		CancelSpinLock::Release();

		m_ReadFifo.Unlock();
	}
}

////////////////////////////////////////////////////////////////////////
//Chat1394Device::DataReceived
//
//
// Parameters 
//	pBuffer - A pointer to the characters received
//  Length - The length in bytes of the buffer pointed to by pBuffer
//
// Returns:
//	  None
//
// Comments
//	 Called from Chat1394Client::OnWrite when some data is written to
//    the allocatecd address range.
//

VOID Chat1394Device::DataReceived(PUCHAR pBuffer, ULONG Length)
{
	//Write the data to the read FIFO
	ULONG BytesWritten = m_ReadFifo.LockedWrite(pBuffer, Length);
	if(BytesWritten < Length)
		m_bReadDataLoss = TRUE;

	m_ReadFifo.Lock();

	// If there is a pended Irp, we will satisfy it here so long as the Irp is
	// not being cancelled.
	if (m_bReadPended)
	{

		// Check to see if the current Irp has been canceled. If so, the
		// cancel routine will clear the read pended flag when it gets called.
		CancelSpinLock::Acquire();
		KIrp Current = CurrentIrp();
		if (Current.WasCanceled())
		{
			CancelSpinLock::Release();
			m_ReadFifo.Unlock();
		}
		else
		{
			Current.SetCancelRoutine(NULL);
			CancelSpinLock::Release();

			// Clear the pending flag
			m_bReadPended = FALSE;
			
			//Get the address in the IRP's Mdl
			KMemory Mdl(Current.Mdl());
			PUCHAR	pBuffer = (PUCHAR) Mdl.MapToSystemSpace();

			// Do a read of the read fifo to fill in the MDL buffer
			Current.Information() = m_ReadFifo.Read(pBuffer, Current.ReadSize(CURRENT));

			// If we had a read failure, complete this Irp with the failure status
			// and clear the failure, otherwise return STATUS_SUCCESS.
			if (m_bReadDataLoss)
			{
				m_bReadDataLoss = FALSE;
				Current.Status() = STATUS_DATA_OVERRUN;
			}
			else
				Current.Status() = STATUS_SUCCESS;

			m_ReadFifo.Unlock();

			PnpNextIrp(Current);
		}
	}
	else
		m_ReadFifo.Unlock();
}


////////////////////////////////////////////////////////////////////////
// Read (member of Chat1394Device)
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

NTSTATUS Chat1394Device::Read(KIrp I) 
{
	t << "Entering Chat1394Device::Read, " << I << EOL;

    if (FALSE)		
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
// Write (member of Chat1394Device)
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
//		This routine handles write requests.  Multiple requests may
//      be received simultaneously.  
//
//      In the normal case, this allocates an IRB and passes it to the
//      lower device.  CompleteWriteIrp is called when the bus driver 
//      completes the IRP.
//

NTSTATUS Chat1394Device::Write(KIrp I) 
{
	t << "Entering Chat1394Device::Write, " << I << EOL;

	//Can't handle buffers larger than the address range size
	if(I.WriteSize() > ADDRESS_RANGE_BUFFER_SIZE)
	{
		I.Information() = 0;
		return I.PnpComplete(this, STATUS_INVALID_PARAMETER);
	}

	// Always ok to write 0 elements.
	if (I.WriteSize() == 0)
	{
		I.Information() = 0;
		return I.PnpComplete(this, STATUS_SUCCESS);
	}

	PWRITE_COMPLETE_CONTEXT pContext = new (NonPagedPool) WRITE_COMPLETE_CONTEXT;
	if(!pContext)
		return STATUS_INSUFFICIENT_RESOURCES;

	//The normal case: Build an IRB.
	KIrb<REQUEST_ASYNC_WRITE> *pIrb = 
		new KIrb<REQUEST_ASYNC_WRITE>; 

	if(!pIrb)
	{
		delete pContext;
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	pIrb->Initialize(m_GenerationCount, 1, 0, I.Mdl());

	//Set up the context for the completion routine
	pContext->pThis = this;
	pContext->pIrb = pIrb;
	pContext->pIrp = I;

	//submit the IRB to the lower device
	I.MarkPending();
	m_Lower.SubmitIrb(pIrb, CompleteWriteIrp, pContext);
	return STATUS_PENDING;  //The IRP will complete in CompleteWriteIrp
}

////////////////////////////////////////////////////////////////////////
// CompleteWriteIrp (member of Chat1394Device)
//
//		Completion routine for write Irps
//
// Input
//		DeviceObject - Pointer to the system device object
//      pIrp         - Pointer to the IRP just completed
//		pContext     - the WRITE_COMPLETE_CONTEXT we passed to SubmitIrb
//
// Output
//		NTSTATUS	Result code
//
// Notes
//	 Cleans up the IRB and context allocated in Write, and completes
//	  the IRP
//
NTSTATUS Chat1394Device::CompleteWriteIrp(
		PDEVICE_OBJECT	DeviceObject,
		PIRP			pIrp,
		PVOID			Context
		)
{
	PWRITE_COMPLETE_CONTEXT pContext = (PWRITE_COMPLETE_CONTEXT) Context;
	KIrp I(pContext->pIrp);

	//Complete the IRP
	I.Information() = I.WriteSize(CURRENT);
	I.PnpComplete(pContext->pThis, STATUS_SUCCESS);

	//Clean up
	delete pContext->pIrb;
	delete pContext;
	
	return STATUS_SUCCESS;
}

