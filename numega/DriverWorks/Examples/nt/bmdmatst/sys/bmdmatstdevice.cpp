// BMDmaTstDevice.cpp
// Implementation of BMDmaTstDevice device class

#include <vdw.h>

#include "BMDmaTst.h"
#include "BMDmaTstDevice.h"

#pragma warning(disable:4065) // Allow switch statement with no cases

extern	KTrace	t;			// Global driver trace object	

#pragma code_seg("INIT")

////////////////////////////////////////////////////////////////////////////////
// BMDmaTstDevice Constructor
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

BMDmaTstDevice::BMDmaTstDevice(ULONG Unit) :
		KDevice(),
			m_IsrProxyTimer(), m_Busy(0)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device BMDmaTstDevice unit number " << Unit << " status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}
	m_Unit = Unit;

	m_RegPath = CreateRegistryPath(L"BMDmaTstDevice", Unit);
	if (m_RegPath == NULL)
	{
		// Error, cannot allocate memory for registry path
		t << "Failed to create registry path\n";
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

	// Load data members from the registry
	LoadRegistryParameters();

	// Set up the DMA Adapter.  Since this is a busmaster device,
	// there is no motherboard resource associated with the DMA.
	m_ConstructorStatus = InitializeAdapter();
	if ( !NT_SUCCESS(m_ConstructorStatus) )
	{
		delete m_RegPath;
		m_ConstructorStatus = STATUS_UNSUCCESSFUL;
		t << "Failed to create DMA adapter. status " << (ULONG) m_ConstructorStatus << EOL;
		return;
	}

	// Setup DPC
	m_DPC.Setup(LinkTo(DpcForIsr), this);

}

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
//		The registry variables are loaded from the registry path:
//		HKEY_LOCAL_MACHINE\
//		  SYSTEM\
//		    CurrentControlSet\
//			  Services\
//			    BMDmaTst\
//				  Devices\
//				    BMDmaTstDevice\
//				      <unit number>\
//					    <param name>

void BMDmaTstDevice::LoadRegistryParameters(void)
{

	KRegistryKey Params(*m_RegPath);
	if (!NT_SUCCESS(Params.LastError()))
	{
		// Error, cannot open device registry key
		t << "Failed to create registry key\n";
		m_ConstructorStatus = STATUS_UNSUCCESSFUL;
		return;
	}

	// Load the registry parameters

	// The member variable m_ScatterGather indicates whether or not our device supports
	// scatter gather capability.  Note: This implies that scatter gather is supported
	// by the actual hardware.  If this is not the case, NT will still make use of
	// "system" scatter gather in which map registers are used to map a fragmented
	// user buffer to a contiguous block of memory maintained by the kernel for DMA
	// use.
	m_ScatterGather = FALSE;
	Params.QueryValue(L"ScatterGather", &m_ScatterGather);
	t << "m_ScatterGather loaded from registry, resulting value: [" << m_ScatterGather << "]\n";

	// If we do support scatter gather, the member variable m_MaxXferRegs indicates
	// how many pairs of address/size descriptors the hardware can handle.  This will
	// be the max number used by the underlying class when setting up a set of descriptors
	// for use in a transfer operation.  Note that this variable is meaningless unless
	// the hardware supports scatter gather.  Note also that the number of map registers
	// reserved by the underlying class (via the system) is dependent on the MaxXferSize,
	// even though no more than the MaxXferRegs will ever be used.  This may result in
	// underuse of map register resources granted by the system (if say the specified
	// MaxXferSize is large but the number of pairs supported is small).  While not
	// usually a problem for X86 hals where map registers are limitless this may not
	// be desirable on all systems.  The algorithm that the underlying kernel call
	// uses to calculate the number of map registers from the MaxXferSize is a bit
	// bizzare (and does not match the DDK documentation) but roughly works out to
	// MaxXferSize/PAGE_SIZE + 1.
	m_MaxXferRegs = 16;
	Params.QueryValue(L"MaxXferRegs", &m_MaxXferRegs);
	t << "m_MaxXferRegs loaded from registry, resulting value: [" << m_MaxXferRegs << "]\n";

	// The member variable m_MaxXferSize indicates the maximum size of a DMA transfer
	// we expect to undertake.  It is used by the system in the underlying call to
	// reserve a number of map registers for used during a DMA transfer.  Larger
	// transfers can be undertaken by the KDmaTransfer class logic, but multiple
	// transfers will be required as the map registers will need to be used multiple
	// times to complete the entire transfer.
	m_MaxXferSize = 0x20000;
	Params.QueryValue(L"MaxXferSize", &m_MaxXferSize);
	t << "m_MaxXferSize loaded from registry, resulting value: [" << m_MaxXferSize << "]\n";
}

////////////////////////////////////////////////////////////////////////
// InitializeAdapter
//
// This member function gets the DMA channel number from the registry
// and tries to claim it as a resource. 
//
// If successful, it then creates an instance of KDmaAdapter for this
// channel.
//
NTSTATUS BMDmaTstDevice::InitializeAdapter()
{
	DEVICE_DESCRIPTION dd;

// Set up the adapter description for the slaved device

	RtlZeroMemory(&dd, sizeof(DEVICE_DESCRIPTION));

	dd.Version			= DEVICE_DESCRIPTION_VERSION;
	dd.Master			= TRUE;
	dd.ScatterGather 	= UCHAR(m_ScatterGather);
	dd.DemandMode		= FALSE;
	dd.AutoInitialize 	= FALSE;
	dd.Dma32BitAddresses= TRUE;
	dd.IgnoreCount 		= FALSE;
	dd.BusNumber 		= 0;
	dd.DmaChannel		= 0;
	dd.InterfaceType	= PCIBus;
	dd.DmaWidth 		= Width32Bits;
	dd.DmaSpeed			= Compatible;
	dd.MaximumLength	= m_MaxXferSize;

// Initialize the adapter object

	m_DmaAdapter.Initialize(&dd, m_MaxXferRegs);

	if ((PADAPTER_OBJECT)m_DmaAdapter == NULL)
		return STATUS_UNSUCCESSFUL;
	else
	{
		t << "m_DmaAdapter.MaxRegisters is " << m_DmaAdapter.MaxRegisters() << EOL;
		return STATUS_SUCCESS;
	}

}

#pragma code_seg()

////////////////////////////////////////////////////////////////////////
// BMDmaTstDevice Destructor
//
//		This is the destructor for the BMDmaTstDevice
//
// Input
//		None
//
// Output
//		None
//
// Notes
//		Disconnect and release resources here.

BMDmaTstDevice::~BMDmaTstDevice()
{
	delete m_RegPath;
}

////////////////////////////////////////////////////////////////////////
// Cancel (member of BMDmaTstDevice)
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

VOID BMDmaTstDevice::Cancel(KIrp I)
{
	t << "Cancel IRP " << I;
	KDeviceQueue dq(DeviceQueue());

	// Test if the IRP is the current IRP.
	if ( (PIRP)I == CurrentIrp() )
	{

		// If so, NULL it out, release the global cancel spinlock, then
		// complete it as canceled and start the next IRP.  Note that
		// if we got here, the IRP was still in a cancelable state when
		// it was canceled, and most likely is just about to start being
		// processed on the device.
		CurrentIrp() = NULL;
		CancelSpinLock::Release(I.CancelIrql());
		t << "IRP canceled " << I << EOL;
		I.Information() = 0;
		I.Status() = STATUS_CANCELLED;
		NextIrp(I);
	}
	// See if the IRP can be removed from the device queue.
	else if (dq.RemoveSpecificEntry(I))
	{

		// If so, release the global cancel spinlock and complete it
		// as canceled.
		CancelSpinLock::Release(I.CancelIrql());
		t << "IRP canceled " << I << EOL;
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
	}
	else
	{

		// If we got here the IRP wasn't the current IRP, and wasn't in the
		// device queue.  This could happen if the IRP was removed by the
		// device queue's cleanup routine, just as it was being canceled.
		// In this case we have no work to do, since the cleanup routine
		// will cancel the IRP.  Simply release the global cancel spinlock.
		CancelSpinLock::Release(I.CancelIrql());
	}
}
////////////////////////////////////////////////////////////////////////
// StartIo (member of BMDmaTstDevice)
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

VOID BMDmaTstDevice::StartIo(KIrp I)
{
	t << "Entering StartIo, " << I;

	// Check to see if the IRP has already been canceled.

	if (!I.TestAndSetCancelRoutine(
			LinkTo(Cancel),
			NULL,
			CurrentIrp()) )
				return;

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
			// Unexpected IRP received in Startio
			t << "Unexpected IRP received in Startio: " << I;
			// NextIrp completes this IRP and starts processing
			// for the next IRP in the queue.
			ASSERT(FALSE);
			I.Status() = STATUS_INVALID_PARAMETER;
			NextIrp(I);
			break;
	}
}


////////////////////////////////////////////////////////////////////////
// SerialRead (member of BMDmaTstDevice)
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
//		queue.  This guarantees that multiple requests are
//		never processed simultaneously.
//
//		This routine is called at dispatch level.

void BMDmaTstDevice::SerialRead(KIrp I)
{
	t << "Entering BMDmaTstDevice::SerialRead, " << I;
	NTSTATUS status		= STATUS_SUCCESS;

	// Create a new DMA transfer object for this IRP
	m_CurrentTransfer = new(NonPagedPool) KDmaTransfer(this, &m_DmaAdapter);

	if ( m_CurrentTransfer == NULL )
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		DbgPrint("BMDmaTstDevice: unable to allocate transfer object: %x\n", status);

		I.Information() = 0;
		I.Status() = status;
		NextIrp(I);
	}

	status = m_CurrentTransfer->Initiate(
		I.Mdl(),
		(I.MajorFunction() == IRP_MJ_READ) ? FromDeviceToMemory : FromMemoryToDevice,
		LinkTo(OnDmaReady)
		);

	// If the transfer cannot be initiated, complete it with an error status.
	if ( ! NT_SUCCESS(status) )
	{
		DbgPrint("BMDmaTstDevice: unable to initiate transfer: %x\n", status);

		delete m_CurrentTransfer;
		m_CurrentTransfer = NULL;

		I.Information() = 0;
		I.Status() = status;
		NextIrp(I);
	}

}

////////////////////////////////////////////////////////////////////////
// Read (member of BMDmaTstDevice)
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
//		The requests are queued to enforce serialization,
//		so that	multiple requests are not processed	
//		simultaneously.

NTSTATUS BMDmaTstDevice::Read(KIrp I)
{
	t << "Entering read, " << I;
// TODO:	Check the incoming request.  Replace "FALSE" in the following
//			line with a check that returns TRUE if the request is not valid.
    if (FALSE)
	{
		I.Information() = 0;
		return I.Complete(STATUS_INVALID_PARAMETER);
	}

	// Always ok to read 0 elements
	if (I.ReadSize() == 0)
	{
		I.Information() = 0;
		return I.Complete(STATUS_SUCCESS);
	}

	// Queue the IRP for processing in StartIO
	// The read function is performed in SerialRead
	return QueueIrp(I, LinkTo(Cancel));
}

////////////////////////////////////////////////////////////////////////
// SerialWrite (member of BMDmaTstDevice)
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
//		queue.  This guarantees that multiple requests are
//		never processed simultaneously.

void BMDmaTstDevice::SerialWrite(KIrp I)
{
	t << "Entering BMDmaTstDevice::SerialWrite, " << I;
	NTSTATUS status		= STATUS_SUCCESS;

	// Create a new DMA transfer object for this IRP
	m_CurrentTransfer = new(NonPagedPool) KDmaTransfer(this, &m_DmaAdapter);

	if ( m_CurrentTransfer == NULL )
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		DbgPrint("BMDmaTstDevice: unable to allocate transfer object: %x\n", status);

		I.Information() = 0;
		I.Status() = status;
		NextIrp(I);
	}

	status = m_CurrentTransfer->Initiate(
		I.Mdl(),
		(I.MajorFunction() == IRP_MJ_READ) ? FromDeviceToMemory : FromMemoryToDevice,
		LinkTo(OnDmaReady)
		);

	// If the transfer cannot be initiated, complete it with an error status.
	if ( ! NT_SUCCESS(status) )
	{
		DbgPrint("BMDmaTstDevice: unable to initiate transfer: %x\n", status);

		delete m_CurrentTransfer;
		m_CurrentTransfer = NULL;

		I.Information() = 0;
		I.Status() = status;
		NextIrp(I);
	}

}


////////////////////////////////////////////////////////////////////////
// Write (member of BMDmaTstDevice)
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
//		The requests are queued to enforce serialization,
//		so that	multiple requests are not processed	
//		simultaneously.

NTSTATUS BMDmaTstDevice::Write(KIrp I)
{
	t << "Entering write, " << I;
// TODO:	Check the incoming request.  Replace "FALSE" in the following
//			line with a check that returns TRUE if the request is not valid.
    if (FALSE)
	{
		I.Information() = 0;
		return I.Complete(STATUS_INVALID_PARAMETER);
	}

	// Always ok to write 0 elements
	if (I.WriteSize() == 0)
	{
		I.Information() = 0;
		return I.Complete(STATUS_SUCCESS);
	}

	// Queue the IRP for processing in StartIO
	// The write function is performed in SerialWrite
	return QueueIrp(I, LinkTo(Cancel));
}

////////////////////////////////////////////////////////////////////////
// SerialDeviceControl (member of BMDmaTstDevice)
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

VOID BMDmaTstDevice::SerialDeviceControl(KIrp I)
{
	NTSTATUS status;
	t << "Entering BMDmaTstDevice::SerialDeviceControl, " << I;
    switch (I.IoctlCode())
	{
		default:
			// Unrecognized request
			status = STATUS_INVALID_PARAMETER;
			break;
	}

	I.Status() = status;

	// NextIrp completes this IRP and starts  
	// processing for the next IRP in the queue.
	NextIrp(I);
}

////////////////////////////////////////////////////////////////////////
// DeviceControl (member of BMDmaTstDevice)
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

NTSTATUS BMDmaTstDevice::DeviceControl(KIrp I) 
{
	NTSTATUS status;

	t << "Entering BMDmaTstDevice::Device Control, " << I;
	switch (I.IoctlCode())
	{
		default:
			// Unrecognized IOCTL request
			status = STATUS_INVALID_PARAMETER;
			break;
	};

	// Drop out of case statement only if request has been handled, not queued
	return I.Complete(status);
}

////////////////////////////////////////////////////////////////////////
// Cleanup (member of BMDmaTstDevice)
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

NTSTATUS BMDmaTstDevice::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I;

    KDeviceQueue dq(DeviceQueue());
	dq.CleanUp(I.FileObject());
	return I.Complete(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
// Create (member of BMDmaTstDevice)
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

NTSTATUS BMDmaTstDevice::Create(KIrp I)
{
	NTSTATUS status;

	t << "Entering Create, " << I;
	I.Information() = 0;

	if ( InterlockedExchange(&m_Busy, TRUE) )
		status = STATUS_DEVICE_BUSY;

	else
		status = STATUS_SUCCESS;

	return I.Complete(status);
}

////////////////////////////////////////////////////////////////////////
// Close (member of BMDmaTstDevice)
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

NTSTATUS BMDmaTstDevice::Close(KIrp I)
{
	t << "Entering Close, " << I;

	m_Busy = FALSE;

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
// Shutdown (member of BMDmaTstDevice)
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

NTSTATUS BMDmaTstDevice::Shutdown(KIrp I)
{
	t << "Entering Shutdown\n";
// TODO:	Insert SHUTDOWN code

   return I.Complete(STATUS_SUCCESS);
}

// 
// This device is a busmaster, so it can write data directly into
// physical memory without the aid of the motherboard DMA hardware.
// The registry parameter loaded when the device was created controls
// scatter/gather.  Our routine to reload the DMA hardware must
// be called for each "chunk" of the transfer.  A "chunk" is a single
// descriptor if not doing scatter/gather, or the number of available
// descriptors on the device if using scatter/gather.  The size of a chunk
// is out of our control and is dependant on the size of the buffer
// that NT uses for our DMA transfer.  If our I/O size is larger
// than the combined size of the nuber of "chunks" provided by the system
// for our use, then the Read/Write request is broken into multiple DMA
// operations.
//

// 
// The order of operations is as follows:
// 
//  --> OnDmaReady() - Called at least twice.  The first call starts the
//  |                 DMA operation.  The last call terminates the operation
//  |                 and completes the IRP.  If the size of the Read() request
//  |                 is larger than can be satisfied in a single DMA transfer,
//  |                 then this routine will be called more than two times.
//  |
//  |  IsrProxy() - Invoked when our DMA hardware (in this case just a system
//	|		   timer emulating hardware) signals that its count has
//  |          run down to 0.  This does not necessarily mean that the
//  |          Read/Write is done.  It may mean that we have reached the end
//  |          of the current chunk(s) of data, and that NT must give us
//  |          the address and length of the next chunk(s).
//  |
//  -- DpcForIsr() - This is the DPC that is invoked by the IsrProxy.  It
//                        calls the Continue() member of the transfer class,
//                        which causes the DMA controller to possibly be set up
//                        again (if there are more bytes remaining), and, in
//                        any case, causes OnDmaReady() to be called again.
// 

//////////////////////////////////////////////////////////////////////////////
// OnDmaReady
//
// The DMA transfer object invokes this function when it has set up the system
// DMA controller for the current segment.  Since we have defined our
// DMA controller as a busmaster, there is no need to use a system DMA
// channel.  Therefore, there will never be any contention here.
//
// The hardware must do two potentially seperate things here.  If this
// is the first call to OnDmaReady() (indicated when BytesRemaining is
// equal to the full size of the Read request), then we must start the
// A/D operation running.
//
// In the case of a data acquisition board, there is usually an
// onboard FIFO.  In this case, we want to tell the board to go for the
// whole size of the Read().  If the DMA must be broken up into smaller
// chunks, then this routine is called multiple times.  We only want
// to program the physical board on the *first* invocation of this
// routine.  That allows the data acquisition to proceed into the
// board's FIFO while the system sets up the next chunk of the DMA
// transfer.
//
// On the first call, and on subsequent calls as long as BytesRemaining is
// > 0, we must also program the DMA hardware on our board with the buffer
// information provided by the system.
//

VOID BMDmaTstDevice::OnDmaReady(KDmaTransfer* pXfer, KIrp I)
{
	t << "Entering OnDmaReady, " << I;

	// All KDmaTransfer callbacks must first check to see if there are any bytes
	// left to transfer.
	if (pXfer->BytesRemaining() == 0)
	{

		// If there are no bytes left to transfer, the callback must call
		// Terminate(). Then it completes the IRP with STATUS_SUCCESS.
		t << "terminating transfer\n";

		StopDMA();
		pXfer->Terminate();
	
		I.Information() = I.ReadSize();
		I.Status() = STATUS_SUCCESS;
		NextIrp(I);

		m_CurrentTransfer = NULL;
		delete pXfer;
		return;
	}

	// Check if the irp was canceled, so we could complete it before 
	// starting next segment. Note: our cancel routine does not complete
	// the irp if the dma transfer has started.
	if (I.WasCanceled()) {
		StopDMA();
		pXfer->Terminate();

		I.SetCancelRoutine(NULL);
		I.Status() = STATUS_CANCELLED; 
		I.Information() = 0;
		NextIrp(I);

		m_CurrentTransfer = NULL;
		delete pXfer;
		return;
	}

	// We must get the descriptor for the physical memory location for
	// the DMA transfer.

	PTRANSFER_DESCRIPTOR ptd;

#if 1 // new way
	while (pXfer->SequenceTransferDescriptors(&ptd)) {
		// program the h/w using  ppTD
		t << " Physical address 0x" << ptd->td_PhysAddr.LowPart << ". Length is 0x"
			<< ptd->td_Length << "." << EOL;
	}

#else 	// old way
	ULONG nTds = pXfer->GetTransferDescriptors(&ptd);

	t << "nTds = " << nTds  << EOL;
	for (ULONG ii=0; ii < nTds; ii++)
		t << " Physical address 0x" << ptd[ii].td_PhysAddr.LowPart << ". Length is 0x"
			<< ptd[ii].td_Length << "." << EOL;
#endif // new/old way

	// Normally at this point we would use the transfer descriptors to set up
	// the hardware for the next DMA.  Once the transfer was setup, it would be
	// initiated and once the transfer was complete, the interrupt would be
	// signaled. We simply set up a timer to emmulate this process.
	LONGLONG DueTime = -10000000;		// 100ns units (1sec)
	m_IsrProxyTimer.Set(
		DueTime,
		LinkTo(IsrProxyTimer),
		this
		);

	// If this is the first time through, then start the DMA going.
	// We only want to do this ONCE for a given Read transfer.  That
	// way, our data will be collected smoothly, without interruptions
	// or dropouts.
	if ((ULONG) pXfer->BytesRemaining() == I.ReadSize())
		StartDMA(pXfer->BytesRemaining());
}

//////////////////////////////////////////////////////////////////////////////
// IsrProxyTimer
//
// This is a proxy for an Interrupt Service Routine.  It is invoked when the
// DMA write count reaches zero in a normal device and when the timer expires
// in our example
//
// Here, we call a routine in the physical device handler class that
// acknowledges and shuts off the interrupt request.  This function
// should return FALSE if it is an unexpected or spurious interrupt
// (which can sometimes be generated at odd times; during board
// initialization, for instance).  If it is a valid DMA interrupt, then
// the Isr queues a DPC, which will handle the continuation of the
// DMA operation.
//
VOID BMDmaTstDevice::IsrProxyTimer(PVOID Arg1, PVOID Arg2)
{
	if(AcknowledgeInterrupt())
		m_DPC.Request(NULL, NULL);
}

//////////////////////////////////////////////////////////////////////////////
// DpcForIsr
// 
// This is the DPC which is queued by the timed callback member object.
// It handles the continuation of the DMA operation (if necessary).
// The Continue() call causes the OnDmaReady() routine to once
// again get invoked.
//
VOID BMDmaTstDevice::DpcForIsr(PVOID Arg1, PVOID Arg2)
{
	t << "Entering DpcForIsr" << EOL;
	m_CurrentTransfer->Continue(UseTransferSize);
}

///////////////////////////////////////////////////////////////////////////////
// StartDMA
//
// Start the DMA.
//
VOID BMDmaTstDevice::StartDMA(ULONG NBytes)
{
}

///////////////////////////////////////////////////////////////////////////////
// StopDMA
//
// Stop the DMA.
//
VOID BMDmaTstDevice::StopDMA(void)
{
	m_IsrProxyTimer.Cancel();
}

///////////////////////////////////////////////////////////////////////////////
// AcknowledgeInterrupt
//
// Called from ISR.  Do whatever is necessary in hardware to
// acknowledge and stop the interrupt.
//
BOOLEAN BMDmaTstDevice::AcknowledgeInterrupt(void)
{
	return TRUE;
}
