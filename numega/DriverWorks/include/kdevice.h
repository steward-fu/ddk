// kdevice.h - Include file for class KDevice
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KDEVICE__
#define __KDEVICE__


///////////////////////////////////////////////////////////////////////////////
// Device object signatures
//
// Depending on which operator new is used to allocate the object,
// data member m_Signature takes one of the two following values.
#define KDEVICE_SIG_IN_DEVICE_EXT			' KDX'
#define KDEVICE_SIG_NOT_IN_DEVICE_EXT		'~KDX'
#define KDEVICE_SIG_MASK					0x7FFFFFFF
#define KDEVICE_SIG_USE_ALLOCATOR			0x80000000
// These are needed so that the delete operator can do the right thing.


class KDeviceBase
{
protected:
	KDeviceBase()
	{
		if ( (m_Signature & KDEVICE_SIG_MASK) != KDEVICE_SIG_IN_DEVICE_EXT )
			m_pDeviceObject = NULL;
	}

	virtual SAFE_DESTRUCTORS;
	virtual ~KDeviceBase(void)
	{
		// If this device is allocated by operator new, let KDevice::operator delete to free it.
		if (m_pDeviceObject && !(m_Signature & KDEVICE_SIG_USE_ALLOCATOR))
			IoDeleteDevice(m_pDeviceObject);
	}

	DEVICE_OBJECT* 		m_pDeviceObject;
	ULONG				m_Signature;
};


class KDevice : public KDeviceBase
{

// Construction
public:
	virtual SAFE_DESTRUCTORS;

	// Overloaded new and delete operators
	void* __cdecl operator new(
		size_t n,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		PCWSTR LinkName=NULL,
		ULONG Characteristics=0,
		ULONG DeviceFlags=DO_BUFFERED_IO
		);

	void* __cdecl operator new(
		size_t n,
		POOL_TYPE Pool
		);

	void __cdecl operator delete(void* p);

	void __cdecl operator delete(
		void* p,
		POOL_TYPE Pool
		)
	{
		operator delete(p);
		UNREFERENCED_PARAMETER(Pool);
	}


protected:
	static KDevice* __construct(
		KDevice* pStore,
		ULONG ExtensionSize,
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		PCWSTR LinkName,
		ULONG Characteristics,
		ULONG DeviceFlags,
		NTSTATUS& status
		);

public:
	KDevice(void);

	KDevice(
		PCWSTR DeviceName,
		DEVICE_TYPE Type,
		PCWSTR LinkName=NULL,
		ULONG Characteristics=0,
		ULONG DeviceFlags=DO_BUFFERED_IO
		);

	KDevice(
		ULONG Context,
		DEVICE_TYPE Type,
		ULONG Characteristics=0,
		ULONG DeviceFlags=DO_BUFFERED_IO
		);

	~KDevice(void);
	BOOLEAN IsValid(void){ return (m_pDeviceObject != NULL); }

// Methods
public:
	NTSTATUS CreateLink(PCWSTR LinkName, BOOLEAN Protected=TRUE);
	NTSTATUS DestroyLink(PCWSTR LinkName);

	// Accessors
	NTSTATUS ConstructorStatus(void);
	BOOLEAN IsPnpDevice(void);
	PUNICODE_STRING DeviceName(void);
	PUNICODE_STRING SymbolicLinkName(void);
	KDevice* NextDevice(void);
	PKDEVICE_QUEUE DeviceQueue(void);
	operator PDEVICE_OBJECT(void);
	static KUnitizedName* CreateRegistryPath(PCWSTR className, ULONG unit);

	// IRP management
	virtual NTSTATUS DeviceIrpDispatch( PIRP pIrp);

	NTSTATUS QueueIrp(
		KIrp I,
		PDRIVER_CANCEL CancelRoutine=NULL,
		PULONG Key=0);
	VOID NextIrp(
		KIrp I,
		CCHAR PriorityBoost=IO_NO_INCREMENT,
		BOOLEAN Cancelable=TRUE);
	PIRP& CurrentIrp(void);

	// operations on lower devices
	NTSTATUS Attach(
		PCWSTR name,
		KLowerDevice** ppLowerDevice,
		BOOLEAN bInheritAlignmentReq=TRUE
		);

	NTSTATUS Attach(
		KLowerDevice* pLowerDevice,
		BOOLEAN bInheritAlignmentReq=TRUE
		);

	VOID Detach(KLowerDevice* pLowerDevice);

	VOID ReserveIrpStackLocation(KLowerDevice* pLD);

	// DPC interface
	VOID InitializeDpcForIsr(PIO_DPC_ROUTINE pRoutine);

	VOID RequestDpc(KIrp I, PVOID Context);

	// Timer interface
	NTSTATUS InitializeTimer(
		PIO_TIMER_ROUTINE TimerRoutine,
		PVOID context
		);

	VOID StartTimer(void);

	VOID StopTimer(void);

	// DMA Adapter Channel interface
	NTSTATUS SynchronizeDmaAdapter(
		KDmaAdapter* pDmaAdapter,
		PDRIVER_CONTROL SynchRoutine,
		ULONG nMapRegisters,
		PVOID Context
		);

	VOID ReleaseDmaAdapter(KDmaAdapter* pDmaAdapter);

#if !defined(_DW_INCLUDE_NTDDK_)
	// Controller interface
	VOID SynchronizeController(
		KController* pController,
		PDRIVER_CONTROL SynchRoutine,
		PVOID Context
		);

	VOID ReleaseController(KController* pController);
#endif

#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
	// Interrupt interface
	BOOLEAN SynchronizeInterrupt(
		KInterrupt* pInterrupt,
		PKSYNCHRONIZE_ROUTINE SynchRoutine,
		PVOID context
		);
#endif

	NTSTATUS SubscribeShutdownNotification(void);

	VOID CancelShutdownNotification(void);

#if !_WDM_ && !(defined(_IA64_) || defined(_AMD64_))
	// Resource release
	NTSTATUS ReleaseResources(PUNICODE_STRING RegistryPath);

	ResourceReleaseMethod_t m_ReleaseMethod;
#endif	// WDM

	NTSTATUS DefaultDispatch(KIrp I);

	// Declare stub IRP handlers for base class
#define DISPATCHER_STUB(Name) \
	virtual NTSTATUS Name(KIrp I) 					\
	{ 												\
		ASSERT(FALSE); 								\
		return I.Complete(STATUS_NOT_IMPLEMENTED);	\
	}

	DISPATCHER_STUB(Create)
	DISPATCHER_STUB(CreateClose)
	DISPATCHER_STUB(CreateNamedPipe)
	DISPATCHER_STUB(Close)
	DISPATCHER_STUB(Read)
	DISPATCHER_STUB(ReadWrite)
	DISPATCHER_STUB(Write)
	DISPATCHER_STUB(QueryInformation)
	DISPATCHER_STUB(SetInformation)
	DISPATCHER_STUB(QueryEa)
	DISPATCHER_STUB(SetEa)
	DISPATCHER_STUB(FlushBuffers)
	DISPATCHER_STUB(QueryVolumeInformation)
	DISPATCHER_STUB(SetVolumeInformation)
	DISPATCHER_STUB(DirectoryControl)
	DISPATCHER_STUB(FileSystemControl)
	DISPATCHER_STUB(DeviceControl)
	DISPATCHER_STUB(InternalDeviceControl)
	DISPATCHER_STUB(Shutdown)
	DISPATCHER_STUB(LockControl)
	DISPATCHER_STUB(CleanUp)
	DISPATCHER_STUB(CreateMailSlot)
	DISPATCHER_STUB(QuerySecurity)
	DISPATCHER_STUB(SetSecurity)
	DISPATCHER_STUB(Power)
	DISPATCHER_STUB(SystemControl)
	DISPATCHER_STUB(DeviceChange)
	DISPATCHER_STUB(QueryQuota)
	DISPATCHER_STUB(SetQuota)
	DISPATCHER_STUB(Pnp)

	virtual VOID StartIo(KIrp I) {UNREFERENCED_PARAMETER(I);}

protected:
	VOID ReleaseSystemDeviceObject(void);

	VOID CheckDriverFunctions(void);

// Data
protected:
	KUstring 			m_DeviceName;
	KUstring 			m_SymbolicLinkName;
	NTSTATUS			m_ConstructorStatus;
	BOOLEAN				m_bIsPnpDevice;
};	// end declaration of KDevice


///////////////////////////////////////////////////////////////////////////////
//
//	KDevicePTR
//		Macro to get KDevice* from system device object
//
//	Parameters:
//		DevObj
//			System device object pointer
//
//	Return Value:
//		Returns a pointer to the KDevice* for the device object
//
//	Comments:
//		Drivers may override the default value of this macro for
//		special drivers that use an alternate device extension
//		format, as long as the #define occurs before #include <vdw.h>.
//
#ifndef KDevicePTR
#define KDevicePTR(DevObj) ((KDevice*)DevObj->DeviceExtension)
#endif	// KDevicePTR

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::ConstructorStatus
//		Get the status value from construction.
//
//	Return Value:
//		Returns the status data member set by the ctor.
//
//	Comments:
//		All ctors set data member m_ConstructorStatus, which this function just
//		returns.
//
inline NTSTATUS KDevice::ConstructorStatus(void)
{
	return m_ConstructorStatus;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::IsPnpDevice
//		Indicates if the object is just a KDevice or is really a KPnpDevice
//
//	Return Value:
//		Returns TRUE if the instance is derived from KPnpDevice
//
//	Comments:
//		The ctor for KDevice sets the protected data member to FALSE, and
//		the ctor for KPnpDevice sets it to TRUE. This accessor is used by
//		all, and just returns the data member's current value.
//
inline BOOLEAN KDevice::IsPnpDevice(void)
{
	return m_bIsPnpDevice;
}

/////////////////////////////////////////////////////////////////////////
//
//	KDevice::NextIrp
//		Complete the specified IRP and start processing the next IRP, if
//		present, in the device queue.
//
//	Parameters
//		I
//			IRP to be completed.
//
//		PriorityBoost
//			The increment in priority to be granted to the thread
//			waiting for the IRP to complete. The default is IO_NO_INCREMENT.
//
//	Cancelable
//			Flag indicates whether or not any IRPs that might
//			currently be in the device queue are cancelable
//			(i.e. have a cancel routine). This tells the I/O
//			manager if it needs to acquire the system-wide cancel
//			spin lock when updating the device queue. The default
//			is TRUE.
//
//	Comments
//			Devices that use the system supplied IRP queue (i.e. StartIo)
//			typically have occasion to use this member in the final stage of IRP
//			processing. When a driver is ready to finish the current IRP and
//			dequeue the next, this member is called to perform both those
//			operations. The caller must set the Status and Information fields of
//			the IRP being completed before calling this function. A typical
//			fragment of code looks like:
//
//				I.Status() = STATUS_SUCCESS;
//				I.Information() = nBytesReturned;
//				NextIrp(I);
//
//			To only start processing the next IRP, without completing any IRP,
//			pass the first parameter as KIrp(0). A class derived from KDevice that
//			does not supply an override for member StartIo cannot call this
//			member. The caller must not be holding any spin locks when this call
//			is made. The underlying system services are IoCompleteRequest and
//			IoStartNextPacket. The caller must be running at DISPATCH_LEVEL.
//
inline VOID KDevice::NextIrp(KIrp I, CCHAR PriorityBoost, BOOLEAN Cancelable)
{
	BOUNDS_CHECKER(NEXT_IRP, (this, I.m_Irp));
	IoStartNextPacket(m_pDeviceObject, Cancelable);
	if (!I.IsNull())
		{IoCompleteRequest(I.m_Irp, PriorityBoost);}
}


///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::DeviceName
//		Returns a pointer to the full device name.
//
//	Return Value:
//		Returns a pointer to the full device name, including the \device prefix.
//		If the device is anonymous, returns a pointer to an empty string.
//
//	Comments:
//		The ctor (actually __construct) saves this value. The string will be
//		empty if the device has no name.
//
inline PUNICODE_STRING KDevice::DeviceName(void)
{
	return m_DeviceName;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::SymbolicLinkName
//		Returns the name of a symbolic link created at construction
//
//	Return Value:
//		Returns a pointer to the full link name, including the \?? prefix.
//		If a link was not created at construction time, it returns a pointer to an
//		empty string.
//
//	Comments:
//		The ctor (actually __construct) saves this value. The string will be
//		empty if no link was created at construction. Note that it is allowable
//		to create any number of symbolic links to the device at other times.
//
inline PUNICODE_STRING KDevice::SymbolicLinkName(void)
{
	return m_SymbolicLinkName;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::operator PDEVICE_OBJECT
//		Overloaded cast to PDEVICE_OBJECT.
//
//	Return Value:
//		Returns a pointer to the underlying system device object.
//
//	Comments:
//		The system device object pointer is originally returned by IoCreateDevice
//		in static member __construct.
//
inline KDevice::operator PDEVICE_OBJECT(void)
{
	return m_pDeviceObject;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::NextDevice
//		Iterator for walking a driver's device list.
//
//	Return Value:
//		Returns a pointer to the next KDevice in the driver's list,
//		or NULL there are no more.
//
//	Comments:
//		The iteration is initiated by a call to KDriver::GetDeviceListHead(void).
//
inline KDevice* KDevice::NextDevice(void)
{
	if (m_pDeviceObject->NextDevice != NULL)
		return KDevicePTR(m_pDeviceObject->NextDevice);
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::Detach
//		Detach the given lower device.
//
//	Parameters:
//		pLowerDevice
//			Pointer to the lower device to be detached.
//
//	Comments:
//		The device must have been attached by an earlier to call to KDevice::Attach,
//			or when the lower device was constructed/initialized as in KPnpLowerDevice.
//		The caller must be running at PASSIVE_LEVEL.
//
inline VOID KDevice::Detach(KLowerDevice* pLowerDevice)
{
	IoDetachDevice(pLowerDevice->DeviceObject());
	BOUNDS_CHECKER(DETACHED_LOWER_DEVICE, (this, pLowerDevice));
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::ReserveIrpStackLocation
//		Sets the device's stack size to accommodate sending IRPs to the
//		given lower device.
//
//	Parameters:
//		pLD
//			Pointer to lower device to which the device may send an IRP.
//
//	Comments:
//		Each device object contains a data member that tells the I/O manager how
//		many IRP stack locations to allocate in IRPs targeted to the device. The
//		stack locations are storage areas that hold parameters specifying the
//		request to be carried out by the device. Suppose a device that receives
//		IRPs needs to pass them to a lower device for processing. If it passes
//		down the same IRP that it receives, then the number of stack locations
//		that the device must require is one greater than the requirement of the
//		called lower device. If there are multiple lower devices to which the
//		device may distribute IRPs, then its requirement is one greater than the
//		maximum of the requirements of the called devices.
//
//		Suppose that the stack location requirement for the calling device is
//		currently S, and that of the lower device specified by parameter
//		pLowerDevice is T. Then if S < T+1, this member function increases the
//		stack location requirement of the calling device to T+1. This function
//		may be called at any IRQL
//
inline VOID KDevice::ReserveIrpStackLocation(KLowerDevice* pLD)
{
	CCHAR LowerStackSize = pLD->DeviceObject()->StackSize;

	if (m_pDeviceObject->StackSize <= LowerStackSize)
		m_pDeviceObject->StackSize = static_cast<CCHAR>(LowerStackSize + 1);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::DeviceQueue
//
//	Return Value:
//		Returns a pointer to the KDEVICE_QUEUE owned by the system
//		device object.
//
//	Comments:
//		Many drivers serialize processing of IRPs by queuing them
//		in internal queues or in the queue that is a component of  the
//		device object, using StartIo. This accessor returns the queue
//		associated with the device object. See class KDeviceQueuefor the
//		operations that can be performed on a device queue. The caller may
//		be running at any IRQL.
//
inline PKDEVICE_QUEUE KDevice::DeviceQueue(void)
{
	return &m_pDeviceObject->DeviceQueue;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::InitializeDpcForIsr
//		Initialize a DPC owned by the system device object.
//
//	Parameters:
//		pRoutine
//			Address of the DPC callback
//
//	Comments:
//		In order to minimize the spent in an interrupt service routine, a driver requests a
//		callback at lower IRQL to complete noncritical interrupt processing. The system
//		device object includes storage for a deferred callback structure, and this member
//		function initializes that structure. This is a special case of the generalized
//		deferred procedure call (DPC) facility that the system provides. The DPC context
//		parameter is initialized to the address of the system device object.
//
//		The prototype of the callback function is:
//
//		VOID DpcForIsrCallback(
//		    IN PKDPC Dpc,
//		    IN PDEVICE_OBJECT DeviceObject,
//		    IN PIRP Irp,
//		    IN PVOID Context
//		    );
//
inline VOID KDevice::InitializeDpcForIsr(PIO_DPC_ROUTINE pRoutine)
{
	IoInitializeDpcRequest(m_pDeviceObject, pRoutine);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::RequestDpc
//		description.
//
//	Parameters:
//		I
//			The current IRP for the device
//
//		Context
//			The parameter to be passed to the callback.
//
//	Comments:
//		This call queues a request for a callback to the function specified in
//		an earlier call to member InitializeDpcForIsr. The underlying system
//		service is IoRequestDpc. The caller must be running at IRQL >
//		DISPATCH_LEVEL. It is almost exclusively called from an interrupt
//		service routine.
//
inline VOID KDevice::RequestDpc(KIrp I, PVOID Context)
{
	IoRequestDpc(m_pDeviceObject, I.m_Irp, Context);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::InitializeTimer
//		Initialize the 1 Hz timer object associated with the device object
//
//	Parameters:
//		TimerRoutine
//			The function to be called each second.
//
//		Context
//			Untyped reference data to be passed to timer callback.
//
//	Return Value:
//		Returns STATUS_SUCCESS if successful.
//
//	Comments:
//		Each system device object has a data member that points to a timer
//		object. The system programs this timer to expire at a frequency of
//		approximately once per second ( 1 Hz ). Devices sometimes use this to
//		measure timeout intervals.
//
//		This member specifies the function to call on each tick of the 1 Hz
//		timer, and the parameter to pass to the timer function each call. The
//		timer does not start calling the function until the driver invokes
//		member StartTimer. Member StopTimer instructs the system to stop
//		calling the timer function.
//
//		The prototype for the timer routine is:
//
//		VOID TimerRoutine (
//		    IN PDEVICE_OBJECT DeviceObject,
//		    IN PVOID Context
//		    );
//
//		The timer routine is not a member of KDevice. It is possible to make
//		the timer routine a member of a class derived from KDevice by
//		inserting macro DEVMEMBER_IOTIMER in the definition of the class.
//
//		This is a special case of a generalized timer capability that the
//		system provides. See class KTimer and class KTimedCallback.
//
//		The underlying system call is IoInitializeTimer.
//
//		The caller must be running at PASSIVE_LEVEL.
//
inline NTSTATUS KDevice::InitializeTimer(
	PIO_TIMER_ROUTINE TimerRoutine,
	PVOID context)
{
	return IoInitializeTimer(m_pDeviceObject, TimerRoutine, context);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::StartTimer
//		Starts the device's 1 Hz timer.
//
//	Comments:
//		Each system device object has a data member that points to a timer
//		object. The system programs this timer to expire at a frequency of
//		approximately once per second ( 1 Hz ). Devices sometimes use this to
//		measure timeout intervals.
//
//		If the driver has initialized the timer with member InitializeTimer,
//		then calling this function instructs the system to begin calling the
//		timer routine specified in the call to InitializeTimer at one second
//		intervals.
//
//		To stop calling the timer routine, call member StopTimer.
//
//		The underlying system call is IoStartTimer.
//
//		The caller must be running at IRQL <= DISPATCH_LEVEL.
//
inline VOID KDevice::StartTimer(void)
{
	IoStartTimer(m_pDeviceObject);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::StopTimer
//		Stops the device's 1 Hz timer.
//
//	Comments:
//		Each system device object has a data member that points to a timer
//		object. The system programs this timer to expire at a frequency of
//		approximately once per second ( 1 Hz ). Devices sometimes use this to
//		measure timeout intervals.
//
//		To initialize the timer, use member InitializeTimer. To instruct the
//		system to begin calling the timer routine, use member StartTimer.
//
//		This member function instructs the system to stop calling the timer
//		routine each second. Calling StartTimer resumes invocation of the
//		timer routine.
//
//		Do not call this member from inside the timer routine.
//
//		The underlying system service is IoStopTimer.
//
//		The caller must be running at IRQL <= DISPATCH_LEVEL.
//
inline VOID KDevice::StopTimer(void)
{
	IoStopTimer(m_pDeviceObject);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::SynchronizeDmaAdapter
//		Request a callback when the DMA adapter is available.
//
//	Parameters:
//		pDmaAdapter
//			Pointer to the instance of class KDmaAdapter to be acquired
//
//		SynchRoutine
//			The function to call when the channel is available.
//
//		nMapRegisters
//			Number of map registers required that will be required for
//			the pending transfer
//
//		Context
//			The parameter to pass to the callback function
//
//	Return Value:
//		Returns STATUS_INSUFFICIENT_RESOURCES if the value of parameter
//		nMapRegisters exceeds the value returned by pAdapter->MaxRegisters( ).
//		Otherwise, returns STATUS_SUCCESS.
//
//	Comments:
//		If the channel specified by parameter pAdapter is available at the
//		time of the call, then the system invokes the callback routine
//		immediately and synchronously. If the channel is not available, the
//		callback request is queued and this function returns to its caller.
//
//		The prototype for the callback function is:
//
//		IO_ALLOCATION_ACTION
//		(*PDRIVER_CONTROL) (
//		    IN PDEVICE_OBJECT DeviceObject,
//		    IN PIRP Irp,
//		    IN PVOID MapRegisterBase,
//		    IN PVOID Context
//		    );
//
//		The system invokes the callback routine at DISPATCH_LEVEL. The return
//		value of the callback instructs the system whether or not the driver
//		continues to require the controller resource. If so, the callback
//		returns KeepObject, otherwise DeallocateObject.
//
//		The callback is not a member of the class. To make it a member of a
//		class derived from KDevice, use macro DEVMEMBER_CONTROLLERinside the
//		definition of the derived class.
//
//		Drivers that use class KDmaTransfer to manage a DMA transfer never
//		call this function directly; it is handled by the class.
//
//		The underlying system service is IoAllocateAdapterChannel.
//
//		This function may be called from IRQL <= DISPATCH_LEVEL. Normally, it
//		is called from member StartIo of a class derived from KDevice. If not
//		called from StartIo, then parameter Irp in the callback routine must
//		not be used
//
inline NTSTATUS KDevice::SynchronizeDmaAdapter(
	KDmaAdapter* pDmaAdapter,
	PDRIVER_CONTROL SynchRoutine,
	ULONG nMapRegisters,
	PVOID Context
	)
{
	return pDmaAdapter->QueueSynchRequest(
		m_pDeviceObject,
		nMapRegisters,
		SynchRoutine,
		Context);
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::ReleaseDmaAdapter
//		Releases a previously allocated DMA adapter.
//
//	Parameters:
//		pDmaAdapter
//			Pointer to the adapter channel object to be released.
//
//	Comments:
//		A driver calls this function when it is done servicing the request for
//		which it acquired the channel. The driver must have called member
//		SynchronizeDmaAdapter to gain control of the channel.
//
//		If the callback routine invoked by the system in response to the
//		SynchronizeDmaAdapter call returns DeallocateObject, that suffices to
//		release the channel, and it not necessary to call this member as
//		well.
//
//		Drivers that use class KDmaTransfer to manage a DMA transfer do not
//		need to call this function.
//
//		The underlying system call is IoFreeAdapterChannel.
//
//		The caller must be running at IRQL <= DISPATCH_LEVEL.
//
inline VOID KDevice::ReleaseDmaAdapter(KDmaAdapter* pDmaAdapter)
{
	pDmaAdapter->Release();
}

#if !defined(_DW_INCLUDE_NTDDK_)
///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::SynchronizeController
//		Arrange for exclusive use of a controller resource by the calling device.
//
//	Parameters:
//		pController
//			Pointer to the system controller object to be secured.
//
//		SynchRoutine
//			The function that the system calls when access to the controller is granted
//
//		Context
//			Context parameter to be passed to SynchRoutine
//
//	Comments:
//		When multiple devices managed by the same driver share a common
//		resource, a controller object provides a means to serialize access to
//		that resource. A driver calls this function to arrange for a callback
//		when the controller object is available. If the controller is
//		available at the time of the call, then the system invokes the
//		callback routine immediately and synchronously. If the controller is
//		not available, the callback request is queued and this function
//		returns to its caller.
//
//		The prototype of the callback routine (PDRIVER_CONTROL) is:
//
//		IO_ALLOCATION_ACTION OnControllerSynched (
//		    IN PDEVICE_OBJECT DeviceObject,
//		    IN PIRP Irp,
//		    IN PVOID SystemReserved,
//		    IN PVOID Context
//		    );
//
//		The system invokes the callback routine at DISPATCH_LEVEL. The return
//		value of the callback instructs the system whether or not the driver
//		continues to require the controller resource. If so, the callback
//		returns KeepObject, otherwise DeallocateObject.
//
//		The callback is not a member of KDevice. To declare a member of a
//		class derived from KDevice as the callback, use macro
//		DEVMEMBER_CONTROLLER.
//
//		Drivers that use a controller almost always serialize IRP processing
//		on a device basis as well, using the device's StartIo member. Callback
//		parameter Irp is not meaningful unless the driver queues IRPs with
//		StartIo, or explicitly sets the CurrentIrp data field in the device
//		object.
//
//		The underlying system service is IoAllocateController.
//
//		The caller must be running at IRQL <= DISPATCH_LEVEL.
//
inline VOID KDevice::SynchronizeController(
	KController* pController,
	PDRIVER_CONTROL SynchRoutine,
	PVOID Context
	)
{
	pController->QueueSynchRequest(SynchRoutine, m_pDeviceObject, Context);
}

///////////////////////////////////////////////////////////////////////////////
//
//	ReleaseController
//		Releases a previously allocated controller.
//
//	Parameters:
//		pController
//			Pointer to controller to be released.
//
//	Comments:
//		A driver calls this function when it is done servicing the request for
//		which it acquired the controller. The driver must have called member
//		SynchronizeController to gain control of the controller.
//
//		If the callback routine invoked by the system in response to the
//		SynchronizeController call returns DeallocateObject, that suffices to
//		release the controller, and it not necessary to call this member as
//		well.
//
//		If the controller has callbacks queued for the device that is
//		associated with the controller at the time this function is called,
//		then the device continues to hold the controller.
//
//		The underlying system call is IoFreeController.
//
//		The caller must be running at IRQL <= DISPATCH_LEVEL.
//
inline VOID KDevice::ReleaseController(KController* pController)
{
	pController->Release();
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::QueueIrp
//		Queues an IRP to the StartIo routine. If the queue is empty, call
//		StartIo synchronously.
//
//	Parameters:
//		I
//			The IRP to be queued.
//
//		CancelRoutine
//			If not NULL, specifies the cancel routine for the IRP. The default
//			value is NULL.
//
//		Key
//			Enables the caller to specify the position in the queue at which the
//			IRP should be queued. When the parameter is not NULL, the value to
//			which it points specifies the position of the IRP in the queue. The
//			feature is rarely used. The default value is NULL.
//
//	Return Value:
//		Always returns STATUS_PENDING.
//
//	Comments:
//		A class derived from KDevice that does not supply an override for
//		member StartIo cannot call this member.
//
//		If the device is not busy when this member is called, processing of
//		the IRP continues synchronously in the StartIo routine. StartIo
//		always executes at DISPATCH_LEVEL.
//
//		This member marks the IRP as pending.
//
//		Parameter CancelRoutine specifies the routine that the system calls if
//		the IRP is cancelled. The format of the callback is:
//
//			VOID CancelRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);
//
//		Many driver writers find it convenient to make the cancel routine a
//		member of a class derived from KDevice. To do this, use macro
//		DEVMEMBER_CANCELIRP inside the definition of the class. There may be
//		multiple cancel routines in a driver or in a device class.
//
//		Although the default value of CancelRoutine is NULL, driver writers are
//		strongly encouraged to make IRPs cancelable.
//
//
//		This member returns STATUS_PENDING because a handler that queues an
//		IRP on the device queue returns STATUS_PENDING to the I/O Manager,
//		indicating that further processing is necessary to complete the IRP.
//		Therefore, a driver can code the queuing operation as:
//
//				return QueueIrp(I, LinkTo(MyCancelRoutine));
//
//		The underlying system services are IoMarkIrpPending and IoStartPacket.
//
//		This member may be called at IRQL <= DISPATCH_LEVEL.
//
inline NTSTATUS KDevice::QueueIrp(
	KIrp I,
	PDRIVER_CANCEL CancelRoutine,
	PULONG Key)
{
	I.MarkPending();
	IoStartPacket(
		m_pDeviceObject,
		I.m_Irp,
		Key,
		CancelRoutine);

	BOUNDS_CHECKER(QUEUED_IRP, (this, I.m_Irp, CancelRoutine));

	return STATUS_PENDING;
}

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::SynchronizeInterrupt
//		Invokes the supplied synchronization routine after raising IRQL and acquiring
//		 the spin lock associated with the interrupt.
//
//	Parameters:
//		pInterrupt
//			Address of the interrupt object.
//
//		SynchRoutine
//			Address of the synchronization routine to call.
//
//		SynchContext
//			Parameter to be passed to the synchronization routine.
//
//	Return Value:
//		Returns the return value of the synchronization routine.
//
//	Comments:
//		This member enables code other than the interrupt service routine (ISR) to
//		access safely data that the ISR also accesses. If a driver allows data to
//		enter a transient state at IRQL <= DISPATCH_LEVEL, and that data may be
//		accessed from an ISR, then the data could be sporadically corrupted when
//		the interrupt occurs. This member gives the driver writer a means to
//		execute non-ISR code in a state where there is no possibility of the ISR
//		running, thereby allowing atomic access to shared data.
//
//		When the call is made, the system raises the IRQL of the caller to the IRQL
//		of the spin lock associated with the interrupt, and then acquires the spin
//		lock. This has the effect of blocking out the ISR. The system then calls
//		the supplied SynchRoutine, passing SynchContext as a parameter. The
//		synchronization routine must execute very quickly, because it is blocking
//		interrupts.
//
//		The prototype for the synchronization routine is:
//
//		BOOLEAN IntrSynchFunction(PVOID SynchronizeContext);
//
//		The class requires the driver writer to declare the synchronization routine
//		using macro DEVMEMBER_SYNCHCRITSECTION inside the declaration of a class
//		derived from KDevice.
//
//		The supplied synchronization function is invoked synchronously with the
//		execution of this member, i.e., this member does not return until the
//		synchronization function has executed.
//
//		The call cannot be made while the interrupt is not connected.
//
//		The caller must be running at IRQL less than or equal to the IRQL
//		associated with the interrupt.
//
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
	typedef struct {
		KDevice* m_device;
		PVOID	 m_context;
	} DEVICE_INTERRUPT_SYNCH_CONTEXT;

inline BOOLEAN KDevice::SynchronizeInterrupt(
	KInterrupt* pInterrupt,
	PKSYNCHRONIZE_ROUTINE SynchRoutine,
	PVOID context
	)
{
	DEVICE_INTERRUPT_SYNCH_CONTEXT SynchContext;

	SynchContext.m_device = this;
	SynchContext.m_context = context;

	return pInterrupt->Synchronize(SynchRoutine, &SynchContext);
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::CurrentIrp
//
//
//	Parameters:
//		Param_name
//			Param description
//
//		Param_name
//			Param description
//
//	Return Value:
//		May be omitted for VOID return
//
//	Comments:
//		This is where verbosity is a virtue.
//
inline PIRP& KDevice::CurrentIrp(void)
{
	return m_pDeviceObject->CurrentIrp;
}


///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::DefaultDispatch
//		Default processing of an IRP
//
//	Parameters:
//		I
//			Request to process
//
//	Return Value:
//		Returns STATUS_SUCCESS
//
//	Comments:
//		Any dispatch member (e.g. Create, Close, Read, Write, etc.) may invoke this
//		member to successfully complete an IRP with Information set to zero and Status
//		STATUS_SUCCESS.
//
//	The function is useful for dispatcher members that do nothing, but are required
//	to complete the request successfully.
//
inline NTSTATUS KDevice::DefaultDispatch(KIrp I)
{
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

#if !_WDM_ && !(defined(_IA64_) || defined(_AMD64_))
///////////////////////////////////////////////////////////////////////////////
//
//	KDevice::ReleaseResources
//		Release resources that were claimed by the device.
//
//	Parameters:
//		RegistryPath
//			The path used to claim the resources.
//
//	Return Value:
//		Returns STATUS_SUCCESS if the resources are released.
//
//	Comments:
//		This function releases any resources claimed for the driver by an earlier call
//		to KResourceRequest::Submit. Note that if the resources were claimed for the
//		driver rather than for a particular device or devices, you must use
//		KDriver::ReleaseResources.
//
//		The underlying system call is IoAssignResources.
//
//		This function may only be called at PASSIVE_LEVEL.
//
inline NTSTATUS KDevice::ReleaseResources(PUNICODE_STRING RegistryPath)
{
	KResourceRequest R(INTERFACE_TYPE(0),ULONG(0),ULONG(0));
	BOUNDS_CHECKER(RELEASED_RESOURCES, (this));
	return R.Release(this, RegistryPath);
}
#endif // WDM

///////////////////////////////////////////////////////////////////////////////
//
//	ReleaseSystemDeviceObject
//		Deletes the system device object associated with the KDevice
//
//	Comments:
//		Just a wrapper around IoDeleteDevice
//
inline VOID KDevice::ReleaseSystemDeviceObject(void)
{
	if (IsValid())
	{
		IoDeleteDevice(m_pDeviceObject);
		m_pDeviceObject = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Macros
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//	LinkTo
//		Macro used to generate the name of a static member function that invokes
//		the named member function.
//
//	Parameters:
//		member_function_name
//			Name of the non-static member function.
//
//	Comments:
//		Various macros that declare special types of member functions use this
//		macro as a means to consistently generate the name of a hidden static
//		member that invokes the non-static member. The static member conforms to
//		the prototype required of system callbacks of the type for which it
//		serves.
//
#define LinkTo(x) x##LINK

/////////////////////////////////////////////////////////////////////////////
// DEVMEMBER_DISPATCHERS
//
//	Include files of the class library framework define this macro based on
//	the contents of file function.h, which is supplied by the driver writer.
//	For each #define DRIVER_FUNCTION_xxxx in function.h, the framework adds a
//	member declaration to the definition of DEVMEMBER_DISPATCHERS. It is left
//	to the driver writer to insert this macro inside the definition of the
//	class or classes derived from KDevice. See topic function.h for the
//	correspondence between the member names and the DRIVER_FUNCTION_xxxx
//	constants.
//
// The following macros are used in the definition of DEVMEMBER_DISPATCHERS

#define _DECL_(Name) virtual NTSTATUS Name(KIrp I);
#define __CONCAT__(a,b) a##b
#define __DOMACRO__(m,a) m(a)
#define MAYBE_MEMBER(x,y) __DOMACRO__( __CONCAT__(DRIVER_FUNCTION_,x)_DECL_, y)

//  The idea here is that MAYBE_MEMBER(x,y) will resolve to either
//  DRIVER_FUNCTION_xxxx_DECL_(y) or _DECL_(y), depending on whether
//  or not DRIVER_FUNCTION_xxxx is defined. If it is defined (to an empty
//  string) then MAYBE_MEMBER(x,y) defines to _DECL_(y), which declares
//  the IRP handler. If it is not defined, then the following macros
//  make MAYBE_MEMBER do nothing at all.

#define DRIVER_FUNCTION_CREATE_DECL_(x)
#define DRIVER_FUNCTION_CREATECLOSE_DECL_(x)
#define DRIVER_FUNCTION_CREATE_NAMED_PIPE_DECL_(x)
#define DRIVER_FUNCTION_CLOSE_DECL_(x)
#define DRIVER_FUNCTION_READ_DECL_(x)
#define DRIVER_FUNCTION_READWRITE_DECL_(x)
#define DRIVER_FUNCTION_WRITE_DECL_(x)
#define DRIVER_FUNCTION_QUERY_INFORMATION_DECL_(x)
#define DRIVER_FUNCTION_SET_INFORMATION_DECL_(x)
#define DRIVER_FUNCTION_QUERY_EA_DECL_(x)
#define DRIVER_FUNCTION_SET_EA_DECL_(x)
#define DRIVER_FUNCTION_FLUSH_BUFFERS_DECL_(x)
#define DRIVER_FUNCTION_QUERY_VOLUME_INFORMATION_DECL_(x)
#define DRIVER_FUNCTION_SET_VOLUME_INFORMATION_DECL_(x)
#define DRIVER_FUNCTION_DIRECTORY_CONTROL_DECL_(x)
#define DRIVER_FUNCTION_FILE_SYSTEM_CONTROL_DECL_(x)
#define DRIVER_FUNCTION_DEVICE_CONTROL_DECL_(x)
#define DRIVER_FUNCTION_INTERNAL_DEVICE_CONTROL_DECL_(x)
#define DRIVER_FUNCTION_SHUTDOWN_DECL_(x)
#define DRIVER_FUNCTION_LOCK_CONTROL_DECL_(x)
#define DRIVER_FUNCTION_CLEANUP_DECL_(x)
#define DRIVER_FUNCTION_CREATE_MAILSLOT_DECL_(x)
#define DRIVER_FUNCTION_QUERY_SECURITY_DECL_(x)
#define DRIVER_FUNCTION_SET_SECURITY_DECL_(x)
#define DRIVER_FUNCTION_POWER_DECL_(x)
#define DRIVER_FUNCTION_SYSTEM_CONTROL_DECL_(x)
#define DRIVER_FUNCTION_DEVICE_CHANGE_DECL_(x)
#define DRIVER_FUNCTION_QUERY_QUOTA_DECL_(x)
#define DRIVER_FUNCTION_SET_QUOTA_DECL_(x)
#define DRIVER_FUNCTION_PNP_DECL_(x)

#ifdef DRIVER_FUNCTION_STARTIO
#define DEVMEMBER_DISPATCHER_STARTIO  virtual VOID StartIo(KIrp I);
#else		// no StartIo
#define DEVMEMBER_DISPATCHER_STARTIO
#endif		// DRIVER_FUNCTION_STARTIO

// Now define DEVMEMBER_DISPATCHERS to be the set of handler
// declarations selected by the DRIVER_FUNCTION_xxxx defintions.

#define DEVMEMBER_DISPATCHERS 					 				\
	DEVMEMBER_DISPATCHER_STARTIO								\
	MAYBE_MEMBER(CREATE, Create)								\
	MAYBE_MEMBER(CREATECLOSE, CreateClose)						\
	MAYBE_MEMBER(CREATE_NAMED_PIPE, CreateNamedPipe)			\
	MAYBE_MEMBER(CLOSE, Close )									\
	MAYBE_MEMBER(READ, Read)									\
	MAYBE_MEMBER(READWRITE, ReadWrite)							\
	MAYBE_MEMBER(WRITE, Write)									\
	MAYBE_MEMBER(QUERY_INFORMATION, QueryInformation)			\
	MAYBE_MEMBER(SET_INFORMATION, SetInformation)				\
	MAYBE_MEMBER(QUERY_EA, QueryEa)								\
	MAYBE_MEMBER(SET_EA, SetEa)									\
	MAYBE_MEMBER(FLUSH_BUFFERS, FlushBuffers)					\
	MAYBE_MEMBER(QUERY_VOLUME_INFORMATION, QueryVolumeInformation) \
	MAYBE_MEMBER(SET_VOLUME_INFORMATION, SetVolumeInformation)	\
	MAYBE_MEMBER(DIRECTORY_CONTROL, DirectoryControl)			\
	MAYBE_MEMBER(FILE_SYSTEM_CONTROL, FileSystemControl)		\
	MAYBE_MEMBER(DEVICE_CONTROL, DeviceControl)					\
	MAYBE_MEMBER(INTERNAL_DEVICE_CONTROL, InternalDeviceControl)\
	MAYBE_MEMBER(SHUTDOWN, Shutdown)							\
	MAYBE_MEMBER(LOCK_CONTROL, LockControl)						\
	MAYBE_MEMBER(CLEANUP, CleanUp)								\
	MAYBE_MEMBER(CREATE_MAILSLOT, CreateMailSlot)				\
	MAYBE_MEMBER(QUERY_SECURITY, QuerySecurity)					\
	MAYBE_MEMBER(SET_SECURITY, SetSecurity)						\
	MAYBE_MEMBER(SYSTEM_CONTROL, SystemControl)					\
	MAYBE_MEMBER(DEVICE_CHANGE, DeviceChange)					\
	MAYBE_MEMBER(QUERY_QUOTA, QueryQuota)						\
	MAYBE_MEMBER(SET_QUOTA, SetQuota)


//////////////////////////////////////////////////////////////////////
// Macros for setting up various callbacks as member
// functions of KDevice subclasses:


//////////////////////////////////////////////////////////////////////
// DEVMEMBER_CANCELIRP
//
// Make a cancel routine a member of KDevice subclass
//
#define DEVMEMBER_CANCELIRP(devclass, func)			\
	VOID func(KIrp I);								\
	static VOID LinkTo(func)(						\
			PDEVICE_OBJECT pObj,					\
			PIRP pIrp)								\
	{												\
		KIrp I(pIrp);								\
		((devclass*)KDevicePTR(pObj))->func(I);		\
	}
//////////////////////////////////////////////////////////////////////
// DEVMEMBER_CONTROLLER
//
// Make a controller callback a member of KDevice subclass
//
#define DEVMEMBER_CONTROLLER(devclass, func)		\
	IO_ALLOCATION_ACTION func(KIrp I,PVOID Context);\
	static IO_ALLOCATION_ACTION LinkTo(func)(		\
		IN PDEVICE_OBJECT DeviceObject, 			\
		IN PIRP pIrp, 								\
		IN PVOID MapRegisterBase, 					\
		IN PVOID Context 							\
		)											\
	{												\
		KIrp I(pIrp);								\
		return ((devclass*)KDevicePTR(DeviceObject))\
			->func(I, Context);						\
		UNREFERENCED_PARAMETER(MapRegisterBase);	\
	}

//////////////////////////////////////////////////////////////////////
// DEVMEMBER_DPCFORISR
//
// Make a DPC a member of KDevice subclass. (See also MEMBER_DPC)
//
#define DEVMEMBER_DPCFORISR(devclass, func)			\
	VOID func(KIrp I, PVOID Context);				\
	static VOID LinkTo(func)(						\
		PKDPC dpc,									\
		PDEVICE_OBJECT pDevObj,						\
		PIRP pIrp,									\
		PVOID context)								\
	{												\
		KIrp I(pIrp);								\
		((devclass*)KDevicePTR(pDevObj))			\
			->func(I, context);						\
		UNREFERENCED_PARAMETER(dpc);				\
	}

//////////////////////////////////////////////////////////////////////
// DEVMEMBER_SYNCHCRITSECTION
//
// Make a synch crit routine a member of KDevice subclass
//
#define DEVMEMBER_SYNCHCRITSECTION(devclass, func)	\
	BOOLEAN func(PVOID context=NULL);				\
	static BOOLEAN LinkTo(func)(PVOID context)		\
	{												\
		DEVICE_INTERRUPT_SYNCH_CONTEXT* dics;		\
													\
		dics = 										\
		 (DEVICE_INTERRUPT_SYNCH_CONTEXT*)context;	\
													\
		return ((devclass*)dics->m_device)->		\
				func(dics->m_context);				\
	}

//////////////////////////////////////////////////////////////////////
// DEVMEMBER_IOTIMER
//
// Make a timer callback a member of KDevice subclass
//
#define DEVMEMBER_IOTIMER(devclass, func)			\
	VOID func(PVOID context);						\
	static VOID LinkTo(func)(						\
		IN PDEVICE_OBJECT pDevObj,					\
		IN PVOID context)							\
	{												\
		((devclass*)KDevicePTR(pDevObj))			\
			->func(context);						\
	}

//////////////////////////////////////////////////////////////////////
// DEVMEMBER_DMAREADY
//
// Make a DMA adapter callback a member of KDevice subclass
//
#define DEVMEMBER_DMAREADY(devclass, func)					\
	VOID func(KDmaTransfer* pXfer, KIrp I);					\
	static VOID LinkTo(func)(								\
		KDevice* pDevice,									\
		KIrp I,												\
		KDmaTransfer* pXfer									\
		)													\
	{														\
		((devclass*)pDevice)->func(pXfer, I);				\
	}

////////////////////////////////////////////////////////////////////////
// Declare irp dispatch table
extern NTSTATUS (KDevice::*IrpDispatchTable[IRP_MJ_MAXIMUM_FUNCTION+1])(KIrp);


#if _WDM_
#define AUTOGENERATED_DEVICE_NAME	( (PCWSTR)(LONG_PTR)(-1) )
#endif

#endif // __KDEVICE_
