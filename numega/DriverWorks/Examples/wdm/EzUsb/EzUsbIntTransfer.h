// EzUsbIntTransfer.h
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1999 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef ___KUsbInterruptTransfer___
#define ___KUsbInterruptTransfer___

//This is the return value status codes
//for client's OnInterrupt callback function
enum ON_INTERRUPT_STATUS 
{
	ON_INTERRUPT_SUCCESS = 0,
	ON_INTERRUPT_SETUP_CALLED,
	ON_INTERRUPT_FAILURE
};

//This is the prototype for the client's
//OnInterrupt callback function
typedef 
	ON_INTERRUPT_STATUS 
		(__stdcall * USB_INTERRUPT_CALLBACK)(
			PURB pUrb,
			PVOID pContext
			);

//Forward declaration
class KUsbInterruptTransfer;

//This is context for completion routine 
typedef struct _INT_COMPLETION_INFO
{
	KUsbInterruptTransfer* m_pClass;
	PURB m_pUrb;
	USB_INTERRUPT_CALLBACK m_pfnOnInterrupt;
	PVOID m_pClientContext;

} INT_COMPLETION_INFO, *PINT_COMPLETION_INFO;



//=============================================================================
// class KUsbInterruptTransfer
//
// This class encapsulates an interrupt transfer for a device on the USB.
// The basic idea of interrupt transfers on USB is that the device will 
// asynchronously signal the USB system on the host for a device specific 
// reason.  For devices that could potentially interrupt at any time, there
// must be an active Interrupt URB at all times.  The host will poll the 
// hardware at the Interval frequency stored in the Pipe object.
//
// After constructing this object, clients can call method StartPolling to 
// initiate continuous Interrupt
// Transfers for the device.  Clients provide an OnInterrupt handler that will 
// be called from the completion routine when an Interrupt Transfer is 
// successfully completed.  By returning a successful status code from the 
// OnInterrupt handler, another URB is sent to the device.  Clients can call  
// StopPolling to stop the continuous Interrupt transfers.
//
//	USAGE:
//
//	Allocate the object dynamically or make it a member of the device sub-class
//	Implement the OnInterrupt callback function in your driver.
//	Provide a transfer buffer for use in the URB.
//	Call StartPolling() to start sending Interrupt transfer requests to the device
//	The OnInterrupt handler will be called when the URB is successfully completed.
//		Use the PURB parameter to obtain the data from the device.  Return a  
//		successful status code to send another URB to the device.  Return a 
//		failure status to stop sending an URB to the device.
//		Optionally call SetupNextTransfer() to pass a different buffer for the 
//		transfer to the device for use in the URB.
//	Call StopPolling() to stop sending Interrupt transfer requests to the device
//		
class KUsbInterruptTransfer
{
//Constructors/Destructors
public:

	SAFE_DESTRUCTORS

	KUsbInterruptTransfer(
		KUsbLowerDevice* Dev,
		KUsbPipe* InterruptPipe,
		BOOLEAN bShortOk=TRUE
		);

	KUsbInterruptTransfer(void);

	NTSTATUS Initialize(
		KUsbLowerDevice* Dev,
		KUsbPipe* InterruptPipe,
		BOOLEAN bShortOk=TRUE
		);

	~KUsbInterruptTransfer(void);

//methods
public:

	//This initiates a USB interrupt transfer that is recycled (MDL form)
	NTSTATUS StartPolling(
		KMemory& Mdl,
		ULONG Length,
		USB_INTERRUPT_CALLBACK pfnOnInterrupt, 
		PVOID pClientContext=NULL
		);

	//This initiates a USB interrupt transfer that is recycled (PVOID form)
	NTSTATUS StartPolling(
		PVOID Buffer,
		ULONG Length,
		USB_INTERRUPT_CALLBACK pfnOnInterrupt, 
		PVOID pClientContext=NULL
		);

	//client's OnInterrupt handler can call this to update buffers
	//after a completed transfer (during the callback ISR) (PVOID form)
	void SetupNextTransfer(
		PVOID Buffer,
		ULONG Length
		);

	//client's OnInterrupt handler can call this to update buffers
	//after a completed transfer (during the callback ISR) (MDL form)
	void SetupNextTransfer(
		KMemory& Mdl,
		ULONG Length
		);

	void StopPolling(void);

	MEMBER_COMPLETEIRPWITHCONTEXT( INT_COMPLETION_INFO, InterruptTransferComplete )

//Accessors
public:

	ULONG InterruptsReceived(void) {return m_NumInterrupts;}
	NTSTATUS Status(void) {return m_CtorStatus;}

//data members
protected:

	//URB continuously recycled
	PURB		m_pIntUrb;
	//IRP continuously recycled
	KIrp		m_KIrp;
	//Pipe to submit the request 
	KUsbPipe*	m_InterruptPipe;
	//Flag to set SHORT_TRANSFER_OK flag in URB
	BOOLEAN		m_bShortOk;
	//Number of stacks 
	CCHAR		m_Stacks;
	NTSTATUS	m_CtorStatus;
	PVOID		m_pClientContext;
	//Counter of number of interrupts
	ULONG		m_NumInterrupts;
	//Flag indicating that the user has requested a stop
	LONG		m_lStopRequested;
	//Flag indicating that an IRP is outstanding
	LONG		m_lPolling;
};



inline KUsbInterruptTransfer::KUsbInterruptTransfer(void) :
	m_pIntUrb(NULL),
	m_KIrp(),
	m_InterruptPipe(NULL),
	m_bShortOk(FALSE),
	m_Stacks(0),
	m_CtorStatus(STATUS_SUCCESS),
	m_pClientContext(NULL),
	m_NumInterrupts(0),
	m_lStopRequested(0),
	m_lPolling(0)
{
}


inline KUsbInterruptTransfer::~KUsbInterruptTransfer(void)
{
	delete m_pIntUrb;

	if (m_KIrp.m_Irp)
		m_KIrp.Deallocate(m_KIrp);
}


//client's OnInterrupt handler can call this to update buffer
//to use during transfer after a completed transfer  (PVOID form)
inline void KUsbInterruptTransfer::SetupNextTransfer(
	PVOID Buffer,
	ULONG Length
	)
{
	m_InterruptPipe->BuildInterruptTransfer(
		Buffer,
		Length,
		m_bShortOk,
		NULL,
		m_pIntUrb
		);

	m_KIrp.Reuse();
}

//client's OnInterrupt handler can call this to update buffer
//to use during transfer after a completed transfer (MDL form)
inline void KUsbInterruptTransfer::SetupNextTransfer(
	KMemory& Mdl,
	ULONG Length
	)
{
	m_InterruptPipe->BuildInterruptTransfer(
		Mdl,
		Length,
		m_bShortOk,
		NULL,
		m_pIntUrb
		);

	m_KIrp.Reuse();
}

//client's can call this method to cancel an outstanding URB
//on the device.  
inline void KUsbInterruptTransfer::StopPolling(void)
{
	InterlockedExchange(&m_lStopRequested, 1);
	if( INTERLOCKED_COMPARE_EXCHANGE(&m_lPolling, 1, 1) )
		IoCancelIrp(m_KIrp);
}


#endif //#ifndef ___KUsbInterruptTransfer___
	