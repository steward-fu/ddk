/////////////////////////////////////////////////////////////////////
// NmPacketBinding.cpp
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================
//
// Packet NDIS binding class implementation
//

#include <vdw.h>
#include "NmPacketBinding.h"
#include "NmPacketDevice.h"
#include "NmPacket.h"
#include "NmPacketioctl.h"

extern	KTrace	t;			// Global driver trace object	

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
NmPacketBinding::NmPacketBinding() :
	KNdisProtocolBinding(),
		m_pDevice(NULL)
{
}


///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	none
NmPacketBinding::~NmPacketBinding()
{
	delete m_pDevice;
}

///////////////////////////////////////////////////////////////////////////////
// NmPacketBinding::Open
//
// The first method called on a newly created binding object.
// The framework calls Open() when a new miniport adapter (NIC) has just 
// registered itself. The handler should inspect configuration, initialize 
// internal objects, fill in medium types we honor and return 
// NDIS_STATUS_SUCCESS if the new binding  to the specified adapter should
// be granted. An error return prevents binding, and the binding object 
// gets immediately destroyed.
//
// Parameters:
//		DeviceName	Name of the NIC device being bound
//		Config		Protocol Configuration object (Registry)
//		Medium		Empty Medium object. The handler must fill in 
//					the medium type(s) supported.
//		AddressInfo	Optional char string the client can pass to talk to the device.
//
// Return:
//		NDIS_STATUS_SUCCESS		The framework should proceed with binding creation	
//		NDIS_STATUS_XXXXXXX		Initialization error, do not bind.
//
NDIS_STATUS NmPacketBinding::Open(IN PNDIS_STRING DeviceName,
				 IN KNdisConfig& Config, 
				 OUT KNdisMedium& Medium, 
				 OUT PSTRING* /*AddressInfo*/)
{
	TRACE("NmPacketBinding::Open() %ws\n", DeviceName->Buffer);
	NDIS_STATUS	status=NDIS_STATUS_FAILURE;

	do {
		// Construct the device name for the binding. It is a
		// "Packet_" appended with the DeviceName GUID, as per the DDK sample.
		// NOTE: do not include \\Device\\; DW does it for you.
		
		KUstring BindDevName(DeviceName->Length + sizeof(L"Packet_"), NonPagedPool);
		if (!BindDevName.IsValid()) {
			ASSERT(!"Construct BindDevName"); 
			status = NDIS_STATUS_RESOURCES;
			break;
		}
		// Use \Device\Packet_GUID as a name
		BindDevName.Assign(L"Packet_");
		BindDevName.Append(&DeviceName->Buffer[8]); // skip '\Device\'

		// Create the device representing this binding:
		
		m_pDevice = new (
			BindDevName.UnicodeString().Buffer,
			FILE_DEVICE_PROTOCOL,
			BindDevName.UnicodeString().Buffer, 
			0,
			DO_DIRECT_IO | DO_EXCLUSIVE
			)
			NmPacketDevice(this); 
		
		if (m_pDevice == NULL || !NT_SUCCESS(status = m_pDevice->ConstructorStatus()) )
		{
			ASSERT(!"Create Device");
			break;
		}

		// Cache the symbolic name. The whole reason for this is that
		// DriverWorks uses L"\\??\\" as symlink prefix, while the DDK's TestApp
		// expects L"\\DosDevices\\" when enumerating adapters...

		static WCHAR Prefix[] = L"\\DosDevices\\";
		m_SymLinkForTestApp.Initialize
			(sizeof(Prefix)+BindDevName.UnicodeString().Length, NonPagedPool);
		m_SymLinkForTestApp.Assign(Prefix);
		m_SymLinkForTestApp.Append(BindDevName);
		
	} while (0);

	// Check if everything was OK. In the case of a failure you can 
	// postpone releasing the allocated resourses to the destructor. 
	// The framework will destroy the object immediately after 
	// any unsuccessful return from this method.

	if (NT_SUCCESS(status)) {
		
		// TODO: Customize the mediums (from the Registry?)
		static NDIS_MEDIUM SupportedMedium[] = { NdisMedium802_3, NdisMedium802_5, NdisMediumFddi };
		
		// Set medium types in the output object:
		
		Medium = SupportedMedium;
		
		// Finally, return to the framework. If the status is success, the framework
		// will attempt to open the underlying NIC adapter. When open adapter completes
		// (it could be asynchronous), the framework calls OpenComplete() below, where
		// we proceed with the initialization.
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////
// NmPacketBinding::OpenComplete
//
// The framework signals the binding to the adapter has been completed.
//
// Parameters:
//		none
// Return:
//		none
// Comments:
//	 The result of operation is passed in data member m_CompleteStatus, which
//	 the driver must check. If m_CompleteStatus equals anything but NDIS_STATUS_SUCCESS
//	 the framework destroys the binding object immediately after return from OpenComplete().
//
void NmPacketBinding::OpenComplete()
{
	TRACE("NmPacketBinding::OpenComplete() %s for [%ws]\n",
		(m_CompleteStatus) ? "FAILED":"OK", 
		(m_CompleteStatus) ?  L"?" : KNdisProtocolBinding::GetAdapterName());  

	if (m_CompleteStatus != NDIS_STATUS_SUCCESS) 
		return;

	// Adapter is bound! Set the interlocked flag, which is used
	// against possible race conditions with i/o requests during
	// unbinding
	m_bBound = 1;

	// Maintain the list of adapters:
	m_BindingList.InsertTail(this);
	
	// For drivers that construct device objects after DriverEntry time, it is necessary
	// for the driver to clear the 'initializing' flag in the system device object:
		
	((PDEVICE_OBJECT) * m_pDevice)->Flags &= ~DO_DEVICE_INITIALIZING;	
}

///////////////////////////////////////////////////////////////////////////////
// NmPacketBinding::Close
//
// The binding goes away. The last call before the object is destroyed
// by the framework. Do your deinitialization here or in the destructor
//
// Parameters:
//		none
// Return:
//		TRUE to allow the destruction of the binding object by the framework.
//		Return FALSE to postpone the deletion; in which case the client must
//		take care of deleting the object itself. 
//
BOOLEAN NmPacketBinding::Close()
{
	// Remove this instance from the list of adapters:
	m_BindingList.Remove(this);

	// Cancel pending reads and wait until the device is clean and destroy it
	if (m_pDevice) {
		m_pDevice->CancelReads();
		m_pDevice->WaitForIrpsToComplete();
	}

	// Return TRUE to tell the framework to destroy this object
	return TRUE; 
}

///////////////////////////////////////////////////////////////////////
// NIC Miniport indicates full packet
//
// Parameters:
// 	Packet               - packet being indicated
// Returns:
// 	none
// Comments:
// 	Returns 0 always
INT NmPacketBinding::Receive(const IN PNDIS_PACKET Packet)
{
	ULONG	 BytesTransfered=0;
	NTSTATUS Status=STATUS_SUCCESS;

	KNdisPacket SrcPacket(Packet);	// declare for convenience

	// Check if there are pending receive-s

	KNdisPacket DstPacket = m_pDevice->m_RcvList.RemoveHead();
	if (!DstPacket.IsValid()) {
		t << "No reads pending; dropping the packet\n";
		return 0;
	}

	// Get the irp associated with the descriptor. It cannot be canceled
	// since it was still in the list

	NmPacketDevice::PacketContext* pCxt = NmPacketDevice::PacketList::GetContext(DstPacket);
	ASSERT(pCxt);

	KIrp Irp = pCxt->Irp;
	ASSERT(!Irp.IsNull());
	Irp.SetCancelRoutine(NULL);

	// Lock packet buffers before calling NdisCopyFromPacketToPacket (which
	// does not according to the DDK's comment). If we fail, complete
	// the irp anyway. This would be an indication of low memory conditions.
	// NOTE: This is not required in 5.1 since Copy() uses safe functions

	if (SrcPacket.LockBuffers()) {

	    //  Attach the actual MDL to the packet and copy into it from the source

		DstPacket.ChainAtFront(Irp.Mdl());
		BytesTransfered = DstPacket.CopyFrom(SrcPacket, 0, Irp.ReadSize(), 0);		
	}
	else
		Status = STATUS_INSUFFICIENT_RESOURCES;	// low memory conditions

	// Complete IRP

	Irp.Information() = BytesTransfered;

	Irp.Complete(Status);

	m_pDevice->m_PacketPool.Free(DstPacket);

	m_pDevice->m_PendingIrps.Decrement();

	// We're not holding on the packet, so return 0
	return 0;
}


///////////////////////////////////////////////////////////////////////
// NIC Miniport indicates header+lookahead is being indicated. 
//
// Parameters:
// 	PacketToAccept       - Cached parameters for NdisTransferData (IN, OUT)
// 	HeaderBuffer         - header
// 	HeaderBufferSize     - its size
// 	LookAheadBuffer      - buffer
// 	LookaheadBufferSize  - its size
// 	PacketSize           - total size
// Returns:
// 	NDIS_STATUS_SUCCESS to initiate transfer of data. No transfer otherwize
// Comments:
// 	The routine should set PacketToAccept with a packet descriptor to
//  transfer data into. This causes the framework to start data transfer.
//  When the transfer is done the framework calls TransferDataComplete().
//
NDIS_STATUS NmPacketBinding::Receive(IN OUT KNdisPartialPacket& PacketToAccept,
						IN  PVOID HeaderBuffer,
						IN  UINT HeaderBufferSize,
						IN	PVOID LookAheadBuffer,
						IN	UINT LookaheadBufferSize,
						IN	UINT PacketSize)
{
	// Ignore runt packets

    if (HeaderBufferSize > ETHERNET_HEADER_SIZE)
        return NDIS_STATUS_SUCCESS;

	// Check if there are pending receive-s

	KNdisPacket DstPacket = m_pDevice->m_RcvList.RemoveHead();
	if (!DstPacket.IsValid()) {
		t << "No reads pending; dropping the packet\n";
		return NDIS_STATUS_NOT_ACCEPTED;
	}

	// Get the irp associated with the descriptor. It cannot be canceled
	// since it was still in the list

	NmPacketDevice::PacketContext* pCxt = NmPacketDevice::PacketList::GetContext(DstPacket);
	ASSERT(pCxt);
	
	KIrp Irp = pCxt->Irp;
	ASSERT(!Irp.IsNull());
	Irp.SetCancelRoutine(NULL);

	// Declare memory object for IRP's buffer for convenience

	KMemory M(Irp.Mdl());

	// Copy the ethernet header first

	NdisMoveMappedMemory((PUCHAR) M.MapToSystemSpace(), (PUCHAR) HeaderBuffer, HeaderBufferSize);

	// Create MDL for the rest of the buffer. @TODO: optimize DW's ctor-s to use IoAllocateMdl

	PMDL pMdl = IoAllocateMdl (M.VirtualAddress(), M.Size(), FALSE, FALSE, NULL);
	if (!pMdl) {
		TransferDataComplete(DstPacket, NDIS_STATUS_RESOURCES, 0); // simulate completion anyway
		return NDIS_STATUS_RESOURCES;
	}


    //  Build the mdl to point to the the portion of the buffer following the header
    
    IoBuildPartialMdl(M, pMdl, PUCHAR(M.VirtualAddress())+ETHERNET_HEADER_SIZE, 0);

    //  Clear the next link in the new MDL

    pMdl->Next=NULL;

	// Store the MDL pointer in the packet context

    pCxt->Data.pMdl=pMdl;

    //  Attach our partial MDL to the packet

	DstPacket.ChainAtFront(pMdl);

	// Set the partial packet for transfer. The framework will automatically
	// issue NdisTransferData() with parameters cached in PacketToAccept.

	ULONG BytesToTransfer = min (PacketSize, Irp.ReadSize() - ETHERNET_HEADER_SIZE);

	PacketToAccept.Set(DstPacket, 0, BytesToTransfer);

	return NDIS_STATUS_SUCCESS;
}

			
///////////////////////////////////////////////////////////////////////
// Partial Receive Data Transfer Complete
//
// Parameters:
// 	Packet               - packet used for transfer into
// 	Status               - result of transfer
// 	BytesTransferred     - bytes moved
// Returns:
// 	none
// Comments:
// 	Free resources allocated for transfer and complete irp
void NmPacketBinding::TransferDataComplete(PNDIS_PACKET Packet,
								NDIS_STATUS Status,
								UINT BytesTransfered)
{
	// Get the irp associated with the descriptor. It cannot be have canceled
	// since it was still in the list

	NmPacketDevice::PacketContext* pCxt = NmPacketDevice::PacketList::GetContext(Packet);

	KIrp Irp = pCxt->Irp;
    PMDL pMdl= pCxt->Data.pMdl;

    //  Free the partial MDL that we allocated

    if(pMdl)
        IoFreeMdl(pMdl);

    //  Put the packet back in the pool

    m_pDevice->m_PacketPool.Free(Packet);

	// Complete the Irp

	Irp.Information() = (Status == NDIS_STATUS_SUCCESS) ?
		BytesTransfered+ETHERNET_HEADER_SIZE : 0;

	Irp.Complete(Status);

	m_pDevice->m_PendingIrps.Decrement();

}


///////////////////////////////////////////////////////////////////////
// Send Complete 
//
// Parameters:
// 	Packet               - Packet submitted for transmission
// 	Status               - Result of transmission
// Returns:
// 	none
// Comments:
// 	This method is called for each packet transmitted with a call to 
//  NdisSend that returned NDIS_STATUS_PENDING. 
//  If an array of packets is sent, it is called once for each packet
//  passed to NdisSendPackets, whether or not it returned pending.
void NmPacketBinding::SendComplete(PNDIS_PACKET Packet, NDIS_STATUS Status)
{
	if (Status != NDIS_STATUS_SUCCESS) {
		t << "NmPacketBinding::SendComplete Status " << ULONG(Status) << EOL;
	}

	// Restore irp associated with the packet

	KIrp Irp = NmPacketDevice::PacketList::GetContext(Packet)->Irp;

#ifdef NDIS51
	// Remove the irp from the waiting list. It's better be there!
	m_pDevice->m_SendList.Remove(Irp);

    //  Put the packet back on the free list - unless the cancel routine still holds it
	if (NdisInterlockedDecrement(
			&NmPacketDevice::PacketList::GetContext(Packet)->Data.RefCount)==0)
#endif

    //  Put the packet back on the free list

    m_pDevice->m_PacketPool.Free(Packet);

	// Complete request

	Irp.SetCancelRoutine(NULL);
	Irp.Information() = (Status == NDIS_STATUS_SUCCESS) ?
		Irp.WriteSize() : 0;

	Irp.Complete(Status);

	m_pDevice->m_PendingIrps.Decrement();

}


///////////////////////////////////////////////////////////////////////
// Reset Request Completed
//
// Parameters:
// 	Status               - status of completion
// Returns:
// 	none
// Comments:
// 	The framework calls this method when NdisReset has completed.
//  NDIS guarantees serialization of resets, so we simply complete
//  the first irp found in the receive list.
void NmPacketBinding::ResetComplete(NDIS_STATUS Status) 
{
	KIrp Irp = m_pDevice->m_ResetList.RemoveHead();
	if (!Irp.IsNull())
	{
		Irp.Complete(Status);
		m_pDevice->m_PendingIrps.Decrement();
	}
}


///////////////////////////////////////////////////////////////////////
// Ndis Request Complete
//
// Parameters:
// 	NdisRequest          - pointer to the submitted request structure
// 	Status               - result of the request
// Returns:
// 	none
// Comments:
// 	Set/Query OID has completed. Complete the associated irp now
//
void NmPacketBinding::RequestComplete(PNDIS_REQUEST NdisRequest, NDIS_STATUS Status)
{
	// restore the original object
	
	KNdisRequestWithContext<PIRP>* req = 
		reinterpret_cast<KNdisRequestWithContext<PIRP>*> (NdisRequest);
	KIrp I = req->Context;

	// Data moved is always the entire buffer. The actual OID data length is
	// passed via PPACKET_OID_DATA structure
	
	I.Information() = I.IoctlInputBufferSize();
	PPACKET_OID_DATA OidData = (PPACKET_OID_DATA) I.IoctlBuffer();

	OidData->Length = (I.IoctlCode() == IOCTL_PROTOCOL_SET_OID) ?
		req->BytesSet() : req->BytesQueried();

	delete req;

	I.Complete(Status);

	m_pDevice->m_PendingIrps.Decrement();
}


// Receive indication burst has completed
void NmPacketBinding::ReceiveComplete()
{
}


// General Status indication:
void NmPacketBinding::StatusIndication(IN NDIS_STATUS GeneralStatus,
						  IN PVOID StatusBuffer,
						  IN UINT StatusBufferSize)
{
}


// The underlying miniport's call to NdisMIndicateStatusComplete causes this call
void NmPacketBinding::StatusComplete()
{
}

// Global list of all available bindings
KNdisInterlockedList<NmPacketBinding> NmPacketBinding::m_BindingList;

