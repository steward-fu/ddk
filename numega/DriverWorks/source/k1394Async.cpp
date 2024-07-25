// K1394async.cpp - Class K1394AsyncClient implemenation
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


#include <vdw.h>

#if _WDM_

//////////////////////////////////////////////////////////////////////
// K1394FifoElement::Allocate
//
// Sets up an ADDRESS_FIFO element to be pushed onto an address Fifo
//  (K1394AddressFifo).  It allocates a buffer and the MDL to describe
//  it, and fills in the fields in the ADDRESS_FIFO structure.
//
// Parameters:
//	BufferLength: The length of the address range described by the
//    K1394AddressFifo class the FIFO element
//
// Returns:
//	STATUS_SUCCESS if the fifo element was allocated
//	STATUS_INSUFFICIENT_RESOURCES if it failled
//
// Comments:
//
NTSTATUS K1394FifoElement::Allocate(ULONG BufferLength)
{
	if( FifoMdl )
	{
		ASSERT(FALSE);
		Free();
	}

	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	PUCHAR pBuffer = new (NonPagedPool) UCHAR[BufferLength];
	if ( pBuffer )
	{
		FifoMdl = IoAllocateMdl(pBuffer, BufferLength, FALSE, FALSE, NULL);

		if( FifoMdl )
		{
			MmBuildMdlForNonPagedPool(FifoMdl);
			status = STATUS_SUCCESS;
		}
		else
		{
			delete[] pBuffer;
		}
	}

	return status;
}

////////////////////////////////////////////////////////////////////////
//K1394FifoElement::Free
//
// Frees the buffer and MDL allocated by Allocate()
//
// Parameters:
//	None.
//
// Returns:
//	None.
//
// Comments:
//

VOID K1394FifoElement::Free()
{
	if( FifoMdl )
	{
		KMemory	Mdl(FifoMdl);
		PUCHAR	pBuffer = (PUCHAR)Mdl.VirtualAddress();

		delete[] pBuffer;
		IoFreeMdl(FifoMdl);

		FifoMdl = NULL;
	}
}

///////////////////////////////////////////////////////////////////////
//K1394AsyncClient::Free
//
// Frees the address range allocated by any version of K1394AsyncClient
//   ::Allocate()
//
// Parameters
//		AddressRange - Address range to be freed
//
// Returns:
//	STATUS_SUCCESS if the operation was successful
//
// Comments:
//   This routine must be called at PASSIVE_LEVEL IRQL.  It works with
//    both K1394AddressFifo and K1394AddressRange.
//
NTSTATUS K1394AsyncClient::Free(K1394AddressRange& AddressRange)
{
	NTSTATUS status = STATUS_SUCCESS;

	// If the address range if it isn't allocated we're done
	if ( AddressRange.m_Handle != NULL )
	{
		// Allocate IRB on stack and zero
		KIrb<REQUEST_FREE_ADDRESS_RANGE> Irb(&AddressRange.m_Handle,
											 AddressRange.m_Count,
											 AddressRange.m_pArray);

		status = m_pLowerDevice->SubmitIrb(Irb);

		// Change our handle data member back to NULL
		AddressRange.m_Handle = NULL;

		// Get rid of address range array since it is no longer valid
		ASSERT( AddressRange.m_pArray != NULL );
		delete[] AddressRange.m_pArray;

		AddressRange.m_pArray = NULL;

		// Set the number of addresses to zero since there aren't any
		AddressRange.m_Count = 0;
	}

	return status;
}


////////////////////////////////////////////////////////////////////////
//K1394AsyncClient::Allocate
//
// This is an internal function used to set up the IRB and context needed
//  to allocate an address range.
//
// Parameters:
//		Length				Length of the address range to allocate
//		AccessFlags			Specifies rather or not read, write, or lock operations
//							may be performed on the address range.
//		NotificationFlags	Specifies rather the notification routine should be called
//							after read, write, or lock operations
//		AddressOffset		Optional hard coded starting address for the address range
//		pMdl				The MDL to receive information from the address range
//		Flags				Specifies rather to use big or little endian format
//		MaxSegmentSize		When the address range cannot be allocate all in one place,
//							but is allocated as several smaller segment, this specifies
//							the maximum size of any segment
//		FifoListHead		An initialized list head to be used as a FIFO
//      FifoSpinlock		An initialized spinlock to be used to protect the FIFO.
//		pAddressRange		The address range structure being allocated.
//
// Returns:
//   A pointer to an initialized context structure containing the IRB,
//    or NULL if the request failled.
//
// Comments:
//
K1394AsyncClient::PALLOCATE_CONTEXT
K1394AsyncClient::Allocate(
		ULONG				Length,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		ADDRESS_OFFSET*		AddressOffset,
		PMDL				pMdl,
		ULONG				Flags,
		ULONG				MaxSegmentSize,
		PSLIST_HEADER		FifoListHead,
		PKSPIN_LOCK			FifoSpinLock,
		K1394AddressRange*	pAddressRange)
{
	//Allocate memory to hold the address array
	ULONG PagesSpanned;
	if( AddressOffset || (!pMdl && !FifoListHead) )
		PagesSpanned = 1;
	else
		PagesSpanned = ADDRESS_AND_SIZE_TO_SPAN_PAGES(0, Length) + 1;

	pAddressRange->m_pArray = new (NonPagedPool) ADDRESS_RANGE[PagesSpanned];

	if ( pAddressRange->m_pArray )
	{
		//Allocate the context
		PALLOCATE_CONTEXT pContext = new ALLOCATE_CONTEXT;
		if ( pContext )
		{
			//Fill in the context
			pContext->pThis = this;
			pContext->pAddressRange = pAddressRange;

			//Initialize the IRB
			pContext->Irb.Initialize(
				NotificationRoutine,
				this,
				Length,
				AccessFlags,
				NotificationFlags,
				AddressOffset,
				pMdl,
				Flags,
				MaxSegmentSize,
				FifoListHead,
				FifoSpinLock,
				pAddressRange->m_pArray);

			return pContext;
		}

		delete pAddressRange->m_pArray;
	}

	OnAllocateComplete(STATUS_INSUFFICIENT_RESOURCES);

	return NULL;
}

////////////////////////////////////////////////////////////////////////
//K1394AsyncClient::AllocateAddressComplete
//
//  Completion routine called when an IRP from any form of allocate that
//   calls SubmitIrb completes.  (That is all forms but the one that calls
//   the bus driver's physical mapping routine)
//
// Parameters
//	DeviceObject - System device object
//  Irp - Irp that completed
//	Context - The ALLOCATE_CONTEXT structure we supplied
//
// Returns - The status in the IRP
//
// Comments
//
NTSTATUS K1394AsyncClient::AllocateAddressComplete(
	PDEVICE_OBJECT	DeviceObject,
	PIRP			Irp,
	PVOID			Context)
{
	PALLOCATE_CONTEXT	pContext = (PALLOCATE_CONTEXT)Context;
	K1394AddressRange*	pAddressRange = pContext->pAddressRange;

	// save handle to address range and number of addresses
	pAddressRange->m_Count = pContext->Irb.u.AllocateAddressRange.AddressesReturned;
	pAddressRange->m_Handle = pContext->Irb.u.AllocateAddressRange.hAddressRange;

	//Call the user's completion routine
	NTSTATUS status = STATUS_SUCCESS;
	if( Irp )
		status = Irp->IoStatus.Status;

	pContext->pThis->OnAllocateComplete(status);

	delete pContext;

	return status;
	UNREFERENCED_PARAMETER(DeviceObject);
}


//////////////////////////////////////////////////////////////////////////////////////////
//K1394AsyncClient::AllocateAddressSpecialComplete
//
// Internal completion routine called when the address range allocation submitted through
//	physical address mapping routine completes
//
//	Parameters:
//		Context	context we passed to SubmitIrb. (type ALLOCATE_CONTEXT)
//
//	Returns
//		None.
//
//  Comments:
//		Direct call to AllocateAddressComplete.
//
VOID K1394AsyncClient::AllocateAddressSpecialComplete(PVOID Context)
{
	AllocateAddressComplete(NULL, NULL, Context);
}

////////////////////////////////////////////////////////////////////////
//K1394AsyncClient::NotificationRoutine
//
// The internal routine called when any operation is performed on the
//	address range, provided the corresponding notification flag was set
//
// Parameters:
//	pInfo			A structure describing the operation that completed
//					See the DDK for more information.
//
// Returns:
//	None.
//
//	Comments:
//    Calls virtual notification functions OnRead, OnWrite, and OnLock
//
VOID K1394AsyncClient::NotificationRoutine(PNOTIFICATION_INFO pInfo)
{
	K1394AsyncClient* pThis = (K1394AsyncClient*)pInfo->Context;

	if( pInfo->Mdl )
	{
		//The normal case: pass the Mdl and offset to the user
		switch( pInfo->fulNotificationOptions )
		{
		case NOTIFY_FLAGS_AFTER_READ:
			pThis->OnRead(pInfo->Mdl, pInfo->ulOffset, pInfo->nLength);
			break;
		case NOTIFY_FLAGS_AFTER_WRITE:
			pThis->OnWrite(pInfo->Mdl, pInfo->ulOffset, pInfo->nLength, pInfo->Fifo);
			break;
		case NOTIFY_FLAGS_AFTER_LOCK:
			pThis->OnLock(pInfo->Mdl, pInfo->ulOffset, pInfo->nLength);
			break;
		}
	}
	else
	{
	//We have no Mdl, the driver must be providing the response packet.
		//Call the alternate callback routine
		switch( pInfo->fulNotificationOptions )
		{
		case NOTIFY_FLAGS_AFTER_READ:
			pThis->OnRead(
				pInfo->RequestPacket, pInfo->ResponseMdl,
				pInfo->ResponsePacket,
				pInfo->ResponseLength,
				pInfo->ResponseEvent);
			break;
		case NOTIFY_FLAGS_AFTER_WRITE:
			pThis->OnWrite(pInfo->RequestPacket, pInfo->ResponseMdl,
				pInfo->ResponsePacket,
				pInfo->ResponseLength,
				pInfo->ResponseEvent);
			break;
		case NOTIFY_FLAGS_AFTER_LOCK:
			pThis->OnLock(pInfo->RequestPacket, pInfo->ResponseMdl,
				pInfo->ResponsePacket,
				pInfo->ResponseLength,
				pInfo->ResponseEvent);
			break;
		}
	}

	return;
}

#endif
