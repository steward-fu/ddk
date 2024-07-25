// iwrite.cpp - device class implementation for USB Isoc Sample Driver
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
// Default handler for IRP_MJ_WRITE - Using USB Isoc Pipe
//
// This routine is the default handler for IRP_MJ_WRITE.
// Incoming IRPs just send data down to the USB device, synchronously.
//
// The KPnpDevice class handles restricting IRP flow
// if the device is stopping or being removed.
//
// The application doing these writes MUST limit the amount of
// data being written to ISOC_NUM_PACKETS_PER_URB * ISOC_PACKET_SIZE,
// or 2048 bytes - to be compatible with all Host Controller drivers
// in Win95 and Win98.
//
// If you want to write more data at once, consider creating multiple
// URBs and IRPs that would span the entire data block you wish to
// write.  Care must be taken that the original IRP is not completed
// until all of the URBs are done.
//
NTSTATUS UsbIsocDevice::Write(KIrp I) 
{
	NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

	I.Information() = 0; 

	Tracer << "UsbIsocDevice::Write() Entry\n";

	// Send down a Pipe Reset.
	// 
	// Since these writes are queued from Ring 3, assume
	//   that the previous write has finished, and the
	//   pipe did not have more data queued up to it in time!
	m_Pipe1.Reset();

	PVOID pBuffer     = I.BufferedWriteSource();
	ULONG dwTotalSize = I.WriteSize(CURRENT);
	ULONG dwBytesSent = 0;

	// Create an URB to do actual Bulk read from Pipe0
	KMemory *pMdl = new (NonPagedPool) KMemory(pBuffer, dwTotalSize);
	if ( !pMdl )
	{
		ASSERT( pMdl != 0 );
		return I.PnpComplete(STATUS_INSUFFICIENT_RESOURCES, IO_NO_INCREMENT);
	}
	pMdl->SetPageArray();	// tells system to set phys page addrs inside MDL

	PISOC_CONTEXT pContext = new (NonPagedPool) ISOC_CONTEXT;
	if ( !pContext ) 
	{
		ASSERT ( pContext != 0 );
		delete pMdl;
		return I.PnpComplete(STATUS_INSUFFICIENT_RESOURCES, IO_NO_INCREMENT);
	}

	memset(pContext, 0, sizeof(ISOC_CONTEXT));

	// NOTE: This assumes you are passing a total size that is
	//       an even multiple of the hard-coded packet size (now
	//       64), is 2048 bytes or less. Packet size is hardware
	//		 dependent.
	//
	//       If USBD gets a "short" packet for transfer, your
	//       pipe will be shut down.
	//
	ULONG numPackets = dwTotalSize / ISOC_PACKET_SIZE;

	if ( (numPackets * ISOC_PACKET_SIZE) != dwTotalSize )
		numPackets++;

	ASSERT( numPackets <= ISOC_NUM_PACKETS_PER_URB );
	
	PURB pUrb =
		m_Pipe1.BuildIsochronousTransfer( numPackets,		// # of Packets
										  ISOC_PACKET_SIZE,	// Packet Size
										  FALSE,			// Output, not in
										  TRUE,				// send ASAP
										  0,				// Start frame (unused if ASAP)
										  pMdl,
										  dwTotalSize
										);

	// Submit the URB to our USB device, asynchronously
	if ( pUrb != 0 ) 
	{
		// Fill in the USBD_ISO_PACKET_DESCRIPTORs
		for (ULONG i=0; i<numPackets; i++)
			pUrb->UrbIsochronousTransfer.IsoPacket[i].Offset = i*ISOC_PACKET_SIZE;

		pContext->pDev = this;
		pContext->pIrp = I;
		pContext->pUrb = pUrb;
		pContext->pMdl = pMdl;

		// clean up is always done in UsbIsocWriteComplete if SubmitUrb is called
		NTSTATUS status = m_Pipe1.SubmitUrb(pUrb, LinkTo(UsbIsocWriteComplete), pContext);
		
		if(status == STATUS_PENDING)
			I.MarkPending();  //This is the normal case. 
		else if(!NT_SUCCESS(status))
			I.PnpComplete(status);
		return status;
	}
	else
	{
		ASSERT ( pUrb != 0 );
		delete pMdl;
		delete pContext;
		return I.PnpComplete(STATUS_INSUFFICIENT_RESOURCES, IO_NO_INCREMENT);
	}
}

NTSTATUS UsbIsocDevice::UsbIsocWriteComplete(KIrp I, PISOC_CONTEXT pContext)
{
// The only field in the IRP that this routine may examine is the status.

	NTSTATUS status = I.Status();

// NOTE: We created this IRP.  We do not have to mark it pending
//       because we are also the last driver to deal with this IRP!
//	if ( I->PendingReturned )
//		I.MarkPending();

	ASSERT ( pContext != NULL );

	ULONG dwBytesSent =
		pContext->pUrb->UrbBulkOrInterruptTransfer.TransferBufferLength;

	if ( dwBytesSent > 0 ) {
		Tracer << "Write() sent "
				<< dwBytesSent
				<< " bytes to USB (status="
				<< (ULONG)status << ")!\n";
	}

	KIrp origI(pContext->pIrp);

	// Clean up all the allocations we did
	if ( pContext->pUrb )
		delete pContext->pUrb;
	
	if ( pContext->pMdl )
		delete pContext->pMdl;
	
	delete pContext;

	// Now complete the IRP that started this write
	origI.Information() = dwBytesSent;

	if (origI->PendingReturned)
		origI.MarkPending();

	origI.PnpComplete(status, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

