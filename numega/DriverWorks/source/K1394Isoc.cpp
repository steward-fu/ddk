// k1394Isoc.cpp - functions for 1394 isochronous classes
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


////////////////////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::Allocate
//
// Allocates isochronous bandwidth
//
// Parameters:
//	Bandwidth			The K1394IsochBandwidth object to be allocated
//	Speed				The requested speed at which to allocate bandwidth
//						Possible values are:
//						SPEED_FLAGS_100 - 100 megabits/second
//						SPEED_FLAGS_200 - 200 megabits/second
//						SPEED_FLAGS_400 - 400 megabits/second
//	MaxBytesPerFrame	The MaxBytesPerFrame the driver may send
//
// Returns
//	STATUS_SUCCESS if successful
//
// Comments:
//	Check K1394IsochBandwidth::SpeedSelected to find the speed allocated to the device.
//	 It may be different from the speed requested.
//
//	This routine must be called at PASSIVE_LEVEL IRQL
//
NTSTATUS K1394LowerDevice::Allocate(
	K1394IsochBandwidth&	Bandwidth,
	ULONG					Speed,
	ULONG					MaxBytesPerFrame)
{
	NTSTATUS status;
	ASSERT(Bandwidth.m_Handle == NULL);

	// Allocate IRB on stack
	KIrb<REQUEST_ISOCH_ALLOCATE_BANDWIDTH> Irb(Speed, MaxBytesPerFrame);

	status = SubmitIrb(&Irb);

	if( NT_SUCCESS(status) )
	{
		// Save the fields returned in the IRB
		Bandwidth.m_Speed = Irb.Speed();
		Bandwidth.m_MaxBytesPerFrame = MaxBytesPerFrame;

		// save handle to bandwidth
		Bandwidth.m_Handle = Irb.Handle();

		// Something is very wrong if we don't have a handle at this point
		ASSERT(Bandwidth.m_Handle != NULL);
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::Free
//
//	Frees the bandwidth allocated in K1394LowerDevice::Allocate
//
// Parameters:
//	Bandwidth	K1394IsochBandwidth object to be freed
//
// Returns
//	STATUS_SUCCESS if successful
//
NTSTATUS K1394LowerDevice::Free(K1394IsochBandwidth& Bandwidth)
{
	NTSTATUS status = STATUS_SUCCESS;

	// If the bandwidth isn't allocated we're done
	if ( Bandwidth.m_Handle != NULL )
	{
		// Allocate IRB on stack
		KIrb<REQUEST_ISOCH_FREE_BANDWIDTH> Irb(Bandwidth.m_Handle);

		status = SubmitIrb(&Irb);

		// Change our handle data member back to NULL and invalidate speed selected
		Bandwidth.m_Handle = NULL;
		Bandwidth.m_Speed = Bandwidth.m_MaxBytesPerFrame = 0;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::Change
//
//	Changes the MaxBytesPerFrame allocated in K1394LowerDevice::Allocate(Bandwidth)
//
// Parameters:
//	Bandwidth			The bandwidth object to have the MaxBytesPerFrame changed
//	MaxBytesPerFrame	The new MaxBytesPerFrame
//
// Returns:
//	STATUS_SUCCESS if successful
//
// Comments:
//	In order to change the bandwidth in use by a transfer object, the K1394IsochResource
//	 object must be freed and reallocated.
//
NTSTATUS K1394LowerDevice::Change(K1394IsochBandwidth& Bandwidth, ULONG MaxBytesPerFrame)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if ( Bandwidth.m_Handle != NULL )
	{
		KIrb<REQUEST_ISOCH_SET_CHANNEL_BANDWIDTH> Irb(Bandwidth.m_Handle, MaxBytesPerFrame);

		status =  SubmitIrb(&Irb);

		if( NT_SUCCESS(status) )
			Bandwidth.m_MaxBytesPerFrame = MaxBytesPerFrame;
	}

	return status;
}


/////////////////////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::Allocate
//
// Allocates an isochronous channel
//
// Parameters:
//	Channel			The channel object to allocate
//	ChannelNumber	The requested channel number.  May be ISOCH_ANY_CHANNEL to let the bus
//					driver choose the channel number.
//
// Returns
//	STATUS_SUCCESS if successful.
//
// Comments
//	Check K1394IsochChannel::NumberSelected to see the channel number allocated.  From
//   experimemtation, if the channel number requested is not available, an error status
//   will be returned.
//
NTSTATUS K1394LowerDevice::Allocate(
	K1394IsochChannel&	Channel,
	ULONG				ChannelNumber)
{
	NTSTATUS status;

	ASSERT( Channel.m_Number == ISOCH_ILLEGAL_CHANNEL );

	KIrb<REQUEST_ISOCH_ALLOCATE_CHANNEL> Irb(ChannelNumber);

	status = SubmitIrb(&Irb);

	if( NT_SUCCESS(status) )
	{
		// save the channel number
		Channel.m_Number = Irb.u.IsochAllocateChannel.Channel;
	}
	else
	{
		// if the call failed, clear state and return error
		Channel.m_Number = ISOCH_ILLEGAL_CHANNEL;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::Free
//
// Frees a previosly allocated isochronous channel object
//
// Parameters:
//	Channel			The channel object to free
//
// Returns:
//	STATUS_SUCCESS if successful
//
// Comments:
//
NTSTATUS K1394LowerDevice::Free(K1394IsochChannel& Channel)
{
	NTSTATUS status = STATUS_SUCCESS;

	// If it isn't allocated we're done
	if ( Channel.m_Number != ISOCH_ILLEGAL_CHANNEL )
	{
		KIrb<REQUEST_ISOCH_FREE_CHANNEL> Irb(Channel.m_Number);

		status = SubmitIrb(&Irb);

		// Change our selected channel back to the illegal value
		Channel.m_Number = ISOCH_ILLEGAL_CHANNEL;
	}

	return status;
}

///////////////////////////////////////////////////////////////////////
//K1394LowerDevice::Allocate
//
// Allocates an isochronous resource handle
//
// Parameters
//		Resource - Will hold the handle to the resource
//		Speed	- Must be one of
//			SPEED_FLAGS_100 - 100 megabits/second
//			SPEED_FLAGS_200 - 200 megabits/second
//			SPEED_FLAGS_400 - 400 megabits/second
//		Flags - specifies the resource flags possible values are:
//			RESOURCE_USED_IN_LISTENING - specify that the handle will be used for listening
//			RESOURCE_USED_IN_TALKING - specify that the handle will be used for talking
//			RESOURCE_BUFFERS_CIRCULAR - when the bus driver reaches the end of
//				the buffer list, it starts again at the beginning.  Buffers will
//				not be automatically removed by the IsochTransfer class as they
//				complete.
//			RESOURCE_SYNCH_ON_TIME - Specifies that a StartTime parameter will be provided
//				when the transfer is started
//			RESOURCE_USE_PACKET_BASED - One packet will be placed in each buffer.  The
//				default is stream based where several packets are placed in a single buffer
//				one after the other until the buffer is full.
//		Channel - specifies the channel number to use.  If ISOCH_ANY_CHANNEL is specified,
//				the bus driver chooses the channel.
//		MaxBytesPerFrame - The maximum bytes to be sent per isochronous frame.  Packets
//				must be split if they are bigger than this number.
//		NumberOfBuffers - The maximum nuber of buffers that may be attached to this
//				resource handle at one time.
//		MaxBufferSize - The size of the largest buffer that may be attached
//		QuadletsToStrip - Specifies the number of 4-byte words to strip from the
//				beginning of a packet.  In most cases, setting this to one will remove
//				the packet headers.
//
//	Comments:
//		This routine makes no check to see if the channel or bandwidth specified have
//			been allocated on the bus.  It will allow you to start the transfer
//			regardless.  Bandwidth and Channel should be allocated for
//			your device to have exclusive access to these resources.
//
//		The bandwidth allocated defines the MaxBytesPerFrame and Speed that may be used.
//		The channel allocated defines the channel number that may be used.
//
//		If QuadletsToStrip is nonzero, the flag, RESOURCE_STRIP_ADDITIONAL_QUADLETS
//        is set automatically.
//
//		This function must be called at PASSIVE_LEVEL IRQL.
//
NTSTATUS K1394LowerDevice::Allocate(
	K1394IsochResource&	Resource,
	ULONG				Speed,
	ULONG				Flags,
	ULONG				Channel,
	ULONG				MaxBytesPerFrame,
	ULONG				NumberOfBuffers,
	ULONG				MaxBufferSize,
	ULONG				QuadletsToStrip
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	// Can't allocate the resource if it is already allocated
	if ( Resource.m_Handle == NULL )
	{
		if( QuadletsToStrip )
			Flags |= RESOURCE_STRIP_ADDITIONAL_QUADLETS;

		KIrb<REQUEST_ISOCH_ALLOCATE_RESOURCES> Irb(	Speed,
													Flags,
													Channel,
													MaxBytesPerFrame,
													NumberOfBuffers,
													MaxBufferSize,
													QuadletsToStrip);

		status = SubmitIrb(&Irb);

		if( NT_SUCCESS(status) )
		{
			// save handle to resource
			Resource.m_Handle = Irb.u.IsochAllocateResources.hResource;
			Resource.m_Flags = Flags;
		}
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////////////
//K1394LowerDevice::Free
//
//  Frees a resource handle allocated with K1394LowerDevice::Allocate
//
//	Parameters
//		None.
//
//	Returns
//		STATUS_SUCCESS if the resource was freed correctly.
//
//	Comments
//
NTSTATUS K1394LowerDevice::Free(K1394IsochResource& Resource)
{
	NTSTATUS status = STATUS_SUCCESS;

	// If the resource if it isn't allocated we're done
	if ( Resource.m_Handle != NULL )
	{
		KIrb<REQUEST_ISOCH_FREE_RESOURCES> Irb(Resource.m_Handle);

		status = SubmitIrb(&Irb);

		// Change our handle data member back to NULL
		Resource.m_Handle = NULL;
	}

	return status;
}


/////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochBufferList::operator new
//
// Allocates a new buffer array and stores the number of buffers allocated in the first
//  buffer in the field DeviceReserved[0]
//
// Parameters:
//	Size				the size to allocate
//	DescriptorCount		the number of buffer descriptors to allocate
//
// Returns
//	A pointer to the descriptor array
//
// Comments
//
PVOID K1394IsochBufferList::operator new(size_t Size, ULONG DescriptorCount)
{
	if( !DescriptorCount )
		DescriptorCount = 1;

	K1394IsochBufferList* pThis = (K1394IsochBufferList*)::new ISOCH_DESCRIPTOR[DescriptorCount];
	if( pThis )
	{
		RtlZeroMemory(pThis, sizeof(ISOCH_DESCRIPTOR)*DescriptorCount);
		pThis[0].DeviceReserved[0] = DescriptorCount;
	}

	return pThis;
	UNREFERENCED_PARAMETER(Size);
}

/////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochBufferList::SetupAllBuffers
//
//	Sets the parameters except the MDL's for all descriptors in the list
//
//	Parameters:
//		MaxBytesPerFrame		On talk operations, this specifies the size of packets the
//								buffer will be split into.  It must evenly divide into the
//								total buffer size.
//		Flags					Specifies flags for this buffer.
//		Synch					Specifies the Sy field for the isochronous packet header on
//								talk operations, or the value to be compared against it on
//								listen operations.  As the field is only 4 bits in length,
//								only values of 0 to 15 are valid.
//		Tag						Specifies the Tag field for the isochronous packet header on
//								talk operations, or the value to be compared against it on
//								listen operations.  As the field is only 2 bits in length,
//								only values of 0 to 3 are valid.
//		CycleTime				The isochronous cycle time on which to start processing
//								this packet
//
// Returns:
//	None
// Comments:
//
VOID K1394IsochBufferList::SetupAllBuffers(
	ULONG		MaxBytesPerFrame,
	ULONG		Flags,
	ULONG		Synch,
	ULONG		Tag,
	PCYCLE_TIME	CycleTime)
{
	ULONG_PTR DescriptorCount = this[0].DeviceReserved[0];

	for(ULONG next = 0; next < DescriptorCount; next++)
	{
		this[next].nMaxBytesPerFrame = MaxBytesPerFrame;
		this[next].fulFlags = Flags;
		this[next].ulSynch = Synch;
		this[next].ulTag = Tag;

		if( CycleTime )
			this[next].CycleTime = *CycleTime;
	}

	return;
}

/////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochBufferList::SetupBuffer
//
//	Sets the parameters for a single descriptors in the list
//
//	Parameters:
//		Index					The zero based index of the descriptor to be set up
//		pMdl					The MDL to be described by this descriptor
//		MaxBytesPerFrame		On talk operations, this specifies the size of packets the
//								buffer will be split into.  It must evenly divide into the
//								total buffer size.
//		Flags					Specifies flags for this buffer.
//		Synch					Specifies the Sy field for the isochronous packet header on
//								talk operations, or the value to be compared against it on
//								listen operations.  As the field is only 4 bits in length,
//								only values of 0 to 15 are valid.
//		Tag						Specifies the Tag field for the isochronous packet header on
//								talk operations, or the value to be compared against it on
//								listen operations.  As the field is only 2 bits in length,
//								only values of 0 to 3 are valid.
//		CycleTime				The isochronous cycle time on which to start processing
//								this packet
//
// Returns:
//		None
//
// Comments:
//

VOID K1394IsochBufferList::SetupBuffer(
	ULONG		Index,
	PMDL		pMdl,
	ULONG		MaxBytesPerFrame,
	ULONG		Flags,
	ULONG		Synch,
	ULONG		Tag,
	PCYCLE_TIME	CycleTime)
{
	if(Index < this[0].DeviceReserved[0])
	{
		this[Index].Mdl = pMdl;
		this[Index].ulLength = MmGetMdlByteCount(pMdl);

		if( MaxBytesPerFrame )
			this[Index].nMaxBytesPerFrame = MaxBytesPerFrame;
		else
			this[Index].nMaxBytesPerFrame = this[Index].ulLength;

		this[Index].fulFlags = Flags;
		this[Index].ulSynch = Synch;
		this[Index].ulTag = Tag;

		if( CycleTime )
			this[Index].CycleTime = *CycleTime;
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochTransfer::AddBufferList
//
// Add a list of buffers to the transfer.  OnBufferComplete will be called when the operation
//   completes.
//
// Parameters:
//	BufferList - A list of buffers to be attached to the transfer
//
// Returns:
//	STATUS_PENDING in the normal case.
//	STATUS_INSUFFICIENT_RESOURCES if the allowed maximum number of buffers will be exceeded
//
// Comments:
//
NTSTATUS K1394IsochTransfer::AddBufferList(K1394IsochBufferList& BufferList)
{
	NTSTATUS    status = STATUS_UNSUCCESSFUL;
    KIRQL       oldIrql;

	//Make sure the class is initialized
	if( m_pLowerDevice && m_pResource->m_Handle )
	{
		PBUFFER_CONTEXT pBufferContext = new (NonPagedPool) BUFFER_CONTEXT;
		if( pBufferContext )
		{
			pBufferContext->pThis = this;
			pBufferContext->pBufferList = &BufferList;
			pBufferContext->Cancelled = FALSE;

			//Allocate the IRB
			KIrb<REQUEST_ISOCH_ATTACH_BUFFERS>* pIrb =
				(KIrb<REQUEST_ISOCH_ATTACH_BUFFERS>*) m_IrbPool.alloc();

			if ( pIrb )
			{
                // Allocate our add buffer context structure
                PADD_BUFFER_CONTEXT pAddBufferContext =
                                new (NonPagedPool) ADD_BUFFER_CONTEXT;

                if (pAddBufferContext != NULL)
                {
			        //Put our completion routine on the last buffer in the list
			        BufferList[BufferList.Count()-1].Callback = LinkTo(BufferComplete);
			        BufferList[BufferList.Count()-1].Context1 = this;
			        BufferList[BufferList.Count()-1].Context2 = pBufferContext;

				    //Initialize the Irb
				    pIrb->Initialize(m_pResource->m_Handle, (ULONG)BufferList.Count(), BufferList);

                    // initialize our add buffer context
                    pAddBufferContext->pIrb = pIrb;
                    pAddBufferContext->pContext = pBufferContext;
                    pAddBufferContext->pThis = this;

                    // Get the buffer list lock
                    KeAcquireSpinLock(&m_BufferListLock, &oldIrql);

                    // Add the buffer to our list
                    pBufferContext->pThis->m_BufferList.InsertTail(pBufferContext);

                    // Release the buffer list lock
                    KeReleaseSpinLock(&m_BufferListLock, oldIrql);

				    //Submit it to the lower device
				    return m_pLowerDevice->SubmitIrb(pIrb, AddBufferComplete, pAddBufferContext);
                }

                m_IrbPool.free(pIrb);
			}

			delete pBufferContext;
		}

		status = STATUS_INSUFFICIENT_RESOURCES;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochTransfer::CancelBufferList
//
//	Removes a list of buffers from the transfer.  OnBufferComplete should be called soon
//	 after calling this routine.
//
// Parameters:
//	BufferList - A list of buffers to be removed from the transfer
//
// Returns:
//	STATUS_PENDING in the normal case.
//	STATUS_SUCCESS if the operation succeeded immediatly.
//
// Comments:
//	Call this routine to cancel a list of attached buffers immediately, or to cancel
//   a list of buffers submitted with RESOURCE_BUFFERS_CIRCULAR specified in the resource
//   handle.  In the normal case, buffers will be detached automatically when they complete.
//

NTSTATUS K1394IsochTransfer::CancelBufferList(K1394IsochBufferList& BufferList)
{
	NTSTATUS    status = STATUS_UNSUCCESSFUL;
    KIRQL       oldIrql;

	if( m_pLowerDevice && m_pResource->m_Handle )
	{
        // Get the buffer list lock
        KeAcquireSpinLock(&m_BufferListLock, &oldIrql);

		//Find the buffer context in the list
		PBUFFER_CONTEXT pBufferContext =  m_BufferList.Head();

		while( pBufferContext && (pBufferContext->pBufferList != &BufferList) )
			pBufferContext = m_BufferList.Next(pBufferContext);

		if( pBufferContext )
		{
			pBufferContext->Cancelled = TRUE;

			KIrb<REQUEST_ISOCH_DETACH_BUFFERS>* pIrb =
				(KIrb<REQUEST_ISOCH_DETACH_BUFFERS>*) m_IrbPool.alloc();

			if( pIrb )
			{
                // Remove the buffer from the list
	            pBufferContext->pThis->m_BufferList.Remove(pBufferContext);

                // Release the buffer list lock
                KeReleaseSpinLock(&m_BufferListLock, oldIrql);

				pIrb->Initialize(m_pResource->m_Handle, (ULONG)BufferList.Count(), BufferList);
				pBufferContext->pIrb = pIrb;

				status = m_pLowerDevice->SubmitIrb(pIrb, RemoveBufferComplete, pBufferContext);
			}
			else
            {
                // Release the buffer list lock
                KeReleaseSpinLock(&m_BufferListLock, oldIrql);

				status = STATUS_INSUFFICIENT_RESOURCES;
            }
		}
		else
        {
            // Release the buffer list lock
            KeReleaseSpinLock(&m_BufferListLock, oldIrql);

			status = STATUS_INVALID_DEVICE_REQUEST;
        }
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochTransfer::CancelAll buffers
//
//	Removes all buffer lists from the transfer.  OnBufferComplete should be called soon
//	 after calling this routine.
//
// Parameters:
//
// Returns:
//	STATUS_SUCCESS if the operation succeeded.
//
// Comments:
//		This routine is called from the desctructor after stopping a
//   transfer in progress.
//

NTSTATUS K1394IsochTransfer::CancelAllBuffers()
{
	NTSTATUS        status = STATUS_UNSUCCESSFUL;
    PBUFFER_CONTEXT pBufferContext;
    PBUFFER_CONTEXT pBufferNextContext;
    KInterlockedList<BUFFER_CONTEXT>    deadBufferList;
    KIRQL           oldIrql;

	if( m_pLowerDevice && m_pResource->m_Handle )
	{
        // Get the buffer list lock
        KeAcquireSpinLock(&m_BufferListLock, &oldIrql);

        // Walk through the buffer list and remove all buffers
        // place context pointers in local list
        pBufferNextContext = m_BufferList.Head();

	    while (pBufferNextContext != NULL)
	    {
	        pBufferContext = pBufferNextContext;

            pBufferNextContext = m_BufferList.Next(pBufferContext);

            // If we fail to allocate an IRB, we will leave the buffer
            // in the list and return a failing status to indicate that
            // not all buffers were cancelled
	        KIrb<REQUEST_ISOCH_DETACH_BUFFERS>* pIrb =
		        (KIrb<REQUEST_ISOCH_DETACH_BUFFERS>*) m_IrbPool.alloc();

	        if( pIrb )
	        {
                deadBufferList.InsertTail(pBufferContext);

                m_BufferList.Remove(pBufferContext);

                pBufferContext->pIrb = pIrb;
            }
        }

        if(m_BufferList.IsEmpty())
        {
            status = STATUS_SUCCESS;
        }

        // Release the buffer list lock
        KeReleaseSpinLock(&m_BufferListLock, oldIrql);

        // Now cancel the buffers we can
        pBufferNextContext = deadBufferList.Head();

	    while (pBufferNextContext != NULL)
	    {
	        pBufferContext = pBufferNextContext;

            pBufferNextContext = deadBufferList.Next(pBufferContext);

            KIrb<REQUEST_ISOCH_DETACH_BUFFERS>* pIrb =
                (KIrb<REQUEST_ISOCH_DETACH_BUFFERS>*)pBufferContext->pIrb;

			pBufferContext->Cancelled = TRUE;

            // Remove the buffer from the list
	        deadBufferList.Remove(pBufferContext);

			pIrb->Initialize(
                        m_pResource->m_Handle,
                        (ULONG)pBufferContext->pBufferList->Count(),
                        *pBufferContext->pBufferList
                        );

			m_pLowerDevice->SubmitIrb(
                            pIrb,
                            RemoveBufferComplete,
                            pBufferContext
                            );
		}
    }

	return status;
}


////////////////////////////////////////////////////////////////////
// K1394IsochClient::RemoveBufferComplete
//
//	RemoveBufferComplete is an internal routine called when some buffers
//   have been removed from the transfer.
//
// Parameters
//	 DeviceObject - Our system device object
//	 Irp		  - The IRP being completed
//	 Context	  - In this case, a BUFFER_DESCRIPTOR structure
//
// Notes
//	 RemoveBufferComplete notifies the user that the transfer is done if
//    this is the last segment, or starts the next segment.
//

NTSTATUS K1394IsochTransfer::RemoveBufferComplete(
		PDEVICE_OBJECT		DeviceObject,
		PIRP				Irp,
		PVOID				Context
		)
{
	PBUFFER_CONTEXT pContext = (PBUFFER_CONTEXT)Context;
	KIrp I(Irp);
	NTSTATUS status = I.Status();

	if( pContext->Cancelled )
		pContext->pThis->OnBufferComplete(STATUS_CANCELLED, pContext->pBufferList);
	else
		pContext->pThis->OnBufferComplete(status, pContext->pBufferList);

	pContext->pThis->m_IrbPool.free(pContext->pIrb);

	delete pContext;

	return status;
	UNREFERENCED_PARAMETER(DeviceObject);
}

////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochTransfer::StartTalk
//
// Starts an isochronous talk operation
//
// Parameters:
//	 StartTime - optionally specifies the isochronous clock cycle on which to start the
//      transfer.  If specified, RESOURCE_FLAGS_SYNCH_ON_TIME must have been specified
//      when the resource handle was allocated.
//
//	Returns:
//		STATUS_SUCCESS if the operation was successful
//		STATUS_INSUFFICIENT_RESOURCES if no buffers were attached to the transfer
//
// Comments:
//   This routine may be called at any IRQL.  OnStartComplete will be called when the
//     operation completes.
//
//	 At least one buffer must be attached for this call to succeed.

NTSTATUS K1394IsochTransfer::StartTalk(PCYCLE_TIME StartTime)
{
	NTSTATUS status = STATUS_SUCCESS;

	//If the transfer has already been started, we're done.
	if( !m_IsStarted )
	{
		//If the object hasn't been initialized, we can't start the transfer.
		if( m_pLowerDevice )
		{
			PSTART_STOP_CONTEXT theContext = new (NonPagedPool) START_STOP_CONTEXT;
			if( theContext )
			{
				KIrb<REQUEST_ISOCH_TALK>* pIrb = (KIrb<REQUEST_ISOCH_TALK>*)&theContext->Irb;
				pIrb->Initialize(m_pResource->m_Handle, StartTime);

				theContext->pThis = this;

				status = m_pLowerDevice->SubmitIrb(pIrb, StartStopComplete, theContext);
			}
			else
				status = STATUS_INSUFFICIENT_RESOURCES;
		}
		else
			status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochTransfer::StartListen
//
// Starts an isochronous listen operation
//
// Parameters:
//	 StartTime - optionally specifies the isochronous clock cycle on which to start the
//      transfer.  If specified, RESOURCE_FLAGS_SYNCH_ON_TIME must have been specified
//      when the resource handle was allocated.
//
//	Returns:
//		STATUS_SUCCESS if the operation was successful
//		STATUS_INSUFFICIENT_RESOURCES if no buffers were attached to the transfer
//
// Comments:
//   This routine may be called at any IRQL.  OnStartComplete will be called when the
//     operation completes.
//
//	 At least one buffer must be attached for this call to succeed.

NTSTATUS K1394IsochTransfer::StartListen(PCYCLE_TIME StartTime)
{
	NTSTATUS status = STATUS_SUCCESS;

	//If the transfer has already been started, we're done.
	if( !m_IsStarted )
	{
		//If the object hasn't been initialized, we can't start the transfer.
		if( m_pLowerDevice )
		{
			PSTART_STOP_CONTEXT theContext = new (NonPagedPool) START_STOP_CONTEXT;
			if( theContext )
			{
				KIrb<REQUEST_ISOCH_LISTEN>* pIrb = (KIrb<REQUEST_ISOCH_LISTEN>*)&theContext->Irb;

				pIrb->Initialize(m_pResource->m_Handle, StartTime);
				theContext->pThis = this;

				status = m_pLowerDevice->SubmitIrb(pIrb, StartStopComplete, theContext);
			}
			else
				status = STATUS_INSUFFICIENT_RESOURCES;
		}
		else
			status = STATUS_UNSUCCESSFUL;
	}

	return status;
}


////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochTransfer::Stop
//
// Stops the transfer currently in progress
//
// Parameters:
//	None.
//
// Returns:
//  STATUS_SUCCESS if successful.
//
// Comments:
//	Stopping the transfer does not automatically remove all buffers from the transfer.

NTSTATUS K1394IsochTransfer::Stop()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	//Can't stop the transfer if the class isn't initialized
	if( m_pLowerDevice )
	{
		//If the transfer isn't started, just return success
		if( m_IsStarted )
		{
			PSTART_STOP_CONTEXT theContext = new (NonPagedPool) START_STOP_CONTEXT;
			if( theContext )
			{
				KIrb<REQUEST_ISOCH_STOP>* pIrb = (KIrb<REQUEST_ISOCH_STOP>*)&theContext->Irb;

				pIrb->Initialize(m_pResource->m_Handle);

				theContext->pThis = this;

				status = m_pLowerDevice->SubmitIrb(pIrb, StartStopComplete, theContext);
			}
			else
				status = STATUS_INSUFFICIENT_RESOURCES;
		}
		else
			status = STATUS_SUCCESS;
	}

	return status;
}

////////////////////////////////////////////////////////////////////
// K1394IsochTransfer::StopComplete
//
// This is an internal routine called when the transfer has been
//  started or stopped.
//
// Parameters
//
//	DeviceObject - A pointer to our system device object
//
//  Irp - A pointer to the IRP just completed
//
//  Context - A context we specified.  In this case, it is of type
//      BUFFER_COMPLETE_CONTEXT
//
// Notes
//

NTSTATUS K1394IsochTransfer::StartStopComplete(
										  PDEVICE_OBJECT		DeviceObject,
										  PIRP					Irp,
										  PVOID					Context
										  )
{
	KIrp I(Irp);
	PSTART_STOP_CONTEXT theContext = (PSTART_STOP_CONTEXT)Context;

	if ( theContext->Irb.FunctionNumber == REQUEST_ISOCH_STOP )
	{
		if ( NT_SUCCESS(I.Status()) )
			theContext->pThis->m_IsStarted = FALSE;

		theContext->pThis->OnStopComplete(I.Status());
	}
	else
	{
		if( NT_SUCCESS(I.Status()) )
			theContext->pThis->m_IsStarted = TRUE;

		theContext->pThis->OnStartComplete(I.Status());
	}

	delete theContext;

	return I.Status();
	UNREFERENCED_PARAMETER(DeviceObject);
}

/////////////////////////////////////////////////////////////////////////////////////////
//K1394IsochTransfer::BufferComplete
//
// This is an internal routine called when the last buffer in the list attached is
//  completed by the bus driver
//
//Parameters:
//	Context - the context parameter passed in the buffer descriptor
//
// Returns:
//	None
//
// Comments:
//	This routine has the resonsibility of removing the completed buffers from the transfer
//   in the normal case.
//  The bus driver does not call this routine if it does not receive exactly the number of
//  bytes to fill the buffer.
//

VOID K1394IsochTransfer::BufferComplete(PVOID Context)
{
	PBUFFER_CONTEXT pContext = (PBUFFER_CONTEXT)Context;
    KIRQL           oldIrql;

	if( m_pResource->m_Flags & RESOURCE_BUFFERS_CIRCULAR )
	{
		//notify the user the buffer completed if we're using circular buffers
		OnBufferComplete(STATUS_SUCCESS, pContext->pBufferList);
	}
	else
	{
		//If we aren't using circular buffers remove the finished buffer list
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		if( m_pResource->m_Handle )
		{
            // Get the buffer list lock
            KeAcquireSpinLock(&m_BufferListLock, &oldIrql);

            // Remove the buffer from the list
	        pContext = pContext->pThis->m_BufferList.Remove(pContext);

            // Release the buffer list lock
            KeReleaseSpinLock(&m_BufferListLock, oldIrql);

            if (pContext != NULL)
            {
			    KIrb<REQUEST_ISOCH_DETACH_BUFFERS>* pIrb =
				    (KIrb<REQUEST_ISOCH_DETACH_BUFFERS>*)m_IrbPool.alloc();

			    if( pIrb )
			    {
				    pIrb->Initialize(m_pResource->m_Handle,
								     (ULONG)pContext->pBufferList->Count(),
								     pContext->pBufferList);


				    pContext->pIrb = pIrb;

				    status = m_pLowerDevice->SubmitIrb(pIrb, RemoveBufferComplete, pContext);
			    }
			    else
                {
			        OnBufferComplete(STATUS_SUCCESS, pContext->pBufferList);
                }
            }
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
// K1394IsochClient::AddBufferComplete
//
//	AddBufferComplete is an internal routine called when some buffers
//   have been added to the transfer.
//
// Parameters
//	 DeviceObject - Our system device object
//	 Irp		  - The IRP being completed
//	 Context	  - In this case, a BUFFER_DESCRIPTOR structure
//
// Notes
//	 AddBufferComplete does nothing unless the call to add the buffers
//	  fails.  In that case, it calls OnBufferComplete and cleans up.
//

NTSTATUS K1394IsochTransfer::AddBufferComplete(
		PDEVICE_OBJECT		DeviceObject,
		PIRP				Irp,
		PVOID				Context
		)
{
	PADD_BUFFER_CONTEXT pContext = (PADD_BUFFER_CONTEXT)Context;
    PBUFFER_CONTEXT     pBufferContext = pContext->pContext;
	KIrp                I(Irp);
	NTSTATUS            status = I.Status();
    KIRQL               oldIrql;

	pContext->pThis->m_IrbPool.free(pContext->pIrb);

	if( !NT_SUCCESS(status) )
	{
        // Get the buffer list lock
        KeAcquireSpinLock(&pContext->pThis->m_BufferListLock, &oldIrql);

        // remove the buffer from the list
        pBufferContext = pContext->pThis->m_BufferList.Remove(pBufferContext);

        // Release the buffer list lock
        KeReleaseSpinLock(&pContext->pThis->m_BufferListLock, oldIrql);

        if (pBufferContext != NULL)
        {
            pContext->pThis->OnBufferComplete(status, pBufferContext->pBufferList);

            delete pBufferContext;
        }
	}

    delete pContext;

	return status;
	UNREFERENCED_PARAMETER(DeviceObject);
}

#endif //WDM
