// IsocClient.cpp
// Implementation of Isoc1394Transfer class
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998-2003 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//

#include <vdw.h>
#include "IsocClient.h"
#include "..\Isoc1394Types.h"
#include "AsyncCommands.h"
#include "Isoc1394Device.h"

extern KTrace t;

//////////////////////////////////////////////////////////////////////////////////////////
//Isoc1394Transfer::Initialize
//
//  Allocates a resource handle and uses it to initialize the transfer object
//
// Parameters:
//	Write - If TRUE, this will be a write transfer, if FALSE, this will be a read transfer
//  lowerDevice - Pointer to a K1394LowerDevice object used to initialize the base class 
//                and to allocate the resource handle
//  theDevice - Pointer to our device object to be used for completing IRPs as buffers 
//               complete
//  Speed - The speed at which to transmit.  It's determined at run time in OnStartDevice.
//
// Returns:
//   STATUS_SUCCESS if successful
//
// Comments: 
//   This function initializes a transfer object.  It handles allocating a resource
//    handle and initializing the base class.
//

NTSTATUS Isoc1394Transfer::Initialize(
	BOOLEAN				bWrite,
	K1394LowerDevice*	pLowerDevice,
	Isoc1394Device*		pDevice,
	ULONG				nSpeed
	) 		
{
	m_pDevice = pDevice;
	m_pLowerDevice = pLowerDevice;
	
	ULONG nFlags = bWrite ? RESOURCE_USED_IN_TALKING : RESOURCE_USED_IN_LISTENING;

	//Allocate our resource handle
	NTSTATUS status = m_pLowerDevice->Allocate(
		m_Resource,
		nSpeed,
		nFlags,	
		2,						// Channel number
		MAX_BYTES_PER_FRAME,	// Max Bytes per frame
		MAX_BUFFER_COUNT,		// buffer count
		MAX_BYTES_PER_FRAME,	// Max Buffer Size
		1);						// Quadlets to strip
	
	if (NT_SUCCESS(status))
	{
		K1394IsochTransfer::Initialize(m_pLowerDevice, &m_Resource);
	}

	return status;
}

//////////////////////////////////////////////////////////////////////////////////////////
//Isoc1394Transfer::OnStartComplete
//
//  Called when the request to start the transfer completes
//
// Parameters:
//  Status	- STATUS_SUCCESS if the transfer is started successfully.  
//				An appropriate error status if the transfer failled to start
//
// Comments:
//   This routine simply cancels all submitted buffers if the transfer could not be
//   started.  That will remove them from the transfer, and cause OnBufferComplete to 
//   be called.  It will complete the associated IRPs with a failure status.
//   
//   No action is taken if the transfer was started successfully.
//

VOID Isoc1394Transfer::OnStartComplete(NTSTATUS Status)
{
	t << "Entering Isoc1394Transfer::OnStartComplete\n";

	//If we didn't succeed, cancel all buffers in progress. 
	 // OnBufferComplete will complete the IRPs
	if (!NT_SUCCESS(Status))
		CancelAllBuffers();
}


/////////////////////////////////////////////////////////////////////////////////////////
//Isoc1394Transfer::OnBufferComplete
//
// Called when a list of buffers submitted is complete
//
// Parameters:
//	Status		- STATUS_SUCCESS if the buffer completed successfully,
//				  STATUS_CANCELLED if the buffer was cancelled early
//				  An appropriate error status if the transfer failled
//
//  pBufferList	- The list of buffers submitted for transmitting.
//
// Returns:
//	  Should usually return the value of BufferStatus
//
// Comments:
//	OnBufferComplete is called either if a loopback buffer was sent/received, or if a 
//		read/write IRP is being processed.
//
//  In the case of a loopback transfer, we need only signal the event to tell the transfer
//   it's operation has completed.
//
//  In the case of an IRP, we need to either complete the IRP if all bytes have been sent
//   or received, or start sending/receiving the next segment of the buffer.
//

VOID Isoc1394Transfer::OnBufferComplete(
	NTSTATUS				Status, 
	K1394IsochBufferList*	pBufferList
	)
{
	PBUFFER_DESCRIPTOR  pContext = (PBUFFER_DESCRIPTOR) pBufferList->CompletionContext(); 
    ULONG               size;

	// true only if processing a loopback transfer
	if (pContext->pEvent)
	{
		m_BufferStatus = Status;
		pContext->pEvent->Set();
	}
	else
	{
	
		//In the normal case when processing a read or write IRP:
		pContext->BytesProcessed += pBufferList->TotalBytesProcessed();

		//Free any MDLs we allocated
		if (pContext->AllocatedMdl)
		{
			for(ULONG next = 0; next < pBufferList->Count(); next++)
			{
				IoFreeMdl(pBufferList[next].Mdl);
			}
		}
		//Update the allocated buffer count
		m_BufferSpinlock.Lock();
		m_BuffersAttached -= (ULONG)pBufferList->Count();
		m_BufferSpinlock.Unlock();

		//Free the buffer list
		delete pBufferList;
		
		//See if we are done
		KIrp I(pContext->pIrp);
        if (I.MajorFunction() == IRP_MJ_READ)
        {
            size = I.ReadSize();
        }
        else
        {
            size = I.WriteSize();
        }

		if ((pContext->BytesProcessed >= size) || !NT_SUCCESS(Status))
		{
			//Complete the IRP
			I.Status() = Status;
			I.Information() = pContext->BytesProcessed;
		
			m_pDevice->PnpNextIrp(I);
			delete pContext;
		}
		else
		{
			//There are more bytes to process

			m_pDevice->StartSegment(pContext);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
//Isoc1394Transfer::~Isoc1394Transfer
//
// Destructor
//
// Parameters: None
//
// Comments:
//	Stops the transfer and frees the associated resource handle.
//

Isoc1394Transfer::~Isoc1394Transfer()
{
	Stop();

	m_pLowerDevice->Free(m_Resource);
}	
