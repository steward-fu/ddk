// KTransportIrpDispatch.h
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

#if !defined(_KTransportIrpDispatch_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_)
#pragma once
#define _KTransportIrpDispatch_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_

extern "C" {
 #include <tdi.h>
 #include <tdikrnl.h>
};

#include <KTA_ADDRESS.h>
#include <KTransportTraits.h>

///////////////////////////////////////////////////////////////////////
// KTransportIrpDispatch
//
// Transport device IRP dispatcher.
//
// A transport driver must expose at least one device object, which
// is to be implemented as a KDevice-derived class.
//
// The device object becomes a "transport" by virtue of exposing a
// TDI-compliant i/o interface, i.e. it has a predefined treatment
// of IRP_MJ_* and IOCTL codes defined in tdikrnl.h.
// KTransportIrpDispatch template is used as a helper to facilitate
// parsing of the TDI IRPs and dispatching them into appropriate
// methods.
//
// The TDI specification defines that transport drivers conceptually
// manage the following internal entities: control channel, transport
// addresses and (optionally) connection endpoints.
//
// Consequently, KTransportIrpDispatch is parametrized by the type of:
//
//	D	-	transport device class implementing a control channel (KDevice-derived)
//	A	-	transport address (KTransportAddress-derived or modeled)
//	C	-	connection endpoint (KTransportConnection-derived or modeled)
//
// The instance of KTransportIrpDispatch is created in DriverEntry()
// using the placement new() operator defined on KDevice. Class D is used
// as a base for KTransportIrpDispatch, so the instantiation of
// KTransportIrpDispatch results in instantiation of the device class D
// instance as well.
// KTransportIrpDispatch overrides the virtual DeviceIrpDispatch member,
// "cracks" the incoming irp and dispatches control into appropriate
// methods definied on D, A or C accordingly.
//
// Methods required by the template parameters
// ===========================================
//
// All (D, A and C) must expose the following methods:
//	NTSTATUS QueryInformation(KIrp);							// TDI queries
//	NTSTATUS SetInformation(KIrp);								// TDI sets
//	NTSTATUS Action(KIrp);										// TDI custom actions
//
// Class D must also expose the following methods:
//	NTSTATUS Create(KIrp);									// create control channel (=device)
//	NTSTATUS Close(KIrp);									// close conrol channel (=device)
//	NTSTATUS CleanUp(KIrp);									// cleanup conrol channel (=device)
//	A* CreateTransportAddress(KIrp, PTRANSPORT_ADDRESS);	// create TA
//  C* CreateConnectionEndpoint(KIrp, PVOID);				// create connection context
//	NTSTATUS CleanUp(KIrp, A*);								// clean up TA
//  NTSTATUS CleanUp(KIrp, C*)								// cleanup connection context
//	NTSTATUS Close(KIrp, A*);								// close TA
//  NTSTATUS Close(KIrp, C*)								// close connection context
//
// Class A must also expose the following methods:
//	NTSTATUS SendDatagram(KIrp);
//	NTSTATUS ReceiveDatagram(KIrp);
//	NTSTATUS SetEvent(KIrp);
//
// Class C must also expose the following methods:
//	NTSTATUS Send(KIrp);
//	NTSTATUS Receive(KIrp);
//	NTSTATUS Associate(KIrp,A*);
//	NTSTATUS Disassociate(KIrp);
//	NTSTATUS Connect(KIrp);
//	NTSTATUS Listen(KIrp);
//	NTSTATUS Accept(KIrp);
//	NTSTATUS Disconnect(KIrp);
//
// NOTES
//
// * Consider: TdiDispatchFastDeviceControl support. Requires
//   setting the FastIoDispatch field of DRIVER_OBJECT (not wrapped by DW).
//
//
template <class D, class A=KTransportAddress, class C=KTransportConnection>
class KTransportIrpDispatch : public D
{
	// Constructors
public:
	KTransportIrpDispatch() : D() {}
	~KTransportIrpDispatch() {}
	SAFE_DESTRUCTORS

	// KDevice's override
	NTSTATUS DeviceIrpDispatch(PIRP pIrp);

	///////////////////////////////////////////////////////////////
	// Define traits class for convenience
	typedef KTransportTraits<D,A,C> Traits;

protected:
	// internal ioctl dispatch helpers
	NTSTATUS Dispatch(KIrp I);
	NTSTATUS Dispatch(KIrp I, A* ta);
	NTSTATUS Dispatch(KIrp I, C* cep);

};

///////////////////////////////////////////////////////////////////////
// Implementation
//
template <class D, class A, class C>
NTSTATUS
KTransportIrpDispatch<D,A,C>::DeviceIrpDispatch(PIRP pIrp)
{
	// This method provides special handling for IRP_MJ_CREATE/CLOSE
	// by acting as a factory for transport addresses (A) and endpoinds (C)
	// All the other MJ must be implemented by the derived class
	//
	KIrp I(pIrp);
	UCHAR Major = I.MajorFunction();
	NTSTATUS status=STATUS_SUCCESS;
#if DBG
	DbgPrint("KTransportIrpDispatch: Major=%u, Minor=%u\n", ULONG(Major), ULONG(I.MinorFunction()));
#endif
	ASSERT (Major <= UCHAR(IRP_MJ_MAXIMUM_FUNCTION));
	NTSTATUS (KDevice::*func)(KIrp) = IrpDispatchTable[Major];

	switch ( Major )
	{
	case IRP_MJ_CREATE:
		{
			// Process create requests for TAs, CEs and control channel.
			// The context of the EA defines what is the request for.
			//
			PFILE_FULL_EA_INFORMATION pEA = (PFILE_FULL_EA_INFORMATION) I.IoctlBuffer();
			if (pEA == NULL) {
				// Control Channel: delegate to the base class' Create(). It MUST complete the irp:
				Traits::SaveContext(I.FileObject());
				return (this->*func)(I);
			}
			else {
				PCHAR pEaName = pEA->EaName;
				PCHAR pEaValue = pEaName + pEA->EaNameLength + 1 /* for separating 0 */;
				union  { A* ta; C* cep; PVOID value; } obj;
				if (TDI_TRANSPORT_ADDRESS_LENGTH==pEA->EaNameLength &&
					memcmp(pEaName, TdiTransportAddress, TDI_TRANSPORT_ADDRESS_LENGTH) == 0) {
					// Transport Address: create and save context
					if (obj.ta = this->CreateTransportAddress(I, PTRANSPORT_ADDRESS(pEaValue)))
						Traits::SaveContext(I.FileObject(), obj.ta);
					else status = I.Status(); // error status to be set by A
				}
				else if (TDI_CONNECTION_CONTEXT_LENGTH==pEA->EaNameLength &&
						memcmp(pEaName, TdiConnectionContext, TDI_CONNECTION_CONTEXT_LENGTH) == 0) {
					// Connection Endpoint: create and save context
					if (obj.cep = this->CreateConnectionEndpoint(I, PVOID(*PULONG_PTR(pEaValue))))
						Traits::SaveContext(I.FileObject(), obj.cep);
					else status = I.Status(); // error status to be set by C
				}
				else {
					// Unknown attribute - fail
					status = STATUS_NONEXISTENT_EA_ENTRY;
				}
			}

			// We're here only when called for TA or CONN. Complete the irp
			return I.Complete(status);

		} // IRP_MJ_CREATE
		break;

	case IRP_MJ_CLOSE:
		{
			// Special handling of destroyiing TAs and CEPs; follows IRP_MJ_CLEANUP
			//
			ASSERT(Traits::IsValid(I.FileObject()));
			if (Traits::ContextType(I.FileObject()) == Traits::TransportAddress)
				status = this->Close(I, reinterpret_cast<A*> (Traits::Context(I.FileObject())));
			else {
				if (Traits::ContextType(I.FileObject()) == Traits::ConnectionEndpoint)
						status = this->Close(I, reinterpret_cast<C*> (Traits::Context(I.FileObject())));
				else
				// delegate to cc's Close
				return (this->*func)(I);
			}
			// We're here only when called for TA or CONN. Complete the irp
			return I.Complete(status);
		}
		break;

	case IRP_MJ_CLEANUP:
		// Called to cancel all active operations (IRPs) pending on the object
		//
		{
			ASSERT(Traits::IsValid(I.FileObject()));
			if (Traits::ContextType(I.FileObject()) == Traits::TransportAddress)
				status = this->CleanUp(I, reinterpret_cast<A*> (Traits::Context(I.FileObject())));
			else {
				if (Traits::IsConnectionOriented() &&
					Traits::ContextType(I.FileObject()) == Traits::ConnectionEndpoint)
					status = this->CleanUp(I, reinterpret_cast<C*> (Traits::Context(I.FileObject())));
			else
				// delegate to cc's CleanUp
				return (this->*func)(I);
			}
			// We're here only when called for TA or CONN. Complete the irp
			return I.Complete(status);
		}
		break;

	case IRP_MJ_DEVICE_CONTROL:
		// A request from a protocol-aware app. It could use the same requests
		// issued by normal TDI client. We use TdiMapUserRequest to map those
		// and fall thru to the IRP_MJ_INTERNAL_DEVICE_CONTROL. Otherwise
		// we call the normal ioctl routine, which performs "private" ioctl processing
		// for this transport
		//
		if (TdiMapUserRequest(*this, I, I.CurrentStackLocation()) != STATUS_SUCCESS)
			return (this->*func)(I);
		// no break - proceed with TDI processing

	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		// A requestfrom a kernel mode TDI client.
		// Delegate to operations on D, A or C. Note we do not complete
		// the irp unless the Dispatch failed to parse the minor TDI code.
		//
		{
			ASSERT(Traits::IsValid(I.FileObject()));
			if (Traits::ContextType(I.FileObject()) == Traits::TransportAddress)
				status = this->Dispatch(I, reinterpret_cast<A*> (Traits::Context(I.FileObject())));
			else {
				if (Traits::IsConnectionOriented() &&
					Traits::ContextType(I.FileObject()) == Traits::ConnectionEndpoint)
					status = Dispatch(I, reinterpret_cast<C*> (Traits::Context(I.FileObject())));
			else
				// Control Channel. Parse it.
				status = Dispatch(I);
			}
		}
		break;

	default:
		// Other (non-TDI) codes (such as read/write) as long as they're defined in function.h
		return (this->*func)(I);
	}

	return status;
}

/////////////////////////////////////////////////////////////
// i/o ctl dispatchers
template <class D, class A, class C>
NTSTATUS
KTransportIrpDispatch<D,A,C>::Dispatch(KIrp I) 	// control channel
{
	switch (I.MinorFunction()) {
		case TDI_QUERY_INFORMATION: return QueryInformation(I);
		case TDI_SET_INFORMATION: return SetInformation(I);
		case TDI_ACTION: return Action(I);
	};
	ASSERT(!"Supported ioctl");
	return I.Complete(STATUS_INVALID_DEVICE_REQUEST);
}

template <class D, class A, class C>
NTSTATUS
KTransportIrpDispatch<D,A,C>::Dispatch(KIrp I, A* ta) // TA object
{
	if (ta->IsValid())
		switch (I.MinorFunction()) {
			case TDI_SEND_DATAGRAM: return ta->SendDatagram(I);
			case TDI_RECEIVE_DATAGRAM: return ta->ReceiveDatagram(I);
			case TDI_QUERY_INFORMATION: return ta->QueryInformation(I);
			case TDI_SET_INFORMATION: return ta->SetInformation(I);
			case TDI_ACTION: return ta->Action(I);
			case TDI_SET_EVENT_HANDLER: return ta->SetEvent(I);
			default: ASSERT(!"Supported ioctl");
		};

	return I.Complete(STATUS_INVALID_DEVICE_REQUEST);
}

template <class D, class A, class C>
NTSTATUS
KTransportIrpDispatch<D,A,C>::Dispatch(KIrp I, C* cep) // CEP object
{
	NTSTATUS Status;
	if (cep->IsValid())
		switch (I.MinorFunction()) {
			case TDI_SEND: return cep->Send(I);
			case TDI_RECEIVE: return cep->Receive(I);
			case TDI_ASSOCIATE_ADDRESS:
				{
					// We're given a TA's handle, but need the file object. So, go get it.
					// When we're done don't forget to dereference it.
					HANDLE hTA = PTDI_REQUEST_KERNEL_ASSOCIATE(&(I.CurrentStackLocation())->Parameters)->AddressHandle;
					PFILE_OBJECT pFO;
					Status = ObReferenceObjectByHandle(hTA, 0, NULL, KernelMode, (PVOID*)&pFO, NULL);
					if (NT_SUCCESS(Status)) {
						ASSERT(Traits::IsValid(pFO));
						Status = cep->Associate(I, reinterpret_cast<A*> (Traits::Context(pFO)));
						ObDereferenceObject(pFO);
						return Status;
					}
					else ASSERT(!"Good hTA");
				}
				break;
			case TDI_DISASSOCIATE_ADDRESS: return cep->Disassociate(I);
			case TDI_CONNECT: return cep->Connect(I);
			case TDI_LISTEN: return cep->Listen(I);
			case TDI_ACCEPT: return cep->Accept(I);
			case TDI_DISCONNECT: return cep->Disconnect(I);
			case TDI_QUERY_INFORMATION: return cep->QueryInformation(I);
			case TDI_SET_INFORMATION: return cep->SetInformation(I);
			case TDI_ACTION: return cep->Action(I);
			default: ASSERT(!"Supported ioctl");
	};

	return I.Complete(STATUS_INVALID_DEVICE_REQUEST);
}

/////////////////////////////////////////////////////////////////////////////
// KIrpParams
//
// Helper template to access the parameter structure of type T, which accompanies
// ioctl requests issued to the device (in IrpSp->Parameters).
// The instance of class behaves as T*.
//
template <typename T> class KIrpParams
{
public:	inline KIrpParams(KIrp I)
	{ ptr = reinterpret_cast<T*> (&(I.CurrentStackLocation()->Parameters));}

	inline operator T*() { return ptr; }
	inline T* operator->() { return ptr; }

protected:	T*	ptr;
};

#ifndef KTRANSPORT_EVENT_TYPES
#define KTRANSPORT_EVENT_TYPES	11		/* TDI_EVENT_ types as per DDK 2000 */
#endif

////////////////////////////////////////////////////////////////////////////
// KTransportEventNotification
//
// Helper class to manage the set of registered TDI client event notifications
// Usually, built into an instance of transport's object managing transport
// address.
//
// Client callback prototypes are defined in tdikrnl.h as PTDI_IND_xxx types.
//
// TBD: The use of TdiDefaultXxx/tdi.sys handlers
//
class KTransportEventNotification
{
public:
	KTransportEventNotification() : m_bEnabled(TRUE) { memset(m_Callback, 0, sizeof(m_Callback)); }

	// Set (or clear) callback
	NTSTATUS Set(ULONG Type, PVOID Handler, PVOID Context=NULL)
	{ if (Type <= KTRANSPORT_EVENT_TYPES) {
		m_Callback[Type].Handler=Handler;  m_Callback[Type].Context=Context;
		return STATUS_SUCCESS;
		}
		else return STATUS_INVALID_PARAMETER;
	}

	// Retrieve
	PVOID Handler(USHORT Type) { ASSERT(Type <= KTRANSPORT_EVENT_TYPES); return m_Callback[Type].Handler; }
	PVOID Context(USHORT Type) { ASSERT(Type <= KTRANSPORT_EVENT_TYPES); return m_Callback[Type].Context; }

	// Enable/disable. This is used to maintain state whether the callbacks are
	// enabled or not. Eg according to the TDI Spec, no client callbacks are allowed
	// whilie Receive() IRP is pending.
	inline void Enable() { m_bEnabled = true; }
	inline void Disable() { m_bEnabled = false; }
	inline bool IsEnabled() const  { return m_bEnabled; }

protected:
	// pair
	struct Callee { PVOID Handler; PVOID Context; };

	Callee	m_Callback[KTRANSPORT_EVENT_TYPES];

	bool	m_bEnabled;
};

// Usage
#if 0
	PTDI_IND_RECEIVE pFunc = reinterpret_cast<PTDI_IND_RECEIVE> (m_EventCallbacks.Handler(TDI_EVENT_RECEIVE));
	if (pFunc && m_EventCallbacks.IsEnabled())
		(*pFunc) (m_EventCallbacks.Context(TDI_EVENT_RECEIVE), ,...);
#endif

#endif  //(_KTransportIrpDispatch_H__04EA8A0B_73DD_4546_9333_2B052237E909__INCLUDED_)
