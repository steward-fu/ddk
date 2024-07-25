// EzUsbIsoTransfer.h
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

#ifndef __EzUsbIsoTransfer__h__
#define __EzUsbIsoTransfer__h__


#define USB_MAX_NUM_ISO_PACKETS_IN_AN_URB	255

//MACROs for iso packets
#define ISO_PKT_STATUS(pUrb, i) pUrb->UrbIsochronousTransfer.IsoPacket[i].Status
#define ISO_PKT_LENGTH(pUrb, i) pUrb->UrbIsochronousTransfer.IsoPacket[i].Length


//This is the typedef for client callback function
typedef 
VOID (*PISO_TRANSFER_COMPLETE_CALLBACK)(
			PVOID pContext
			);


//This is context for completion routine 
typedef struct _ISOCH_COMPLETION_INFO
{
	PVOID m_pClass;
	PURB m_pUrb;
	PVOID m_pClientContext;
	PISO_TRANSFER_COMPLETE_CALLBACK m_pfnClientCallback;

} ISOCH_COMPLETION_INFO, *PISOCH_COMPLETION_INFO;


//This is the return value status codes
//for client's callback function
enum USB_STREAM_STATUS 
{
	USB_STREAM_STATUS_CONTINUE = 0,
	USB_STREAM_STATUS_STOP
};


//=============================================================================
// template< class D >
// class KUsbIsochStream
//
//	This class abstracts a continuous stream of IRP's from a driver to USBD.
//
template< class D >
class KUsbIsochStream 
{

//Constructors/Destructors
public:

	KUsbIsochStream<D>(void) 	
	{
		Data = NULL;
		m_bStopTheStream = FALSE;
		m_bStarted = FALSE;
		m_NumIrps = 0; 
		m_ConstructorStatus = STATUS_SUCCESS;
		m_pIsoXferDoneEvent = NULL;
		m_ppIrp = NULL;
		m_pDev = NULL;
		m_pContext = NULL;
	}

	NTSTATUS Initialize(
		D* d,
		KUsbLowerDevice* dev
		);

	SAFE_DESTRUCTORS;

	~KUsbIsochStream<D>(void);

	void Invalidate(void);

	BOOLEAN IsValid(void) { return NULL != Data;}


//Methods
public:
	//Initiate continuous isoch xfers (asynchronous form)
	NTSTATUS StartStream(
		PISO_TRANSFER_COMPLETE_CALLBACK pfnCallback,
		PVOID pClientContext=NULL
		);

	//Initiate continuous isoch xfers (synchronous form)
	NTSTATUS StartStream(ULONG mSecTimeOut=0);

	//stop isoch xfers (eventually)  
	NTSTATUS StopStream(
		BOOLEAN bWait = TRUE, 
		ULONG mSecTimeOut = 100
		);

	BOOLEAN IsStarted(void){ return m_bStarted; }
	BOOLEAN StopRequested(void){ return m_bStopTheStream; }

protected:
	//used by synch form of Start to wait for completion routine to signal event
	NTSTATUS WaitForTransferToFinish(ULONG mSecTimeOut);  

	//completion routine(s)
	NTSTATUS IsoTransferCompletionRoutine(KIrp I, PISOCH_COMPLETION_INFO pContext);	
	static NTSTATUS __stdcall IsoTransferComplete(			
		PDEVICE_OBJECT pDevObj,						
		PIRP pIrp,									
		PVOID context
		)								
	{												
		KIrp I(pIrp);								
		return reinterpret_cast<KUsbIsochStream<D>*>(((PISOCH_COMPLETION_INFO)context)->m_pClass)	
			->IsoTransferCompletionRoutine(I, (PISOCH_COMPLETION_INFO)context);		
		UNREFERENCED_PARAMETER(pDevObj);			
	}

//Data members
protected:

	D* Data;
	KUsbLowerDevice* m_pDev;

	//Count of IRPs in use. 
	KInterlockedCounter m_PendingIrpCount;
	//event set in completion routine 
	KEvent* m_pIsoXferDoneEvent;
	//Pointer to IRPs
	PIRP* m_ppIrp;
	//Pointer to completion routine context
	PISOCH_COMPLETION_INFO m_pContext;
	//flag to stop the stream
	BOOLEAN m_bStopTheStream;
	//flag indicating that the stream has been started
	BOOLEAN m_bStarted;
	NTSTATUS m_ConstructorStatus;
	//Total number of IRPs sent
	ULONG m_dwTotalTransfers;
	//this is how many IRPs are used
	UCHAR m_NumIrps; 
};

template <class D>
NTSTATUS KUsbIsochStream<D>::Initialize(
	D* pD,
	KUsbLowerDevice* pDev
	)
{
	ASSERT( pDev );
	ASSERT( pD );

//Initialize data members
	m_pDev = pDev;
	Data = pD;
	m_dwTotalTransfers = 0;
	m_bStopTheStream = FALSE;
	m_NumIrps = pD->NumberOfUrbs();

	ASSERT(m_NumIrps);

	m_pIsoXferDoneEvent = new (NonPagedPool) KEvent(SynchronizationEvent, FALSE);

	if( NULL == m_pIsoXferDoneEvent )
	{
		return m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
	}

	m_ppIrp = new (NonPagedPool) PIRP[m_NumIrps];

	if( NULL == m_ppIrp )
	{
		delete m_pIsoXferDoneEvent;
		m_pIsoXferDoneEvent = NULL;
		return m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
	}

	CCHAR stacks = static_cast<UCHAR>(pDev->StackRequirement());
	ASSERT( stacks );

	for( UCHAR i = 0; i < m_NumIrps; i++)
	{
		m_ppIrp[i] = IoAllocateIrp(stacks, FALSE);

		if( NULL == m_ppIrp[i] )
		{
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}
	}

	//check for successful completion of the above loop
	if( STATUS_INSUFFICIENT_RESOURCES == m_ConstructorStatus )
	{
		for( UCHAR j = 0; j < i; j++ )
		{
			IoFreeIrp( m_ppIrp[j] );
		}
	
		delete m_pIsoXferDoneEvent;
		m_pIsoXferDoneEvent = NULL;
		delete m_ppIrp;
		m_ppIrp = NULL;
	}

	return m_ConstructorStatus;
}

template <class D>
KUsbIsochStream<D>::~KUsbIsochStream<D>(void)
{
	if( IsValid() )
		Invalidate();
}

template <class D>
void KUsbIsochStream<D>::Invalidate(void)
{

	for( UCHAR k = 0; k < m_NumIrps; k++)
	{
		if( m_ppIrp[k] )
			IoFreeIrp( m_ppIrp[k] );
	}

	delete m_ppIrp;
	m_ppIrp = NULL;

	delete m_pIsoXferDoneEvent;
	m_pIsoXferDoneEvent = NULL;

	Data = NULL;
	m_bStopTheStream = FALSE;
	m_bStarted = FALSE;
	m_NumIrps = 0; 
	m_ConstructorStatus = STATUS_SUCCESS;
}

template <class D>
NTSTATUS KUsbIsochStream<D>::StartStream(ULONG mSecTimeOut)
{
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );

	NTSTATUS status = StartStream(PISO_TRANSFER_COMPLETE_CALLBACK(NULL));

	if( STATUS_SUCCESS == status )
	{
		status = WaitForTransferToFinish(mSecTimeOut);
	}

	return status;
}

template <class D>
NTSTATUS KUsbIsochStream<D>::StartStream(
	PISO_TRANSFER_COMPLETE_CALLBACK pfnCallback,
	PVOID pClientContext
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	m_pContext = new (NonPagedPool) ISOCH_COMPLETION_INFO[m_NumIrps];

	if( NULL == m_pContext )
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	for( UCHAR i = 0; i < m_NumIrps; i++ )
	{

		PURB pUrb = Data->GetUrb(i);

		if( pUrb )
		{
			m_pContext[i].m_pClass = this;
			m_pContext[i].m_pUrb = pUrb;
			m_pContext[i].m_pClientContext = pClientContext;
			m_pContext[i].m_pfnClientCallback = pfnCallback;

			++m_PendingIrpCount;
			status = m_pDev->SubmitUrb(m_ppIrp[i], pUrb, IsoTransferComplete, &m_pContext[i]);
		}
	}

	m_bStarted = TRUE;

	return STATUS_SUCCESS;
}

template <class D>
NTSTATUS KUsbIsochStream<D>::StopStream(BOOLEAN bWait, ULONG mSecTimeOut)
{
	NTSTATUS status = STATUS_SUCCESS;

	m_bStopTheStream = TRUE;

	if(bWait)
	{
		ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );
		status = WaitForTransferToFinish(mSecTimeOut);  
	}

	m_bStarted = FALSE;

	return status;
}

template <class D>
NTSTATUS KUsbIsochStream<D>::IsoTransferCompletionRoutine(
	KIrp I, 
	PISOCH_COMPLETION_INFO pContext
	)
{
	ASSERT( pContext );
	PURB pUrb = pContext->m_pUrb;
	ASSERT( pUrb );

	m_dwTotalTransfers++;

	BOUNDS_CHECKER(COMPLETED_URB, (m_pDev, pUrb, I, I.Status()) );

	USB_STREAM_STATUS StreamStatus = Data->OnCompletedUrb(pUrb, I.Status());

	if( (USB_STREAM_STATUS_CONTINUE == StreamStatus) && !StopRequested() )
	{
// handle stream continuation
		I.Reuse();

		m_pDev->SubmitUrb(I, pUrb, IsoTransferComplete, pContext);
		return STATUS_MORE_PROCESSING_REQUIRED;
	}

// The stream has been stopped (This URB was not re-submitted)
	if(! --m_PendingIrpCount)
	{
		//for synchronous starts, set event that StartStream is waiting on
		m_pIsoXferDoneEvent->Set();
		
		//for asynchronous starts, call client callback routine
		if( pContext->m_pfnClientCallback )
		{
			pContext->m_pfnClientCallback(
				pContext->m_pClientContext
				);
		}

		delete m_pContext;
		m_bStarted = FALSE;
	}

	return STATUS_MORE_PROCESSING_REQUIRED;
}

template <class T>
NTSTATUS KUsbIsochStream<T>::WaitForTransferToFinish(ULONG mSecTimeOut)  
{ 
	ASSERT( KeGetCurrentIrql() < DISPATCH_LEVEL );

	ULONGLONG TimeOut100nSec;
	PLARGE_INTEGER pTimeOut;
	NTSTATUS status = STATUS_SUCCESS;

	if( 0 != mSecTimeOut )
	{
		// it's positive, and then convert to negative afterwards.
		LONG mSecTimeOutSigned = LONG(mSecTimeOut);
		
		if(mSecTimeOutSigned < 0) 
		{
			mSecTimeOutSigned = -mSecTimeOutSigned;
		}

		mSecTimeOut = ULONG(mSecTimeOutSigned);

		// Calculate wait as 100ns intervals.  Negative is relative
		TimeOut100nSec = mSecTimeOut;
		TimeOut100nSec *= -10000;	// convert units
		pTimeOut = PLARGE_INTEGER(&TimeOut100nSec);		
	}			
	else
	{
		pTimeOut = NULL;
	}

	status = m_pIsoXferDoneEvent->Wait(KernelMode, FALSE, pTimeOut, Executive); 

	return status;
}



//=============================================================================
// class EzUsbIsoTransferBase
//
// This class is a base class for EzUsbIsoTransfer and EzUsbDriverManagedIsoTransfer.
// 
class EzUsbIsoTransferBase
{
//Construction/Destruction
protected:
	EzUsbIsoTransferBase(
		KUsbPipe& pipe,
		USHORT PacketSize=0,
		UCHAR PacketsPerUrb=0,
		UCHAR NumUrbs=2
		);

	SAFE_DESTRUCTORS

	~EzUsbIsoTransferBase(void);

private:
	EzUsbIsoTransferBase(void);
	EzUsbIsoTransferBase(EzUsbIsoTransferBase&);
	EzUsbIsoTransferBase& operator=(EzUsbIsoTransferBase&);

//Methods
public:
	PURB GetUrb(UCHAR index);
	UCHAR NumberOfUrbs(void)	{return m_NumUrbs;}
	NTSTATUS Status(void)		{return m_ConstructorStatus;}
	ULONG TotalPackets(void)	{return m_dwTotalPacketsProcessed;}
	ULONG TotalBytes(void)		{return m_dwTotalBytesProcessed;}
	ULONG ErrorPackets(void)	{return m_dwErrorPacketCount;}

//Data Members
protected:
	KUsbPipe& m_Pipe;
	//pointer to an array of PMDLs
	MDL** m_ppMdl;	 
	//pointer to an array of PURBs
	PVOID m_pUrb;	 
	//size in bytes of an URB
	ULONG m_UrbSize; 
	//number of packets per URB
	UCHAR m_PacketsPerUrb;
	//size of data payload in one packet
	USHORT m_PacketSize;
	//Direction of transfer TRUE if its an IN(read from device), FALSE if its an OUT(write to device)
	bool m_bRead;
	NTSTATUS m_ConstructorStatus;
	//Number of URBs to use for transfers
	UCHAR m_NumUrbs;
	//Size of the TransferBuffer
	ULONG m_TransferSize;
	//total number of packets processed
	ULONG m_dwTotalPacketsProcessed;
	//total number of successful bytes processed
	ULONG m_dwTotalBytesProcessed;
	//total number of packets with bad status
	ULONG m_dwErrorPacketCount;
};

inline PURB EzUsbIsoTransferBase::GetUrb(UCHAR index)
{
	return reinterpret_cast<PURB>(reinterpret_cast<PUCHAR>(m_pUrb) + m_UrbSize * index);
}



//=============================================================================
// class EzUsbIsoTransfer
//
// This class abstracts an isochronous transfer.  It can be used for reads 
// and writes.  The class accepts a KMemory object that models a locked buffer
// to use in the transfer.  The class will segment the buffer into URB's.  
// After constructing an instance with a valid KMemory object with a resident
// locked buffer, drivers call method Start to begin isochronous transfers.
// The tranfer will be stopped on failure or by calling method Stop.
// 
class EzUsbIsoTransfer : public EzUsbIsoTransferBase
{
//Construction/Destruction
public:
	EzUsbIsoTransfer(
		KMemory& Mdl,			//Data Buffer
		ULONG Length,			//Size of Data Buffer in bytes
		KUsbPipe& pipe,			//Pipe to perform the isochronous transfer
		USHORT PacketSize=0,	//Iso Packet Size to use in the transfer (default is max for the pipe)
		UCHAR PacketsPerUrb=0,	//Number of packets per urb (max of 255)
		PVOID pDescBuffer=NULL,	//Optional pointer to packet buffer
		UCHAR NumUrbs=2			//Number of queued transfers at a time
		);

	SAFE_DESTRUCTORS;

	~EzUsbIsoTransfer(void);

private:
	EzUsbIsoTransfer(void);
	EzUsbIsoTransfer(EzUsbIsoTransfer&);
	EzUsbIsoTransfer& operator=(EzUsbIsoTransfer&);

//Methods
public:
	//Initiate continuous isoch xfers (asynchronous form)
	NTSTATUS Start(
		PISO_TRANSFER_COMPLETE_CALLBACK pfnCallback, //Callback when operation is complete
		PVOID pClientContext=NULL					 //context for callback
		);
	//Initiate continuous isoch xfers (synchronous form)
	NTSTATUS Start(ULONG mSecTimeOut=0);
	//stop isoch xfers   
	NTSTATUS Stop(
		BOOLEAN bWait = TRUE, 
		ULONG mSecTimeOut = 100
		);
	//This Method is called by KUsbIsochStream<EzUsbIsoTransfer> 
	USB_STREAM_STATUS OnCompletedUrb(PURB pUrb, NTSTATUS status);

//Data members
protected:

	//Client buffer
	KMemory m_Mem;
	//Buffer base address
	PVOID m_VirtualAddress;
	//Offset into buffer
	ULONG m_dwOffset;
	//Length of user supplied buffer
	ULONG m_Length;
	//Optional pointer to a buffer to hold USBD_ISO_PACKET_DESCRIPTOR's (client provides storage)
    PVOID m_pDescriptorBuffer;
	//Offset into descriptor buffer
	ULONG m_dwDescrOffset;
	//This object abstracts IRP processing
	KUsbIsochStream<EzUsbIsoTransfer> stream;
};


inline NTSTATUS EzUsbIsoTransfer::Start(
	PISO_TRANSFER_COMPLETE_CALLBACK pfnCallback,
	PVOID pClientContext
	)
{
	return stream.StartStream(pfnCallback,pClientContext);
}


inline NTSTATUS EzUsbIsoTransfer::Start(ULONG mSecTimeOut)
{
	return stream.StartStream(mSecTimeOut);
}


inline NTSTATUS EzUsbIsoTransfer::Stop(
	BOOLEAN bWait, 
	ULONG mSecTimeOut
	)
{
	return stream.StopStream(bWait,mSecTimeOut);
}



//=============================================================================
// class EzUsbDriverManagedIsoTransfer
//
// This class abstracts isochronous transfers.  The class provides a buffer for
// performing Isochronous reads from a USB device.  After constructing an 
// instance, drivers call method Start to begin continuous isochronous transfers
// from the USB device.  Drivers call method ReadBuffer to obtain data from 
// the buffer. Iso Packet Descriptors are optionally saved by the object.  
// Method ReadDescriptors can be used to obtain them.  The continuous tranfers
// will be stopped on failure or by calling method Stop.
//
class EzUsbDriverManagedIsoTransfer : public EzUsbIsoTransferBase, protected KLockableFifo<UCHAR>
{
//Construction/Destruction
public:
	EzUsbDriverManagedIsoTransfer(
		KUsbPipe& pipe,		//Pipe to perform iso transfers
		ULONG Length,		//Size of Data Buffer in bytes
		USHORT PacketSize,  //Size of packet to use
		UCHAR PacketsPerUrb,//Number of packets per URB (max 255)
		UCHAR NumUrbs=2,	//Number of queued URB's at a time
		ULONG NumPackets=0,	//Optional Number of Packets to save in a buffer
		PVOID pBuffer=NULL	//Optional pointer to a data buffer
		);

	SAFE_DESTRUCTORS
	~EzUsbDriverManagedIsoTransfer(void);

private:
	EzUsbDriverManagedIsoTransfer(void);
	EzUsbDriverManagedIsoTransfer(EzUsbDriverManagedIsoTransfer&);
	EzUsbDriverManagedIsoTransfer& operator=(EzUsbDriverManagedIsoTransfer&);

//Methods 
public:
	//clients use this copy data from the data buffer
	ULONG ReadBuffer(PVOID ptr, ULONG len);

	//clients use this to copy USBD_ISO_PACKET_DESCRIPTORS from the buffer 
	ULONG ReadDescriptorBuffer(PVOID pBuff, ULONG dwNumDescriptorsToRead);

	//Initiate continuous isoch xfers (asynchronous form)
	NTSTATUS Start(
		PISO_TRANSFER_COMPLETE_CALLBACK pfnCallback,
		PVOID pClientContext=NULL
		);

	//Initiate continuous isoch xfers (synchronous form)
	NTSTATUS Start(ULONG mSecTimeOut=0);

	//stop isoch xfers 
	NTSTATUS Stop(
		BOOLEAN bWait = TRUE, 
		ULONG mSecTimeOut = 100
		);

	BOOLEAN IsStarted(void){ return stream.IsStarted(); }

	//This method is called by KUsbIsochStream<EzUsbDriverManagedIsoTransfer> 
	USB_STREAM_STATUS OnCompletedUrb(PURB pUrb, NTSTATUS status);

protected:

	ULONG WriteBuffer(PURB);

	ULONG WriteDescriptorBuffer(PUSBD_ISO_PACKET_DESCRIPTOR ptr, ULONG dwNumDescriptorsToPutInBuffer);

//Data members
protected:
	//Optional USBD_ISO_PACKET_DESCRIPTOR buffer
    KLockableFifo<USBD_ISO_PACKET_DESCRIPTOR>* m_pDescriptorBuffer;
	//Temporary buffers
	PVOID m_TransferBuffers;
	//Object that manages the recycling IRPs to perform the transfer
	KUsbIsochStream<EzUsbDriverManagedIsoTransfer> stream;
};


inline NTSTATUS EzUsbDriverManagedIsoTransfer::Start(
	PISO_TRANSFER_COMPLETE_CALLBACK pfnCallback,
	PVOID pClientContext
	)
{
	return stream.StartStream(pfnCallback,pClientContext);
}


inline NTSTATUS EzUsbDriverManagedIsoTransfer::Start(ULONG mSecTimeOut)
{
	return stream.StartStream(mSecTimeOut);
}


inline NTSTATUS EzUsbDriverManagedIsoTransfer::Stop(
	BOOLEAN bWait, 
	ULONG mSecTimeOut
	)
{
	return stream.StopStream(bWait,mSecTimeOut);
}

#endif // #ifndef __EzUsbIsoTransfer__h__
