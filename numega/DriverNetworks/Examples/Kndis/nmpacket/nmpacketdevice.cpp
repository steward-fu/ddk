// NmPacketDevice.cpp
// Implementation of NmPacketDevice device class
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
// Generated by DriverWizard version DriverStudio 2.0.0 (Build 4409)
// Requires Compuware's DriverWorks classes
//

#include <vdw.h>

#include "NmPacket.h"
#include "NmPacketDevice.h"
#include "NmPacketioctl.h"
#include "NmPacketBinding.h"

#pragma warning(disable:4065) // Allow switch statement with no cases

extern	KTrace	t;			// Global driver trace object	

////////////////////////////////////////////////////////////////////////
// NmPacketDevice::~NmPacketDevice
//
//	Routine Description:
//		This is the destructor for the NmPacketDevice
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		Disconnect and release resources here.
//
//		Although the use of SAFE_DESTRUCTORS in the class definition cures
//		improper emission of the destructor into the INIT section most of the
//		time, certain rare cases can still cause improper behavior.  To avoid
//		these cases, the destructor	must preceed the INIT section, causing it
//		to be referenced first by the default compiler section.
//

NmPacketDevice::~NmPacketDevice()
{
}


////////////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::NmPacketDevice
//
//	Routine Description:
//		The device constructor is typically responsible for allocating
//		any physical resources that are associated with the device.
//
//	Parameters:
//		Binding		-	NDIS Binding Object, which this device represents
//
//	Return Value:
//		None
//
//	Comments:
//
NmPacketDevice::NmPacketDevice(NmPacketBinding* Binding) :
	KDevice(),
		m_pBinding(Binding)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device NmPacketDevice, err"  << (ULONG) m_ConstructorStatus << EOL;
		return;
	}

	// Allocate an NDIS packet pool to serve the i/o requests:
	m_ConstructorStatus = m_PacketPool.Initialize
		(NumberOfPacketDescriptors, m_RcvList.ReservedSize());
	if (!NT_SUCCESS(m_ConstructorStatus)) {
		ASSERT(!"Alloc Packet Pool");
	}

#if NDIS51
	// Init Cancel ID generator. We'll use it to label packets-on-the-fly in NmPacketDevice::Write()
	m_CancelId.Initialize();
#endif


}

////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::Read
//
//	Routine Description:
//		Handler for IRP_MJ_READ
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This routine handles read requests.

NTSTATUS NmPacketDevice::Read(KIrp I)
{
	t << "Entering read, " << I;

	m_PendingIrps.Increment();

	// Check if the binding is still active
	if(m_pBinding==NULL || (m_pBinding && !m_pBinding->IsBound()))
    {
	    I.Complete(STATUS_DEVICE_NOT_READY);
		m_PendingIrps.Decrement();
		return STATUS_DEVICE_NOT_READY;
    }

	// Make sure the read size is sufficient
	if (I.ReadSize() < ETHERNET_HEADER_SIZE)
	{
		t << "Buffer too small: " << I.ReadSize();
		m_PendingIrps.Decrement();
		return I.Complete(STATUS_BUFFER_TOO_SMALL);
	}

	// Allocate packet descriptor
	KNdisPacket packet = m_PacketPool.Allocate();	
	if (!packet.IsValid()) {
		t << "No free packets\n";
		m_PendingIrps.Decrement();
		return I.Complete(STATUS_INSUFFICIENT_RESOURCES);
	}

	// Initialize packet's context (reserved fields)
	PacketList::GetContext(packet)->Irp = I;
	PacketList::GetContext(packet)->Data.pMdl = I.Mdl();

	// Mark the irp pending, set up cancel routine
	I.MarkPending();
	I.SetCancelRoutine(LinkTo(CancelRead));

	// Queue the packet into the receive list and return
	m_RcvList.InsertTail(packet);
    
	return STATUS_PENDING;
}


////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::Write
//
//	Routine Description:
//		Handler for IRP_MJ_WRITE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This routine passes packet with user's data down the binding

NTSTATUS NmPacketDevice::Write(KIrp I)
{
	t << "Entering write, " << I;

	m_PendingIrps.Increment();

	// Check if the binding is still active
	if(m_pBinding==NULL || (m_pBinding && !m_pBinding->IsBound()))
    {
	    I.Complete(STATUS_UNSUCCESSFUL);
		m_PendingIrps.Decrement();
		return STATUS_UNSUCCESSFUL;
    }

	// Allocate packet descriptor
	KNdisPacket packet = m_PacketPool.Allocate();	
	if (!packet.IsValid()) {
		t << "No free packets\n";
		I.Complete(STATUS_INSUFFICIENT_RESOURCES);
		m_PendingIrps.Decrement();
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Initialize packet's context (reserved fields)
	PacketList::GetContext(packet)->Irp = I;
	PacketList::GetContext(packet)->Data.RefCount=1;

    //  Attach irp's MDL as NDIS_BUFFER
    packet.ChainAtFront(I.Mdl());

	// Mark the irp pending
	I.MarkPending();

#ifdef NDIS51
	// Manage Tx packets for possible cancellation.

	// Label the packet
	packet.CANCEL_ID(m_CancelId.Next());

	// Save packet pointer in the irp. Note we can use DriverContext[4] since
	// the IRPs are not queued at the device
	I.DriverContext() = PNDIS_PACKET(packet);

	// Queue the packet into the receive list and return
	m_SendList.InsertTail(I);

	// Set up cancel routine
	I.SetCancelRoutine(LinkTo(CancelWrite));
#endif // NDIS51

	// Submit the packet to the miniport. When the send is done the
	// framework calls m_pBinding->SendComplete(). Simulate the
	// same if the request gets completed synchronously.
	NDIS_STATUS status = m_pBinding->Send(packet);

    if (status != NDIS_STATUS_PENDING)
		m_pBinding->SendComplete(packet, status);

    return STATUS_PENDING;
}

////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::DeviceControl
//
//	Routine Description:
//		Handler for IRP_MJ_DEVICE_CONTROL
//
//	Parameters:
//		I - Current IRP
// 
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		This routine is the first handler for Device Control requests. 

NTSTATUS NmPacketDevice::DeviceControl(KIrp I) 
{
	NTSTATUS status=STATUS_UNSUCCESSFUL;

	t << "Entering NmPacketDevice::Device Control, " << I;

	// Increment pending count and check if we're still bound
	m_PendingIrps.Increment();

	if (!m_pBinding->IsBound())
	{
		I.Complete(status);
		m_PendingIrps.Decrement();
		return status;
	}

	switch (I.IoctlCode())
	{
		case IOCTL_PROTOCOL_SET_OID:
		case IOCTL_PROTOCOL_QUERY_OID:
		    status = IOCTL_PROTOCOL_SETQUERY_OID_Handler(I);
			break;

		case IOCTL_PROTOCOL_RESET:
		    status = IOCTL_PROTOCOL_RESET_Handler(I);
			break;

		default:
			// Unrecognized IOCTL request
			status = STATUS_INVALID_PARAMETER;
			break;
	}

	if (status != STATUS_PENDING)
		m_PendingIrps.Decrement();
	
	return status;
}

////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::IOCTL_PROTOCOL_SETQUERY_OID_Handler
//
//	Routine Description:
//		Handler for IO Control Code IOCTL_PROTOCOL_SET_OID
//
//	Parameters:
//		I		- IRP containing IOCTL request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		This routine implements both IOCTL_PROTOCOL_SET_OID and IOCTL_PROTOCOL_QUERY_OID.
//		This routine runs at passive level.
//

NTSTATUS NmPacketDevice::IOCTL_PROTOCOL_SETQUERY_OID_Handler(KIrp I)
{
	t << "Entering NmPacketDevice::IOCTL_PROTOCOL_SETQUERY_OID_Handler, " << I << EOL;

	I.MarkPending();

	// User's buffer is formatted as PACKET_OID_DATA
	PPACKET_OID_DATA OidData = (PPACKET_OID_DATA) I.IoctlBuffer();

	// Check buffer size
	if ( (I.IoctlInputBufferSize() != I.IoctlOutputBufferSize()) ||
		 (I.IoctlInputBufferSize() < sizeof(PACKET_OID_DATA))  ||
		 (I.IoctlInputBufferSize() < sizeof(PACKET_OID_DATA)-1+OidData->Length)
		)
		return I.Complete(NDIS_STATUS_FAILURE);

	// Allocate new NDIS request. The irp is used as a context
	KNdisRequestWithContext<PIRP>* req = new KNdisRequestWithContext<PIRP>;
	if (req == NULL)
		return I.Complete(STATUS_INSUFFICIENT_RESOURCES);
	req->Context = I;

	// Submit the request
	NDIS_STATUS status = (I.IoctlCode()==IOCTL_PROTOCOL_SET_OID) ?
		req->Set(*m_pBinding, OidData->Oid, OidData->Data, OidData->Length) :
		req->Query(*m_pBinding, OidData->Oid, OidData->Data, OidData->Length);

	// If the request completes asynchronously the framework calls 
	// m_pBinding->RequestComplete. Simulate the same otherwise
	if (status != NDIS_STATUS_PENDING)
		m_pBinding->RequestComplete(*req, status);

	return status;
}

////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::IOCTL_PROTOCOL_RESET_Handler
//
//	Routine Description:
//		Handler for IO Control Code IOCTL_PROTOCOL_RESET
//
//	Parameters:
//		I - IRP containing IOCTL request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		This routine implements the IOCTL_PROTOCOL_RESET function.
//		This routine runs at passive level.
//

NTSTATUS NmPacketDevice::IOCTL_PROTOCOL_RESET_Handler(KIrp I)
{
	t << "Entering NmPacketDevice::IOCTL_PROTOCOL_RESET_Handler, " << I;

	I.MarkPending();

	// Save the irp in the list used in a FIFO manner. NDIS does not
	// provide any context for pending resets; it just guarantees the resets
	// get completed in the order they arrive
	m_ResetList.InsertTail(I);

	// Pass the request down. If it completes synchronously
	// simulate m_pBinding->ResetComplete(), which is otherwise
	// called by the framework.
	NTSTATUS status = m_pBinding->Reset();

    if (status != NDIS_STATUS_PENDING)		
		m_pBinding->ResetComplete(status);

	return status;
}

////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::Cleanup
//
//	Routine Description:
//		Handler for IRP_MJ_CLEANUP	
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS	Result code
//
//	Comments:
//		Here we have to cancel all the pending IRPs (reads)
//

NTSTATUS NmPacketDevice::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I;

	CancelReads();

	WaitForIrpsToComplete();

	return I.Complete(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::Create
//
//	Routine Description:
//		Handler for IRP_MJ_CREATE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//		Fail the creation if not bound (in transition)

NTSTATUS NmPacketDevice::Create(KIrp I)
{
	t << "Entering Create, " << I;

	NTSTATUS status = ( !m_pBinding || (m_pBinding && !m_pBinding->IsBound()) ) ?
				STATUS_DEVICE_NOT_READY : STATUS_SUCCESS;
	
	if (status == STATUS_SUCCESS)
	{
		// Increment the ref count because an applications obtained a handle to the device
		m_RefCount.Increment();
	}

	return I.Complete(status);
}

////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::Close
//
//	Routine Description:
//		Handler for IRP_MJ_CLOSE
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS - Result code
//
//	Comments:
//

NTSTATUS NmPacketDevice::Close(KIrp I)
{
	t << "Entering Close, " << I;
	
	// Decrement the ref count because an applications invalidated the handle
	m_RefCount.Decrement();

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  NmPacketDevice::Shutdown
//
//	Routine Description:
//		Handler for IRP_MJ_SHUTDOWN
//
//	Parameters:
//		I - Current IRP
//
//	Return Value:
//		NTSTATUS	Result code	
//
//	Comments:
//

NTSTATUS NmPacketDevice::Shutdown(KIrp I)
{
	t << "Entering Shutdown\n";
// TODO:	Insert SHUTDOWN code

   return I.Complete(STATUS_SUCCESS);
}

///////////////////////////////////////////////////////////////////////
// This method is called when a pending read IRP is being cancelled
//
// Parameters:
// 	I                    - irp being cancelled
// Returns:
// 	none
// Comments:
// 	none
void NmPacketDevice::CancelRead(KIrp I)
{
	t << "Entering CancelRead, " << I;

	// Browse the receive list to find and remove the packet associated
	// with given IRP

	PNDIS_PACKET packet = KListBrowser<NDIS_PACKET>::RemoveByKey(m_RcvList, I);

	// Release global cancel spinlock

	CancelSpinLock::Release(KeGetCurrentIrql());

	// If such a packet is found, free the packet and complete the IRP
	KNdisPacket pkt(packet);
	if (pkt.IsValid()) {
		m_PacketPool.Free(packet);
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
		m_PendingIrps.Decrement();
	}
}


///////////////////////////////////////////////////////////////////////
// Cancel all pending reads
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	This internal routine is called on
//  (1) device cleanup irp
//  (2) unbinding
void NmPacketDevice::CancelReads()
{
	if (!m_RcvList.IsEmpty())
		t << "Cancelling " << m_RcvList.Count() << " pending read-s" << EOL;

	KNdisPacket packet;
	
	// In order to make list access atomic, a secondary list is used as a temporary
	//storage for the packet elements.
	PacketList tempList;
	m_RcvList.Lock();
	while ( (packet=m_RcvList.RemoveHeadNoLock()).IsValid() ) 
		tempList.InsertTail(packet);
	m_RcvList.Unlock();

	while ( (packet=tempList.RemoveHead()).IsValid() ) {
		KIrp I = PacketList::GetContext(packet)->Irp;
		m_PacketPool.Free(packet);
		I.SetCancelRoutine(NULL);
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
		m_PendingIrps.Decrement();
	}
}


///////////////////////////////////////////////////////////////////////
// Waits for pending IRPs to complete. This includes both read, write
// on oid-s.
//
// Parameters:
// 	none
// Returns:
// 	none
// Comments:
// 	Must be called at PASSIVE_LEVEL
void NmPacketDevice::WaitForIrpsToComplete()
{
#ifdef NDIS51
	// In 5.1 let's ensure all the IRPs are completed.
	// NOTE that maintaining m_PendingIrps is reduntant in this case
	//      since we always cancel reads before calling WaitForIrpsToComplete.
	if (m_pBinding) {
		ULONG IoCount, loop=30;
		while ((IoCount=m_pBinding->QueryPendingIOCount()) && --loop) {
			t << "Waiting for " << IoCount << " IRPs to comlete..." << EOL;
			m_pBinding->Sleep(1000);
		}
		ASSERT(IoCount==0); // or smth got stuck
	}
#endif
	m_PendingIrps.Wait();
}


#ifdef NDIS51
///////////////////////////////////////////////////////////////////////
// This method is called when a pending Write IRP is being cancelled
//
// Parameters:
// 	I                    - irp being cancelled
// Returns:
// 	none
// Comments:
// 	none
void NmPacketDevice::CancelWrite(KIrp I)
{
	t << "Entering CancelWrite, " << I;

	// Browse the send IRP list for this IRP
	// If found, get the associated packet and issue Cancel to NDIS. 
	// The IRP will be completed on SendComplete()

	KListBrowser<IRP> browser;

	if (browser.Lookup(m_SendList, I)) {
	
		PNDIS_PACKET packet = (PNDIS_PACKET&) I.DriverContext();
		ASSERT(PacketList::GetContext(packet)->Irp == I);

		t << "Cancelling packet " << TraceHex 
		  << ULONG_PTR(packet) << ", ID=" << ULONG_PTR(NDIS_GET_PACKET_CANCEL_ID(packet)) << EOL;

		// Bump the ref counter as the packet can get completed before we issue Cancel().
		// In which case freeing  it in SendComplete() would be bad...

		NdisInterlockedIncrement(
			&PacketList::GetContext(packet)->Data.RefCount);

		m_pBinding->CancelSendPackets(packet);

		if (NdisInterlockedDecrement(
			&PacketList::GetContext(packet)->Data.RefCount)==0) {
				m_PacketPool.Free(packet);
		}
	}

}
#endif // NDIS51

