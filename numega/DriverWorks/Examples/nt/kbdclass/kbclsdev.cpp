// kbclsdev.cpp - device class implementation for keyboard class driver
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
#include <ntddkbd.h>

#include "kbdlog.h"
#include "kbdmou.h"
#include "kbclsdev.h"

/////////////////////////////////////////////////////////////////////////////
// Begin INIT section code
#pragma code_seg("INIT")

/////////////////////////////////////////////////////////////////////////////
// Constructor
//
KbdclassDevice::KbdclassDevice(PWSTR Name, ULONG QueueSize, ULONG NumberOfPorts) :
	KDevice(Name, FILE_DEVICE_KEYBOARD, NULL, 0, DO_BUFFERED_IO),
	m_Queue(QueueSize, NonPagedPool),
	m_Lock()
{

	// Make sure construction of base class succeeded
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
#if DBG
		DbgPrint("KBDCLASS: failed to create device object: %x\n", m_ConstructorStatus);
#endif
		return;
	}

	// Initialize data members
	m_RequestIsPending = FALSE;
	m_CleanupWasInitiated = FALSE;
	m_TrustedSubsystemConnected = FALSE;
	m_NumberOfPorts = NumberOfPorts;
	m_OkayToLogOverflow = TRUE;
	m_ErrorLogSeqNumber = 0;

	// Make sure the queue object was successfully constructed. If not, log an error.
	if ( !m_Queue.IsValid() )
	{
		KErrorLogEntry::SimpleError(
			STATUS_INSUFFICIENT_RESOURCES,
			KBDCLASS_NO_BUFFER_ALLOCATED,
			KEYBOARD_ERROR_VALUE_BASE + 20,
			this);

		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

	// Allocate an array of pointers to port devices. Log an error if it fails.
	m_Ports = new(NonPagedPool) KLowerDevice*[NumberOfPorts];
	if (m_Ports == NULL)
	{
		ULONG RequiredSize = sizeof(KLowerDevice*)*NumberOfPorts;

		KErrorLogEntry::SimpleError(
			STATUS_INSUFFICIENT_RESOURCES,
			KBDCLASS_NO_BUFFER_ALLOCATED,
			KEYBOARD_ERROR_VALUE_BASE + 10,
			this);

		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		m_NumberOfPorts = 0;
		return;
	}
	else
		RtlZeroMemory(m_Ports, sizeof(KLowerDevice*)*NumberOfPorts);

	m_ConstructorStatus = STATUS_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
// ConnectToPort
//
// Creates an instance of KLowerDevice for the named port, and stores a 
// pointer to it in m_ports[]. Then sends an IOCTL to the port to connect
// it.
//
NTSTATUS KbdclassDevice::ConnectToPort(PUNICODE_STRING PortName)
{
	ULONG i;
	IO_STATUS_BLOCK IoStatus;
	NTSTATUS status;

	// Find the first free (zero) entry in the array of pointers to port devices
	for (i = 0; i < m_NumberOfPorts; i++)
		if (m_Ports[i] == 0)
			break;

	// Fail if none free
	if (i == m_NumberOfPorts) 
		return STATUS_INSUFFICIENT_RESOURCES;

	// Create a lower device object
	m_Ports[i]  = new(NonPagedPool) 
		KLowerDevice(PortName, FILE_READ_ATTRIBUTES);

	// Fail if unable to create it
	if (m_Ports[i] == NULL) 
		return STATUS_INSUFFICIENT_RESOURCES;

	// Set our StackSize to accommodate this device
	ReserveIrpStackLocation(m_Ports[i]);

	// Send an IOCTL to the device to connect it
	CONNECT_DATA ConnectData;
	ULONG Info;

	ConnectData.ClassDeviceObject = m_pDeviceObject;
	ConnectData.ClassService = PortServiceCallbackDispatch;

	return m_Ports[i]->DeviceIoControl(
			IOCTL_INTERNAL_KEYBOARD_CONNECT,
			&ConnectData,
			sizeof(CONNECT_DATA),
			NULL,
			0,
			TRUE,
			&Info
			);
}
#pragma code_seg()
// End INIT section code
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Destructor
//
KbdclassDevice::~KbdclassDevice()
{

	// Release all ports
	if (m_Ports)
	{
		for (ULONG i = 0; i < m_NumberOfPorts; i++)
			if (m_Ports[i])
				delete m_Ports[i];	

		delete m_Ports;
	}
}
		
/////////////////////////////////////////////////////////////////////////////
// StartIo
//
// Unlike most higher and intermediate drivers, the keyboard class driver
// serializes read IRPs.
//
VOID KbdclassDevice::StartIo(KIrp I)
{
	KIRQL CancelIrql;
	ULONG nRead;

	// Bump the counter
	m_ErrorLogSeqNumber++;

	// If the IRP was canceled, bail out
	m_Lock._lock(TRUE);
	CancelIrql = CancelSpinLock::Acquire();

	if (I->Cancel || m_CleanupWasInitiated)
	{
		CancelSpinLock::Release(CancelIrql);
		m_Lock._unlock(DISPATCH_LEVEL);
		return;
	}

	// Check if there is keyboard data from the port driver ready to be read.
	if ( !m_Queue.IsEmpty() )
	{

		// If so, read it out of the queue and complete the IRP.
		I.SetCancelRoutine(NULL);
		CurrentIrp() = NULL;
		CancelSpinLock::Release(CancelIrql);

		nRead = m_Queue.Read(
			(PKEYBOARD_INPUT_DATA)I.BufferedReadDest(), 
			I.ReadSize()/sizeof(KEYBOARD_INPUT_DATA));

		ULONG BytesTransferred = nRead*sizeof(KEYBOARD_INPUT_DATA);

		m_RequestIsPending = FALSE;
		I.Information() = BytesTransferred;
		I.ReadSize() = BytesTransferred;

		m_Lock._unlock(DISPATCH_LEVEL);

		I.Status() = STATUS_SUCCESS;
		NextIrp(I, IO_NO_INCREMENT, TRUE);
	}
	else
	{

		// Otherwise, let it pend.
		m_RequestIsPending = TRUE;
		CancelSpinLock::Release(CancelIrql);
		m_Lock._unlock(DISPATCH_LEVEL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Create
//
// This gets called when the Win32 subsystem opens the device during initializaion.
//
NTSTATUS KbdclassDevice::Create(KIrp I)
{
#if NTVERSION > 351
	LUID priv;
#else
	LARGE_INTEGER priv;
#endif
	priv.HighPart = 0;
	priv.LowPart = SE_TCB_PRIVILEGE;

	// Check if the caller is privileged. If so, set the file object
	// context to 1 to flag an authorized reader.
	if (SeSinglePrivilegeCheck(priv, I->RequestorMode)) 
	{
		m_Lock._lock(FALSE);

		if (!m_TrustedSubsystemConnected) 
		{
		    m_CleanupWasInitiated = FALSE;
		    I.FileObject()->FsContext = (PVOID) 1;
	   		m_TrustedSubsystemConnected = TRUE;
		}

		m_Lock._unlock(PASSIVE_LEVEL);
	}

	// Try to enable all the ports.
	I.Information()=0;
	return I.Complete(EnableDisableAllPorts(TRUE));
}

/////////////////////////////////////////////////////////////////////////////
// Close
//
// Called when the Win32 subsystem closes the device
//
NTSTATUS KbdclassDevice::Close(KIrp I)
{
	m_Lock._lock(FALSE);

	// Verify that the caller is privileged. 
	if (I.FileObject()->FsContext != 0)
   		m_TrustedSubsystemConnected = FALSE;

	m_Lock._unlock(PASSIVE_LEVEL);

	I.Information()=0;
	return I.Complete(EnableDisableAllPorts(FALSE));
}

/////////////////////////////////////////////////////////////////////////////
// CleanUp
//
// Flush all the IRPs for the specified file object
//
NTSTATUS KbdclassDevice::CleanUp(KIrp I)
{
	KDeviceQueue dq(DeviceQueue());

	m_Lock._lock(FALSE);
	CancelSpinLock::Acquire();

	if (I.FileObject()->FsContext)
	{
		m_CleanupWasInitiated = TRUE;
		KIrp J = CurrentIrp();
		CurrentIrp() = NULL;


		while (! J.IsNull() )
		{
			J.SetCancelRoutine(NULL);
			J.Status() = STATUS_CANCELLED;
			J.Information() = 0;

			CancelSpinLock::Release();
			m_Lock._unlock(PASSIVE_LEVEL);
			J.Complete(STATUS_CANCELLED);

			m_Lock._lock(FALSE);
			CancelSpinLock::Acquire();

			J = KIrp(dq.RemoveHead());
		}
	}

	CancelSpinLock::Release();
	m_Lock._unlock(PASSIVE_LEVEL);

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
// DeviceControl
//
NTSTATUS KbdclassDevice::DeviceControl(KIrp I)
{
	ULONG Unit;
	NTSTATUS status;

	// DeviceIoControl calls are passed through to one of the port drivers. If
	// the size of the input buffer is zero, port 0 is assumed. Otherwise, the
	// unit is passed in the input buffer.
	if (I.IoctlInputBufferSize() == 0)
	{

		// Default unit number is zero
		Unit = 0;
		status = STATUS_SUCCESS;
	}
	else if (I.IoctlInputBufferSize() < sizeof(KEYBOARD_UNIT_ID_PARAMETER))
		status = STATUS_BUFFER_TOO_SMALL;
	else
	{

		// Get the unit number from the input buffer
		Unit = ((PKEYBOARD_UNIT_ID_PARAMETER)I.IoctlBuffer())->UnitId;

		// Verify that it is in bounds
		if ( (Unit > m_NumberOfPorts)  || (m_Ports[Unit] == NULL) )
			status = STATUS_INVALID_PARAMETER;			
		else
			status = STATUS_SUCCESS;		
	}

	// If we have a valid unit number, pass the IRP down as an internal Ioctl.
	if (status == STATUS_SUCCESS)
	{
		I.CopyParametersDown();
		I.MajorFunction(NEXT) = IRP_MJ_INTERNAL_DEVICE_CONTROL;
		I.MarkPending();

		return m_Ports[Unit]->Call(I);
	}
	// Otherwise, fail it with the already determined error status.
	else
	{
		I.Information() = 0;
		return I.Complete(status);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Read
//
// This is where the device delivers data from the port driver to the Win32
// subsystem.
//
NTSTATUS KbdclassDevice::Read(KIrp I)
{
	NTSTATUS status;
	ULONG nbytes = I.ReadSize();

	// Make sure the buffer is an exact multiple of the record size
	if ( (nbytes % sizeof(KEYBOARD_INPUT_DATA)) != 0 )
		status = STATUS_BUFFER_TOO_SMALL;

	// Make sure the reader is privileged
	else if (I.FileObject()->FsContext == 0)
		status = STATUS_PRIVILEGE_NOT_HELD;

	// Make sure the input buffer size is not zero, and queue the IRP to StartIo
	else if ( nbytes != 0 )
		status = QueueIrp(I, LinkTo(CancelRead)); // returns STATUS_PENDING

	else
	{
		I.Information() = 0;
		status =  I.Complete(STATUS_SUCCESS);
	}

	return status;			
}

/////////////////////////////////////////////////////////////////////////////
// FlushBuffers
//
NTSTATUS KbdclassDevice::FlushBuffers(KIrp I)
{
	m_Lock._lock(FALSE);
	m_Queue.Flush();
	m_OkayToLogOverflow = TRUE;
	m_Lock._unlock(PASSIVE_LEVEL);

	I.Information() = 0;

	return I.Complete(STATUS_SUCCESS);
}

/////////////////////////////////////////////////////////////////////
// CancelRead
//
VOID KbdclassDevice::CancelRead(KIrp I)
{
	KDeviceQueue dq(DeviceQueue());
	CancelSpinLock::Release(I.CancelIrql());
	m_Lock.Lock();
	I.Information() = 0;

	if (m_RequestIsPending && ( I.m_Irp == CurrentIrp() ) )
	{
		CurrentIrp() = NULL;
		m_RequestIsPending = FALSE;
		m_Lock.Unlock();

		I.Status() = STATUS_CANCELLED;
		NextIrp(I, IO_NO_INCREMENT, TRUE);
	}
	else
	{
		CancelSpinLock::Acquire();

		if (!dq.RemoveSpecificEntry(I.m_Irp))
		{
#if DBG
			DbgPrint(
				"KBDCLASS-KeyboardClassCancel: Irp 0x%p not in device queue?!?\n",
				I.m_Irp
				);
#endif
		}

		CancelSpinLock::Release();
		m_Lock.Unlock();
		I.Complete(STATUS_CANCELLED);
	}
}

/////////////////////////////////////////////////////////////////////////////
// EnableDisableAllPorts
//
// Loop through the port list and enable or disable each one
//
NTSTATUS KbdclassDevice::EnableDisableAllPorts(BOOLEAN enable)
{
	NTSTATUS status;
	BOOLEAN AtLeastOneSuccess = FALSE;
	KLowerDevice** ppPort;
	ULONG i;

	for (i=0, ppPort=m_Ports; i < m_NumberOfPorts; i++, ppPort++)
	{
		if (*ppPort == NULL)
			continue;

		status = EnableDisablePort(*ppPort, enable);
		if ( NT_SUCCESS(status) )
			AtLeastOneSuccess = TRUE;
		else
			KErrorLogEntry::SimpleError(
				status,
				enable ? KBDCLASS_PORT_INTERRUPTS_NOT_ENABLED:
				         KBDCLASS_PORT_INTERRUPTS_NOT_DISABLED,
				KEYBOARD_ERROR_VALUE_BASE + 120,
				this);
	}

	if (AtLeastOneSuccess)
		return STATUS_SUCCESS;
	else
		return status;
}

/////////////////////////////////////////////////////////////////////////////
// EnableDisablePort
//
// Sends an IOCTL to the port to enable or disable it, based on the input
// parameter
//
NTSTATUS KbdclassDevice::EnableDisablePort(
	KLowerDevice* port, 
	BOOLEAN enable)
{
	ULONG IoctlCode;
	ULONG Info;

	if (enable)
		IoctlCode = IOCTL_INTERNAL_KEYBOARD_ENABLE;
	else
		IoctlCode = IOCTL_INTERNAL_KEYBOARD_DISABLE;
		
	return port->DeviceIoControl(IoctlCode, NULL, 0, NULL, 0, TRUE,	&Info);
}

/////////////////////////////////////////////////////////////////////////////
// PortServiceCallbackDispatch
//
// Called by the port driver when it has data to deliver. This routine dispatches
// to the correct instance of the device class. 
//
VOID KbdclassDevice::PortServiceCallbackDispatch(
	IN PDEVICE_OBJECT DeviceObject,
	IN PKEYBOARD_INPUT_DATA InputDataStart,
	IN PKEYBOARD_INPUT_DATA InputDataEnd,
	IN OUT PULONG InputDataConsumed)
{
	((KbdclassDevice*)KDevicePTR(DeviceObject))->PortServiceCallback(
		InputDataStart,
		InputDataEnd,
		InputDataConsumed
		);
}

/////////////////////////////////////////////////////////////////////////////
// PortServiceCallback
//
// This routine is called by the port driver (via the above dispatch routine).
// It is responsible for picking up data from the port driver, and either
// using that data to complete a pending IRP, or storing in the output queue
// for later retrieval.
//
VOID KbdclassDevice::PortServiceCallback(
	IN PKEYBOARD_INPUT_DATA InputDataStart,
	IN PKEYBOARD_INPUT_DATA InputDataEnd,
	IN OUT PULONG InputDataConsumed)
{
	ULONG nItemsMoved;
	ULONG nItemsAvailable = InputDataEnd - InputDataStart;
	ULONG nItemsInReadRequest;
	ULONG nBytesMoved;
	KIrp I(NULL);

	*InputDataConsumed = 0;

	m_Lock._lock(TRUE);

	// Check if there is a pending IRP
	if (m_RequestIsPending)
	{
		CancelSpinLock::Acquire();

		I = CurrentIrp();
		I.SetCancelRoutine(NULL);
		CurrentIrp() = NULL;

		CancelSpinLock::Release();
		m_RequestIsPending = FALSE;

		// Move data from the port's buffer into the IRP output buffer
		nItemsInReadRequest = I.ReadSize()/sizeof(KEYBOARD_INPUT_DATA);
		nItemsMoved = _min_(nItemsInReadRequest,nItemsAvailable);
		nBytesMoved = nItemsMoved * sizeof(KEYBOARD_INPUT_DATA);

		RtlMoveMemory(I.BufferedReadDest(), InputDataStart, nBytesMoved);

		// There may be additional data that cannot be sent with this IRP.
		// This must be copied to the output queue.
		InputDataStart += nItemsMoved;
		*InputDataConsumed += nItemsMoved;
		nItemsAvailable -= nItemsMoved;

		I.Status() = STATUS_SUCCESS;
		I.Information() = nBytesMoved;
		I.ReadSize()  = nBytesMoved;
	}

	// Write data to queue, if any
	if (nItemsAvailable != 0)
	{

		// Is the queue full? If so, log an event.
		if (m_Queue.IsFull())
		{
			if (m_OkayToLogOverflow)
			{
				struct OverflowData_t od;
				od.BytesIn = nItemsAvailable*sizeof(KEYBOARD_INPUT_DATA);
				od.QueueSize = m_Queue.BufferItemCapacity()*sizeof(KEYBOARD_INPUT_DATA);

				KErrorLogEntry E; PIO_ERROR_LOG_PACKET p = E;

				p->FinalStatus = STATUS_SUCCESS;
				p->ErrorCode = KBDCLASS_KBD_BUFFER_OVERFLOW;
				p->UniqueErrorValue = KEYBOARD_ERROR_VALUE_BASE + 210;

				E.SetDumpData(&od, sizeof(od));
				E.Post();

				m_OkayToLogOverflow = FALSE;
			}
		}	
		// If the queue is not full, write out the data
		else 
			*InputDataConsumed += 
				m_Queue.Write(InputDataStart, nItemsAvailable);
	}	

	m_Lock._unlock(DISPATCH_LEVEL);

	// If an IRP was satisfied, complete it now, and get the next IRP.
	if ( !I.IsNull() )
		NextIrp(I, IO_NO_INCREMENT, TRUE);
}
