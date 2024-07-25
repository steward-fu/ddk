// kwmiblock.h - definitions for Windows Management Instrumentation support
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

#if _WDM_

#if !defined(__KWMIBLOCK__)
#define __KWMIBLOCK__

#include <kwmi.h>

//////////////////////////////////////////////////////////////////////////
//
// KWmiDataBlock<T,U,F>

template <class T, class U = _none_, class F = KStdWmiInterface>
class KWmiDataBlock : public KWmiBlock<F>
{
public:

// typedefs
//
// A METHOD is a pointer to a function in the 'method' class U. The parameter sequence is fixed, and
// corresponds to the fields passed to _DpWmiExecuteMethod.

	typedef NTSTATUS (U::*METHOD)(
		T* pBlockData,
		ULONG InBufferSize,
		ULONG OutBufferSize,
		PUCHAR Buffer,
		PULONG BufferUsed
		);

// Non-static Data members

	T m_Data;
	U* m_MethodObject;

// Ctor
	KWmiDataBlock(void) : KWmiBlock<F>() {}

	KWmiDataBlock(
		KWmiContext<F>*& pContext,
		const GUID* pGuid,
		ULONG Flags=0,
		U* MethodObject=NULL
		) :

		KWmiBlock<F>(pContext, pGuid, Flags),
		m_MethodObject(MethodObject)
	{
	}

	VOID Initialize(
		KWmiContext<F>*& pContext,
		const GUID* pGuid,
		ULONG Flags=0,
		U* MethodObject=NULL
		)
	{
		KWmiBlock<F>::Initialize(pContext, pGuid, Flags);
		m_MethodObject = MethodObject;
	}

	~KWmiDataBlock(void)
	{
		if ( IsValid() )
		{
			Invalidate();
		}
	}


	VOID Invalidate(void)
	{
		m_MethodObject = NULL;
		KWmiBlock<F>::Invalidate();
	}

// Cast to block data member type

	operator T*() { return &m_Data; }

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock<T,U,F>::Query
//		Query the block.
//
//	Parameters:
//		pIrp
//			Pointer to the requesting IRP. The function does not have to use
//			this, but it is passed just in case.
//
//		BufferSize
//			Size of buffer (in bytes) to receive data.
//
//		pData
//			Pointer to instance of T to receive block
//
//		pBufferUsed
//			Pointer to ULONG that receives number of bytes written on output.
//
//		Last
//			TRUE if this is the last block to be requested for the IRP. For the
//			common case where just one block is requested, this will be TRUE.
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		The default implementation copies the block to pData. If the object has variable
//		size, or if there are other complicating factors, then this will have to be
//		specialized by the driver writer.
//
	NTSTATUS Query(PIRP pIrp, ULONG BufferSize, T* pData, PULONG pBufferUsed, BOOLEAN Last)
	{
		NTSTATUS status;

		if ( BufferSize >= sizeof T )
		{
			*pData = m_Data;
			status = STATUS_SUCCESS;
		}
		else
			status = STATUS_BUFFER_TOO_SMALL;

		*pBufferUsed = sizeof T;
		return status;

		UNREFERENCED_PARAMETER(pIrp);
		UNREFERENCED_PARAMETER(Last);
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock<T,U,F>::Set
//		Set one of the provider's data blocks
//
//	Parameters:
//		pIrp
//			Pointer to the requesting IRP. The function does not have to use
//			this, but it is passed just in case.
//
//		BufferSize
//			Size in bytes of input data at pData
//
//		pData
//			Points to instance of T to be stored in block
//
//	Return Value:
//		Returns status.
//
//	Comments:
//		The framework calls this to set the provider's data.
//
//		The default implementation copies from pData to the block. If the object has
//		variable size, or if there are other complicating factors, then this will have
//		to be specialized by the driver writer.
//
	NTSTATUS Set(PIRP pIrp, ULONG BufferSize, T* pData)
	{
		NTSTATUS status;

		if ( BufferSize == sizeof T )
		{
			m_Data = *pData;
			status = STATUS_SUCCESS;
		}
		else if ( (BufferSize < sizeof T) && ( sizeof T < (BufferSize+sizeof (long)) ) )
		{
			memcpy(&m_Data, pData, BufferSize);
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}

		return status;

		UNREFERENCED_PARAMETER(pIrp);
	}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock<T,U,F>::SetItem
//		The system calls this to set a specific item the provider's block. Each
//		template instantiaton must customize this, since each block will have
//		items of differing type and size.
//
//	Parameters:
//		pIrp
//			Pointer to the requesting IRP
//
//		ItemId
//			Id of the item to set (per MOF declaration)
//
//		BufferSize
//			Size, in bytes, of ItemBuffer
//
//		ItemBuffer
//			Buffer containing data for item
//
//	Return Value:
//		Returns status (may be STATUS_BUFFER_TOO_SMALL)
//
//	Comments:
//		This function must be specialized before it is used.
//
	virtual
	NTSTATUS SetItem(PIRP pIrp, ULONG ItemId, ULONG BufferSize, PUCHAR ItemBuffer)
	{
/*
		switch ( ItemId )
		{
		case 1:		// set item 1 with data from ItemBuffer
		case 2:		// set item 2 with data from ItemBuffer
		. . .
		}

		return STATUS_SUCCESS
*/
		return STATUS_NOT_IMPLEMENTED;

		UNREFERENCED_PARAMETER(pIrp);
		UNREFERENCED_PARAMETER(ItemId);
		UNREFERENCED_PARAMETER(BufferSize);
		UNREFERENCED_PARAMETER(ItemBuffer);
	}


///////////////////////////////////////////////////////////////////////////////
//
//	KWmiBlock<T,U,F>::FireEvent
//		Fires the block as an event.
//
//	Return Value:
//		Returns status (may be STATUS_INSUFFICIENT_RESOURCES)
//
//	Comments:
//		If the size of the block is not fixed (e.g. if it contains a string),
//		then this function must be specialized.
//
	NTSTATUS FireEvent(void)
	{
		NTSTATUS status;

		// Do nothing if events are not enabled for this block
		if ( ! TestEventsEnabled() )
		{
			return STATUS_UNSUCCESSFUL;
		}

		// struct to hold the event header and the data
		struct SINGLE_INSTANCE_EVENT
		{
			UCHAR m_si[sizeof(WNODE_SINGLE_INSTANCE)];
			T m_data;
		} *pE = (SINGLE_INSTANCE_EVENT*)ExAllocatePool(NonPagedPool, sizeof(SINGLE_INSTANCE_EVENT));

		// allocate the memory to store the event information
		if ( pE == NULL )
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		// initialize the event header
		WNODE_SINGLE_INSTANCE* pSI = (WNODE_SINGLE_INSTANCE*)pE;
		memset(pE, 0, sizeof(SINGLE_INSTANCE_EVENT));

		KeQuerySystemTime(&pSI->WnodeHeader.TimeStamp);
		pSI->WnodeHeader.BufferSize = sizeof (SINGLE_INSTANCE_EVENT);

#if !defined(VXD_COMPATLIB) && !((WDM_MAJORVERSION ==1) && (WDM_MINORVERSION == 0))
		pSI->WnodeHeader.ProviderId = IoWMIDeviceObjectToProviderId(m_Context->m_DeviceObject);
#else
		pSI->WnodeHeader.ProviderId = UINT_PTR(m_Context->m_DeviceObject);
#endif

		pSI->WnodeHeader.Guid = *m_Context->m_GuidInfo[m_GuidIndex].Guid;
		pSI->WnodeHeader.Flags =	WNODE_FLAG_EVENT_ITEM |
									WNODE_FLAG_SINGLE_INSTANCE |
									WNODE_FLAG_STATIC_INSTANCE_NAMES;
		pSI->InstanceIndex = m_BlockIndex;
		pSI->DataBlockOffset = sizeof(WNODE_SINGLE_INSTANCE);
		pSI->SizeDataBlock = sizeof(T);

		// copy the block data to the event buffer
		pE->m_data = m_Data;

		// send the event to WMI
		status = IoWMIWriteEvent(pE);

		// if the event firing fails, we must free the block
		if ( !NT_SUCCESS(status) )
		{
			ExFreePool(pE);
		}

		return status;
	}
/////////////////////////////////////////////////////////////////////////////////
// Specialize these routines to receive notifications

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiDataBlock<T,U,F>::OnEnableEvents
//
//	Comments:
//		Called by the framework when events are enabled for this block.
//
	virtual VOID OnEnableEvents(void)		{}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiDataBlock<T,U,F>::OnDisableEvents
//
//	Comments:
//		Called by the framework when events are enabled for this block.
//
	virtual VOID OnDisableEvents(void)		{}


///////////////////////////////////////////////////////////////////////////////
//
//	KWmiDataBlock<T,U,F>::OnEnableCollection
//
//	Comments:
//		Called by the framework when collection is enabled for this block.
//
	virtual VOID OnEnableCollection(void)	{}

///////////////////////////////////////////////////////////////////////////////
//
//	KWmiDataBlock<T,U,F>::OnDisableCollection
//
//	Comments:
//		Called by the framework when collection is disabled for this block.
//
	virtual VOID OnDisableCollection(void)	{}

protected:
///////////////////////////////////////////////////////////////////////////////////////////////
// These virtuals should not require specialization.

	virtual NTSTATUS _query(PIRP pIrp, ULONG BufferSize, PUCHAR Buffer, PULONG pBufferUsed, BOOLEAN Last)
		{return Query(pIrp, BufferSize, reinterpret_cast<T*>(Buffer), pBufferUsed, Last); }

	virtual NTSTATUS _set(PIRP Irp, ULONG BufferSize, PUCHAR Buffer)
		{return Set(Irp, BufferSize, reinterpret_cast<T*>(Buffer));}

	virtual NTSTATUS _execute_method(
		PIRP Irp,
		ULONG MethodId,
		ULONG InBufferSize,
		ULONG OutBufferSize,
		PUCHAR Buffer,
		PULONG pBufferUsed
		)
	{
		ULONG MethodCount;
		METHOD* pMethods;
		NTSTATUS status;

		if ( m_MethodObject == NULL )
		{
			m_MethodObject = (U*)m_Context->GetDefaultMethodObject();

			if ( m_MethodObject == NULL )
			{
				return STATUS_NOT_IMPLEMENTED;
			}
		}

		m_Context->GetMethods(&MethodCount, &pMethods);
		status = (m_MethodObject->*pMethods[MethodId-1])(&m_Data, InBufferSize, OutBufferSize, Buffer, pBufferUsed);
		return status;

		UNREFERENCED_PARAMETER(Irp);
	}
///////////////////////////////////////////////////////////////////
// TraceEvent
//
// This function is not supported at this time
public:
	NTSTATUS TraceEvent()
	{
		// Do nothing if events are not enabled for this block
		if ( ! TestEventsEnabled() )
		{
			return STATUS_UNSUCCESSFUL;
		}
#if DBG
		ULONG BlockFlags;
		BlockFlags = m_Context->m_GuidInfo[m_GuidIndex].Flags;
		ASSERT ( (BlockFlags & WMIREG_FLAG_TRACED_GUID) != 0 );
#endif // DBG

		// the WNODE_HEADER and EVENT_TRACE_HEADER are overlaid. See
		// evntrace.h

		struct DW_TRACE_EVENT
		{
			EVENT_TRACE_HEADER m_header;
			T m_data;
		} Event;

		WNODE_HEADER* pNode = (WNODE_HEADER*)&Event;

		// zero out the memory for the event
		memset(&Event, 0, sizeof(Event));

		// set up the WNODE_HEADER
		pNode->BufferSize = sizeof(DW_TRACE_EVENT);
		pNode->HistoricalContext = m_Context->m_hTrace;

		// set up the EVENT_TRACE_HEADER
		Event.m_header.GuidPtr =
		ULONGLONG(ULONG_PTR(&m_Context->m_GuidInfo[m_GuidIndex].Guid));
		Event.m_header.Flags = WNODE_FLAG_USE_GUID_PTR | WNODE_FLAG_TRACED_GUID;

		// copy the event information
		Event.m_data = m_Data;

		// send the event to WMI
		return IoWMIWriteEvent(&Event);
	}
};

#endif // __KWMIBLOCK__

#endif // _WDM_
