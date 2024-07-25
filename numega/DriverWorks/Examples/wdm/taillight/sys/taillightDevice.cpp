// TailLightDevice.cpp
// Implementation of TailLightDevice device class
//
// Generated by DriverWizard version DriverStudio 3.0.0 (Build 826)
// Requires Compuware's DriverWorks classes
//

#include <vdw.h>
#include <Kwdmfltr.cpp>
#include "TailLightDevice.h"


extern KDebugOnlyTrace t;	// Global driver trace object


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::TailLightDevice
//
//	Routine Description:
//		This is the constructor for the Filter Device Object.
//		It is derived from KWdmFilterDevice, which builds in automatic
//	    dispatching of subfunctions of IRP_MJ_POWER and IRP_MJ_PNP to
//		virtual member functions.
//
//	Parameters:
//		Pdo - Physical Device Object - this is a pointer to a system
//			device object that represents the physical device.
//
//		Unit - Unit number. This is a number to append to the device's
//			base device name to form the Logical Device Object's name
//
//	Return Value:
//		None   
//
//	Comments:
//		The object being constructed contains a data member (m_Lower) of type
//		KPnpLowerDevice. By initializing it, the driver binds the Filter 
//		Device Object to the device stack.
//
TailLightDevice::TailLightDevice(PDEVICE_OBJECT Pdo, ULONG Unit) :
	KWdmFilterDevice(Pdo, NULL),
// @@++ADDED_CODE
		m_bOn(false)
// @@--ADDED_CODE
{
	t <<"Entering TailLightDevice::TailLightDevice (constructor)\n";

	// Check constructor status
    if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
	    return;
	}

	// Remember our unit number
	m_Unit = Unit;

	// Initialize the lower device
	m_Lower.Initialize(this, Pdo);

	// Attach the filter
	NTSTATUS status = AttachFilter(&m_Lower); 

	// Check the status
	if ( !NT_SUCCESS(status) )
	{
		m_ConstructorStatus = status;
		return;
	}

	// Initialize the Filter Power Policy settings 
	SetFilterPowerPolicy();

	// Initialize the Filter PnP Policy settings 
	SetFilterPnpPolicy();
}


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::InternalDeviceControl
//
//	Routine Description:
//		Handler for IRP_MJ_INTERNAL_DEVICE_CONTROL
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This routine can be called at DISPATCH_LEVEL since another driver's
//		completion routine may have called IoCallDriver with this IRP.
//
NTSTATUS TailLightDevice::InternalDeviceControl(KIrp I)
{ 
	t <<"TailLightDevice::InternalDeviceControl\n";

// TODO: Add driver specific code to process the IRP prior to forwarding
// to the lower device.  Call PassThrough() to send the IRP to the lower 
// device.  Optionally, use a completion routine to process the IRP after 
// the lower device has completed it.
//
//	return PassThrough(I, LinkTo(IrpCompletionRoutine), this);

	// The USB WDM function driver will issue URB requests to the
	// USB host and mouse to control the USB mouse.  For these 
	// requests, we will insert a completion routine that gets called
	// when the request is completed.  Other requests are just
	// passed down the stack.
	if (I.IoctlCode() != IOCTL_INTERNAL_USB_SUBMIT_URB)
		return DefaultPnp(I);

	return PassThrough(I, LinkTo(IrpCompletionRoutine), this);
}


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::IrpCompletionRoutine
//
//	Routine Description:
//		Completion Handler for IRPs
//
//	Parameters:
//		I - IRP just completed by lower driver
//
//	Parameters:
//		NTSTATUS - STATUS_SUCCESS
//
//	Comments:
//		This routine is called when the lower driver completes the request.
//	Assume to be called at DISPATCH_LEVEL.
//
NTSTATUS TailLightDevice::IrpCompletionRoutine(KIrp I)
{
	t <<"TailLightDevice::IrpCompletionRoutine\n";

// @@++ADDED_CODE
	PURB p = I.Urb(CURRENT);

	if (p)
	{
		// The USB function driver for the mouse (hidusb.sys) will issue
		// a Select Configuration request to the USB mouse.  After this
		// request is completed, the USB mouse is ready to process I/O
		// requests from the system
		if (URB_FUNCTION_SELECT_CONFIGURATION == p->UrbHeader.Function)
		{
			if (NULL != p->UrbSelectConfiguration.ConfigurationDescriptor)
			{
				// When the select configuation request is completed,
				// Start thread
				NTSTATUS status = StartThread();
			}
		}
	}
// @@--ADDED_CODE

	return I.Status();
}


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::OnStartDevice
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_START_DEVICE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		There is no required functionality here, other than to pass the IRP 
//		through to the lower device. However, the handler exercises some of  
//		the other functions of KPnpLowerDevice, and displays information to 
//		the debug console about the lower device that is filtered.
//
NTSTATUS TailLightDevice::OnStartDevice(KIrp I)
{
	t <<"TailLightDevice::OnStartDevice\n";

	// Retrieve some interesting information about the lower device
	// that is being filtered and display to the debug console

	USHORT buf[256]; 
	ULONG PropLen;
	NTSTATUS status = STATUS_SUCCESS;

	m_Lower.GetDeviceDescription( buf, sizeof(buf), &PropLen );
	t <<"Device Description: |"
	  << buf
	  << "|\n";

	m_Lower.GetPhysicalObjectName( buf, sizeof(buf), &PropLen );
	t <<"PDO Name:	|"
	  << buf
	  << "|\n";

	m_Lower.GetHardwareID( buf, sizeof(buf), &PropLen );
	t <<"Hardware ID:	|"
	  << buf
	  << "|\n";

	m_Lower.GetClassName( buf, sizeof(buf), &PropLen );
	t <<"Class Name:	|"
	  << buf
	  << "|\n";

	m_Lower.GetManufacturer( buf, sizeof(buf), &PropLen );
	t <<"Manufacturer:	|"
	  << buf
	  << "|\n";

	PCM_RESOURCE_LIST pcrl;
	m_Lower.GetBootConfiguration( FALSE, &pcrl, &PropLen ); 

	return PassThrough(I);
}


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::OnRemoveDevice
//
//	Routine Description:
//		Handler for IRP_MJ_PNP subfcn IRP_MN_REMOVE_DEVICE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//
NTSTATUS TailLightDevice::OnRemoveDevice(KIrp I)
{
	t <<"TailLightDevice::OnRemoveDevice\n";

//++ @@ADDED_CODE
	// Signal an event to stop the thread routine.  Also, wait
	// on the thread to complete before continuing, otherwise
	// the driver will unload and a crash will likely occur.
	StopThread();
//-- @@ADDED_CODE

	return PassThrough(I);
}


// @@++ADDED_CODE
////////////////////////////////////////////////////////////////////////
//  TailLightDevice::ThreadRoutine
//
//	Routine Description:
//		The thread routine for the thread created by this driver
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		This routine creates a timer and waits on it.  When the timer
//	expires, a request is sent to the USB device to toggle the light
//	on the mouse.  The code also waits on an event.  The event is
//	signalled by the OnRemoveDevice handler when the device is being
//	removed (unplugged, uninstalled, or disabled by the user.)  When
//	this happens, the thread terminates itself.  Note that this 
//	function NEVER returns after the call to Terminate().
//
VOID TailLightDevice::ThreadRoutine(void)
{
	t <<"TailLightDevice::ThreadRoutine START\n";

	KTimer			Timer(SynchronizationTimer);
	NTSTATUS		Status			  = STATUS_SUCCESS;
	LARGE_INTEGER	DueTime			  = {0};  // expressed in 100-nanosecond intervals
	const ULONG		ulArrSize		  = 2;
	const LONG		lInterval		  = 250;  // expressed in milliseconds
	PVOID			Object[ulArrSize] = { m_Event.Object(), Timer.Object() };

	// set the timer to expire at relative time DueTime
	// and periodically at lInterval milliseconds
	Timer.SetPeriodic(DueTime, lInterval);

	do
	{
		// Wait on the timer or the event to become signalled.  
		Status = Timer.WaitMultiple(
							ulArrSize,	//	ULONG			Count	
							Object,		//	PVOID			Object[]
							WaitAny,	//	WAIT_TYPE		WaitType
							Executive,	//	KWAIT_REASON	WaitReason
							KernelMode, //	KPROCESSOR_MODE WaitMode
							FALSE,		//	BOOLEAN			Alertable
							NULL,		//	PLARGE_INTEGER  Timeout
							NULL);		//	PKWAIT_BLOCK	WaitBlockArray

		if (!NT_SUCCESS(Status))
		{
			// Error returned
			break;
		}

		if (STATUS_WAIT_0 == Status)
		{
			// Event was signalled, meaning the thread should terminate
			break;
		}

		if (STATUS_WAIT_1 == Status)
		{
			// Timer expired and became signalled
			// Do some I/O

			m_bOn = !m_bOn;
			Status = SetTailLight(m_bOn);
		}

	} while (TRUE);

	// Forget to do this = crash!!!!
	// this calls Cancel() and the DTOR
	Timer.Invalidate();

	t <<"TailLightDevice::ThreadRoutine END\n";

	m_Thread.Terminate(Status);

	// any code after the Terminate call will not be
	// executed.  Since Invalidate is called on the
	// Timer object, this effectively is the DTOR so its ok
}


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::SetTailLight
//
//	Routine Description:
//		This routine creates a USB request to turn on or off the
//	tail light of the USB mouse.
//
//	Parameters:
//		bOn		- true to turn on light, false to turn it off
//
//	Return Value:
//		NTSTATUS - status of the USB request
//
//	Comments:
//		
NTSTATUS TailLightDevice::SetTailLight(bool bOn)
{
	URB U = {0};
	UCHAR Buffer = bOn ? 1 : 0;

	m_Lower.BuildClassRequest(
		&Buffer,		// PUCHAR	TransferBuffer,
		sizeof(UCHAR),	// ULONG	TransferBufferLength,
		0x22,			// UCHAR	RequestTypeReservedBits,
		9,				// UCHAR	Request,
		0x300,			// USHORT	Value,
		FALSE,			// BOOLEAN	bIn,
		FALSE,			// BOOLEAN	bShortOk,
		NULL,			// PURB		Link,
		0,				// USHORT	Index,
		URB_FUNCTION_CLASS_INTERFACE,	
						// USHORT	Function,
		&U				// PURB		pUrb
		);

	NTSTATUS Status = m_Lower.SubmitUrb(&U);

	if (!NT_SUCCESS(Status))
	{
		t <<" SetTailLight() failed, Status = " << Status << "\n";
	}

	return Status;
}


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::StartThread
//
//	Routine Description:
//		This routine initializes an event and starts the thread.
//
//	Parameters:
//		None
//
//	Return Value:
//		NTSTATUS - status of starting the thread
//
//	Comments:
//		
NTSTATUS TailLightDevice::StartThread()
{
	m_Event.Initialize(NotificationEvent);
	return m_Thread.Start(LinkTo(ThreadRoutine),this);
}


////////////////////////////////////////////////////////////////////////
//  TailLightDevice::StopThread
//
//	Routine Description:
//		This routine stops the thread.
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		This routine signals (sets) an event and waits for the thread 
//	to complete.  The thread routine waits on the event.  When it 
//	becomes signalled, the thread exits.  Its important that the driver
//	wait for the thread to complete, otherwise the driver could unload
//	from memory with the thread routine still running.  This causes
//	a blue screen.  :)
//	 
void TailLightDevice::StopThread() 
{
	if (m_Event.IsValid()) {
		m_Event.Set();
	}

	if (m_Thread.IsValid()) {
		m_Thread.Wait();
	}
}

// @@--ADDED_CODE