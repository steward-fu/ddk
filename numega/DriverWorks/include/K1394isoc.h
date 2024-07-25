// k1394isoc.h - include file for 1394 isochronous classes
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

#ifndef _K1394_ISOCH_H_
#define _K1394_ISOCH_H_

////////////////////////////////////////////////////////////////////
// class K1394IsochBandwidth
//
// This class saves the handle to the bandwidth allocated and keeps
//  track of the speed and max bytes per frame
//

class K1394IsochBandwidth
{
	SAFE_DESTRUCTORS
public:
	K1394IsochBandwidth():
		m_Handle(NULL),
		m_MaxBytesPerFrame(0),
		m_Speed(0) {}

	inline ULONG SpeedSelected(void) { return m_Speed; }
	inline ULONG MaxBytesPerFrame(void) { return m_MaxBytesPerFrame; }

protected:
	// Data Members
	HANDLE				m_Handle;
	ULONG				m_MaxBytesPerFrame;
	ULONG				m_Speed;

	friend class K1394LowerDevice;
};

#define ISOCH_ILLEGAL_CHANNEL	ISOCH_MAX_CHANNEL + 1

///////////////////////////////////////////////////////////////
// class K1394IsochChannel
//
// This class allocates an isochronous channel.
//

class K1394IsochChannel
{
	SAFE_DESTRUCTORS
public:
	K1394IsochChannel():
		m_Number(ISOCH_ILLEGAL_CHANNEL) {}

	inline ULONG NumberSelected() { return m_Number; }

protected:
	// Data Members
	ULONG				m_Number;

	friend class K1394LowerDevice;
};


////////////////////////////////////////////////////////////////
// class K1394IsochResource
//
// This class saves the handle to the isochronous resource handle,
//  and allows buffers to be attached and removed from the handle.
//

class K1394IsochResource
{
	SAFE_DESTRUCTORS
public:
	K1394IsochResource():
		m_Handle(NULL),
		m_Flags(0) {}

	operator HANDLE() { return m_Handle; }

protected:
	// Data Members
	HANDLE				m_Handle;
	ULONG				m_Flags;

	friend class K1394LowerDevice;
	friend class K1394IsochTransfer;
};


///////////////////////////////////////////////////////////////////
// class K1394IsochBufferList
//
// Note: The class uses DeviceReserved[0] of the first buffer allocated
//   to keep track of the number of descriptors in the array.

class K1394IsochBufferList : public ISOCH_DESCRIPTOR
{
	SAFE_DESTRUCTORS
public:
	operator PISOCH_DESCRIPTOR() { return reinterpret_cast<PISOCH_DESCRIPTOR> (this); }
	PVOID operator new(size_t Size, ULONG DescriptorCount = 1);
	VOID operator delete(PVOID p) { ::delete[] p; }
	ULONG_PTR Count() { return this[0].DeviceReserved[0]; }

	VOID SetupAllBuffers(
		ULONG		MaxBytesPerFrame,
		ULONG		Flags = 0,
		ULONG		Synch = 0,
		ULONG		Tag = 0,
		PCYCLE_TIME	CycleTime = NULL);

	VOID SetupBuffer(
		ULONG		Index,
		PMDL		pMdl,
		ULONG		MaxBytesPerFrame = 0,
		ULONG		Flags = 0,
		ULONG		Synch = 0,
		ULONG		Tag = 0,
		PCYCLE_TIME	CycleTime = NULL);

	PVOID& CompletionContext() { return (PVOID&)this[0].DeviceReserved[1]; }
	VOID  SetMdl(ULONG Index, PMDL pMdl);
	ULONG TotalBytesProcessed();
};


inline VOID  K1394IsochBufferList::SetMdl(ULONG Index, PMDL pMdl)
{
	if( Index < this[0].DeviceReserved[0] )
	{
		this[Index].Mdl = pMdl;
		this[Index].ulLength = MmGetMdlByteCount(pMdl);
	}

	return;
}

inline ULONG K1394IsochBufferList::TotalBytesProcessed()
{
	ULONG Count = 0;

	for(ULONG next = 0; next < this[0].DeviceReserved[0]; next++)
		Count += this[next].ulLength;

	return Count;
}


////////////////////////////////////////////////////////////////
// MEMBER_1394_ISOCH_CALLBACK_ROUTINE
//
// This macro sets up a callback routine to be called when the
// 1394 bus is done processing the attached buffers.  The class
// of which the function is a member must be passed into the
// Context1 parameter.  Context2 can be any useful parameter.


#define MEMBER_1394_ISOCH_CALLBACK_ROUTINE(objclass, func)							\
	VOID __stdcall func(PVOID Context);												\
	static VOID __stdcall LinkTo(func)(IN PVOID Context1, IN PVOID Context2)		\
	{																				\
		((objclass*)Context1)->func(Context2);										\
	}


////////////////////////////////////////////////////////////////////////
//Class K1394IsochTransfer
//
// Wraps the isochronous transfer.  Allows the transfer to be started,
//  or stopped, and allows buffers to be attached.  It handles
//  processing the buffer lists, and provides an overridable callback
//  to notify the user when a list of buffers completes.
//
class K1394IsochTransfer
{
	SAFE_DESTRUCTORS
	//Internal completion context
	typedef struct _BUFFER_CONTEXT
	{
		K1394IsochBufferList*	pBufferList;
		K1394IsochTransfer*		pThis;
		BOOLEAN					Cancelled;
		LIST_ENTRY				m_ListEntry;
		PIRB					pIrb;
	} BUFFER_CONTEXT, *PBUFFER_CONTEXT;

    typedef struct _ADD_BUFFER_CONTEXT
    {
        PBUFFER_CONTEXT         pContext;
        PIRB                    pIrb;
        K1394IsochTransfer*		pThis;
    } ADD_BUFFER_CONTEXT, *PADD_BUFFER_CONTEXT;

	typedef struct _START_STOP_CONTEXT
	{
		K1394IsochTransfer*		pThis;
		K1394Irb				Irb;
	} START_STOP_CONTEXT, *PSTART_STOP_CONTEXT;

public:
	K1394IsochTransfer():
		m_pLowerDevice(NULL),
		m_pResource(NULL),
		m_IsStarted(FALSE) {}

	K1394IsochTransfer(
		K1394LowerDevice*	LowerDevice,
		K1394IsochResource*	Resource,
		USHORT				MaxIrbs = 4
		);

	VOID Initialize(
		K1394LowerDevice*	LowerDevice,
		K1394IsochResource*	Resource,
		USHORT				MaxIrbs = 4
		);

	VOID Invalidate();
	~K1394IsochTransfer();

	NTSTATUS AddBufferList(K1394IsochBufferList& BufferList);
	NTSTATUS CancelBufferList(K1394IsochBufferList& BufferList);
	NTSTATUS CancelAllBuffers();

	NTSTATUS StartTalk(PCYCLE_TIME StartTime = NULL);
	NTSTATUS StartListen(PCYCLE_TIME StartTime = NULL);
	NTSTATUS Start(PCYCLE_TIME StartTime = NULL);

	NTSTATUS Stop();
	BOOLEAN	 IsStarted() { return m_IsStarted; }

	virtual VOID OnBufferComplete(
		NTSTATUS				BufferStatus,
		K1394IsochBufferList*	BufferList
		)
	{
		return;
		UNREFERENCED_PARAMETER(BufferStatus);
		UNREFERENCED_PARAMETER(BufferList);
	}

	virtual VOID OnStartComplete(NTSTATUS StartStatus)
	{
		return;
		UNREFERENCED_PARAMETER(StartStatus);
	}

	virtual VOID OnStopComplete(NTSTATUS StopStatus)
	{
		return;
		UNREFERENCED_PARAMETER(StopStatus);
	}

protected:
	K1394IsochResource*					m_pResource;
	K1394LowerDevice*					m_pLowerDevice;
	BOOLEAN								m_IsStarted;
	KInterlockedList<BUFFER_CONTEXT>	m_BufferList;
    KSPIN_LOCK                          m_BufferListLock;
	KHeap<IRB>							m_IrbPool;

protected:
	//Some internal routines
	static NTSTATUS _stdcall StartStopComplete(
		PDEVICE_OBJECT		DeviceObject,
		PIRP				Irp,
		PVOID
		);

	static NTSTATUS _stdcall AddBufferComplete(
		PDEVICE_OBJECT		DeviceObject,
		PIRP				Irp,
		PVOID
		);

	static NTSTATUS _stdcall RemoveBufferComplete(
		PDEVICE_OBJECT		DeviceObject,
		PIRP				Irp,
		PVOID
		);

	MEMBER_1394_ISOCH_CALLBACK_ROUTINE(K1394IsochTransfer, BufferComplete);
};

////////////////////////////////////////////////////////////////////////
//Inline functions
////////////////////////////////////////////////////////////////////////

inline K1394IsochTransfer::K1394IsochTransfer(
	K1394LowerDevice*	LowerDevice,
	K1394IsochResource* Resource,
	USHORT				MaxIrbs
	) :
		m_pLowerDevice(LowerDevice),
		m_pResource(Resource),
		m_IsStarted(FALSE),
		m_IrbPool(MaxIrbs)
{
}


inline VOID K1394IsochTransfer::Initialize(
	K1394LowerDevice*	LowerDevice,
	K1394IsochResource* Resource,
	USHORT				MaxIrbs
	)
{
	m_pLowerDevice = LowerDevice;
	m_pResource = Resource;
	m_IrbPool.Initialize(MaxIrbs);
    KeInitializeSpinLock(&m_BufferListLock);
}

inline VOID K1394IsochTransfer::Invalidate()
{
	Stop();				//Stop the current transfer
	CancelAllBuffers(); //Detach all buffers

	//Reset the class' internal state
	m_pLowerDevice = NULL;
	m_pResource = NULL;
	m_IrbPool.Invalidate();
}

inline K1394IsochTransfer::~K1394IsochTransfer()
{
	Invalidate();
}

inline NTSTATUS K1394IsochTransfer::Start(PCYCLE_TIME StartTime)
{
	if ( m_pResource->m_Flags & RESOURCE_USED_IN_TALKING )
		return StartTalk(StartTime);
	else
		return StartListen(StartTime);
}

inline NTSTATUS K1394LowerDevice::Reallocate(K1394IsochBandwidth& Bandwidth)
{
	Bandwidth.m_Handle = NULL;
	return Allocate(Bandwidth, Bandwidth.m_Speed, Bandwidth.m_MaxBytesPerFrame);
}

inline NTSTATUS K1394LowerDevice::Reallocate(K1394IsochChannel& Channel)
{
	ULONG Number = Channel.m_Number;
	Channel.m_Number = ISOCH_ILLEGAL_CHANNEL;
	return Allocate(Channel, Number);
}

inline NTSTATUS K1394LowerDevice::Allocate(
	K1394IsochResource&		Resource,
	K1394IsochBandwidth&	Bandwidth,
	K1394IsochChannel&		Channel,
	ULONG					Flags,
	ULONG					NumberOfBuffers,
	ULONG					MaxBufferSize,
	ULONG					QuadletsToStrip
	)
{
	return Allocate(Resource,
				Bandwidth.m_Speed,
				Flags,
				Channel.m_Number,
				Bandwidth.m_MaxBytesPerFrame,
				NumberOfBuffers,
				MaxBufferSize,
				QuadletsToStrip);
}

#endif
