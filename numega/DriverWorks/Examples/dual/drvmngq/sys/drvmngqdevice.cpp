// DrvmngqDevice.cpp
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
// Implementation of DrvmngqDevice device class

#include <vdw.h>

#include "Drvmngq.h"
#include "DrvmngqDevice.h"
#include "..\drvmngqioctl.h"

#pragma warning(disable:4065) // Allow switch statement with no cases

extern	KTrace	t;			// Global driver trace object

////////////////////////////////////////////////////////////////////////
// DrvmngqDevice Destructor
//
//		This is the destructor for the DrvmngqDevice
//
// Input
//		None
//
// Output
//		None
//
// Notes
//		Disconnect and release resources here.
//
//		Although the use of SAFE_DESTRUCTORS in the class definition cures
//		improper emission of the destructor into the INIT section most of the
//		time, certain rare cases can still cause improper behavior.  To avoid
//		these cases, the destructor	must preceed the INIT section, causing it
//		to be referenced first by the default compiler section.
//
DrvmngqDevice::~DrvmngqDevice()
{
}

#if ! _WDM_ // THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS AN NT4 DRIVER

#pragma code_seg("INIT")

////////////////////////////////////////////////////////////////////////////////
// DrvmngqDevice Constructor
//
//		The device constructor is typically responsible for allocating
//		any physical resources that are associated with the device.
//
// Input
//		Unit	Unit number - this is a number to append to the device's
//				base device name to distinguish multiple units of this
//				device type.
//
// Output
//		None
//
// Notes
//		The device constructor often reads the registry to setup
//		various configurable parameters.

DrvmngqDevice::DrvmngqDevice(ULONG Unit) :
	KDevice(),
	m_TimedCallback(),
	m_TimedCallbackCancelable(),
	m_CancelSync(),
	m_WaitForCancel(),
	m_CancelFinished(SynchronizationEvent),
	m_DriverManagedQueue(),
	m_TimerIntervalMsecs(1000),
	m_QueueCancelable(TRUE),
	m_DispatchIrqlStartIo(TRUE)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device DrvmngqDevice unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}
	m_Unit = Unit;

	// Load data members from the registry
	LoadRegistryParameters();

#if _WDM_
	m_WaitForCancel.Initialize(*this);
	m_DriverManagedQueue.Initialize(*this);
#else
	m_WaitForCancel.Initialize(LinkTo(WaitForCancel), this);

	// IRPs are normally queued in a cancelable state.  The driver managed queue
	// class supports overriding this default behavior.  Of course this should only
	// be done if the IRP will not stay on the queue long.
	//
	// Set the driver managed queue to either queue IRPs as cancelable
	// or not depending on value set in registry
	m_DriverManagedQueue.SetQueueCancelable(static_cast<BOOLEAN>(m_QueueCancelable));
#endif

	// StartIo is normally called at DISPATCH_LEVEL.  The driver managed queue
	// class supports overriding this default behavior.  This is useful if StartIo
	// must call a function that cannot be called at IRQL DISPATCH_LEVEL and a work
	// queue item is not used.  This driver does make use of this to open a window
	// wherin the IRP can be canceled
	//
	// Set the driver managed queue to either run the StartIo queue at DISPATCH_LEVEL
	// or below depending on value set in registry
	m_DriverManagedQueue.SetDispatchIrqlStartIo(static_cast<BOOLEAN>(m_DispatchIrqlStartIo));

}

//////////////////////////////////////////////////////////////////////
// End INIT section code
//
#pragma code_seg()

#else // ! _WDM_ THE FOLLOWING SECTION ONLY PERTAINS TO BUILDING AS A WDM DRIVER

////////////////////////////////////////////////////////////////////////////////
// DrvmngqDevice Constructor
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

DrvmngqDevice::DrvmngqDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :

	KPnpDevice(Pdo, NULL),
	m_TimedCallback(),
	m_TimedCallbackCancelable(),
	m_CancelSync(),
	m_WaitForCancel(),
	m_CancelFinished(SynchronizationEvent),
	m_DriverManagedQueue(),
	m_TimerIntervalMsecs(1000),
	m_QueueCancelable(TRUE),
	m_DispatchIrqlStartIo(TRUE)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device DrvmngqDevice unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}
	m_Unit = Unit;

	// Load data members from the registry
	LoadRegistryParameters();

#if _WDM_
	m_WaitForCancel.Initialize(Pdo);
	m_DriverManagedQueue.Initialize(Pdo);
#else
	m_WaitForCancel.Initialize(LinkTo(WaitForCancel), this);

	// IRPs are normally queued in a cancelable state.  The driver managed queue
	// class supports overriding this default behavior.  Of course this should only
	// be done if the IRP will not stay on the queue long.
	//
	// Set the driver managed queue to either queue IRPs as cancelable
	// or not depending on value set in registry
	m_DriverManagedQueue.SetQueueCancelable(static_cast<BOOLEAN>(m_QueueCancelable));
#endif

	// StartIo is normally called at DISPATCH_LEVEL.  The driver managed queue
	// class supports overriding this default behavior.  This is useful if StartIo
	// must call a function that cannot be called at IRQL DISPATCH_LEVEL and a work
	// queue item is not used.  This driver does make use of this to open a window
	// wherin the IRP can be canceled
	//
	// Set the driver managed queue to either run the StartIo queue at DISPATCH_LEVEL
	// or below depending on value set in registry
	m_DriverManagedQueue.SetDispatchIrqlStartIo(static_cast<BOOLEAN>(m_DispatchIrqlStartIo));

	// Initialize the lower device
	m_Lower.Initialize(this, Pdo);

    // Inform the base class of the lower edge device object
	SetLowerDevice(&m_Lower);

	// Initialize the PnP Policy settings to the "standard" policy
	SetPnpPolicy();
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
		"<unknown minor function>",
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
// DefaultPnp (member of DrvmngqDevice)
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

NTSTATUS DrvmngqDevice::DefaultPnp(KIrp I)
{
	t << "Entering DrvmngqDevice::DefaultPnp with IRP minor function="
	  << PNPMinorFunctionName(I.MinorFunction()) << EOL;

	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}


////////////////////////////////////////////////////////////////////////
// DefaultPower (member of DrvmngqDevice)
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

NTSTATUS DrvmngqDevice::DefaultPower(KIrp I)
{
	t << "Entering DrvmngqDevice::DefaultPower\n";

	I.IndicatePowerIrpProcessed();
	I.CopyParametersDown();
	return m_Lower.PnpPowerCall(this, I);
}

////////////////////////////////////////////////////////////////////////////////
// SystemControl
//
// This routine just passes the IRP through to the next device since this driver
// is not a WMI provider.
//
NTSTATUS DrvmngqDevice::SystemControl(KIrp I)
{
	I.ForceReuseOfCurrentStackLocationInCalldown();
	return m_Lower.PnpCall(this, I);
}

////////////////////////////////////////////////////////////////////////
// OnStartDevice (member of DrvmngqDevice)
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
//		Initialize the physical device.  Typically, the driver initializes
//		physical resources here.  Call I.AllocatedResources() for a list
//		of the raw resources that the system has assigned to the device,
//		or I.TranslatedResources() for the translated resource list.

NTSTATUS DrvmngqDevice::OnStartDevice(KIrp I)
{
	t << "Entering DrvmngqDevice::OnStartDevice\n";

   NTSTATUS status = STATUS_SUCCESS;

	I.Information() = 0;

	// The default Pnp policy has already cleared the IRP with the lower device
	// We have no physical device to initialize.
    // The base class will handle completion

	return status;
}


////////////////////////////////////////////////////////////////////////
// OnStopDevice (member of DrvmngqDevice)
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

NTSTATUS DrvmngqDevice::OnStopDevice(KIrp I)
{
	NTSTATUS status = STATUS_SUCCESS;

	t << "Entering DrvmngqDevice::OnStopDevice\n";

	return status;

	// The following macro simply allows compilation at Warning Level 4
	UNREFERENCED_PARAMETER(I);
}


////////////////////////////////////////////////////////////////////////
// OnRemoveDevice (member of DrvmngqDevice)
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

NTSTATUS DrvmngqDevice::OnRemoveDevice(KIrp I)
{
	t << "Entering DrvmngqDevice::OnRemoveDevice\n";

	// Nothing specific to do, all PnP operations handled by base class

	return STATUS_SUCCESS;

	// The following macro simply allows compilation at Warning Level 4
	UNREFERENCED_PARAMETER(I);
}

#endif // ! _WDM_

////////////////////////////////////////////////////////////////////////
// LoadRegistryParams
//
//		Load parameters from the registry and store them in class
//		member variables.
//
// Input
//		None
//
// Output
//		Member variables are updated with values read from registry
//
// Notes
//		The registry variables we load are actually global to the driver
//		and are are loaded from the registry path:
//		HKEY_LOCAL_MACHINE\
//		  SYSTEM\
//		    CurrentControlSet\
//			  Services\
//			    Drvmngq\
//				  Parameters\
//				    <param name>
//
void DrvmngqDevice::LoadRegistryParameters(void)
{


	KRegistryKey Params((KDriver::DriverInstance())->RegistryPath(), L"\\Parameters");
	if (!NT_SUCCESS(Params.LastError()))
	{
		// Error, cannot open device registry key
		t << "Failed to create registry key\n";
		m_ConstructorStatus = STATUS_UNSUCCESSFUL;
		return;
	}

	// Load the registry parameters
	m_TimerIntervalMsecs = 1000;
	Params.QueryValue(L"TimerIntervalMsecs", &m_TimerIntervalMsecs);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_TimerIntervalMsecs is default value: [" << m_TimerIntervalMsecs << "]\n";
	else
		t << "m_TimerIntervalMsecs loaded from registry, resulting value: [" << m_TimerIntervalMsecs << "]\n";

	m_QueueCancelable = TRUE;
	Params.QueryValue(L"QueueCancelable", &m_QueueCancelable);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_QueueCancelable is default value: [" << m_QueueCancelable << "]\n";
	else
		t << "m_QueueCancelable loaded from registry, resulting value: [" << m_QueueCancelable << "]\n";

	m_DispatchIrqlStartIo = TRUE;
	Params.QueryValue(L"DispatchIrqlStartIo", &m_DispatchIrqlStartIo);
	if (!NT_SUCCESS(Params.LastError()))
		t << "Registry key load failed, m_DispatchIrqlStartIo is default value: [" << m_DispatchIrqlStartIo << "]\n";
	else
		t << "m_DispatchIrqlStartIo loaded from registry, resulting value: [" << m_DispatchIrqlStartIo << "]\n";
}

////////////////////////////////////////////////////////////////////////
// SerialDeviceControl (member of DrvmngqDevice)
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
//		queue.  This guarantees that multiple requests are
//		never processed simultaneously.

VOID DrvmngqDevice::SerialDeviceControl(KIrp I)
{
	NTSTATUS status;
	t << "Entering DrvmngqDevice::SerialDeviceControl, " << I;
    switch (I.IoctlCode())
	{
		case DRVMNGQ_IOCTL_801_QUEUE:
			status = DRVMNGQ_IOCTL_801_QUEUE_Handler(I);
			break;
		case DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE:
			status = DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE_Handler(I);
			break;
		default:
			// Unrecognized request
			status = STATUS_INVALID_PARAMETER;
			break;
	}

	// If the IOCTL handler routine returned anything other than
	// STATUS_PENDING, complete this IRP and start processing the next one.
	// If the handler returned STATUS_PENDING, we cannot continue to the next
	// IRP, since the current IRP is not finished.  It is up to the driver
	// to complete the IRP later and start processing the next IRP.
	if (status != STATUS_PENDING)
	{
		I.Status() = status;

		// NextIrp completes this IRP and starts processing
		// for the next IRP in the driver managed queue.
		m_DriverManagedQueue.DMQ_NEXT_IRP(I);
	}
}

////////////////////////////////////////////////////////////////////////
// DeviceControl (member of DrvmngqDevice)
//
//		Handler for IRP_MJ_DEVICE_CONTROL
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes:
//		This routine is the first handler for Device Control requests.
//		Some function codes may be handled immediately,
//		while others may be serialized.
//

NTSTATUS DrvmngqDevice::DeviceControl(KIrp I)
{
	NTSTATUS status;

	t << "Entering DrvmngqDevice::Device Control, " << I;
	switch (I.IoctlCode())
	{
		case DRVMNGQ_IOCTL_800_IMMEDIATE:
		    status = DRVMNGQ_IOCTL_800_IMMEDIATE_Handler(I);
			break;

		case DRVMNGQ_IOCTL_801_QUEUE:
		case DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE:

			// Queue this request for serialized handling to the driver
			// managed queue.  Use a work item to do this so that the
			// dispatching thread can return to user mode.  Normally we
			// wouldn't do this, but we are trying to thoroughly test
			// the driver managed queue class, and this allows
			// a race to occur between the IRP being canceled (by the user
			// thread), and being queued by the work item thread.
			{

				// Allocate a work item off the heap
#if _WDM_
				status = m_DriverManagedQueue.QueueIrp(I);
#else
				QueueRequestWorkItem* pItem = new (NonPagedPool)
					QueueRequestWorkItem( LinkTo(QueueRequest), this, I );

				if (pItem == NULL)
				{

					// If allocation fails, queue in the "normal" fashion.
					status = m_DriverManagedQueue.QueueIrp(I);
				}
				else
				{

					// Set the status to STATUS_PENDING, and mark the Irp pending.
					// Normally, KDriverManagedQueueEx::QueueIrp()
					// would do all of this, but we won't run that code until
					// our work item thread runs, so we will do it here instead.
					I.MarkPending();
					status = STATUS_PENDING;
					pItem->Queue();
				}
#endif
			}
			break;

		default:
			// Unrecognized IOCTL request
			status = STATUS_INVALID_PARAMETER;
			break;
	}

	// If the IRP was queued, or its IOCTL handler deferred processing using some
	// driver specific scheme, the status variable is set to STATUS_PENDING.
	// In this case we simply return that status, and the IRP will be completed
	// later.  Otherwise, complete the IRP using the status returned by the
	// IOCTL handler.
	if (status == STATUS_PENDING)
	{
		return status;
	}
	else
	{
		return I.COMPLETE(status);
	}
}

////////////////////////////////////////////////////////////////////////
// QueueRequest (member of DrvmngqDevice)
//
//		Queues the request to the driver managed queue.
//
// Input
//		pItem		Pointer to our work item
//
// Output
//
// Notes
//		NT version PWORKER_THREAD_ROUTINE callback has 'parameter'
//		WDM version PIO_WORKITEM_ROUTINE callback doesn't have it.
//		Longhorn PIO_WORKITEM_ROUTINE_EX has PIO_WORKITEM IoWorkItem again.
//		This QueueRequest may be revised again for Longhorn for workitem to pass an IRP to driver managed queue.
//

#if _WDM_
#else
VOID DrvmngqDevice::QueueRequest(KWorkItem* pItem)
{
	QueueRequestWorkItem* pQueueRequestItem = static_cast<QueueRequestWorkItem*>(pItem);

	// Open a window so the IRP can be canceled before it is queued.
	if (KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		LARGE_INTEGER Pause; Pause.QuadPart = -10000*50; // 50 msec
		KeDelayExecutionThread(KernelMode, FALSE, &Pause);
	}

	KIRQL Irql = KeGetCurrentIrql();

	m_DriverManagedQueue.QueueIrp(pQueueRequestItem->m_Irp);
	delete pQueueRequestItem;
}
#endif

////////////////////////////////////////////////////////////////////////
//	DRVMNGQ_IOCTL_800_IMMEDIATE_Handler (member of DrvmngqDevice)
//
//		Handler for IO Control Code DRVMNGQ_IOCTL_800_IMMEDIATE
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the DRVMNGQ_IOCTL_800_IMMEDIATE function.
//		This routine runs at passive level.
//

NTSTATUS DrvmngqDevice::DRVMNGQ_IOCTL_800_IMMEDIATE_Handler(KIrp I)
{
	t << "Entering DrvmngqDevice::DRVMNGQ_IOCTL_800_IMMEDIATE_Handler, " << I;

	// Get a pointer to the caller's input buffer
	PIOCTL_DATA_INPUT pInput = (PIOCTL_DATA_INPUT)I.IoctlBuffer();
	// Declare a memory object
	KMemory Mem(I.Mdl());
	// Use the memory object to create a pointer to the caller's output buffer
	PIOCTL_DATA_OUTPUT pOutput = (PIOCTL_DATA_OUTPUT)Mem.MapToSystemSpace();

	// Setup the data in the output buffer
	pOutput->nID = pInput->nID;
	pOutput->handling = DIH_COMPLETED_IN_DISPATCH;
    I.Information() = sizeof(IOCTL_DATA_OUTPUT);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//	DRVMNGQ_IOCTL_801_QUEUE_Handler (member of DrvmngqDevice)
//
//		Handler for IO Control Code DRVMNGQ_IOCTL_801_QUEUE
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the DRVMNGQ_IOCTL_801_QUEUE function.
//		This function was queued, so this
//		handler is serialized with other requests.
//		This routine runs at dispatch level.
//

NTSTATUS DrvmngqDevice::DRVMNGQ_IOCTL_801_QUEUE_Handler(KIrp I)
{
	t << "Entering DrvmngqDevice::DRVMNGQ_IOCTL_801_QUEUE_Handler, " << I;

	// Simulate processing to interrupt.  Schedule the callback to occur
	// in m_TimerIntervalMsecs msecs.
	m_TimedCallback.Set( (LONGLONG) (-10000 * m_TimerIntervalMsecs),
		LinkTo(TimerCallback), (PVOID)this );

	return STATUS_PENDING;
}

////////////////////////////////////////////////////////////////////////
// TimerCallback (member of DrvmngqDevice)
//
//		Completes the current IOCTL IRP and starts the next one
//
// Input
//		arg1		PVOID
//		arg2		PVOID
//
// Output
//
// Notes
//

VOID DrvmngqDevice::TimerCallback(PVOID arg1, PVOID arg2)
{

	// Since the IRP was not cancelable while we were processing it,
	// we don't have to worry about whether or not it has been
	// canceled.  Consequently, the current IRP cannot have changed,
	// so it is safe to obtain our current IRP context here directly
	// from the driver managed queue.
	KIrp I = m_DriverManagedQueue.CurrentIrp();
	t << "Entering DrvmngqDevice::TimerCallback " << I;

	I.Status() = STATUS_SUCCESS;

	// Get a pointer to the caller's input buffer
	PIOCTL_DATA_INPUT pInput = (PIOCTL_DATA_INPUT)I.IoctlBuffer();
	// Declare a memory object
	KMemory Mem(I.Mdl());
	// Use the memory object to create a pointer to the caller's output buffer
	PIOCTL_DATA_OUTPUT pOutput = (PIOCTL_DATA_OUTPUT)Mem.MapToSystemSpace();

	// Setup the data in the output buffer
	pOutput->nID = pInput->nID;
	pOutput->handling = DIH_COMPLETED_IN_CALLBACK;
    I.Information() = sizeof(IOCTL_DATA_OUTPUT);
	m_DriverManagedQueue.DMQ_NEXT_IRP(I);
}

////////////////////////////////////////////////////////////////////////
//	DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE_Handler (member of DrvmngqDevice)
//
//		Handler for IO Control Code DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE
//
// Input
//		I			IRP containing IOCTL request
//
// Output
//		NTSTATUS	Status code indicating success or failure
//
// Note
//		This routine implements the DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE function.
//		This function was queued, so this
//		handler is serialized with other requests.
//		This routine runs at dispatch level.
//

NTSTATUS DrvmngqDevice::DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE_Handler(KIrp I)
{
	t << "Entering DrvmngqDevice::DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE_Handler, " << I;

	// Open a window so the IRP can be canceled while in StartIo.  Note that this
	// will only happen when the queue is running StartIo at IRQL less than DISPATCH_LEVEL.
	if (KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		LARGE_INTEGER Pause; Pause.QuadPart = -10000*50; // 50 msec
		KeDelayExecutionThread(KernelMode, FALSE, &Pause);
	}

	// Acquire the spinlock m_CancelSync, which we will hold to prevent our cancel
	// routine from running before we are ready.
	m_CancelSync.Lock();

	// Make our IRP cancelable again by setting a new cancel routine on
	// it.
	I.SetCancelRoutine(LinkTo(CancelInProgressIrp));

	// Test to see if our IRP has been canceled.
	if (I.WasCanceled())
	{

		// The IRP has been canceled, we must clear the cancel routine we just set
		// and determine if it has been called.  We do this by checking the
		// return value of I.SetCancelRoutine()
		PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);
		if (pLastCancel == LinkTo(CancelInProgressIrp))
		{

			// If the cancel routine we set was still on set on the IRP,
			// it hasn't been called.  In this case we can dispose of the
			// IRP here.  Release our spinlock, then complete the IRP with
			// a canceled status and start the next IRP on the device.
			// Note that we return STATUS_PENDING from this routine to
			// satisfy our SerialDeviceControl() routine logic.  The
			// ultimate status of the IRP does not depend on this
			// value.
			m_CancelSync.Unlock();
			I.Status() = STATUS_CANCELLED;

			// Get a pointer to the caller's input buffer
			PIOCTL_DATA_INPUT pInput = (PIOCTL_DATA_INPUT)I.IoctlBuffer();
			// Declare a memory object
			KMemory Mem(I.Mdl());
			// Use the memory object to create a pointer to the caller's output buffer
			PIOCTL_DATA_OUTPUT pOutput = (PIOCTL_DATA_OUTPUT)Mem.MapToSystemSpace();

			// Setup the data in the output buffer
			pOutput->nID = pInput->nID;
			pOutput->handling = DIH_CANCELED_IN_START_IO;
			I.Information() = sizeof(IOCTL_DATA_OUTPUT);
			m_DriverManagedQueue.DMQ_NEXT_IRP(I);
			return STATUS_PENDING;
		}
		else
		{

			// If the cancel routine has changed (has been set to NULL by the
			// system), the cancel routine has been called and will run when
			// we release the spinlock.  We must proceed in initiating our
			// action on the device (setting the timer) and let the cancel
			// routine handle cleaning up the device state.
			ASSERT(pLastCancel == NULL);
		}
	}

	// Simulate processing to interrupt.  Schedule the callback to occur
	// in m_TimerIntervalMsecs msecs.
	m_TimedCallbackCancelable.Set( (LONGLONG) -10000 * m_TimerIntervalMsecs,
		LinkTo(TimerCallbackCancelable), (PVOID)this );

	// Release the spinlock, allowing the cancel routine to proceed if called.
	m_CancelSync.Unlock();

	return STATUS_PENDING;
}

////////////////////////////////////////////////////////////////////////
// TimerCallbackCancelable (member of DrvmngqDevice)
//
//		Completes the current IOCTL IRP and starts the next one
//
// Input
//		arg1		PVOID
//		arg2		PVOID
//
// Output
//
// Notes
//

VOID DrvmngqDevice::TimerCallbackCancelable(PVOID arg1, PVOID arg2)
{

	// Even though the IRP was cancelable while we were waiting for
	// the timer callback, we can be sure that if this routine gets
	// called, the driver managed queue's current IRP is still the same
	// one we were processing.  That is because our cancel logic is designed
	// such that this routine will only get called if it is resolving the
	// device state and starting the next IRP on the device.  Otherwise,
	// the routine would not have been called (see CancelInProgressIrp())
	KIrp I = m_DriverManagedQueue.CurrentIrp();
	t << "Entering DrvmngqDevice::TimerCallback " << I;

	// Make the IRP noncancelable.
	PDRIVER_CANCEL pLastCancel = I.SetCancelRoutine(NULL);

	// Test to see if our IRP has been canceled.  Note that this must
	// be done after setting the cancel routine to NULL.
	if (I.WasCanceled())
	{

		// The IRP has been canceled.  We must check to see if our cancel
		// routine was called.  We do this by checking the value returned
		// when we set the cancel routine.
		if (pLastCancel == LinkTo(CancelInProgressIrp))
		{

			// The cancel routine was still set on the IRP when we made it
			// noncancelable.  This means our cancel routine was not called,
			// and that the IRP must have been canceled in the very small
			// window after we made it noncancelable.  Complete the IRP here
			// and start the next IRP on the device.
			I.Status() = STATUS_CANCELLED;

			// Get a pointer to the caller's input buffer
			PIOCTL_DATA_INPUT pInput = (PIOCTL_DATA_INPUT)I.IoctlBuffer();
			// Declare a memory object
			KMemory Mem(I.Mdl());
			// Use the memory object to create a pointer to the caller's output buffer
			PIOCTL_DATA_OUTPUT pOutput = (PIOCTL_DATA_OUTPUT)Mem.MapToSystemSpace();

			// Setup the data in the output buffer
			pOutput->nID = pInput->nID;
			pOutput->handling = DIH_CANCELED_IN_CALLBACK;
			I.Information() = sizeof(IOCTL_DATA_OUTPUT);
			m_DriverManagedQueue.DMQ_NEXT_IRP(I);
		}
		else
		{

			// The cancel routine had already been cleared, indicating that our
			// cancel routine was called.  Normally if this happens, the cancel routine
			// will have succesfully canceled the timer and called NextIrp, so that
			// we never end up here with a cancelled IRP.  There is a window,
			// however, where the cancel routine was unable to cancel the timer since it
			// was already expiring.  We will complete it here after cleaning up
			// our device state.  Since we want all processing to be done, we must wait
			// to make sure the cancel routine has run, otherwise it might try to
			// cancel the timer for the next IRP. In order to make sure the cancel
			// routine has run, we will wait for an event to be signaled in the
			// cancel routine.  Since we are in a DPC at DIRQL we can't wait here,
			// so we will schedule a work item to call NextIrp once the wait has been
			// satisfied.  Note that our work item is a member of the class, and not
			// dynamically allocated.  We must be certain that the work item only
			// gets queued once (otherwise its list entry will be corrupted).  This
			// of course will be the case since we are serializing IRPs, and won't
			// start the next IRP until the work item completes.
#if _WDM_
			m_WaitForCancel.Queue(LinkTo(WaitForCancel), this, CriticalWorkQueue);
#else
			m_WaitForCancel.Queue(CriticalWorkQueue);
#endif
		}
	}
	else
	{

		// The IRP has not been canceled and is now in a noncancelable state.
		// Complete it and start the next IRP on the device.
		I.Status() = STATUS_SUCCESS;

		// Get a pointer to the caller's input buffer
		PIOCTL_DATA_INPUT pInput = (PIOCTL_DATA_INPUT)I.IoctlBuffer();
		// Declare a memory object
		KMemory Mem(I.Mdl());
		// Use the memory object to create a pointer to the caller's output buffer
		PIOCTL_DATA_OUTPUT pOutput = (PIOCTL_DATA_OUTPUT)Mem.MapToSystemSpace();

		// Setup the data in the output buffer
		pOutput->nID = pInput->nID;
		pOutput->handling = DIH_COMPLETED_IN_CALLBACK;
		I.Information() = sizeof(IOCTL_DATA_OUTPUT);
		m_DriverManagedQueue.DMQ_NEXT_IRP(I);
	}
}

////////////////////////////////////////////////////////////////////////
// WaitForCancel (member of DrvmngqDevice)
//
//		Waits for the cancel routine to signal before completing the
//		current IRP in a canceled state and starting the next IRP
//		on the device.
//
// Input
//		pItem		Pointer to our work item
//
// Output
//
// Notes
//

#if _WDM_
VOID DrvmngqDevice::WaitForCancel()
#else
VOID DrvmngqDevice::WaitForCancel(KWorkItem* pItem)
#endif
{

	// Wait for cancel to signal that it is finished.
	m_CancelFinished.Wait();

	// Now that the cancel routine has run, we can complete this IRP and
	// start the next one on the device.  Our IRP to complete is still the
	// current IRP, so use CurrentIrp() to get it.
	KIrp I(m_DriverManagedQueue.CurrentIrp());
	I.Status() = STATUS_CANCELLED;

	// Get a pointer to the caller's input buffer
	PIOCTL_DATA_INPUT pInput = (PIOCTL_DATA_INPUT)I.IoctlBuffer();
	// Declare a memory object
	KMemory Mem(I.Mdl());
	// Use the memory object to create a pointer to the caller's output buffer
	PIOCTL_DATA_OUTPUT pOutput = (PIOCTL_DATA_OUTPUT)Mem.MapToSystemSpace();

	// Setup the data in the output buffer
	pOutput->nID = pInput->nID;
	pOutput->handling = DIH_CANCELED_IN_CALLBACK_WORKQUEUE;
	I.Information() = sizeof(IOCTL_DATA_OUTPUT);
	m_DriverManagedQueue.DMQ_NEXT_IRP(I);

#if _WDM_
#else
	UNREFERENCED_PARAMETER(pItem);
#endif
}



////////////////////////////////////////////////////////////////////////
// CancelInProgressIrp (member of DrvmngqDevice)
//
//		Cancels the in progress IRP, completes it and starts the next IRP on the
//			device.
//
// Input
//		I			The IRP being canceled
//
// Output
//
// Notes
//

VOID DrvmngqDevice::CancelInProgressIrp(KIrp I)
{
	ASSERT( (PIRP)I == m_DriverManagedQueue.CurrentIrp() );

	// Release the global cancel spinlock since we will be completing
	// the IRP unconditionally and it is not protecting anything.
	CancelSpinLock::Release(I.CancelIrql());

	// Acquire and then drop the cancel sync spinlock.  This ensures that the cancel
	// routine will not proceed until the timer is set (see
	// DRVMNGQ_IOCTL_802_QUEUE_PROCESS_CANCELABLE_Handler).
	m_CancelSync.Lock();
	m_CancelSync.Unlock();

	// Try to cancel the timer
	if (m_TimedCallbackCancelable.Cancel())
	{

		// We succeeded in canceling the timer.  We can complete the IRP with
		// a canceled status, and start the next IRP on the device.
		I.Status() = STATUS_CANCELLED;

		// Get a pointer to the caller's input buffer
		PIOCTL_DATA_INPUT pInput = (PIOCTL_DATA_INPUT)I.IoctlBuffer();
		// Declare a memory object
		KMemory Mem(I.Mdl());
		// Use the memory object to create a pointer to the caller's output buffer
		PIOCTL_DATA_OUTPUT pOutput = (PIOCTL_DATA_OUTPUT)Mem.MapToSystemSpace();

		// Setup the data in the output buffer
		pOutput->nID = pInput->nID;
		pOutput->handling = DIH_CANCELED_IN_INPROGRESS_CANCEL;
		I.Information() = sizeof(IOCTL_DATA_OUTPUT);
		m_DriverManagedQueue.DMQ_NEXT_IRP(I);
	}
	else
	{

		// We did not succeed in canceling the timer.  This means our callback
		// will be running soon.  When it does, it will find that the IRP
		// has been canceled.  We cannot complete the IRP here, or start the
		// next IRP since the device state is not fully resolved.  We
		// will set an event indicating the cancel logic is finished.  This
		// will be used by the timer callback logic in determining when the
		// device state is resolved and processing of the next IRP can
		// continue.
		m_CancelFinished.Set();
	}
}

////////////////////////////////////////////////////////////////////////
// Cleanup (member of DrvmngqDevice)
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

NTSTATUS DrvmngqDevice::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I;

	m_DriverManagedQueue.CLEAN_UP(I.FileObject());
	return I.COMPLETE(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
// Create (member of DrvmngqDevice)
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

NTSTATUS DrvmngqDevice::Create(KIrp I)
{
	t << "Entering Create, " << I;
	I.Information() = 0;
	return I.COMPLETE(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
// Close (member of DrvmngqDevice)
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

NTSTATUS DrvmngqDevice::Close(KIrp I)
{
	t << "Entering Close, " << I;
	I.Information() = 0;
	return I.COMPLETE(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
// Shutdown (member of DrvmngqDevice)
//
//		Handler for IRP_MJ_SHUTDOWN
//
// Input
//		I			Current IRP
//
// Output
//		NTSTATUS	Result code
//
// Notes
//

NTSTATUS DrvmngqDevice::Shutdown(KIrp I)
{
	t << "Entering Shutdown\n";

   return I.COMPLETE(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
// DrvmngqDevice_DriverManagedQueue Constructor
//
// Input
//		None
//
// Output
//		None
//
// Notes

DrvmngqDevice_DriverManagedQueue::DrvmngqDevice_DriverManagedQueue(void) :
#if _WDM_
	KDriverManagedQueueCsq()
#else
	KDriverManagedQueueEx()
#endif
{
}

////////////////////////////////////////////////////////////////////////////////
// StartIo (member of DrvmngqDevice_DriverManagedQueue)
//
//		This routine is called when an IRP is taken off
//		the Driver Managed Queue (used for serializing I/O) and
//		presented for processing.
//
// Input
//		I			IRP removed from queue
//
// Output
//		None
//
// Notes
//		When we enter this routine, the IRP is in a noncancelable
//		state.
//

VOID DrvmngqDevice_DriverManagedQueue::StartIo(KIrp I)
{
	t  << "Entering DrvmngqDevice_DriverManagedQueue StartIo, " << I;

	// Find the device class so we can call the serialized
	// routines in the device class.  The handlers can be
	// moved to the DriverManagedQueue class if it is more
	// convenient.
	DrvmngqDevice *pDev = (DrvmngqDevice *) KDevicePTR(I.DeviceObject());

	// Start processing request.

	// Switch on the IRP's function:
	switch (I.MajorFunction())
	{
		case IRP_MJ_DEVICE_CONTROL:
			pDev->SerialDeviceControl(I);
			break;

		default:
			// Error - unexpected IRP received
			// NextIrp completes this IRP and starts processing
			// for the next IRP in the queue.
			ASSERT(FALSE);
			I.Status() = STATUS_INVALID_PARAMETER;
			NEXT_IRP(I);
			break;
	}
}
