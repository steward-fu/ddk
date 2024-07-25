// iread.cpp - read operation for USB Isoc Sample Driver
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

#include <vdw.h>
#include <kusb.h>
#include "isochdev.h"
#include "isocioct.h"

extern KTrace Tracer;          // Global driver trace object

////////////////////////////////////////////////////////////////////////////////
// Default handler for IRP_MJ_READ - Using USB Isoc Pipe
//
// This routine is the default handler for IRP_MJ_READ.
// Incoming IRPs read from a FIFO, filled by constantly
//   running Isoc URBs.  When FIFO fills, it is emptied
//   enough to put the new data into it!
//
// The KPnpDevice class handles restricting IRP flow
// if the device is stopping or being removed.
//
NTSTATUS UsbIsocDevice::Read(KIrp I) 
{
	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	Tracer << "UsbIsocDevice::Read() Entry\n";

	PUCHAR pBuffer      = (PUCHAR)I.BufferedReadDest();

	ULONG  dwTotalSize = I.ReadSize(CURRENT);
	ULONG  dwBytesRead = 0;

	if ( !m_pReadFifo ) {
		I.Information() = 0;
		I.PnpComplete(this, STATUS_UNSUCCESSFUL, IO_NO_INCREMENT);
		return STATUS_UNSUCCESSFUL;
	}

	// Get all data waiting in m_pReadFifo
	status = STATUS_SUCCESS;

	m_pReadFifo->Lock();
	
	if ( m_pReadFifo->NumberOfItemsAvailableForRead() == 0 ) 
	{
		m_pReadFifo->Unlock();
		I.Information() = 0;
		I.PnpComplete(this, status, IO_NO_INCREMENT);
		return status;
	}

	dwBytesRead = m_pReadFifo->Read(pBuffer, dwTotalSize);
	m_pReadFifo->Unlock();
	
	if ( dwBytesRead != dwTotalSize ) {
		Tracer << "Read FIFO Read of "
					<< dwBytesRead << "/" << dwTotalSize << " Bytes\n";
	} else {
		Tracer << "Read FIFO Read of " << dwBytesRead << " Bytes\n";
	}

	I.Information() = dwBytesRead;
	I.PnpComplete(this, status, IO_NO_INCREMENT);

	return status;
}




////////////////////////////////////////////////////////////////////////////////
// Starts a single ISOC read - allocating structures and submitting URB
//
NTSTATUS UsbIsocDevice::StartIsocReads() 
{
	NTSTATUS status;

	for (int i=0; i<ISOC_NUM_READ_URBS; i++) 
	{
		UCHAR *p = new (NonPagedPool) UCHAR[ISOC_URB_DATA_SIZE];
		ASSERT(p);
		if ( !p )  break;
		
		PURB pUrb =
			m_Pipe0.BuildIsochronousTransfer(
				ISOC_NUM_PACKETS_PER_URB,	// # packets
				ISOC_PACKET_SIZE,			// packet size
				TRUE,						// bIn
				TRUE,						// bASAP
				USBD_ISO_START_FRAME_RANGE,// Start Frame
				p,							// pBuffer
				ISOC_URB_DATA_SIZE			// Length
				);

		ASSERT(pUrb);
		if ( !pUrb ) {
			delete p;
			break;
		}

		// Do not get an error on short or zero length packets
		pUrb->UrbIsochronousTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

		PISOC_CONTEXT pContext = new (NonPagedPool) ISOC_CONTEXT;
		ASSERT(pContext);
		if ( !pContext ) {
			delete pUrb;
			delete p;
			break;
		}

		// Create the accompanying IRP once, since the kernel
		//   does not like us deleting and allocating IRPs
		//   on every completion routine.
		CCHAR stackSize = m_Usb.StackRequirement() + 1;
		PIRP pIrp = IoAllocateIrp(stackSize, FALSE);

		pContext->pNext = m_pReadContexts;
		pContext->bUsed = TRUE;
		pContext->pDev  = this;
		pContext->pIrp  = pIrp;
		pContext->pUrb  = pUrb;
		pContext->pData = p;
		pContext->pMdl  = NULL;
			
		m_pReadContexts = pContext;

		IncrementOutstandingRequestCount();

		status = QueueIsocRead(pContext);

		if ( !NT_SUCCESS(status) )
			break;
	}

	if ( i != ISOC_NUM_READ_URBS ) 
	{
		Tracer << "StartReads() allocation FAILURE!\n";
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	return STATUS_SUCCESS;
}



NTSTATUS
UsbIsocDevice::QueueIsocRead(PISOC_CONTEXT pContext)
{
	NTSTATUS status;
	_URB_HEADER         *pUHdr = (_URB_HEADER *)pContext->pUrb;
	_URB_ISOCH_TRANSFER *pIsoc = &pContext->pUrb->UrbIsochronousTransfer;

	pUHdr->UsbdDeviceHandle     = NULL;
	pUHdr->UsbdFlags            = 0;
	pIsoc->TransferBufferLength = ISOC_URB_DATA_SIZE;
	pIsoc->TransferBufferMDL    = NULL;
	pIsoc->StartFrame           = 0;

	memset(&pIsoc->hca, 0, sizeof(_URB_HCD_AREA));

	status = m_Pipe0.SubmitUrb(
				pContext->pIrp,
				pContext->pUrb,
				LinkTo(UsbIsocReadComplete),
				pContext
				);

	if ( !NT_SUCCESS(status) ) 
	{
		Tracer << "Submission of Isoc Read URB FAILED (status="
					<< (ULONG)status << ")!\n";
	}

	return status;
}


NTSTATUS
UsbIsocDevice::UsbIsocReadComplete(KIrp I, PISOC_CONTEXT pContext)
{
	NTSTATUS status;

	if ( m_State.m_StopPending ||
		 m_State.m_RemovePending ||
		 m_State.m_Removed ||
		 m_State.m_SurpriseRemoval
	   )
	{
		// Do not re-submit this URB
		pContext->bUsed = FALSE;
		DecrementOutstandingRequestCount();
		return STATUS_MORE_PROCESSING_REQUIRED;
	}

	if ( !NT_SUCCESS(I.Status()) ) 
	{
		// URB submission FAILED!
		Tracer << "Isoc Read IRP FAILED (status="<< (ULONG)I.Status() << ")!\n";

		pContext->bUsed = FALSE;
		DecrementOutstandingRequestCount();
		return STATUS_MORE_PROCESSING_REQUIRED;
	}

	// If we have nowhere to put its data, just re-cycle this buffer. 
	if ( !m_pReadFifo ) {
		status = QueueIsocRead(pContext);
		return STATUS_MORE_PROCESSING_REQUIRED;
	}
	

	_URB_ISOCH_TRANSFER *pIsoc = &pContext->pUrb->UrbIsochronousTransfer;
	
	PUSBD_ISO_PACKET_DESCRIPTOR ppd;

	UCHAR   temp[ISOC_PACKET_SIZE];

	BOOLEAN bOverflow   = FALSE;
	ULONG   dwDataCount = 0;

	// Take data from URB completion, and copy it into our FIFO
	//   If our FIFO fills up, then clear out enough for the new data!
	for (int i=0; i<ISOC_NUM_PACKETS_PER_URB; i++) 
	{
		ppd = &pIsoc->IsoPacket[i];

		// We ignore zero length packets in the data stream for now.
		//   You might want to have a zero length packet signal the
		//   end of a large block of data.  If so, then you need to
		//   handle a zero length packet arriving at any time!
		if ( USBD_SUCCESS(ppd->Status) && (ppd->Length > 0) ) 
		{

			PUCHAR p = ((UCHAR *)pIsoc->TransferBuffer) + ppd->Offset;

			m_pReadFifo->Lock();
			if ( ISOC_PACKET_SIZE > m_pReadFifo->NumberOfItemsAvailableForWrite() )
			{
				bOverflow = TRUE;

				// Discard oldest data from the Read FIFO
				m_pReadFifo->Read(temp, ppd->Length);
			}

			m_pReadFifo->Write(p, ppd->Length);
			m_pReadFifo->Unlock();

			dwDataCount += ppd->Length;
		}
	}

	if ( bOverflow ) 
	{
		Tracer << "Read FIFO overflow!\n";
		m_readOverflow++;
	}

	if ( (dwDataCount > 0)) 
		Tracer << "Isoc Read received " << dwDataCount	<< " bytes\n";

	
	// Resubmit this URB.
	status = QueueIsocRead(pContext);

	return STATUS_MORE_PROCESSING_REQUIRED;
}


