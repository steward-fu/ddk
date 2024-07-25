// K1394Async.h - classes K1394AsyncClient, K1394AddressRange,
//   and K1394AddressFifo
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


#ifndef __K1394ASYNC_H__
#define __K1394ASYNC_H__

//The latest Whistler DDK changed the name of the big endian constant
#ifndef BIG_ENDIAN_ADDRESS_RANGE
#define BIG_ENDIAN_ADDRESS_RANGE ALLOCATE_ADDRESS_FLAGS_USE_BIG_ENDIAN
#endif
///////////////////////////////////////////////////////////////////////
// Class K1394 Address range is a simple class meant to hold the handle,
//  and the array of ADDRESS_RANGE structures defining an allocated
//  address range.  The address range is allocated and freed in
//  K1394AsyncClient using this information.
//
// Note: K1394AddressFifo should be used if a FIFO of MDL's is to be used
//  as a backing store.

class K1394AddressRange
{
	SAFE_DESTRUCTORS
public:
	K1394AddressRange() : m_Handle(NULL), m_Count(0), m_pArray(NULL) {}
	PADDRESS_RANGE	Array() { return m_pArray; }
	ULONG			Count() { return m_Count; }

protected:
	HANDLE				m_Handle;
	ULONG				m_Count;
	PADDRESS_RANGE		m_pArray;

	friend class K1394AsyncClient;
};

////////////////////////////////////////////////////////////////////////
//Class K1394AddressFifo is the class used to hold the state for an
//  address range that uses a FIFO of MDL's as a backing store.
//
// In addition to storing the state needed to allocate and free an
//  address range, it maintains the FIFO by storing its state, and
//  providing members Push() and Pop().  Note that the FIFO is
//  initialized in the constructor and freed in the destructor.  The
//  address range is initialized in K1394AsyncClient::Allocate and freed
//  in K1394AsyncClient::Free.  Elements may be pushed and popped onto the
//  FIFO when the address range is not allocated.
//
class K1394AddressFifo : public K1394AddressRange
{
	SAFE_DESTRUCTORS
public:
	K1394AddressFifo() { ExInitializeSListHead(&m_FifoListHead); }
	void Push(PADDRESS_FIFO	pFifoElement);
	PADDRESS_FIFO Pop(void);

protected:
	SLIST_HEADER		m_FifoListHead;
	KSpinLock			m_FifoSpinLock;

	friend class K1394AsyncClient;
};

///////////////////////////////////////////////////////////////////////
//class K1394FifoElement wraps the ADDRESS_FIFO structure, the element
// that may be pushed and popped onto the FIFO described by
// K1394AddressFifo.
//
// This class keeps track of a buffer and the MDL describing it to be
//  placed in the ADDRESS_FIFO structure.
//
class K1394FifoElement : public ADDRESS_FIFO
{
public:
	NTSTATUS Allocate(ULONG BufferLength);
	VOID Free();

	K1394FifoElement() { FifoMdl = NULL; }
	K1394FifoElement(ULONG BufferLength) { FifoMdl = NULL; Allocate(BufferLength); }
	~K1394FifoElement() { Free(); }

	BOOLEAN IsValid() { return (FifoMdl != NULL); }
	operator PADDRESS_FIFO() { return reinterpret_cast<PADDRESS_FIFO>(this); }
};


////////////////////////////////////////////////////////////////////////
//Class K1394AsyncClient allocates an address range (or address Fifo)
//  and provides overridable callbacks for any activity in the address
//  range.
//
//  It must have a class derrived from it to override one or more of
//  the callbacks: OnRead, OnWrite, OnLock, or OnAllocateComplete.
//
class K1394AsyncClient
{
	SAFE_DESTRUCTORS
public:
	//Constructors
	K1394AsyncClient() : m_pLowerDevice(NULL) {}
	K1394AsyncClient(K1394LowerDevice* pLowerDevice);
	VOID Initialize(K1394LowerDevice* pLowerDevice) { m_pLowerDevice = pLowerDevice; }

	///////////////////////////////////////////////////////////////
	//Allocate : Use a single MDL, and a required offset
	NTSTATUS Allocate(
		K1394AddressRange&	AddressRange,
		PMDL				pMdl,
		ADDRESS_OFFSET		RequiredOffset,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		BOOLEAN				UseBigEndian = FALSE);

	//Allocate : Use a single MDL, but no required offset
	NTSTATUS Allocate(
		K1394AddressRange&	AddressRange,
		PMDL				pMdl,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		ULONG				MaxSegmentSize = 0,
		BOOLEAN				UseBigEndian = FALSE);

	//Allocate : Use a FIFO of MDLs with no required offset
	NTSTATUS Allocate(
		K1394AddressFifo&	AddressFifo,
		ULONG				Length,
		ULONG				AccessFlags = ACCESS_FLAGS_TYPE_WRITE,
		ULONG				MaxSegmentSize = 0,
		BOOLEAN				UseBigEndian = FALSE);

	//Allocate : Use the address FIFO with a required offset
	NTSTATUS Allocate(
		K1394AddressFifo&	AddressFifo,
		ULONG				Length,
		ADDRESS_OFFSET		RequiredOffset,
		ULONG				AccessFlags = ACCESS_FLAGS_TYPE_WRITE,
		BOOLEAN				UseBigEndian = FALSE);

	//Allocate : The driver provides the response packet
	 // no required offset
	NTSTATUS Allocate(
		K1394AddressRange&	AddressRange,
		ULONG				Length,
		ULONG				AccessFlags,
		BOOLEAN				UseBigEndian = FALSE);

	//Allocate : The driver provides the response packet, uses
	 //a required offset
	NTSTATUS Allocate(
		K1394AddressRange&	AddressRange,
		ULONG				Length,
		ADDRESS_OFFSET		RequiredOffset,
		ULONG				AccessFlags,
		BOOLEAN				UseBigEndian = FALSE);

	//Allocate : uses the special allocation routine
	NTSTATUS Allocate(
		K1394AddressRange&		AddressRange,
		PGET_LOCAL_HOST_INFO4	AddressMappingRoutine,
		PMDL					pMdl,
		ULONG					AccessFlags = 0,		//Appears to be ignored
		ADDRESS_OFFSET*			AddressOffset = NULL,	//Appears to be ignored
		ULONG					MaxSegmentSize =0,
		BOOLEAN					UseBigEndian = FALSE);

	NTSTATUS Free(K1394AddressRange& AddressRange);

	//Overridable handlers if the driver provides some backing store
	virtual VOID OnRead(PMDL pMdl, ULONG Offset, ULONG Length)
	{
		return;
		UNREFERENCED_PARAMETER(pMdl);
		UNREFERENCED_PARAMETER(Offset);
		UNREFERENCED_PARAMETER(Length);
	}

	virtual VOID OnWrite(PMDL pMdl, ULONG Offset, ULONG Length, PADDRESS_FIFO pAddressFifo)
	{
		return;
		UNREFERENCED_PARAMETER(pMdl);
		UNREFERENCED_PARAMETER(Offset);
		UNREFERENCED_PARAMETER(Length);
		UNREFERENCED_PARAMETER(pAddressFifo);
	}

	virtual VOID OnLock(PMDL pMdl, ULONG Offset, ULONG Length)
	{
		return;
		UNREFERENCED_PARAMETER(pMdl);
		UNREFERENCED_PARAMETER(Offset);
		UNREFERENCED_PARAMETER(Length);
	}

	//OnAllocateComplete is called in all situations
	virtual VOID OnAllocateComplete(NTSTATUS AllocationStatus)
	{
		return;
		UNREFERENCED_PARAMETER(AllocationStatus);
	}

	//Overridable handlers if the driver provides no backing store
	virtual VOID OnRead(
		PVOID		RequestPacket,
		PMDL		ResponseMdl,
		PVOID*		ResponsePacket,
		PULONG		ResponseLength,
		PKEVENT*	ResponseEvent
		)
	{
		return;
		UNREFERENCED_PARAMETER(RequestPacket);
		UNREFERENCED_PARAMETER(ResponseMdl);
		UNREFERENCED_PARAMETER(ResponsePacket);
		UNREFERENCED_PARAMETER(ResponseLength);
		UNREFERENCED_PARAMETER(ResponseEvent);
	}

	virtual VOID OnWrite(
		PVOID		RequestPacket,
		PMDL		ResponseMdl,
		PVOID*		ResponsePacket,
		PULONG		ResponseLength,
		PKEVENT*	ResponseEvent
		)
	{
		return;
		UNREFERENCED_PARAMETER(RequestPacket);
		UNREFERENCED_PARAMETER(ResponseMdl);
		UNREFERENCED_PARAMETER(ResponsePacket);
		UNREFERENCED_PARAMETER(ResponseLength);
		UNREFERENCED_PARAMETER(ResponseEvent);
	}

	virtual VOID OnLock(
		PVOID		RequestPacket,
		PMDL		ResponseMdl,
		PVOID*		ResponsePacket,
		PULONG		ResponseLength,
		PKEVENT*	ResponseEvent
		)
	{
		return;
		UNREFERENCED_PARAMETER(RequestPacket);
		UNREFERENCED_PARAMETER(ResponseMdl);
		UNREFERENCED_PARAMETER(ResponsePacket);
		UNREFERENCED_PARAMETER(ResponseLength);
		UNREFERENCED_PARAMETER(ResponseEvent);
	}

protected:
	typedef struct _ALLOCATE_CONTEXT
	{
		KIrb<REQUEST_ALLOCATE_ADDRESS_RANGE>	Irb;
//		PIRB				pIrb;
		K1394AsyncClient*						pThis;
		K1394AddressRange*						pAddressRange;
	} ALLOCATE_CONTEXT, *PALLOCATE_CONTEXT;

	//Internal routines
	PALLOCATE_CONTEXT Allocate(
		ULONG				Length,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		ADDRESS_OFFSET*		AddressOffset,
		PMDL				pMdl,
		ULONG				Flags,
		ULONG				MaxSegmentSize,
		PSLIST_HEADER		FifoListHead,
		PKSPIN_LOCK			FifoSpinLock,
		K1394AddressRange*	pAddressRange);

	static NTSTATUS _stdcall AllocateAddressComplete(PDEVICE_OBJECT DeviceObject,
													 PIRP			Irp,
													 PVOID			Context);

	static VOID _stdcall AllocateAddressSpecialComplete(PVOID Context);
	static VOID _stdcall NotificationRoutine(PNOTIFICATION_INFO pInfo);

protected:
	K1394LowerDevice*	m_pLowerDevice;
};

///////////////////////////////////////////////////////////////////////
//Inline functions for K1394AddressFifo
inline void K1394AddressFifo::Push(PADDRESS_FIFO pFifoElement)
{
	ExInterlockedPushEntrySList(&m_FifoListHead,
								&(pFifoElement->FifoList),
								m_FifoSpinLock);
}

inline PADDRESS_FIFO K1394AddressFifo::Pop(void)
{
	return (PADDRESS_FIFO)ExInterlockedPopEntrySList(&m_FifoListHead,
													 m_FifoSpinLock);
}

///////////////////////////////////////////////////////////////////////
//Inline functions for K1394AsyncClient
inline K1394AsyncClient::K1394AsyncClient(K1394LowerDevice* pLowerDevice)
{
	Initialize(pLowerDevice);
}

inline NTSTATUS K1394AsyncClient::Allocate(
		K1394AddressRange&	AddressRange,
		ULONG				Length,
		ADDRESS_OFFSET		RequiredOffset,
		ULONG				AccessFlags,
		BOOLEAN				UseBigEndian)
{
	PALLOCATE_CONTEXT pContext = Allocate(
		Length,
		AccessFlags,
		0,						//Notification flags
		&RequiredOffset,		//Required offset
		NULL,					//pMdl
		(UseBigEndian) ? BIG_ENDIAN_ADDRESS_RANGE : 0,
		0,						//Max segment size
		NULL,					//Fifo list head
		NULL,					//Fifo spin lock
		&AddressRange);

	if( pContext )
		return m_pLowerDevice->SubmitIrb(pContext->Irb, AllocateAddressComplete, pContext);
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}



inline NTSTATUS K1394AsyncClient::Allocate(
		K1394AddressRange&	AddressRange,
		ULONG				Length,
		ULONG				AccessFlags,
		BOOLEAN				UseBigEndian)
{
	PALLOCATE_CONTEXT pContext = Allocate(
		Length,
		AccessFlags,
		0,			//Notification flags
		NULL,		//Required offset
		NULL,		//pMdl
		(UseBigEndian) ? BIG_ENDIAN_ADDRESS_RANGE : 0,
		0,			//Max segment size
		NULL,		//Fifo list head
		NULL,       //Fifo spin lock
		&AddressRange);

	if( pContext )
		return m_pLowerDevice->SubmitIrb(pContext->Irb, AllocateAddressComplete, pContext);
	else
		return STATUS_INSUFFICIENT_RESOURCES;

}

inline NTSTATUS K1394AsyncClient::Allocate(
		K1394AddressFifo&	AddressFifo,
		ULONG				Length,
		ADDRESS_OFFSET		RequiredOffset,
		ULONG				AccessFlags,
		BOOLEAN				UseBigEndian)
{
	PALLOCATE_CONTEXT pContext = Allocate(
		Length,
		AccessFlags,
		NOTIFY_FLAGS_AFTER_WRITE,
		&RequiredOffset,
		NULL,
		(UseBigEndian) ? BIG_ENDIAN_ADDRESS_RANGE : 0,
		0,
		&AddressFifo.m_FifoListHead,
		AddressFifo.m_FifoSpinLock,
		&AddressFifo);

	if( pContext )
		return m_pLowerDevice->SubmitIrb(pContext->Irb, AllocateAddressComplete, pContext);
	else
		return STATUS_INSUFFICIENT_RESOURCES;

}

inline NTSTATUS K1394AsyncClient::Allocate(
		K1394AddressFifo&	AddressFifo,
		ULONG				Length,
		ULONG				AccessFlags,
		ULONG				MaxSegmentSize,
		BOOLEAN				UseBigEndian)
{
	PALLOCATE_CONTEXT pContext = Allocate(
		Length,
		AccessFlags,
		NOTIFY_FLAGS_AFTER_WRITE,
		NULL,
		NULL,
		(UseBigEndian) ? BIG_ENDIAN_ADDRESS_RANGE : 0,
		MaxSegmentSize,
		&AddressFifo.m_FifoListHead,
		AddressFifo.m_FifoSpinLock,
		&AddressFifo);

	if( pContext )
		return m_pLowerDevice->SubmitIrb(pContext->Irb, AllocateAddressComplete, pContext);
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

inline NTSTATUS K1394AsyncClient::Allocate(
		K1394AddressRange&	AddressRange,
		PMDL				pMdl,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		ULONG				MaxSegmentSize,
		BOOLEAN				UseBigEndian)
{
	PALLOCATE_CONTEXT pContext = Allocate(
		MmGetMdlByteCount(pMdl),
		AccessFlags,
		NotificationFlags,
		NULL,
		pMdl,
		(UseBigEndian) ? BIG_ENDIAN_ADDRESS_RANGE : 0,
		MaxSegmentSize,
		NULL,
		NULL,
		&AddressRange);
	if(pContext)
		return m_pLowerDevice->SubmitIrb(pContext->Irb, AllocateAddressComplete, pContext);
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

inline NTSTATUS K1394AsyncClient::Allocate(
		K1394AddressRange&	AddressRange,
		PMDL				pMdl,
		ADDRESS_OFFSET		RequiredOffset,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		BOOLEAN				UseBigEndian)
{
	PALLOCATE_CONTEXT pContext = Allocate(
		MmGetMdlByteCount(pMdl),
		AccessFlags,
		NotificationFlags,
		&RequiredOffset,
		pMdl,
		(UseBigEndian) ? BIG_ENDIAN_ADDRESS_RANGE : 0,
		0,
		NULL,
		NULL,
		&AddressRange);

	if( pContext )
		return m_pLowerDevice->SubmitIrb(pContext->Irb, AllocateAddressComplete, pContext);
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

inline NTSTATUS K1394AsyncClient::Allocate(
									K1394AddressRange&		AddressRange,
									PGET_LOCAL_HOST_INFO4	AddressMappingRoutine,
									PMDL					pMdl,
									ULONG					AccessFlags,	   //Appears to be ignored
									ADDRESS_OFFSET			*AddressOffset, //Appears to be ignored
									ULONG					MaxSegmentSize,
									BOOLEAN					UseBigEndian)
{
	PALLOCATE_CONTEXT pContext = Allocate(
		MmGetMdlByteCount(pMdl),
		AccessFlags,
		0,
		AddressOffset,
		pMdl,
		(UseBigEndian) ? BIG_ENDIAN_ADDRESS_RANGE : 0,
		MaxSegmentSize,
		NULL,
		NULL,
		&AddressRange);

	if( pContext )
	{
		//Set the completion routine
		pContext->Irb.u.AllocateAddressRange.Callback = AllocateAddressSpecialComplete;
		pContext->Irb.u.AllocateAddressRange.Context = pContext;

		//Call the address mapping routine
		return (AddressMappingRoutine->PhysAddrMappingRoutine)(AddressMappingRoutine->Context,
															   pContext->Irb);
	}
	else
		return STATUS_INSUFFICIENT_RESOURCES;
}

#endif
