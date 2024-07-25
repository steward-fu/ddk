// tdiDtgSocket.cpp
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
// Implements TDI-based datagram "socket"
//

#include	"tdisocket.h"

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pTA                  - Pointer to Transport Address. Default: NULL (OK for client-side sockets)
// 	nBuffers             - Number of NDIS_BUFFER descriptors (messages in-the-flight)
// 	Protocol             - Protocol number according to ipnumbers.h. Default: UDP
// 	szProvider           - Device-provider name. Default: Microsoft UDP device.
// Returns:
// 	none
// Comments:
// 	The default form of the constructor (with no parameters) opens a UDP client
//	side socket with a transport driver-assigned port number.
//  Optionally, the user can specify a specific port number by supplying a pointer
//	to a correct TRANSPORT_ADDRESS. CIPTRANSPORT_ADDRESS class can be conveniently
//	used in this case.
//  Also, the user might want to indicate the maximum number of messages "in-the-flight"
//	(nBuffers) sent over this socket. The framework preallocates the specified number
//	of NDIS buffer descriptors for the socket. The default value is 4. If the number of
//	pending sendto-s exceeds the number of allocated descriptors the messages will be
//	dropped and an error status TDI_NO_RESOURCES will be return on sendto-s until the
//	descriptors become available again. The driver writer should specify an appropriate
//	nBuffers value based on the driver's traffic load requirements.
//	The 'Protocol' parameter indicates the protocol number used for the socket; this
//	should be IPPROTO_UDP (default).
//	Finally, 'szProvider' specifies the device implementing the transport. By default,
//	the framework uses the UDP device of the standard Microsoft TCP/IP driver.
//	After the object has been constructed the user should call IsCreated() to
//	make sure the contsruction succeeded. Note that after successful construction
//	the events on the socket (the OnXxx handlers) are still disabled until the
//	user explicitely enables them with SetEvents(TRUE).
//	The constructor should be called at PASSIVE_LEVEL.
KDatagramSocket::KDatagramSocket(PTRANSPORT_ADDRESS pTA, UINT nBuffers, UINT Protocol,
                                 const char* szProvider)
    : KTdiClient((pTA) ? pTA : CIPTRANSPORT_ADDRESS(0,0), Protocol, nBuffers,
		(szProvider==NULL) ? UDP_DEVICE_NAME : szProvider),
			m_PartialRcv(RcvDGEvent_complete)
{
    if (!IsCreated()) {
        TDI_ASSERT(!"KDatagramSocket"); // problem. e.g. non-existing explicit IP specified
    }
}

///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// Returns:
// Comments:
//	Disables network events and closes the TDI objects associated
//	with the socket.
//	NOTE the user should make sure all pending send requests have completed
//	prior destroying the object. A memory leak might result otherwise.
//	The desstructor should be called at PASSIVE_LEVEL.
KDatagramSocket::~KDatagramSocket()
{
    // TODO: Check for pending reqs. How?
	SetEvents(FALSE);
}


///////////////////////////////////////////////////////////////////////
// Send a datagram
//
// Parameters:
// 	pConn                - Destination Info (port/address)
// 	pBuf                 - Message to send (page-locked buffer pointer)
// 	Size                 - Size of the message in bytes
// 	pCxt                 - User context (opaque to the framework)
// Returns:
// 	TDI_PENDING if the message has been successfully submitted for transmission (normal)
//  TDI_SUCCESS if the message has been sent (unlikely)
//	TDI error code (e.g. TDI_NO_RESOURCES) when the message can not be sent.
// Comments:
// 	When the user calls sendto(), the network transport attempts to send
//	the message to the specified remote recipient.
//	The recipient is defined by 'pConn' parameter. 'pConn' can be conveniently initialized
//	using the CTDI_CONNECTION_INFORMATION class, which takes as an input
//	the UDP port (16-bit integer) and IP address (32-bit integer).
//	The message specified by the buffer pointer 'pBuf' and length 'Size' should
//	be located in the non-paged memory and should not be touched until the send
//	request completes via On_sendtoComplete().
//	The user passes an opaque to the framework 'pCxt' pointer that gets forwarded
//	all the way back to the On_sendtoComplete() completion routine. Usually,
//	pCxt is used to point to dynamically allocated objects managed by the
//	user's driver using the socket.
//	sendto() should be called at IRQL <= DISPATCH_LEVEL.
TDI_STATUS KDatagramSocket::sendto(PTDI_CONNECTION_INFORMATION pConn,
                             void* pBuf, UINT Size, PVOID pCxt)
{
    TDI_STATUS Status;

	// alloc buf descr & operation context
	PNDIS_BUFFER pNdisBuf = AllocBuf(pBuf, Size);
	PTDI_REQCXT pTdiCxt = new (m_pContextStorage) TDI_REQCXT (this, pNdisBuf, pCxt);

	if (pNdisBuf && pTdiCxt)
	{
        TDIREQ	Req(m_Handle, pTdiCxt, sendto_complete);
    	UINT	bytesSent = 0;

		Status = TdiSendDatagram(Req, pConn, Size, &bytesSent, pNdisBuf);
#if TDI_WINDOWS_95
		if ( Status != TDI_PENDING )
		{
			On_sendtoComplete(pCxt, Status, bytesSent);
			Status = TDI_PENDING;
		}
#endif

#if TDI_WINDOWS_NT
        if (Status == STATUS_INSUFFICIENT_RESOURCES)
		{
			// free the allocated stuff if done
			delete pTdiCxt;
        	FreeBuf(pNdisBuf);
			On_sendtoComplete(pCxt, Status, bytesSent);
		}
#endif
    }
	else
	{
		// problem with resources
    	TDI_TRACE("KDatagramSocket::sendto(): No resources\n");
		if (pTdiCxt)
		{
			TDI_ASSERT(pNdisBuf);
			delete pTdiCxt;
		}
		if (pNdisBuf)
		{
			TDI_ASSERT(pTdiCxt);
        	FreeBuf(pNdisBuf);
		}

		On_sendtoComplete(pCxt, TDI_NO_RESOURCES, 0);
#if TDI_WINDOWS_95
		Status = TDI_PENDING;
#endif
#if TDI_WINDOWS_NT
		Status = TDI_NO_RESOURCES;
#endif
    }

	return Status;
}

///////////////////////////////////////////////////////////////////////
// KDatagramSocket::recvfrom()
//
// Explicit receive datagram request. Rarely used. MS recommends to use
// event callbacks instead.
//
// Parameters:
//     See the TDI spec
// Returns:
//     TDI_STATUS
//

///////////////////////////////////////////////////////////////////////
// Receive datagram request (rarely used)
//
// Parameters:
// 	pFilterInfo          -
// 	pFromInfo            -
// 	pBuf                 -
// 	RcvSize              -
// 	pBytesReceived       -
// 	pCxt                 -
// Returns:
// 	TDI_STATUS
// Comments:
// 	Submits an asynchronous receive request to the transport. On completion,
//	the On_recvfromComplete() handler is to be called unless the transport
//	already has received data intended for this socket.
//  NOTE: This form is NOT recommended.
//	The drivers should use the receive event notfications. See OnReceive().
TDI_STATUS KDatagramSocket::recvfrom(PTDI_CONNECTION_INFORMATION pFilterInfo,
                  PTDI_CONNECTION_INFORMATION pFromInfo,
                    void* pBuf, UINT RcvSize, UINT* pBytesReceived, void* pCxt)
{
	TDI_STATUS Status;

	// alloc buf descr & operation context
	PNDIS_BUFFER pNdisBuf = AllocBuf(pBuf, RcvSize);
	PTDI_REQCXT pTdiCxt = new (m_pContextStorage) TDI_REQCXT (this, pNdisBuf, pCxt);

	if (pNdisBuf && pTdiCxt) {
    	TDIREQ	Req(m_Handle, pTdiCxt, recvfrom_complete);
    	Status = TdiReceiveDatagram(Req, pFilterInfo, pFromInfo,
                    	RcvSize, pBytesReceived, pNdisBuf);
#if TDI_WINDOWS_95
		if ( Status != TDI_PENDING )
		{
			On_recvfromComplete(pCxt, Status, *pBytesReceived);
			Status = TDI_PENDING;
		}
#endif
#if 0
    	if (Status != TDI_PENDING)
		{
			// free the allocated stuff if done
			delete pTdiCxt;
        	FreeBuf(pNdisBuf);
		}
#endif
    }
	else
	{
		// problem with resources
    	TDI_TRACE("KDatagramSocket::recvfrom(): No resources\n");
		if (pTdiCxt)
		{
			TDI_ASSERT(pNdisBuf);
			delete pTdiCxt;
		}
		if (pNdisBuf)
		{
			TDI_ASSERT(pTdiCxt);
        	FreeBuf(pNdisBuf);
		}
		On_recvfromComplete(pCxt, TDI_NO_RESOURCES, 0);
#if TDI_WINDOWS_95
		Status = TDI_PENDING;
#endif
#if TDI_WINDOWS_NT
		Status = TDI_NO_RESOURCES;
#endif
    }
	return Status;
}

///////////////////////////////////////////////////////////////////////
// Enable/Disable network event notification
//
// Parameters:
// 	On                   - TRUE to enable events, FALSE to disable.
// Returns:
// 	none
// Comments:
// 	For KDatagramSocket, SetEvents(TRUE) enables notifications on
//	data arrived for the socket. The user might disable and/or reenable
//	events anytime.
//	SetEvents should be called at IRQL <= DISPATCH_LEVEL.
void KDatagramSocket::SetEvents(BOOLEAN On)
{
    // Turn on/off receiving datagrams
	TdiSetEvent(m_Handle, TDI_EVENT_RECEIVE_DATAGRAM, (On) ? RcvDGEvent : NULL, this);
}

///////////////////////////////////////////////////////////////////////
// Join an IP multicast group.
////
// Parameters:
//     IpMGroup  -   IP muticast addrress to join (224.0.0.X), in net order.
//     IpIFace	 -	 IP interface to join at (0 for default), in net order.
// Returns:
//     TDI_STATUS
// Comments:
//	Should be called at PASSIVE_LEVEL
TDI_STATUS KDatagramSocket::JoinMGroup(ULONG IpMGroup, ULONG IpIFace)
{
    struct {
        ULONG  imr_multiaddr;  /* IP multicast address of group */
        ULONG  imr_interface;  /* local IP address of interface */
    } mreq;

    mreq.imr_multiaddr = IpMGroup;
    mreq.imr_interface = IpIFace;

    return SetOption(CL_TL_ENTITY, INFO_CLASS_PROTOCOL,
						INFO_TYPE_ADDRESS_OBJECT, AO_OPTION_ADD_MCAST,
                            &mreq, sizeof(mreq));
}

///////////////////////////////////////////////////////////////////////
// Leave an IP multicast group
//
// Parameters:
//     IpMGroup  -   IP muticast addrress (224.0.0.X), in net order.
//     IpIFace	 -	 IP interface at (0 for default), in net order.
// Returns:
//     TDI_STATUS
// Comments:
//	Should be called at PASSIVE_LEVEL
TDI_STATUS KDatagramSocket::LeaveMGroup(ULONG IpMGroup, ULONG IpIFace)
{
    struct {
        ULONG  imr_multiaddr;  /* IP multicast address of group */
        ULONG  imr_interface;  /* local IP address of interface */
    } mreq;

    mreq.imr_multiaddr = IpMGroup;
    mreq.imr_interface = IpIFace;

    return SetOption(CL_TL_ENTITY, INFO_CLASS_PROTOCOL,
						INFO_TYPE_ADDRESS_OBJECT, AO_OPTION_DEL_MCAST,
                            &mreq, sizeof(mreq));
}


///////////////////////////////////////////////////////////////////////
// internal callbacks                                                 /
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// KDatagramSocket::sendto_complete(void* pContext, uint Status, uint ByteCount)
//
// Send completed. Delegates the callback to a derived class.
//
// Parameters:
//    pContext    -   object context
//    Status      -   TDI result code
//    ByteCount   -   byte counter
//Returns:
//    none
//
TDI_COMPLETE_CALLBACK (KDatagramSocket::sendto_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	PTDI_REQCXT pTdiCxt = reinterpret_cast<TDI_REQCXT*>(pContext);
	TDI_ASSERT(pTdiCxt);
	KDatagramSocket* pClient = reinterpret_cast<KDatagramSocket*> (pTdiCxt->This);
	PVOID UContext = pTdiCxt->User;
	PNDIS_BUFFER pBuf = (PNDIS_BUFFER) pTdiCxt->Buffer;
    if (Status)
        TDI_TRACE("KDatagramSocket::sendto_complete() with err %d\n", Status);
    if (pClient->IsValid()) {
		delete pTdiCxt;
        pClient->FreeBuf(pBuf);
        pClient->On_sendtoComplete(UContext, Status, (ULONG)ByteCount);
    }
	else {
	    TDI_ASSERT(!"Good client");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}

///////////////////////////////////////////////////////////////////////
// KDatagramSocket::recvfrom_complete(void* pContext, uint Status, uint ByteCount)
//
//  Solicited datagram receive complete callback. Delegates the
//  callback to the derived class.
//
// Parameters:
//     pContext    -   object context
//     Status      -   TDI result code
//     ByteCount   -   byte counter
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KDatagramSocket::recvfrom_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	PTDI_REQCXT pTdiCxt = reinterpret_cast<TDI_REQCXT*>(pContext);
	TDI_ASSERT(pTdiCxt);
	KDatagramSocket* pClient = reinterpret_cast<KDatagramSocket*> (pTdiCxt->This);
	PNDIS_BUFFER pBuf = (PNDIS_BUFFER) pTdiCxt->Buffer;
	PVOID UContext = pTdiCxt->User;
    TDI_ASSERT(pClient->IsValid());
    if (pClient->IsValid()) {
		delete pTdiCxt;
    	pClient->FreeBuf(pBuf);
        pClient->On_recvfromComplete(UContext, Status, (ULONG)ByteCount);
    }
	else {
	    TDI_ASSERT(!"Good client");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}

///////////////////////////////////////////////////////////////////////
// KDatagramSocket::RcvDGEvent_complete(void* pContext, uint Status, uint ByteCount)
//
// A partially indicated TSDU receive has completed. Delegate to the client's
// derived object.
//
// Parameters:
//     pContext    -   object context
//     Status      -   TDI result code
//     ByteCount   -   byte counter
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KDatagramSocket::RcvDGEvent_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	KDatagramSocket* pClient = (KDatagramSocket*) pContext;

    if (pClient->IsValid())
	{

		// retrieve the buffer for indication
		KTdiPartialReceive* pRcvBuf = &pClient->m_PartialRcv;
		PNDIS_BUFFER pBuf = *pRcvBuf;

		uchar* buf;
		uint buflen; // >= ByteCount
		NdisQueryBuffer(pBuf, (PVOID*) &buf, &buflen);

		// free the descriptor used for the transfer
		pClient->FreeBuf(pBuf);

		// indicate to the client
		pClient->OnReceiveComplete (Status,
#ifdef TDI_WINDOWS_95
				pRcvBuf->m_uTAlength, pRcvBuf->m_TA,
				pRcvBuf->m_uOptlength, (pRcvBuf->m_uOptlength) ? pRcvBuf->m_Options : NULL,
				ByteCount, buf);
#endif
#ifdef TDI_WINDOWS_NT
				pRcvBuf->m_RetInfo.RemoteAddressLength, (PTRANSPORT_ADDRESS) pRcvBuf->m_RetInfo.RemoteAddress,
				pRcvBuf->m_RetInfo.OptionsLength, pRcvBuf->m_RetInfo.Options,
				(ULONG)ByteCount, buf);
#endif
	}
	else TDI_ASSERT(!"pClient");

    TDI_COMPLETE_CALLBACK_END(pClient);
}

///////////////////////////////////////////////////////////////////////
// KDatagramSocket::RcvDGEvent()
//
//  Datagram Event Callback: datagram rcv data ara available.
//  In NT, the last parameter, Buffer, is of type OUT PIRP. We should submit
//  a ReceiveData IRP to let the receive complete.
// Parameters:
//     See the TDI Spec
// Returns:
//     TDI_STATUS
//
TDI_STATUS KDatagramSocket::RcvDGEvent(PVOID EventContext,
                	uint AddressLength,
                	PTRANSPORT_ADDRESS Address,
                	uint OptionsLength, PVOID Options,  uint Flags,
                	uint Indicated, uint Available,
                	uint *Taken, uchar *Data,
					EventRcvBuffer/*IRP*/ **Buffer)
{
	UNREFERENCED_PARAMETER(Flags);

	KDatagramSocket* pClient = (KDatagramSocket*)EventContext;
    TDI_ASSERT(pClient->IsValid());
    if (!pClient->IsValid())
        return TDI_NOT_ACCEPTED;

	*Buffer = NULL;

	uchar* RcvBuffer=NULL;
	uint RcvSize=(uint)Flags; // piggyback

	// Call the client
	*Taken = pClient->OnReceive(AddressLength, Address,
		OptionsLength, Options,
		Indicated, Data,
		Available, &RcvBuffer, &RcvSize);

	// If all the available data are indicated return now
	if (Available <= Indicated)
		return (*Taken != uint(-1)) ? TDI_SUCCESS : TDI_NOT_ACCEPTED;

	// Process the lookahead indication, if the buffer is supplied.
	// If not, the data will be lost

	if (RcvBuffer==NULL || RcvSize==NULL)
		return (*Taken != uint(-1)) ? TDI_SUCCESS : TDI_NOT_ACCEPTED;

	// The client has provided a buffer to schedule the receive for more data.
	// Describe and schedule receive on this buffer

	if (RcvSize > Available)
		RcvSize = Available;

	PNDIS_BUFFER pBuf = pClient->AllocBuf(RcvBuffer, RcvSize);
	if (!pBuf) {
		TDI_ASSERT(!"Buffers");
		pClient->OnReceiveComplete(TDI_NO_RESOURCES, 0, NULL, 0, NULL, 0, RcvBuffer);
		return TDI_SUCCESS; // call client?
	}

	// Cache the indicated datagram attributes

	pClient->m_PartialRcv.Cache(AddressLength, Address,
		OptionsLength, Options,
		pBuf, pClient);

	// Schudule receive on this buffer

#ifdef TDI_WINDOWS_95
	// simply point to the ERB and return "more processing"
	*Buffer = pClient->m_PartialRcv;
#endif
#ifdef TDI_WINDOWS_NT
	// Build the TDI_RECEIVE_DATAGRAM irp for the remainder of TSDU
	PIRP irp = pClient->AllocIrp();
	if (irp == NULL)
	{ TDI_ASSERT(!"irp");
	  pClient->FreeBuf(pBuf);
	  pClient->OnReceiveComplete(TDI_NO_RESOURCES, 0, NULL, 0, NULL, 0, RcvBuffer);
	  return TDI_NOT_ACCEPTED;
	}

	TdiBuildReceiveDatagram (irp,
		pClient->m_pDevice,
		pClient->m_pfileTA,
		(PIO_COMPLETION_ROUTINE) RcvDGEvent_complete,
		pClient,
		pBuf,
		0,	/*RcvSize*/
		&pClient->m_PartialRcv.m_ReqInfo,
		&pClient->m_PartialRcv.m_RetInfo,
		TDI_RECEIVE_NORMAL);

	// Advance IRP's stack location since we do NOT IoCallDriver here:
	IoSetNextIrpStackLocation(irp);
	*Buffer = irp;
#endif
	return TDI_MORE_PROCESSING;
}


///////////////// Default handler implementations ///////////////////
// NOTE: Those are virtuals usually overridden by the derived class /
/////////////////////////////////////////////////////////////////////
#pragma warning (disable:4100)	// non-ref formals

///////////////////////////////////////////////////////////////////////
// Send Completion (handler)
//
// Parameters:
// 	pCxt                 - User context passed on sendto()
// 	Status               - Status Code (result) of the request
// 	ByteCount            - Bytes actually transferred
// Returns:
// 	none
// Comments:
// 	The framework calls this handler when the associated sendto() request has
//	completed. The user overrides this handler to perform custom processing
//	of send completions. Usually, the handler frees or recycles some internal
//	resources identified by 'pCxt'. E.g., it could be the pointer to a buffer
//	allocated for this transfer.
//	On successful return, 'Status' will be equal to TDI_SUCCESS (zero) and ByteCount
//	to the length of the buffer submitted with sendto().
//	Note that for datagram transports, a successful completion means only that the
//	message has been put on the network for transfer; it does not guarantee a reception
//	of the datagram by the remote destination.
//	On_sendtoComplete() is called at the DISPATCH_LEVEL
void KDatagramSocket::On_sendtoComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount) {};

///////////////////////////////////////////////////////////////////////
// Recvfrom Completion (handler) - rarely used
//
// Parameters:
// 	pCxt                 - User context passed on recvfrom()
// 	Status               - Status Code (result) of the request
// 	ByteCount            - Bytes actually received
// Returns:
// Returns:
// 	none
// Comments:
// 	The recvfrom() is not recommended. Use OnReceive() notifications.
void KDatagramSocket::On_recvfromComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount) {};

///////////////////////////////////////////////////////////////////////
// Receive Indication (handler)
//
// Parameters:
// 	AddressLength        - Length in bytes of the sender's transport address (pTA)
// 	pTA                  - Pointer to the sender's transport address
// 	OptionsLength        - Length in bytes of the IP options data associated with the datagram (usually, 0)
// 	Options              - Pointer to the IP options data associated with the datagram (usually, 0)
// 	Indicated            - Number of bytes being indicated (length of the datagram)
// 	Data                 - Pointer to the datagram buffer
// 	Available            - Number of bytes available (Available > Indicated)
// 	RcvBuffer            - Pointer to the pointer to a received buffer to get the rest of the TSDU (output param)
// Returns:
// 	Number of bytes consumed, or 0 to reject the datagram
// Comments:
// 	OnReceive() is called by the framework to indicate the reception of a complete
//	TSDU (datagram) by the transport. The user overrides OnReceive() to implement
//	custom receive processing. After examining the buffer indicated by 'Data', the
//	user should return a number of bytes "consumed"; usually it is equal to 'Indicated'.
//
//  OnReceive() returns the number of bytes accepted in this indication (less or
//  equal to Indicated). In addition, if the client is interested in the rest of available
//  data, the client provides a paged-locked buffer, RcvBuffer/RcvBufferLen, to
//  receive the rest of data into. If the client choses to do so, the framework schedules an
//  IRP to asynchronously move the available data from the transport. When the IRP completes,
//  the framework calls OnReceveComplete().
//
//	OnReceive() is called at DISPATCH_LEVEL. Whenever a lengthy processing is required to
//	process the received message, the driver should schedule a work item to do so.
uint KDatagramSocket::OnReceive(uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options,
                	uint Indicated, uchar *Data,
					uint Available, uchar **RcvBuffer, uint* RcvBufferLen)
{
	TDI_ASSERT(!"KDatagramSocket::OnReceive(): handler?\n");
	UNREFERENCED_PARAMETERS6(AddressLength,pTA,OptionsLength,Options,Indicated,Data);
	UNREFERENCED_PARAMETERS2(RcvBuffer, RcvBufferLen);
	return 0; // none read: discarded
}


///////////////////////////////////////////////////////////////////////
// Partial recieve completion (handler)
//
// Parameters:
// 	AddressLength        - Length in bytes of the sender's transport address (pTA)
// 	pTA                  - Pointer to the sender's transport address
// 	OptionsLength        - Length in bytes of the IP options data associated with the datagram (usually, 0)
// 	Options              - Pointer to the IP options data associated with the datagram (usually, 0)
// 	Indicated            - Number of bytes being indicated (length of the datagram)
// 	Data                 - Pointer to the datagram buffer
// Returns:
// 	none
// Comments:
// 	OnReceiveComplete() is called as a result of the request to retrieve partially indicated
//	datagram from OnReceive().
void KDatagramSocket::OnReceiveComplete(TDI_STATUS Status,
					uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options,
                	uint Indicated, uchar *Data)
{
	TDI_ASSERT(!"KDatagramSocket::OnReceiveComplete(): handler?\n");
	UNREFERENCED_PARAMETERS6(AddressLength,pTA,OptionsLength,Options,Indicated,Data);
}

#pragma warning (default:4100)	// non-ref formals
