// NmProtDevice.cpp
// Implementation of NmProtDevice device class
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
//=============================================================================
//

#include <vdw.h>

#include "NmProt.h"
#include "NmProtBinding.h"
#include "NmProtDevice.h"
#include "NmProtIoctl.h"

#pragma warning(disable:4065) // Allow switch statement with no cases

extern	KTrace	t;			// Global driver trace object

////////////////////////////////////////////////////////////////////////
// NmProtDevice::~NmProtDevice
//
//	Routine Description:
//		This is the destructor for the NmProtDevice
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
NmProtDevice::~NmProtDevice()
{
	delete m_RegPath;
}

#pragma code_seg("INIT")

////////////////////////////////////////////////////////////////////////////////
//  NmProtDevice::NmProtDevice
//
//	Routine Description:
//		The device constructor is typically responsible for allocating
//		any physical resources that are associated with the device.
//
//	Parameters:
//		None
//
//	Return Value:
//		None
//
//	Comments:
//		The device constructor often reads the registry to setup
//		various configurable parameters.
NmProtDevice::NmProtDevice() :
	KDevice()
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		t << "Failed to create device NmProtDevice, err"  << (ULONG) m_ConstructorStatus << EOL;
		return;
	}

	m_RegPath = CreateRegistryPath(L"NmProtDevice", 0);
	if (m_RegPath == NULL)
	{
		// Error, cannot allocate memory for registry path
		t << "Failed to create registry path\n";
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}

}
#pragma code_seg()


////////////////////////////////////////////////////////////////////////
//  NmProtDevice::DeviceControl
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
NTSTATUS NmProtDevice::DeviceControl(KIrp I)
{
	NTSTATUS	NtStatus;
	NDIS_STATUS	Status;

	t << "Entering NmProtDevice::Device Control, " << I;

	// Increment pending count
	m_PendingIrps.Increment();

	switch (I.IoctlCode())
	{
		case IOCTL_NDISPROT_BIND_WAIT:
			//
			//  Block until we have seen a NetEventBindsComplete event,
			//  meaning that we have finished binding to all running
			//  adapters that we are supposed to bind to.
			//
			//  If we don't get this event in 5 seconds, time out.
			//
			ASSERT((I.IoctlCode() & 0x3) == METHOD_BUFFERED);

			if (NmProtBinding::m_BindsComplete.Wait(5000))
				NtStatus = STATUS_SUCCESS;
			else
				NtStatus = STATUS_TIMEOUT;
			t << "IoControl: BindWait returning " << NtStatus << EOL;
			break;

		case IOCTL_NDISPROT_QUERY_BINDING:
			ASSERT((I.IoctlCode() & 0x3) == METHOD_BUFFERED);

			Status = QueryBinding(I);
			NDIS_STATUS_TO_NT_STATUS(Status, &NtStatus);
			break;

		case IOCTL_NDISPROT_OPEN_DEVICE:
			ASSERT((I.IoctlCode() & 0x3) == METHOD_BUFFERED);
			if (I.FileObject()->FsContext != NULL)
			{
				t << "IoControl: OPEN_DEVICE: FileObj " << I.FileObject()
					<<" already associated with open " << I.FileObject()->FsContext << EOL;
				NtStatus = STATUS_DEVICE_BUSY;
				break;
			}

			NtStatus = OpenDevice(I);
			break;

		case IOCTL_NDISPROT_QUERY_OID_VALUE:
		case IOCTL_NDISPROT_SET_OID_VALUE:
			ASSERT((I.IoctlCode() & 0x3) == METHOD_BUFFERED);
			if (I.FileObject()->FsContext != NULL)
			{
				Status = QuerySetOidValue(I);
				NDIS_STATUS_TO_NT_STATUS(Status, &NtStatus);
			}
			else
			{
				NtStatus = STATUS_DEVICE_NOT_CONNECTED;
			}
			break;

		case IOCTL_NDISPROT_INDICATE_STATUS:
			ASSERT((I.IoctlCode() & 0x3) == METHOD_BUFFERED);
			if (I.FileObject()->FsContext != NULL)
				NtStatus = QueueStatusIndicationIrp(I);
			else
				NtStatus = STATUS_DEVICE_NOT_CONNECTED;
			break;

		default:
			// Unrecognized IOCTL request
			NtStatus = STATUS_INVALID_PARAMETER;
			break;
	}

	// If the IRP's IOCTL handler deferred processing using some driver
	// specific scheme, the status variable is set to STATUS_PENDING.
	// In this case we simply return that status, and the IRP will be
	// completed later.  Otherwise, complete the IRP using the status
	// returned by the IOCTL handler.
	if (NtStatus == STATUS_PENDING)
	{
		return NtStatus;
	}
	else
	{
		m_PendingIrps.Decrement();
		return I.Complete(NtStatus);
	}
}

////////////////////////////////////////////////////////////////////////
//  NmProtDevice::QueryBinding
//
//	Routine Description:
//		Handler for IO Control Code IOCTL_NDISPROT_QUERY_BINDING
//
//	Parameters:
//		I - IRP containing IOCTL request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		Returns a single adapter name into the supplied buffer
//
NDIS_STATUS NmProtDevice::QueryBinding(KIrp I)
{
	t << "Entering NmProtDevice::QueryBinding, " << I << EOL;

	NDIS_STATUS Status;
	do
	{
		if (I.IoctlInputBufferSize() < sizeof(NDISPROT_QUERY_BINDING))
		{
			Status = NDIS_STATUS_RESOURCES;
			break;
		}

		if (I.IoctlOutputBufferSize() < sizeof(NDISPROT_QUERY_BINDING))
		{
			Status = NDIS_STATUS_BUFFER_OVERFLOW;
			break;
		}

		ULONG Remaining = I.IoctlOutputBufferSize() - sizeof(NDISPROT_QUERY_BINDING);

		PNDISPROT_QUERY_BINDING pQueryBinding = (PNDISPROT_QUERY_BINDING)I.IoctlBuffer();
		ULONG BindingIndex = pQueryBinding->BindingIndex;

		Status = NDIS_STATUS_ADAPTER_NOT_FOUND;

		NmProtBinding *b =
			KListBrowser<NmProtBinding>::At(NmProtBinding::m_BindingList, BindingIndex);
		if(b)
		{
			NmProtBinding::m_BindingList.Lock();

			PNDIS_STRING pDeviceName = b->GetOpenContext()->DeviceName();
			pQueryBinding->DeviceNameLength = pDeviceName->Length + sizeof(WCHAR);
			pQueryBinding->DeviceDescrLength = b->GetAdapterNameString().Size() + sizeof(WCHAR);
			if (Remaining < pQueryBinding->DeviceNameLength +
							pQueryBinding->DeviceDescrLength)
			{
				NmProtBinding::m_BindingList.Unlock();
				Status = NDIS_STATUS_BUFFER_OVERFLOW;
				break;
			}

			NdisZeroMemory((PUCHAR)pQueryBinding + sizeof(NDISPROT_QUERY_BINDING),
							pQueryBinding->DeviceNameLength +
							pQueryBinding->DeviceDescrLength);

			pQueryBinding->DeviceNameOffset = sizeof(NDISPROT_QUERY_BINDING);
			NdisMoveMemory((PUCHAR)pQueryBinding + pQueryBinding->DeviceNameOffset,
							pDeviceName->Buffer,
							pDeviceName->Length);

			pQueryBinding->DeviceDescrOffset = pQueryBinding->DeviceNameOffset +
												pQueryBinding->DeviceNameLength;
			NdisMoveMemory((PUCHAR)pQueryBinding + pQueryBinding->DeviceDescrOffset,
							b->GetAdapterNameString(),
							b->GetAdapterNameString().Size());

			NmProtBinding::m_BindingList.Unlock();

			I.Information() = pQueryBinding->DeviceDescrOffset + pQueryBinding->DeviceDescrLength;
			Status = NDIS_STATUS_SUCCESS;
			break;
		}
	}
	while (FALSE);

	return Status;
}

////////////////////////////////////////////////////////////////////////
//  NmProtDevice::OpenDevice
//
//	Routine Description:
//		Handler for IO Control Code IOCTL_NDISPROT_OPEN_DEVICE
//
//	Parameters:
//		I - IRP containing IOCTL request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		Returns a single adapter name into the supplied buffer
//
NDIS_STATUS NmProtDevice::OpenDevice(KIrp I)
{
	t << "Entering NmProtDevice::QueryBinding, " << I << EOL;

	NTSTATUS NtStatus;
	do
	{
		NmProtBinding *b =
			KListBrowser<NmProtBinding>::LookupByFunc(NmProtBinding::m_BindingList, ComparingBinding(), I);
		if(!b)
		{
			NtStatus = STATUS_OBJECT_NAME_NOT_FOUND;
			break;
		}

		NmOpenContext *pOpenContext = b->GetOpenContext();
		pOpenContext->m_Lock.Lock();

		if (!NPROT_TEST_FLAGS(pOpenContext->m_Flags, NUIOO_OPEN_FLAGS, NUIOO_OPEN_IDLE))
		{
			ASSERT(pOpenContext->m_pFileObject != NULL);

			t << "NmProtDevice::OpenDevice: " << pOpenContext << "/" << pOpenContext->m_Flags
				<< " already associated with another FileObject " << pOpenContext->m_pFileObject;

			pOpenContext->m_Lock.Unlock();

			NtStatus = STATUS_DEVICE_BUSY;
			break;
		}
		//
		// This InterlockedXXX function performs an atomic operation: First it compare
		// I.FileObject()->FsContext with NULL, if they are equal, the function puts  pOpenContext
		// into FsContext, and return NULL. Otherwise, it return pFileObject->FsContext without
		// changing anything.
		//
		NmOpenContext *pCurrentOpenContext = (NmOpenContext*)InterlockedCompareExchangePointer (& (I.FileObject()->FsContext), pOpenContext, NULL);
		if (pCurrentOpenContext != NULL)
		{
			//
			// pFileObject->FsContext already is used by other open
			//
			t << "NmProtDevice::OpenDevice: " << pOpenContext << "/" << pOpenContext->m_Flags
				<< " already associated with another FileObject " << pOpenContext->m_pFileObject;

			pOpenContext->m_Lock.Unlock();

			NtStatus = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}

		pOpenContext->m_pFileObject = I.FileObject();

		NPROT_SET_FLAGS(pOpenContext->m_Flags, NUIOO_OPEN_FLAGS, NUIOO_OPEN_ACTIVE);

		pOpenContext->m_Lock.Unlock();

		//
		//  Set the packet filter now.
		//
		// Allocate new NDIS request. The irp is used as a context
		KNdisRequestWithContext<PIRP> Req;
		Req.Context = I;
		b->ResetCompletion();

		// Submit the request
		ULONG PacketFilter = NUIOO_PACKET_FILTER;
		NDIS_STATUS Status = Req.Set(*(pOpenContext->m_pBinding), OID_GEN_CURRENT_PACKET_FILTER, &PacketFilter, sizeof(PacketFilter));

		if (Status == NDIS_STATUS_PENDING)
		{
			b->WaitForCompletion();
			Status = I.Status();
		}
		if (Status == NDIS_STATUS_SUCCESS)
			I.Information() = Req.BytesQueried();

		if (Status != NDIS_STATUS_SUCCESS)
		{
			t << "openDevice: Open " << pOpenContext << " set packet filter (" << PacketFilter << ") failed: " << NtStatus << "\n";

			//  Undo all that we did above.
			pOpenContext->m_Lock.Lock();
			//
			// Need to set pFileObject->FsContext to NULL again, so others can open a device
			// for this file object later
			//
			pCurrentOpenContext = (NmOpenContext*)InterlockedCompareExchangePointer (& (I.FileObject()->FsContext), NULL, pOpenContext);

			ASSERT(pCurrentOpenContext == pOpenContext);

			NPROT_SET_FLAGS(pOpenContext->m_Flags, NUIOO_OPEN_FLAGS, NUIOO_OPEN_IDLE);
			pOpenContext->m_pFileObject = NULL;

			pOpenContext->m_Lock.Unlock();

			NDIS_STATUS_TO_NT_STATUS(Status, &NtStatus);
			break;
		}
		NtStatus = STATUS_SUCCESS;
	}
	while (FALSE);

	return NtStatus;
}

///////////////////////////////////////////////////////////////////////
// Function operator called by KListBrowser in framework library.
// It compares current binding's adapter name with user specified name
// in IRP passed by DeviceIoControl.
//
// Parameters:
// 	b		-	Binding object the function is called for
//  I		-	caller's IRP
// Returns:
// 	TRUE	-	name matches
//	FALSE	-	name doesn't match
// Comments:
// 	Called by KListBrowser::LookupByFunc().
BOOLEAN NmProtDevice::ComparingBinding::operator() (NmProtBinding* b, KIrp I)
{
	PUNICODE_STRING pDeviceName = b->GetOpenContext()->DeviceName();
	if(pDeviceName->Length == I.IoctlInputBufferSize() &&
		NdisEqualMemory(pDeviceName->Buffer, (PUCHAR)I.IoctlBuffer(), I.IoctlInputBufferSize()))
	{
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//  NmProtDevice::Read
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
NTSTATUS NmProtDevice::Read(KIrp I)
{
	t << "Entering read, " << I;

	NmOpenContext *pOpenContext = (NmOpenContext*)I.FileObject()->FsContext;

	if(pOpenContext->m_pBinding==NULL)
	{
		I.Complete(STATUS_INVALID_HANDLE);
		return STATUS_INVALID_HANDLE;
	}

	// Check if the binding is still active
	if(!pOpenContext->m_pBinding->IsBound())
	{
		I.Complete(STATUS_DEVICE_NOT_READY);
		return STATUS_DEVICE_NOT_READY;
	}

	// Make sure the MDL is not NULL
	if (I.Mdl() == NULL)
	{
		I.Complete(STATUS_INVALID_PARAMETER);
		return STATUS_INVALID_PARAMETER;
	}

	// Allocate packet descriptor
	KNdisPacket packet = pOpenContext->m_PacketPool.Allocate();
	if (!packet.IsValid())
	{
		t << "No free packets\n";
		return I.Complete(STATUS_INSUFFICIENT_RESOURCES);
	}

	m_PendingIrps.Increment();
	// Initialize packet's context (reserved fields)
	PacketList::GetContext(packet)->Irp = I;
	PacketList::GetContext(packet)->Data.pMdl = I.Mdl();

	// Mark the irp pending, set up cancel routine
	I.MarkPending();
	I.SetCancelRoutine(LinkTo(CancelRead));

	// Queue the packet into the receive list and return
	pOpenContext->m_RcvList.InsertTail(packet);

	return STATUS_PENDING;
}


////////////////////////////////////////////////////////////////////////
//  NmProtDevice::Write
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
NTSTATUS NmProtDevice::Write(KIrp I)
{
	t << "Entering write, " << I;

	NmOpenContext *pOpenContext = (NmOpenContext*)I.FileObject()->FsContext;

	if(pOpenContext->m_pBinding==NULL)
	{
		I.Complete(STATUS_INVALID_HANDLE);
		return STATUS_INVALID_HANDLE;
	}

	// Check if the binding is still active
	if(!pOpenContext->m_pBinding->IsBound())
	{
		I.Complete(STATUS_DEVICE_NOT_READY);
		return STATUS_DEVICE_NOT_READY;
	}

	// Allocate packet descriptor
	KNdisPacket packet = pOpenContext->m_PacketPool.Allocate();
	if (!packet.IsValid())
	{
		t << "No free packets\n";
		I.Complete(STATUS_INSUFFICIENT_RESOURCES);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	m_PendingIrps.Increment();
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
	packet.CANCEL_ID(pOpenContext->m_CancelId.Next());

	// Save packet pointer in the irp. Note we can use DriverContext[4] since
	// the IRPs are not queued at the device
	I.DriverContext() = PNDIS_PACKET(packet);

	// Queue the packet into the receive list and return
	pOpenContext->m_SendList.InsertTail(I);

	// Set up cancel routine
	I.SetCancelRoutine(LinkTo(CancelWrite));
#endif // NDIS51

	// Submit the packet to the miniport. When the send is done the
	// framework calls m_pBinding->SendComplete(). Simulate the
	// same if the request gets completed synchronously.
	NDIS_STATUS status = pOpenContext->m_pBinding->Send(packet);

	if (status != NDIS_STATUS_PENDING)
		pOpenContext->m_pBinding->SendComplete(packet, status);

	return STATUS_PENDING;
}


////////////////////////////////////////////////////////////////////////
//  NmProtDevice::QuerySetOidValue
//
//	Routine Description:
//		Handler for IO Control Code IOCTL_NDISPROT_QUERY_BINDING and IOCTL_NDISPROT_SET_BINDING
//
//	Parameters:
//		I - IRP containing IOCTL request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		This routine implements both IOCTL_PROTOCOL_SET_OID and IOCTL_PROTOCOL_QUERY_OID.
//		This routine runs at passive level.
//
NDIS_STATUS NmProtDevice::QuerySetOidValue(KIrp I)
{
	t << "Entering NmOpenContext::QuerySetOidValue, " << I << EOL;

	NDIS_STATUS		Status;

	if (I.IoctlInputBufferSize() < sizeof(NDISPROT_QUERY_OID))
	{
		I.Complete(NDIS_STATUS_BUFFER_TOO_SHORT);
		return NDIS_STATUS_BUFFER_TOO_SHORT;
	}

	// User's buffer is formatted as NDISPROT_QUERY_OID
	PNDISPROT_QUERY_OID pOidData = (PNDISPROT_QUERY_OID) I.IoctlBuffer();

	// Check buffer size
	if ( (I.IoctlInputBufferSize() != I.IoctlOutputBufferSize()) ||
		 (I.IoctlInputBufferSize() < sizeof(NDISPROT_QUERY_OID)))
		return I.Complete(NDIS_STATUS_FAILURE);

	// Allocate new NDIS request. The irp is used as a context
	KNdisRequestWithContext<PIRP>* req = new KNdisRequestWithContext<PIRP>;
	if (req == NULL)
		return I.Complete(STATUS_INSUFFICIENT_RESOURCES);
	req->Context = I;

	NmOpenContext *pOpenContext = (NmOpenContext*)I.FileObject()->FsContext;
	// Submit the request
	Status = (I.IoctlCode()==IOCTL_NDISPROT_SET_OID_VALUE) ?
		req->Set(*(pOpenContext->m_pBinding), pOidData->Oid, pOidData->Data, I.IoctlOutputBufferSize() - FIELD_OFFSET(NDISPROT_QUERY_OID, Data)) :
		req->Query(*(pOpenContext->m_pBinding), pOidData->Oid, pOidData->Data, I.IoctlInputBufferSize() - FIELD_OFFSET(NDISPROT_QUERY_OID, Data));

	if (Status == NDIS_STATUS_PENDING)
	{
		pOpenContext->m_pBinding->WaitForCompletion();
		Status = I.Status();
	}

	if (Status == NDIS_STATUS_SUCCESS)
	{
		I.Information() = req->BytesQueried();
		if(I.IoctlCode()==IOCTL_NDISPROT_QUERY_OID_VALUE)
			I.Information() += FIELD_OFFSET(NDISPROT_QUERY_OID, Data);
		delete req;
	}

	return (Status);
}


////////////////////////////////////////////////////////////////////////
//  NmProtDevice::QuerySetOidValue
//
//	Routine Description:
//		Handler for IO Control Code IOCTL_NDISPROT_INDICATE_STATUS
//
//	Parameters:
//		I - IRP containing IOCTL request
//
//	Return Value:
//		NTSTATUS - Status code indicating success or failure
//
//	Comments:
//		Not implemented till app actually uses it.
//
NDIS_STATUS NmProtDevice::QueueStatusIndicationIrp(KIrp I)
{
	t << "Entering QueueStatusIndicationIrp, " << I;
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}


////////////////////////////////////////////////////////////////////////
//  NmProtDevice::Create
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
//
NTSTATUS NmProtDevice::Create(KIrp I)
{
	t << "Entering Create, " << I;
	I.FileObject()->FsContext = NULL;
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  NmProtDevice::Close
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
NTSTATUS NmProtDevice::Close(KIrp I)
{
	t << "Entering Close, " << I;
	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

////////////////////////////////////////////////////////////////////////
//  NmProtDevice::Cleanup
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
//
NTSTATUS NmProtDevice::CleanUp(KIrp I)
{
	t << "Entering CleanUp, " << I;

	NmOpenContext *pOpenContext = (NmOpenContext*)I.FileObject()->FsContext;
	if (pOpenContext != NULL)
	{
		//
		//  Mark this endpoint.
		//
		pOpenContext->m_Lock.Lock();
		NPROT_SET_FLAGS(pOpenContext->m_Flags, NUIOO_OPEN_FLAGS, NUIOO_OPEN_IDLE);
		pOpenContext->m_pFileObject = NULL;
		pOpenContext->m_Lock.Unlock();

		//
		//  Set the packet filter to 0, telling NDIS that we aren't
		//  interested in any more receives.
		//
		KNdisRequestWithContext<PIRP> Req;
		Req.Context = I;
		pOpenContext->m_pBinding->ResetCompletion();

		// Submit the request
		ULONG PacketFilter = 0;
		NDIS_STATUS Status = Req.Set(*(pOpenContext->m_pBinding), OID_GEN_CURRENT_PACKET_FILTER, &PacketFilter, sizeof(PacketFilter));

		if (Status == NDIS_STATUS_PENDING)
		{
			pOpenContext->m_pBinding->WaitForCompletion();
			Status = I.Status();
		}

		if (Status != NDIS_STATUS_SUCCESS)
		{
			t << "Cleanup: Open " << pOpenContext << ", set packet filter (" << PacketFilter << ") failed: " << Status << EOL;
			//
			//  Ignore the result. If this failed, we may continue
			//  to get indicated receives, which will be handled
			//  appropriately.
			//
			Status = NDIS_STATUS_SUCCESS;
		}

		//
		//  Cancel any pending reads.
		//
		pOpenContext->CancelAllPendingReads();
	}
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
void NmProtDevice::CancelRead(KIrp I)
{
	t << "Entering CancelRead, " << I;

	// Browse the receive list to find and remove the packet associated
	// with given IRP
	NmOpenContext *pOpenContext = (NmOpenContext*)I.FileObject()->FsContext;
	PNDIS_PACKET packet = KListBrowser<NDIS_PACKET>::RemoveByKey(pOpenContext->m_RcvList, I);

	// Release global cancel spinlock
	CancelSpinLock::Release(KeGetCurrentIrql());

	// If such a packet is found, free the packet and complete the IRP
	KNdisPacket pkt(packet);
	if (pkt.IsValid())
	{
		pOpenContext->m_PacketPool.Free(packet);
		I.Information() = 0;
		I.Complete(STATUS_CANCELLED);
		m_PendingIrps.Decrement();
	}
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
void NmProtDevice::CancelWrite(KIrp I)
{
	t << "Entering CancelWrite, " << I;

	// Browse the send IRP list for this IRP
	// If found, get the associated packet and issue Cancel to NDIS.
	// The IRP will be completed on SendComplete()

	NmOpenContext *pOpenContext = (NmOpenContext*)I.FileObject()->FsContext;
	KListBrowser<IRP> browser;

	if (browser.Lookup(pOpenContext->m_SendList, I))
	{
		PNDIS_PACKET packet = (PNDIS_PACKET&) I.DriverContext();
		ASSERT(PacketList::GetContext(packet)->Irp == I);

		t << "Cancelling packet " << TraceHex
		  << ULONG_PTR(packet) << ", ID=" << ULONG_PTR(NDIS_GET_PACKET_CANCEL_ID(packet)) << EOL;

		// Bump the ref counter as the packet can get completed before we issue Cancel().
		// In which case freeing  it in SendComplete() would be bad...

		NdisInterlockedIncrement(&PacketList::GetContext(packet)->Data.RefCount);

		pOpenContext->m_pBinding->CancelSendPackets(packet);

		if (NdisInterlockedDecrement(&PacketList::GetContext(packet)->Data.RefCount)==0)
		{
			pOpenContext->m_PacketPool.Free(packet);
		}
	}
}
#endif // NDIS51
