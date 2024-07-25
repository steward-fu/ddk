// KTransportPnp.cpp: implementation of the KTransportPnp classes.
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

#undef BINARY_COMPATIBLE
#define BINARY_COMPATIBLE 0

#include <kndis.h>
extern "C" {
 #include <ntddk.h>
 #include <tdi.h>
 #include <tdikrnl.h>
};

#include <KTransportPnp.h>

//////////////////////////////////////////////////////////////////////
// KTransportPnpObject
//

KTransportPnpObject::KTransportPnpObject() : KObject<HANDLE, 'tpnp'>()
{
}

KTransportPnpObject::~KTransportPnpObject()
{
	Deregister();
}

///////////////////////////////////////////////////////////////////////
// Register binding (based on underlying NIC name)
//
// Parameters:
// 	TranspName           - Transport Device name (prefix)
// 	DeviceGUID=NULL      - Optional NIC adapter GUID name in the form \device\{...}
// Returns:
// 	Status
// Comments:
// 	Use this form for designs where no device objects are used per binding.
//  In this case, Register() makes up the name to be registered as
//  TranspName_DeviceGUID, eg "TCPIP_\device\{4d36e972-e325-11ce-bfc1-08002be10318}"
//  The adapter name in the GUID form is usually passed on KNdisProtocolBinding::Open().
//
NTSTATUS KTransportPnpObject::Register(PUNICODE_STRING TranspName,
								PUNICODE_STRING DeviceGUID)
{
	ASSERT(!IsValid() && TranspName);

	unsigned len = TranspName->Length;
	if (DeviceGUID)
		len += (DeviceGUID->Length + 4);

	m_wsName.Initialize((USHORT)len);
	if (!m_wsName.IsValid())
		return STATUS_INSUFFICIENT_RESOURCES;

	m_wsName.Append(TranspName);
	if (DeviceGUID) {
		m_wsName.Append(PUNICODE_STRING (KNDIS_STRING_CONST("_")) );
		m_wsName.Append(DeviceGUID);
	}
	return ::TdiRegisterDeviceObject(m_wsName, &m_Handle);
}


void KTransportPnpObject::Deregister()
{
	if (IsValid()) {
		::TdiDeregisterDeviceObject(m_Handle);
		m_wsName.Invalidate();
		Invalidate();
	}
}


///////////////////////////////////////////////////////////////////////
// Post power event to TDI clients
//
// Parameters:
// 	PowerEvent           - power event object
// 	Context1=NULL        - context (optional)
// 	Context2=NULL        - additional context (optional)
// 	bWait=TRUE           - Syncronous (TRUE) or asynchronous (FALSE) request
// Returns:
// 	STATUS_SUCCESS if all notified clients returned STATUS_SUCCESS to indicate
//  their acceptance of the power-state change.
//	STATUS_PENDING if at least one client will handle this notification
//	asynchronously (when bWait is FALSE only). The callback will be called in this case.
//	STATUS_INSUFFICIENT_RESOURCES - problem with TDI.
// Comments:
// 	This method is usually called from one of KNdisProtocolBinding::OnPnpXxx()
//  handlers to forward the NIC power state changes to PNP-aware TDI clients.
NTSTATUS KTransportPnpObject::PowerRequest(IN PNET_PNP_EVENT PowerEvent,
			   IN PTDI_PNP_CONTEXT Context1,
			   IN PTDI_PNP_CONTEXT Context2,
			   BOOLEAN bWait)
{
	NTSTATUS Status;
	if (bWait) {
		reinterpret_cast<CallbackContext*>(PowerEvent->TransportReserved)->m_this = this;
		reinterpret_cast<CallbackContext*>(PowerEvent->TransportReserved)->m_pStatus = &Status;
		m_Event.Reset();
	}
	Status = ::TdiPnPPowerRequest(m_wsName, PowerEvent, Context1, Context2, CompletionCallback);
	if (Status == STATUS_PENDING && bWait) {
		m_Event.Wait();
	}
	return Status;
}

///////////////////////////////////////////////////////////////////////
// Completion routine (static)
//
// Parameters:
// 	NetEvent             - event passed
// 	FinalStatus          - final status
// Returns:
// 	none
// Comments:
// 	Called from TDI whenever a TDI client decided to complete the PNP notification
//  asynchronously. Delegates to the overridable handler.
VOID KTransportPnpObject::CompletionCallback(IN PNET_PNP_EVENT PowerEvent, IN NTSTATUS FinalStatus)
{
	KTransportPnpObject* obj = reinterpret_cast<CallbackContext*>(PowerEvent->TransportReserved)->m_this;
	if (obj->IsValid())
		obj->CompletionHandler(PowerEvent, FinalStatus);
	else ASSERT(!"Good PNP completion");
}


///////////////////////////////////////////////////////////////////////
// Overridable request completion handler
//
// Parameters:
// 	PowerEvent           - event passed
// 	FinalStatus          - final status
// Returns:
// 	none
// Comments:
// 	The default implementation provides for the synchronous mode by simply
//	setting the internal event, which PowerRequest() is waiting upon.
//  Derive your class from KTransportPnpObject and override CompletionHandler()
//  to immplement some custom asynchronous completion processing,
void KTransportPnpObject::CompletionHandler (IN PNET_PNP_EVENT PowerEvent, IN NTSTATUS FinalStatus)
{
	NTSTATUS* pStatus = reinterpret_cast<CallbackContext*>(PowerEvent->TransportReserved)->m_pStatus;
	if (pStatus)
		*pStatus = FinalStatus;
	m_Event.Set();
}
