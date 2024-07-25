// tdiStrSocket.cpp
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
// Implements TDI-based stream "socket". Client-side.
//

#include	"tdisocket.h"

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pTA                  - Pointer to Transport Address. Default: NULL (OK for client-side sockets)
// 	nBuffers             - Number of NDIS_BUFFER descriptors ("send-s" in-the-flight)
// 	Protocol             - Protocol number according to ipnumbers.h. Default: TCP
// 	szProvider           - Device-provider name. Default: Microsoft TCP device.
// Returns:
// 	none
// Comments:
// 	The default form of the constructor (with no parameters) opens a TCP client
//	side socket with a transport driver-assigned port number.
//  Optionally, the user can specify a specific port number by supplying a pointer
//	to a correct TRANSPORT_ADDRESS. CIPTRANSPORT_ADDRESS class can be conveniently
//	used in this case.
//  Also, the user might want to indicate the maximum number of send-s "in-the-flight"
//	(nBuffers) sent over this socket. The framework preallocates the specified number
//	of NDIS buffer descriptors for the socket. If the number of
//	pending send-s exceeds the number of allocated descriptors the messages will be
//	dropped and an error status TDI_NO_RESOURCES will be return on send-s until the
//	descriptors become available again. The driver writer should specify an appropriate
//	nBuffers value based on the driver's traffic load requirements.
//	The 'Protocol' parameter indicates the protocol number used for the socket; this
//	should be IPPROTO_TCP (default).
//	Finally, 'szProvider' specifies the device implementing the transport. By default,
//	the framework uses the TCP device of the standard Microsoft TCP/IP driver.
//	After the object has been constructed the user should call IsCreated() to
//	make sure the contsruction succeeded. Note that after successful construction
//	the events on the socket (the OnXxx handlers) are still disabled until the
//	user explicitely enables them with SetEvents(TRUE).
//	The constructor should be called at PASSIVE_LEVEL.
KStreamSocket::KStreamSocket(PTRANSPORT_ADDRESS pTA, UINT nBuffers, UINT Protocol,
                             const char* szProvider)
	: KTdiClient((pTA) ? pTA : CIPTRANSPORT_ADDRESS(0,0), Protocol, nBuffers,
		(szProvider==NULL) ? TCP_DEVICE_NAME : szProvider),
			m_PartialRcv(ReceiveEvent_complete),
    			m_hConnectionCxt(0), m_State(CONN_IDLE), m_pConnAddr(NULL), m_uTimeout(0)
{
 	if (KTdiClient::IsCreated()) {
    	SYNCTDIREQ	Req(m_Handle,this);
    	TDI_STATUS Status = TdiOpenConnection(Req, this);
    	if (Status != TDI_SUCCESS) {
        	TDI_TRACE("KTdiClient::TdiOpenConnection() failed with err %d\n", Status);
        	return;
        }
    	Status = TdiAssociateAddress(Req, m_Handle);
    	if (Status != TDI_SUCCESS) {
        	TDI_TRACE("KTdiClient::TdiAssociateAddress() failed with err %d\n", Status);
        	return;
        }
	m_hConnectionCxt = Req.ConnectionContext();
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
KStreamSocket::~KStreamSocket()
{
	SetEvents(FALSE);

	if (m_hConnectionCxt) {

	    // Re TdiDisAssociateAddress: "The connection object must not be connected to a
		// remote peer when this primitive is called" [MSTCP,p.11]

		TDI_ASSERT(m_State <= CONN_REQUESTING);

    	SYNCTDIREQ	Req(m_hConnectionCxt, this);
        TDI_STATUS Status = TdiDisAssociateAddress(Req);

		// Emperically: at NT,W2K,95,98 TdiDisAssociateAddress/TdiCloseConnection
		// are usually synchronous; at Me asynchronous. Thus, do it at PASSIVE only.

		if (Status == TDI_PENDING) {
			Status = Req.Wait();
		}

        if (Status == TDI_SUCCESS) {
			Req.Reinit(m_hConnectionCxt, this);
            Status = TdiCloseConnection(Req);
			if (Status == TDI_PENDING) {
				Status = Req.Wait();
			}
        }
        else {
            // TDI_CONNECTION_ACTIVE is a normal status in this framework...
            if (Status != TDI_CONNECTION_ACTIVE) {
                TDI_TRACE("KStreamSocket::~KStreamSocket() possible resource leak due to TDI err %d\n", Status);
			}
        }
    }

   	delete m_pConnAddr;
}


///////////////////////////////////////////////////////////////////////
// Copy constructor
//
// Parameters:
// 	s                    - source socket
// Returns:
// 	none
// Comments:
//    Creates a new non-connected KStreamSocket object mapped to the TA object's
//    of the source. The object is marked as "clone" in the base class, so
//    destruction of it does not cause the destruction of TA.
KStreamSocket::KStreamSocket(const KStreamSocket& s) : KTdiClient(s),
	m_PartialRcv(ReceiveEvent_complete),
		m_hConnectionCxt(0), m_State(CONN_IDLE), m_pConnAddr(NULL), m_uTimeout(s.m_uTimeout)
{
}

///////////////////////////////////////////////////////////////////////
// Enable/Disable network event notification
//
// Parameters:
// 	On                   - TRUE to enable events, FALSE to disable.
// Returns:
// 	none
// Comments:
// 	For KStreamSocket, SetEvents(TRUE) enables notifications on the
//	following events:
//
//		Connect Indication:		OnConnect() handler
//		Disconnect Indication:	OnDisconnect() handler
//		Recieve Data:			OnReceive() handler.
//
//	The user might disable and/or reenable events anytime.
//	SetEvents should be called at IRQL <= DISPATCH_LEVEL.
void KStreamSocket::SetEvents(BOOLEAN On)
{
  	TdiSetEvent(m_Handle, TDI_EVENT_CONNECT, (On) ? ConnectEvent : NULL, this);
	TdiSetEvent(m_Handle, TDI_EVENT_DISCONNECT, (On) ? DisconnectEvent : NULL, this);
	TdiSetEvent(m_Handle, TDI_EVENT_RECEIVE, (On) ? RcvEvent : NULL, this);
	TdiSetEvent(m_Handle, TDI_EVENT_RECEIVE_EXPEDITED , (On) ? RcvExpeditedEvent : NULL, this);
}

///////////////////////////////////////////////////////////////////////
// Establish connection
//
// Parameters:
// 	RequestAddr          - Destination (server) address, e.g. IP addr/port pair
// 	TimeoutMs            - Timeout for the operation in seconds
// Returns:
// 	TDI_PENDING:	Connection request has been transmitted to the server (normal)
//	TDI_SUCCESS:	Connection has been established (unlikely)
//	TDI_ error code: an error occured (e.g. TDI_NO_RESOURCES)
// Comments:
// 	connect() submits a request to establish connection (TCP connection for a TCP
//	socket) with a remote peer.
//	The 'RequestAddr' parameter specifies the address (TCP port, IP address) of the remote.
//	Class CTDI_CONNECTION_INFORMATION could be conveniently used to build RequestAddr
//	from the given IP address/port pair.
//	The timeout 'TimeoutMs' is optional; when it is 0, a transport-chosen timeout
//	is used. Emperically, it's about 1 minute for the standard TCP/IP driver for
//	the default.
//	When the connect request gets completed, On_connectComplete() handler is called.
//	connect() should be called at IRQL <= DISPATCH_LEVEL.
TDI_STATUS	KStreamSocket::connect(PTDI_CONNECTION_INFORMATION RequestAddr, UINT TimeoutMs)
{
	TDI_ASSERT(m_State == CONN_IDLE);    // reality check
	if (m_State != CONN_IDLE)
	{
		On_connectComplete(this, TDI_INVALID_REQUEST, 0);
#if TDI_WINDOWS_95
		return TDI_PENDING;
#endif
#if TDI_WINDOWS_NT
    	return TDI_INVALID_REQUEST;
#endif
	}

	if (m_pConnAddr == NULL)            // allocate it for the TDI's use
    {
    	TDI_ADDRESS_IP bogus = {0,0};
    	m_pConnAddr =  new CTDI_CONNECTION_INFORMATION(bogus);
    }
	if (!m_pConnAddr)
	{
		On_connectComplete(this, TDI_NO_RESOURCES, 0);
#if TDI_WINDOWS_95
		return TDI_PENDING;
#endif
#if TDI_WINDOWS_NT
    	return TDI_NO_RESOURCES;
#endif
	}

	UINT	Timeout = TimeoutMs;        // 0 means "up to the transport", [MSTCP,p.11]
	TDIREQ	Req(m_hConnectionCxt, this, connect_complete);

    m_uTimeout = TimeoutMs;             // save timeout used
	m_State = CONN_REQUESTING;
	TDI_STATUS Status = TdiConnect(Req, &Timeout, RequestAddr, *m_pConnAddr);

#if TDI_WINDOWS_95
	if ( Status != TDI_PENDING )
	{
		On_connectComplete(this, Status, 0);
		Status = TDI_PENDING;
	}
#endif

	if (Status != TDI_PENDING) {
    	if (Status == TDI_SUCCESS)
        {
        	m_State = CONN_ACTIVE;        // unlikely, but possible
        }
    	else {
        	TDI_TRACE("KStreamSocket::connect() failed, err=%X\n", Status);
        	m_State = CONN_IDLE;
        }
    }

	return Status;
}

///////////////////////////////////////////////////////////////////////
// Drop connection
//
// Parameters:
// 	bAbort               - abortive (TRUE) vs. graceful (FALSE) disconnect
// 	TimeoutMs            - Timeout in msec
// Returns:
// 	TDI_PENDING:	disconnect request has been sent to the peer (normal for graceful disconnects)
//	TDI_SUCCESS:	disconnected (could be the case for abortive disconnects)
//	TDI_ error code: can't perform operation
// Comments:
// 	Parameter bAbort determines whether RST (abortive) and FIN (graceful) form of
//	disconnect should be used.
//	When a graceful disconnect (FIN) is used the connection gets into TIME_WAIT state
//	internally. There is NO indication from TDI as to when the connection went back
//  to IDLE, and this might take minutes. An attempt to call TdiDisAssociateAddress()
//	in this state returns error TDI_CONNECTION_ACTIVE, so TdiCloseConnection() can't be
//  called either.
//	To avoid this problems kernel mode drivers should use abortive disconnects.
//  The simplest solution: always use non-graceful (RST) disconnect in the kernel.
TDI_STATUS	KStreamSocket::disconnect(BOOLEAN bAbort, UINT TimeoutMs)
{
	TDIREQ	Req(m_hConnectionCxt, this, disconnect_complete);
	ushort  Flags = (bAbort) ? TDI_DISCONNECT_ABORT : TDI_DISCONNECT_WAIT;
    UINT	Timeout = (TimeoutMs) ? TimeoutMs : m_uTimeout;

	m_State = CONN_DISCONNECTING;
	TDI_STATUS Status = TdiDisconnect(Req, &Timeout, Flags, NULL, NULL);

#if TDI_WINDOWS_95
	if ( Status != TDI_PENDING )
	{
		On_disconnectComplete(this, Status, 0);
		Status = TDI_PENDING;
	}
#endif

	if (Status != TDI_PENDING) {
    	if (Status != TDI_SUCCESS && Status != TDI_GRACEFUL_DISC)
        	TDI_TRACE("KStreamSocket::disconnect() failed with err %d\n", Status);
    	m_State = CONN_IDLE;                        // unlikely, but possible
    }

	return Status;
}


///////////////////////////////////////////////////////////////////////
// Send data over connection
//
// Parameters:
// 	pBuf                 - Data block to send (page-locked buffer pointer)
// 	Size                 - Size of the data block in bytes
// 	pCxt                 - User context (opaque to the framework)
// 	Flags                - optional TCP flags used for send.
// Returns:
// 	TDI_PENDING if the data has been successfully submitted for transmission (normal)
//  TDI_SUCCESS if the data has been sent (unlikely)
//	TDI error code (e.g. TDI_NO_RESOURCES) when the data can not be sent.
// Comments:
// 	When the user calls send(), the network transport attempts to send
//	the data block over the connectiion.
//	The data block specified by the buffer pointer 'pBuf' and length 'Size' should
//	be located in the non-paged memory and should not be touched until the send
//	request completes via On_sendComplete().
//	The user passes an opaque to the framework 'pCxt' pointer that gets forwarded
//	all the way back to the On_sendComplete() completion routine. Usually,
//	pCxt is used to point to dynamically allocated objects managed by the
//	user's driver using the socket.
//	Note that unlike datagram (connectionless) transports, the stream-oriented transport
//	does not guarantee one-to-one correspondence between data blocks submitted by the
//	sender and ones received by the recepient. The transport does guarantee the orderly
//	reliable delivery of all the bytes submitted by the sender. The receiver, however,
//	must not assume that the receive notifications are delivered in the exact correspondence
//	to the send requests issued by the sender.
//	send() should be called at IRQL <= DISPATCH_LEVEL.
TDI_STATUS	KStreamSocket::send(void* pBuf, UINT Size, PVOID pCxt, USHORT Flags)
{
	TDI_STATUS Status;
	TDI_ASSERT(m_State == CONN_ACTIVE);                        // reality check

	// alloc buf descr & operation context
	PNDIS_BUFFER pNdisBuf = AllocBuf(pBuf, Size);
	PTDI_REQCXT pTdiCxt = new (m_pContextStorage) TDI_REQCXT (this, pNdisBuf, pCxt);

	if (pNdisBuf && pTdiCxt)
	{
		// build the req
    	TDIREQ	Req(m_hConnectionCxt, pTdiCxt, send_complete);
    	Status = TdiSend(Req, Flags, Size, pNdisBuf);
#if TDI_WINDOWS_95
		if ( Status != TDI_PENDING )
		{
			On_sendComplete(pCxt, Status, Size);
			Status = TDI_PENDING;
		}
#endif
#if 0
		if (Status != TDI_PENDING) {  // free the allocated stuff if done
			delete pTdiCxt;
        	FreeBuf(pNdisBuf);
		}
#endif
    }
	else
	{
		// problem with resources
    	TDI_TRACE("KStreamSocket::send(): No resources\n");
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

		On_sendComplete(pCxt, TDI_NO_RESOURCES, 0);
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
// Solicit an explicit receive
//
// Parameters:
// 	buf                  - buffer to receive data
// 	bytecnt              - length of the buffer
// 	Flags                - flags (optional)
// Returns:
// 	TDI_STATUS
// Comments:
//	Do NOT use this request for purposes other than implementing
//	a flow control mechanism for the receive path. When the server runs out
//	of input buffers, it might trigger a control flow event in OnReceive() handler,
//	which supresses further receive indications and, in turn, causes the transport
//  protocol flow control to activate. When buffers become available, the server
//	issues recv(), usually at PASSIVE_LEVEL, which unlocks the transport driver
//  receive indications.
//	When the request completes, the overridable On_recvComplete() handler is called.
TDI_STATUS KStreamSocket::recv(void* buf, UINT bytecnt, ULONG Flags)
{
	TDI_STATUS Status;

	// alloc buf descr & operation contex
	PNDIS_BUFFER pNdisBuf = AllocBuf(buf, bytecnt);
	PTDI_REQCXT pTdiCxt = new (m_pContextStorage) TDI_REQCXT (this, pNdisBuf, buf /*user cxt */);

	if (pNdisBuf && pTdiCxt)
	{
    	TDIREQ	Req(m_hConnectionCxt, pTdiCxt, recv_complete);
    	Status = TdiReceive(Req, (ushort*) &Flags, (uint*) &bytecnt, pNdisBuf);

#if TDI_WINDOWS_95
		if ( Status != TDI_PENDING )
		{
			On_recvComplete(buf, Status, bytecnt);
			Status = TDI_PENDING;
		}
#endif
#if 0
		if (Status != TDI_PENDING) {  // free the allocated stuff if done
			delete pTdiCxt;
        	FreeBuf(pNdisBuf);
		}
#endif
    }
	else
	{
		// problem with resources
    	TDI_TRACE("KStreamSocket::recv(): No resources\n");
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

		On_recvComplete(buf, TDI_NO_RESOURCES, 0);
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
// Get connection information
//
// Parameters:
// 	info                 - reference to TDI_CONNECTION_INFO to be filled in
// Returns:
// 	TDI_SUCCESS if the request succeeded. error code otherwise.
// Comments:
// 	Assumes the transport always completes this request synchronously.
//
// typedef struct _TDI_CONNECTION_INFO {
//   ULONG State;                        // current state of the connection.
//   ULONG Event;                        // last event on the connection.
//    ULONG TransmittedTsdus;             // TSDUs sent on this connection.
//   ULONG ReceivedTsdus;                // TSDUs received on this connection.
//    ULONG TransmissionErrors;           // TSDUs transmitted in error/this connection.
//    ULONG ReceiveErrors;                // TSDUs received in error/this connection.
//    LARGE_INTEGER Throughput;           // estimated throughput on this connection.
//    LARGE_INTEGER Delay;                // estimated delay on this connection.
//    ULONG SendBufferSize;               // size of buffer for sends - only
//    ULONG ReceiveBufferSize;            // size of buffer for receives - only
//    BOOLEAN Unreliable;                 // is this connection "unreliable".
//} TDI_CONNECTION_INFO, *PTDI_CONNECTION_INFO;
//
TDI_STATUS KStreamSocket::getinfo(TDI_CONNECTION_INFO& info)
{
    // TDI_CONNECTION_INFO contains status/stats info on given TCP/IP connection

	union { TDI_CONNECTION_INFO i; char whatever[256]; } u;
	TDI_STATUS Status = TDI_NO_RESOURCES;
	ULONG Size = sizeof(u);
	PNDIS_BUFFER pNdisBuf = AllocBuf(&u, Size);

	if (pNdisBuf) {
    	SYNCTDIREQ	Req(m_hConnectionCxt,this);
    	Status = TdiQueryInformation(Req, TDI_QUERY_CONNECTION_INFO,
        	pNdisBuf, (uint*)&Size, 0);
    	TDI_ASSERT(Status != TDI_PENDING); // would be bad...
    	FreeBuf(pNdisBuf);
		if (Status == TDI_SUCCESS)
			NdisMoveMemory(&info, &u.i, sizeof(info));
    }

	return Status;
}

/////////////////////// TDI Notifications /////////////////////////////

///////////////////////////////////////////////////////////////////////
// KStreamSocket::ConnectEvent()
//
//    Indication for incoming TCP connect request. Delegates to the
//    derived class object to get a response whether the connection
//	  should be granted.
//
// Parameters:
//     See TDI spec.
// Returns:
//     TDI_STATUS
//
TDI_STATUS	KStreamSocket::ConnectEvent(PVOID EventContext, uint AddressLength,
                     PTRANSPORT_ADDRESS Address,
                     uint/*UserDataLength*/, PVOID/*UserData*/,
                     uint OptionsLength, PVOID Options,
                     PVOID *AcceptingID, ConnectEventInfo/*PIRP*/ *EventInfo)
{
	KStreamSocket* pClient = (KStreamSocket*) EventContext;
    TDI_ASSERT(pClient->IsValid());

	if (pClient->m_State != CONN_IDLE) {

    	TDI_TRACE("KStreamSocket::ConnectEvent() in state %d, rejecting",
            pClient->m_State);
    	return TDI_NOT_ACCEPTED;
    }

	if (pClient->OnConnect(AddressLength, Address, OptionsLength, Options))
    {
        // client wants the connection, let it be
        // NOTE: we're hiding the 2-phase callback nature of the connect event
        // ===== from the client. The second TDI callback doesn't get propogated
        //       to the derived object. This might result in a race condition
        //       for a send() issued too early (unless checking m_State).

        *AcceptingID = pClient; // my orig context

    	pClient->m_State = CONN_ACCEPTING;

#ifdef TDI_WINDOWS_95
        // fill in the structure and schedule accept proceeding
    	EventInfo->cei_rtn = (CTEReqCmpltRtn) ConnectEvent_complete;
    	EventInfo->cei_context = pClient;
    	EventInfo->cei_acceptinfo = NULL;    // ?
    	EventInfo->cei_conninfo = NULL;      // ?
#endif
#ifdef TDI_WINDOWS_NT
		// Allocate an IRP, a context structure, and a connection information structure
        PIRP irp = pClient->AllocIrp();
        PCONNECT_EVENT_CONTEXT pContext = new CONNECT_EVENT_CONTEXT;
		CTDI_CONNECTION_INFORMATION* pClientAddress = new CTDI_CONNECTION_INFORMATION(Address, AddressLength);

		//Return if there were problems allocating memory
		if((irp == NULL) ||
			(pContext == NULL) ||
			(pClientAddress == NULL))
		{
            TDI_ASSERT(!"Problems allocating  memory");
		    if(irp) pClient->FreeIrp(irp);
			if(pContext) delete pContext;
			if(pClientAddress) delete pClientAddress;
			*AcceptingID = NULL;
	        *EventInfo = NULL; //(PIRP*)
			return TDI_NOT_ACCEPTED;
		}

		//Set up the context
		pContext->pConnectionInformation = pClientAddress;
		pContext->SessionContext = pClient;


        TdiBuildAccept (irp,
            pClient->KTDInterface::m_pDevice,
            pClient->KTDInterface::m_pfileCONN, // conn endpoint
            ConnectEvent_complete,
            pContext,                            // complete context
            *pClientAddress,
            NULL                // ReturnConnectionInfo: don't have to supply?
        );

		// Advance IRP's stack location since we do NOT IoCallDriver here:
        IoSetNextIrpStackLocation(irp);
        *EventInfo  = irp;

#endif

    	return TDI_MORE_PROCESSING;
    }
	else
    {
        // client doesn't want it
    	return TDI_NOT_ACCEPTED;
    }
}

///////////////////////////////////////////////////////////////////////
// KStreamSocket::DisconnectEvent()
//
//  Disconnect event is signaled on the TCP connection.
//  Delegate to the derived class object.
//
// Parameters:
//     See TDI spec
// Returns:
//     TDI_STATUS
//
TDI_STATUS	KStreamSocket::DisconnectEvent(PVOID EventContext, PVOID /*ConnectionContext*/,
                	uint /*DisconnectDataLength*/, PVOID /*DisconnectData*/,
                	uint OptionsLength, PVOID Options, ulong Flags)
{
    // disconnect is signaled: notify the client

	KStreamSocket* pClient = (KStreamSocket*) EventContext;
    TDI_ASSERT(pClient->IsValid());
	if (!pClient->IsValid())
    	return TDI_SUCCESS;
	TDI_ASSERT(pClient->m_State == CONN_ACTIVE);

	BOOLEAN bAbort = (Flags == TDI_DISCONNECT_ABORT);
	if (bAbort)
    	TDI_TRACE("StreamSocket::DisconnectEvent(): Abort on client %X\n", pClient);

    // confirm disconnect according to [MSTCP, p.18]

	if (bAbort)
    	pClient->m_State = CONN_IDLE;        // dead on arrival
    else {
        // Ack disconnect abortively to avoid the pending status.
        // The latter could cause race conditions on async destruction.
    	SYNCTDIREQ	Req(pClient->m_hConnectionCxt,pClient);
        UINT	Timeout = 0;
        Req.FinalStatus = pClient->TdiDisconnect(Req, &Timeout,
            TDI_DISCONNECT_ABORT, NULL, NULL);
        pClient->m_State = CONN_IDLE;
    }

    // NOTE: inform the client when it's all gone.
    // ===== Maybe it's better in the reverse order?
    //       In any case, there is a potential for race conditions...
	pClient->OnDisconnect(OptionsLength, Options, bAbort);

    // should we ever return anything but success?
	return TDI_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// KStreamSocket::RcvEvent()
//
// A data block received over the TCP connection. Delegate to the derived
// class object.
//
// Parameters:
//     See TDI spec
// Returns:
//     TDI_STATUS
//
TDI_STATUS	KStreamSocket::RcvEvent(PVOID EventContext,
                    PVOID ConnectionContext,  ulong Flags,
                	uint Indicated, uint Available, uint *Taken, uchar *Data,
                	EventRcvBuffer **Buffer)
{

	KStreamSocket* pClient = (KStreamSocket*)EventContext;
	TDI_ASSERT(pClient->IsValid());
    if (!pClient->IsValid())
        return TDI_NOT_ACCEPTED;

	*Buffer = NULL;

	uchar* RcvBuffer=NULL;
	uint RcvSize=(uint)Flags; // piggyback

	// Call the client
	*Taken = pClient->OnReceive(Indicated, Data,
		Available, &RcvBuffer, &RcvSize);

	// If all the available data are indicated return now
	if (Available <= Indicated)
		return (*Taken != uint(-1)) ? TDI_SUCCESS : TDI_NOT_ACCEPTED;

	// Process the lookahead indication, if the buffer is supplied.
	// If not, further callbacks will be supressed until explicit recvfrom()

	if (RcvBuffer==NULL || RcvSize==NULL)
		return (*Taken != uint(-1)) ? TDI_SUCCESS : TDI_NOT_ACCEPTED;

	// The client has provided a buffer to schedule the receive for more data.
	// Describe and schedule receive on this buffer

	if (RcvSize > Available)
		RcvSize = Available;

	PNDIS_BUFFER pBuf = pClient->AllocBuf(RcvBuffer, RcvSize);
	if (!pBuf) {
		TDI_ASSERT(!"Buffers");
		pClient->OnReceiveComplete(TDI_NO_RESOURCES, 0, RcvBuffer);
		return TDI_SUCCESS;
	}

	// Cache the context data

	pClient->m_PartialRcv.Cache(pBuf, pClient);


#ifdef TDI_WINDOWS_95
	// simply point to the ERB and return "more processing"
	*Buffer = pClient->m_PartialRcv;
#endif
#ifdef TDI_WINDOWS_NT
	// Build the TDI_RECEIVE irp for the remainder of data
	PIRP irp = pClient->AllocIrp();
	if (irp == NULL)
	{
		TDI_ASSERT(!"irp");
		pClient->FreeBuf(pBuf);
		pClient->OnReceiveComplete(TDI_NO_RESOURCES, 0, RcvBuffer);
		return TDI_SUCCESS;
	}

	TdiBuildReceive (irp,
		pClient->m_pDevice,
		pClient->m_pfileCONN,
		(PIO_COMPLETION_ROUTINE) ReceiveEvent_complete,
		pClient,
		pBuf,
		0, /* TDI_RECEIVE_NORMAL | TDI_RECEIVE_EXPEDITED */
		RcvSize
		);

	// Advance IRP's stack location since we do NOT IoCallDriver here:
	IoSetNextIrpStackLocation(irp);
	*Buffer = irp;
#endif
	return TDI_MORE_PROCESSING;

	// 'cause it's client-side: single connection per TA
	UNREFERENCED_PARAMETER(ConnectionContext);
	UNREFERENCED_PARAMETER(Flags);
}

//////////////////////////////////////////////////////////////////////
// KStreamSocket::RcvExpeditedEvent()
//
//    An expedited  data block received over the TCP connection.
//    Delegate to the derived class object.
// Parameters:
//     See TDI spec
// Returns:
//     TDI_STATUS
//
TDI_STATUS KStreamSocket::RcvExpeditedEvent(PVOID EventContext,
                    PVOID ConnectionContext,  ulong Flags,
                	uint Indicated, uint Available, uint *Taken, uchar *Data,
                	EventRcvBuffer **Buffer)
{
	// delegate to normal receive
	return RcvEvent(EventContext, ConnectionContext, Flags,
        	Indicated, Available, Taken, Data, Buffer);
}



//////////////////////////////////////////////////////////////////////
// KStreamSocket::ConnectEvent_complete()
//
//    Remote connect event has completed, finally (2nd TDI callback)
//
// Parameters:
//     pContext    -   object context
//     Status      -   TDI result code
//     ByteCount   -   user data count (not used)
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamSocket::ConnectEvent_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;
	UNREFERENCED_PARAMETER(ByteCount);
	KStreamSocket* pClient;

#ifdef TDI_WINDOWS_NT
	PCONNECT_EVENT_CONTEXT pConnectContext = (PCONNECT_EVENT_CONTEXT)pContext;
	pClient = reinterpret_cast<KStreamSocket*> (pConnectContext->SessionContext);
	delete pConnectContext->pConnectionInformation;
	delete pConnectContext;
#else
	pClient = reinterpret_cast<KStreamSocket*> (pContext);
#endif

    TDI_ASSERT(pClient->IsValid());
	TDI_ASSERT(pClient->m_State == CONN_ACCEPTING);
	if (Status == TDI_SUCCESS) {
    	pClient->m_State = CONN_ACTIVE;
    }
	else {
    	TDI_TRACE("KStreamSocket::ConnectEvent_complete() err %X\n", Status);
    	pClient->m_State = CONN_IDLE;
    }

    TDI_COMPLETE_CALLBACK_END(pClient);
}

//////////////////////////////////////////////////////////////////////
// KStreamSocket::connect_complete()
//
//    Our solicited connect() request has completed. Delegate to the derived
//    class object.
//
// Parameters:
//     pContext    -   object context
//     Status      -   TDI result code
//     ByteCount   -   user data count
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamSocket::connect_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	KStreamSocket* pClient = reinterpret_cast<KStreamSocket*> (pContext);
    TDI_ASSERT(pClient->IsValid());
	// CONN_DISCONNECTING is valid if disconnect() is called before connect() has completed.
	TDI_ASSERT((pClient->m_State == CONN_REQUESTING) || (pClient->m_State == CONN_DISCONNECTING));
	if (Status == TDI_SUCCESS) {
    	pClient->m_State = CONN_ACTIVE;
    }
	else {
    	TDI_TRACE("KStreamSocket::connect_complete() failed, err %X\n", Status);
    	pClient->m_State = CONN_IDLE;
    }

	pClient->On_connectComplete(pContext, Status, (ULONG)ByteCount);

    TDI_COMPLETE_CALLBACK_END(pClient);
}

//////////////////////////////////////////////////////////////////////
// KStreamSocket::disconnect_complete()
//
//     NORMALLY, our disconnect() request has completed, notify the client.
//     HOWEVER, when a client has issued more than 1 disconnect() (e.g. 2nd ABORT one)
//     we might get 2 or more callbacks, such as with TDI_CONNECTION_ABORTED status.
//     Also, TDI might return an "error" code 22 (TDI_GRACEFUL_DISC), which shoudn't
//     really be treated as error.
//
// Parameters:
//     pContext    -   object context
//     Status      -   TDI result code
//     ByteCount   -   user data count
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamSocket::disconnect_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	KStreamSocket* pClient = reinterpret_cast<KStreamSocket*> (pContext);
    TDI_ASSERT(pClient->IsValid());
	if (Status != TDI_SUCCESS) {
    	TDI_TRACE("KStreamSocket::disconnect_complete() with status %d\n", Status);
    }

    pClient->m_State = CONN_IDLE;
	pClient->On_disconnectComplete(pContext, Status, (ULONG)ByteCount);

    TDI_COMPLETE_CALLBACK_END(pClient);
}

//////////////////////////////////////////////////////////////////////
// KStreamSocket::send_complete()
//
//   Our solicited send() request has completed, free the buffer descr and
//   notify the client.
// Parameters:
//     pContext    -   object context (PTDI_REQCXT)
//     Status      -   TDI result code
//     ByteCount   -   user data count
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamSocket::send_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	PTDI_REQCXT pTdiCxt = reinterpret_cast<TDI_REQCXT*>(pContext);
	TDI_ASSERT(pTdiCxt);
	KStreamSocket* pClient = reinterpret_cast<KStreamSocket*> (pTdiCxt->This);
	PNDIS_BUFFER pBuf = (PNDIS_BUFFER) pTdiCxt->Buffer;
	PVOID UContext = pTdiCxt->User;
    TDI_ASSERT(pClient->IsValid());
    if (pClient->IsValid()) {
		delete pTdiCxt;
    	pClient->FreeBuf(pBuf);
        pClient->On_sendComplete(UContext, Status, (ULONG)ByteCount);
    }
	else {
	    TDI_ASSERT(!"Good client");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}

///////////////////////////////////////////////////////////////////////
// KStreamSocket::ReceiveEvent_complete(void* pContext, uint Status, uint ByteCount)
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
TDI_COMPLETE_CALLBACK (KStreamSocket::ReceiveEvent_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	KStreamSocket* pClient = reinterpret_cast<KStreamSocket*> (pContext);

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
		pClient->OnReceiveComplete(Status, (ULONG)ByteCount, buf);

	} else TDI_ASSERT(!"pClient");

    TDI_COMPLETE_CALLBACK_END(pClient);
}


///////////////////////////////////////////////////////////////////////
// KStreamSocket::recv_complete(void* pContext, uint Status, uint ByteCount)
//
// Previously issued recv() request completes
//
// Parameters:
//     pContext    -   object context
//     Status      -   TDI result code
//     ByteCount   -   byte counter
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamSocket::recv_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	PTDI_REQCXT pTdiCxt = reinterpret_cast<TDI_REQCXT*>(pContext);
	TDI_ASSERT(pTdiCxt);
	KStreamSocket* pClient = reinterpret_cast<KStreamSocket*> (pTdiCxt->This);
	PNDIS_BUFFER pBuf = (PNDIS_BUFFER) pTdiCxt->Buffer;
	PVOID UContext = pTdiCxt->User; // user's buffer just completed
    TDI_ASSERT(pClient->IsValid());
    if (pClient->IsValid()) {
		delete pTdiCxt;
    	pClient->FreeBuf(pBuf);
        pClient->On_recvComplete(UContext, Status, (ULONG)ByteCount);
    }
	else {
	    TDI_ASSERT(!"Good client");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}


///////////////// Default handler implementations ///////////////////
// NOTE: Those are virtuals usually overridden by the derived class /
/////////////////////////////////////////////////////////////////////
#pragma warning (disable:4100)	// non-ref formals


///////////////////////////////////////////////////////////////////////
// Connect Indication (handler)
//
// Parameters:
// 	AddressLength        - length of the source address structure pointed to by pTA
// 	pTA                  - pointer to TRANSPORT_ADDRESS of the source requesting connection
// 	OptionsLength        - option structure length (usually, 0)
// 	Options              - option structure (usually, 0)
// Returns:
// 	TRUE to accept the connection, FALSE to reject it.
// Comments:
// 	The framework calls OnConnect() when the connect request arrives to the
//	socket's transport address.
//	Note since KStreamSocket is supposed to be used for client side sockets,
//	the driver writer would not normally implement OnConnect() handler.
//	For kenel mode server functionality please refer to KStreamServer class.
BOOLEAN KStreamSocket::OnConnect(uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options)
{
	TDI_TRACE("KStreamSocket::OnConnect(): no handler\n");
	UNREFERENCED_PARAMETERS4(AddressLength,pTA,OptionsLength,Options);
	return FALSE; /* reject */
}


///////////////////////////////////////////////////////////////////////
// Disconnect Indication (handler)
//
// Parameters:
// 	OptionsLength        - option structure length (usually, 0)
// 	Options              - option structure (usually, 0)
// 	bAbort               - TRUE if abortive, FALSE if graceful disconnect
// Returns:
// 	none
// Comments:
// 	The framework calls OnDisconnect() whenever the remote peer has requested
//	disconnect - either abortively or gracefully.
//	On abortive disconnect indication (bAbort is TRUE) the connection has already
//	been destroyed by the transport, so the indication is an "after the fact" one.
//	On graceful disconnect the transport attempts to deliver data pending for the
//	transfer. Thus, the driver might assume that all the data submitted with send()
//	get delivered. For the abortive disconnect, the pended data can get lost.
//	The user overrides OnDisconnect() to perform custom processing of disconnect
//	indications.
//	Called at the DISPATCH_LEVEL.
void KStreamSocket::OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort)
{
	UNREFERENCED_PARAMETERS3(OptionsLength,Options,bAbort);
}


///////////////////////////////////////////////////////////////////////
// Send Completion (handler)
//
// Parameters:
// 	pCxt                 - User context passed on send()
// 	Status               - Status Code (result) of the request
// 	ByteCount            - Bytes actually transferred
// Returns:
// 	none
// Comments:
// 	The framework calls this handler when the associated send() request has
//	completed. The user overrides this handler to perform custom processing
//	of send completions. Usually, the handler frees or recycles some internal
//	resources identified by 'pCxt'. E.g., it could be the pointer to a buffer
//	allocated for this transfer.
//	On successful return, 'Status' will be equal to TDI_SUCCESS (zero) and ByteCount
//	to the length of the buffer submitted with send().
//	Note that a successful completion means that the
//	data block has been delivered at the remote peer's transport address. It does not
//	imply that the remote socket actually retrieved the data from the transport buffer.
//	On_sendComplete() is called at the DISPATCH_LEVEL
void KStreamSocket::On_sendComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount) {};

///////////////////////////////////////////////////////////////////////
// Recv() Completion (handler)
//
// Parameters:
// 	Status               - Status Code (result) of the request
// 	ByteCount            - Bytes actually received
// Returns:
// 	none
// Comments:
// 	The framework calls this handler when the associated recv() request has
//	completed. The user overrides this handler to perform custom processing
//	of recv() completions. Since recv() is usually used for flow control in the
//  path, after this callback the driver should be ready to start getting
//  OnReceive() events (suppressed by the flow control) again.
//	On successful return, 'Status' will be equal to TDI_SUCCESS (zero) and ByteCount
//	to the length of the buffer filled (usually, equal to one passed in recv()).
//	Called at the DISPATCH_LEVEL.
void KStreamSocket::On_recvComplete(PVOID, TDI_STATUS status, uint bytecnt) {};

///////////////////////////////////////////////////////////////////////
// Connect Completion (handler)
//
// Parameters:
// 	pCxt                 - User context passed on connect()
// 	Status               - Status Code (result) of the request
// 	ByteCount            - Bytes actually transferred
// Returns:
// 	none
// Comments:
// 	The framework calls this handler when the associated connect() request has
//	completed. The user overrides this handler to perform custom processing
//	of connect completions.
//	Status other than TDI_SUCCESS indicates that the connection attempt has failed.
//	If the connection attempt succeeded the client can start be sending data and
//	getting receive indications over the connection immediately.
//	Called at the DISPATCH_LEVEL.
void KStreamSocket:: On_connectComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount) {};


///////////////////////////////////////////////////////////////////////
// Disconnect Complete Indication (handler)
//
// Parameters:
// 	pCxt                 - User context passed on sendto()
// 	Status               - Status Code (result) of the request
// 	ByteCount            - Bytes actually transferred
// Returns:
// 	none
// Comments:
// 	The framework calls this handler when the associated disconnect() request has
//	completed. The user overrides this handler to perform custom processing
//	of connect completions.
//	Called at the DISPATCH_LEVEL.
void KStreamSocket::On_disconnectComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount) {};


///////////////////////////////////////////////////////////////////////
// Receive Indication (handler)
//
// Parameters:
// 	Indicated            - Number of bytes being indicated
// 	Data                 - Pointer to the data buffer
// 	Available            - Number of bytes available
// 	RcvBuffer            - Pointer to the pointer to a received buffer to get the rest of the TSDU (output param)
//	RcvBufferLen		 - Pointer to uint value to set the number of bytes t receive into RcvBuffer (output param)
// Returns:
// 	Number of bytes consumed, or 0 to reject the indication
// Comments:
// 	The framework calls OnReceive() whenever a data block sent by the remote peer
//	arrived over the connection.
//	The user overrides OnReceive() to perform receive data processing.
//	Note that the transport does guarantee the orderly
//	reliable delivery of all the bytes submitted by the sender.
//	The receiver, however, must not assume that the receive notifications are
//	delivered in the exact correspondence to the send requests issued by the sender.
//
//  OnReceive() returns a number of bytes accepted in this indication (less or
//  equal to Indicated). In addition, if the client is interested in the rest of available
//  data, the client provides a paged-locked buffer, RcvBuffer/RcvBufferLen, to
//  receive the rest of data into. If the client choses to do so, the framework schedules an
//  IRP to asynchronously move the available data from the transport. When the IRP completes,
//  the framework calls OnReceveComplete().
//
//	Called at the DISPATCH_LEVEL.
//
//  The client should implement this handler; the default one asserts when called.
//
uint KStreamSocket::OnReceive(uint Indicated, uchar *Data,
								uint Available, uchar **RcvBuffer, uint* RcvBufferLen)
{
	TDI_ASSERT(!"KStreamSocket::OnReceive(): handler?\n");
	UNREFERENCED_PARAMETERS5(Indicated, Data, Available, RcvBuffer, RcvBufferLen);
	return 0; /* ignored */
}

///////////////////////////////////////////////////////////////////////
// Partial recieve completion (handler)
//
// Parameters:
//	Status				 - Completion status of the operation
// 	Indicated            - Number of bytes being indicated
// 	Data                 - Pointer to the data buffer
// Returns:
// 	none
// Comments:
// 	OnReceiveComplete() is called as a result of the request to retrieve partially indicated
//	data from OnReceive().
//  The client should implement this handler; the default one asserts when called.
void KStreamSocket::OnReceiveComplete(TDI_STATUS Status, uint Indicated, uchar *Data)
{
	TDI_ASSERT(!"KStreamSocket::OnReceiveComplete(): handler?\n");
	UNREFERENCED_PARAMETERS3(Status, Indicated, Data);
}

#pragma warning (default:4100)	// non-ref formals
