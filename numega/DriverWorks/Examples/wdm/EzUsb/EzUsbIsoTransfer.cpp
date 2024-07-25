// EzUsbIsoTransfer.cpp 
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

#include <vdw.h>
#include <kusb.h>
#include "EzUsbIsoTransfer.h"

// Global driver trace object
extern KDebugOnlyTrace t;				

//=============================================================================
// class EzUsbIsoTransferBase implementation
//

EzUsbIsoTransferBase::EzUsbIsoTransferBase(
	KUsbPipe& pipe,
	USHORT PacketSize,
	UCHAR PacketsPerUrb,
	UCHAR NumUrbs
	) : m_Pipe(pipe)
{
	m_ConstructorStatus = STATUS_SUCCESS;

	m_ppMdl = NULL;
	m_bRead = m_Pipe.EndpointAddress() >= 0x80;
	m_PacketsPerUrb = PacketsPerUrb;
	m_PacketSize = PacketSize ? PacketSize : m_Pipe.MaximumPacketSize();
	ASSERT( m_PacketSize );
	m_NumUrbs = NumUrbs;
	m_dwTotalPacketsProcessed = 0;
	m_dwTotalBytesProcessed = 0;
	m_dwErrorPacketCount = 0;

	if( m_PacketsPerUrb )
	{
		m_TransferSize = m_PacketSize * m_PacketsPerUrb;
		ASSERT( m_TransferSize <= m_Pipe.MaximumTransferSize() );
	}
	else
	{
		m_TransferSize = m_Pipe.MaximumTransferSize();
		m_PacketsPerUrb = static_cast<UCHAR>(m_TransferSize/m_PacketSize);
	}

// Allocate internal data structures
	//Allocate URBs contiguously from NonPagedPool
	m_UrbSize = GET_ISO_URB_SIZE(m_PacketsPerUrb);
	m_pUrb = new (NonPagedPool) UCHAR[ m_UrbSize * m_NumUrbs ];
	if( NULL == m_pUrb )
	{
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

	RtlZeroMemory(m_pUrb, m_UrbSize * m_NumUrbs);
	
	//Create MDLs for each URB.
	//Allocate memory to store an array of PMDL's
	m_ppMdl = new PMDL[m_NumUrbs];

	if( NULL == m_ppMdl )
	{
		delete m_pUrb;
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

	RtlZeroMemory(m_ppMdl, sizeof(PMDL) * m_NumUrbs);

	return;
}


EzUsbIsoTransferBase::~EzUsbIsoTransferBase(void)
{
	delete m_ppMdl;
	delete m_pUrb;
}


//=============================================================================
// class EzUsbIsoTransfer implementation
//

EzUsbIsoTransfer::EzUsbIsoTransfer(
	KMemory& Mdl,
	ULONG Length,		//Size of Data Buffer in bytes
	KUsbPipe& pipe,
	USHORT PacketSize,
	UCHAR PacketsPerUrb,
	PVOID pDescBuffer,	//Optional pointer to packet buffer
	UCHAR NumUrbs
	)
	: EzUsbIsoTransferBase( pipe, PacketSize, PacketsPerUrb, NumUrbs)
{
	if( NT_SUCCESS(m_ConstructorStatus) )
	{

	// Initialize data members
		m_Mem = Mdl;
		m_Length = Length;
		m_pDescriptorBuffer = pDescBuffer;
		m_dwDescrOffset = 0;
		m_dwOffset = 0;

		//This is ok since we don't really use this virtual address 
		m_VirtualAddress = m_Mem.VirtualAddress();

		for( UCHAR i = 0; i < m_NumUrbs; i++ )
		{
			//Allocate the Mdl and store it in the array of ptrs to Mdl's.  Since the buffer may not be page aligned,
			//the size of the IRP adds a PAGE_SIZE so that the Mdl will be large enough to describe a buffer
			//that spans more than one physical page.
			reinterpret_cast<MDL**>(m_ppMdl)[i] = IoAllocateMdl(NULL, m_TransferSize + PAGE_SIZE, FALSE, FALSE, NULL);
			if( NULL == m_ppMdl[i] )
			{
				m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
				break;
			}

			//Map part of the client's MDL buffer into this MDL
			IoBuildPartialMdl( m_Mem, m_ppMdl[i], reinterpret_cast<PVOID>(ULONG_PTR(m_VirtualAddress) + m_dwOffset), m_TransferSize );
			m_dwOffset += m_TransferSize;

			KMemory M(m_ppMdl[i]);

			//Initialize the URBs
			m_Pipe.BuildIsochronousTransfer(
					m_PacketsPerUrb,
					m_PacketSize,
					m_bRead,
					TRUE, 
					0, 
					M,
					m_TransferSize,
					reinterpret_cast<PURB>(reinterpret_cast<ULONG_PTR>(m_pUrb) + m_UrbSize * i)
					);
		}

		// Check for allocation failure
		if( STATUS_INSUFFICIENT_RESOURCES == m_ConstructorStatus )
		{
			// An allocation error occurred, Free all MDL's allocated.
			for( UCHAR j = 0; j < i; j++ )
			{
				IoFreeMdl(m_ppMdl[j]);
				m_ppMdl[j] = NULL;
			}
		}

		stream.Initialize(this, m_Pipe.Interface()->UsbLowerDevice());
	}
}


EzUsbIsoTransfer::~EzUsbIsoTransfer(void)
{
	// Free all MDL's allocated.
	for( UCHAR j = 0; j < m_NumUrbs; j++ )
	{
		if( NULL != m_ppMdl[j] )
		{
			IoFreeMdl(m_ppMdl[j]);
			m_ppMdl[j] = NULL;
		}
	}
}


USB_STREAM_STATUS EzUsbIsoTransfer::OnCompletedUrb(PURB pUrb, NTSTATUS status)
{
	USB_STREAM_STATUS streamStatus = USB_STREAM_STATUS_STOP;

// Error checking
	//check the Irp status
	if( !NT_SUCCESS(status) || !pUrb )
	{
		ASSERT(pUrb);
		//handle error condition for this device
		t << "Irp Error " << status <<"\n";
		//this prevents re-sending the URB to the bus driver
		return streamStatus;
	}

//++ Debugging instrumentation
	t << (m_bRead ? "Frame[" : "                Frame[")
	  << ULONG_PTR(pUrb)
	  << "-"
	  << (m_bRead ? " IN" : "OUT")
	  << "] = " 
	  << ULONG(pUrb->UrbIsochronousTransfer.StartFrame) 
	  << "\n";
//-- Debugging instrumentation

	//check the URB header status
	USBD_STATUS usbdStatus = URB_STATUS( pUrb );

	if( USBD_SUCCESS(usbdStatus) )
	{
		if( USBD_STATUS_SUCCESS != usbdStatus )
		{
			t << "usbdStatus = " << usbdStatus << "\n";
		}
	}
	else
	{
		t << "usbdStatus = " << usbdStatus << "\n";
	}

	ASSERT( pUrb->UrbIsochronousTransfer.NumberOfPackets ) ;

	for( ULONG i = 0; i < pUrb->UrbIsochronousTransfer.NumberOfPackets; i++ )
	{
		m_dwTotalPacketsProcessed++;

		if( USBD_SUCCESS( ISO_PKT_STATUS(pUrb, i) ) ) 
		{
			m_dwTotalBytesProcessed += (ISO_PKT_LENGTH(pUrb, i));

			if( USBD_STATUS_SUCCESS != ( ISO_PKT_STATUS(pUrb, i) ) )
			{
				t << "usbd Iso Packet Status = " << ( ISO_PKT_STATUS(pUrb, i) ) << "\n"; 
			}
		}
		else 
		{
			t << "usbd Iso Packet Status = " << ( ISO_PKT_STATUS(pUrb, i) ) << "\n"; 
			m_dwErrorPacketCount++;
		}

		// Save the descriptors (if necessary)
		if( m_pDescriptorBuffer )
		{
			RtlCopyMemory( 
				reinterpret_cast<PUCHAR>(m_pDescriptorBuffer) + m_dwDescrOffset, 
				&pUrb->UrbIsochronousTransfer.IsoPacket[i], 
				sizeof(USBD_ISO_PACKET_DESCRIPTOR) 
				);

			m_dwDescrOffset += sizeof(USBD_ISO_PACKET_DESCRIPTOR);
		}
	}

	//Determine if there is more data to transfer
	if( m_dwOffset + m_TransferSize > m_Length)
	{
		return streamStatus;
	}

	streamStatus = USB_STREAM_STATUS_CONTINUE;

	RtlZeroMemory(&pUrb->UrbIsochronousTransfer.hca, sizeof(_URB_HCD_AREA));

	pUrb->UrbIsochronousTransfer.Hdr.UsbdDeviceHandle	= NULL;
	pUrb->UrbIsochronousTransfer.Hdr.UsbdFlags			= 0;
	pUrb->UrbIsochronousTransfer.TransferBuffer			= NULL;
	pUrb->UrbIsochronousTransfer.TransferBufferLength	= m_TransferSize;
	pUrb->UrbIsochronousTransfer.StartFrame				= 0;

	IoBuildPartialMdl( 
		m_Mem, 
		pUrb->UrbIsochronousTransfer.TransferBufferMDL, 
		reinterpret_cast<PVOID>(ULONG_PTR(m_VirtualAddress) + m_dwOffset), 
		m_TransferSize 
		);

	m_dwOffset += m_TransferSize;

	return streamStatus;
}

//=============================================================================
// class EzUsbDriverManagedIsoTransfer implementation
//

EzUsbDriverManagedIsoTransfer::EzUsbDriverManagedIsoTransfer(
	KUsbPipe& pipe,
	ULONG Length,		//Size of Data Buffer in bytes
	USHORT PacketSize,
	UCHAR PacketsPerUrb,
	UCHAR NumUrbs,
	ULONG NumPackets,	//Size of Packet Buffer
	PVOID pBuffer		//Optional pointer to a buffer
	) 
	: EzUsbIsoTransferBase( pipe, PacketSize, PacketsPerUrb, NumUrbs)
{
	if( NT_SUCCESS(m_ConstructorStatus) )
	{

		//Allocate NonPagedPool buffer for USBD_ISO_PACKET_DESCRIPTOR's
		if(NumPackets)
		{
			m_pDescriptorBuffer = new KLockableFifo<USBD_ISO_PACKET_DESCRIPTOR>(NumPackets, NonPagedPool);
			if( NULL == m_pDescriptorBuffer )
			{
				m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;			
				return;
			}
		}
		else
		{
			m_pDescriptorBuffer = NULL;
		}
		
		//Allocate NonPagedPool buffer for URB TransferBuffer
		m_TransferBuffers = new (NonPagedPool) UCHAR[m_TransferSize * m_NumUrbs];
		if( NULL == m_TransferBuffers )
		{
			delete m_pDescriptorBuffer;
			m_pDescriptorBuffer = NULL;
			m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;			
			return;
		}

		RtlZeroMemory(m_TransferBuffers, m_TransferSize * m_NumUrbs);

		for( UCHAR i = 0; i < m_NumUrbs; i++ )
		{
			//Allocate the Mdl and store it in the array of ptrs to Mdl's.  
			reinterpret_cast<MDL**>(m_ppMdl)[i] = IoAllocateMdl(
				reinterpret_cast<PUCHAR>(m_TransferBuffers) + m_TransferSize * i, m_TransferSize, FALSE, FALSE, NULL);
			if( NULL == m_ppMdl[i] )
			{
				m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
				break;
			}

			KMemory M(m_ppMdl[i]);
			M.SetPageArray();

			//Initialize the URBs
			m_Pipe.BuildIsochronousTransfer(
					m_PacketsPerUrb,
					m_PacketSize,
					m_bRead,
					TRUE, 
					0, 
					M,
					m_TransferSize,
					reinterpret_cast<PURB>(reinterpret_cast<ULONG_PTR>(m_pUrb) + m_UrbSize * i)
					);
		}

		// Check for allocation failure
		if( STATUS_INSUFFICIENT_RESOURCES == m_ConstructorStatus )
		{
			// An allocation error occurred, Free all MDL's allocated.
			for( UCHAR j = 0; j < i; j++ )
			{
				IoFreeMdl(m_ppMdl[j]);
				m_ppMdl[j] = NULL;
			}

			delete m_pDescriptorBuffer;
			m_pDescriptorBuffer = NULL;
			delete m_TransferBuffers;
			m_TransferBuffers = NULL;
			return;
		}

		stream.Initialize(this, m_Pipe.Interface()->UsbLowerDevice());

		if( pBuffer )
		{
			KFifo<UCHAR>::Initialize(Length+1, pBuffer);
		}
		else
		{
			KFifo<UCHAR>::Initialize(Length, NonPagedPool);
		}
	}
}


EzUsbDriverManagedIsoTransfer::~EzUsbDriverManagedIsoTransfer(void)
{
	// Free all MDL's allocated.
	for( UCHAR j = 0; j < m_NumUrbs; j++ )
	{
		if( NULL != m_ppMdl[j] )
		{
			IoFreeMdl(m_ppMdl[j]);
			m_ppMdl[j] = NULL;
		}
	}

	delete m_TransferBuffers;
	delete m_pDescriptorBuffer;
}


USB_STREAM_STATUS EzUsbDriverManagedIsoTransfer::OnCompletedUrb(PURB pUrb, NTSTATUS status)
{
	ASSERT(pUrb);

	USB_STREAM_STATUS streamStatus = USB_STREAM_STATUS_STOP;

// Error checking
	//check the Irp status
	if( !NT_SUCCESS(status) )
	{
		//handle error condition for this device
		
		//this prevents re-sending the URB to the bus driver
		return streamStatus;
	}

	//check the URB header status
	USBD_STATUS usbdStatus = URB_STATUS( pUrb );

	if( USBD_SUCCESS(usbdStatus) )
	{
		if( USBD_STATUS_SUCCESS != usbdStatus )
		{
			t << "usbdStatus = " << usbdStatus << "\n";
		}
	}
	else
	{
		t << "usbdStatus = " << usbdStatus << "\n";
	}

	ASSERT( pUrb->UrbIsochronousTransfer.NumberOfPackets ) ;

	for( ULONG i = 0; i < pUrb->UrbIsochronousTransfer.NumberOfPackets; i++ )
	{
		m_dwTotalPacketsProcessed++;

		if( USBD_SUCCESS( ISO_PKT_STATUS(pUrb, i) ) ) 
		{
			m_dwTotalBytesProcessed += (ISO_PKT_LENGTH(pUrb, i));

			if( USBD_STATUS_SUCCESS != ( ISO_PKT_STATUS(pUrb, i) ) )
			{
				t << "usbd Iso Packet Status = " << ( ISO_PKT_STATUS(pUrb, i) ) << "\n"; 
			}
		}
		else 
		{
			t << "usbd Iso Packet Status = " << ( ISO_PKT_STATUS(pUrb, i) ) << "\n"; 
			m_dwErrorPacketCount++;
		}
	}
	
// This is a Read operation on the device (an IN isochronous transfer)
		
	//store the data from the device (described by the URB) in our buffer
	ULONG NumWrote = WriteBuffer(pUrb);

	//determine if we successfully wrote the data to the buffer
	if( 0 == NumWrote )
	{
		t << "USB_STREAM_STATUS_STOP\n";

		streamStatus = USB_STREAM_STATUS_STOP;
	}
	else
	{
		streamStatus = USB_STREAM_STATUS_CONTINUE;

	}

	//put descriptors into FIFO
	WriteDescriptorBuffer(
		pUrb->UrbIsochronousTransfer.IsoPacket, 
		pUrb->UrbIsochronousTransfer.NumberOfPackets
		);

	RtlZeroMemory(&pUrb->UrbIsochronousTransfer.hca, sizeof(_URB_HCD_AREA));

	return streamStatus;
}


ULONG EzUsbDriverManagedIsoTransfer::WriteBuffer(PURB pUrb)
{
	ASSERT( pUrb );
	_URB_ISOCH_TRANSFER* pIsoUrb = (_URB_ISOCH_TRANSFER*)pUrb;
	KMemory M(pIsoUrb->TransferBufferMDL);

	PUCHAR p = (PUCHAR)M.MapToSystemSpace();
	ULONG len = pIsoUrb->TransferBufferLength;
	PUCHAR ptemp = NULL;

	Lock();
	ULONG NumAvail = NumberOfItemsAvailableForWrite();
	if ( len > NumAvail )
	{
		// Allocate a temporary holding place for old data
		ptemp = (PUCHAR) new (NonPagedPool) UCHAR[len];
		// Discard oldest data from the FIFO by Reading elements
		Read(ptemp, len);
	}

	ULONG dwNumWrote = Write(p, len);
	Unlock();

	delete ptemp;

	return dwNumWrote;
}


ULONG EzUsbDriverManagedIsoTransfer::ReadBuffer(PVOID ptr, ULONG len)
{
	ULONG dwNumRead = 0;

	Lock();
	dwNumRead = Read(static_cast<PUCHAR>(ptr), len);
	Unlock();

	return dwNumRead;
}


ULONG EzUsbDriverManagedIsoTransfer::ReadDescriptorBuffer(
	PVOID pBuff, 
	ULONG dwNumDescriptorsToRead
	)
{
	if(NULL == m_pDescriptorBuffer)
	{
		return 0;
	}

	ULONG dwNumRead = 0;

	m_pDescriptorBuffer->Lock();
	dwNumRead = m_pDescriptorBuffer->Read(static_cast<PUSBD_ISO_PACKET_DESCRIPTOR>(pBuff), dwNumDescriptorsToRead);
	m_pDescriptorBuffer->Unlock();

	return dwNumRead;
}


ULONG EzUsbDriverManagedIsoTransfer::WriteDescriptorBuffer(
	PUSBD_ISO_PACKET_DESCRIPTOR pFirst, 
	ULONG dwNumToWrite
	)
{
	if(NULL == m_pDescriptorBuffer)
	{
		return 0;
	}

	PUSBD_ISO_PACKET_DESCRIPTOR ptemp = NULL;

	m_pDescriptorBuffer->Lock();
	ULONG dwNumAvailable = m_pDescriptorBuffer->NumberOfItemsAvailableForWrite();

	if ( dwNumToWrite > dwNumAvailable )
	{
		ULONG dwNumToDiscard = dwNumToWrite - dwNumAvailable;

		ptemp = (PUSBD_ISO_PACKET_DESCRIPTOR) new (NonPagedPool) 
						USBD_ISO_PACKET_DESCRIPTOR[dwNumToDiscard];

		ASSERT( NULL != ptemp );
		if(NULL == ptemp)
		{
			m_pDescriptorBuffer->Unlock();
			return 0;
		}

		// Discard oldest data from the FIFO by Reading elements
		m_pDescriptorBuffer->Read(
			ptemp, 
			dwNumToDiscard
			);
	}

	ULONG dwNumWrote = m_pDescriptorBuffer->Write(
						pFirst, 
						dwNumToWrite
						);

	m_pDescriptorBuffer->Unlock();

	delete ptemp;

	return dwNumWrote;
}
