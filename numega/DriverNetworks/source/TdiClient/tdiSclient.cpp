// tdiSclient.cpp     --  Classes for server-side implementation.
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

#include <tdiSclient.h>

//////////////////////////////////////////////////////////////////////
// Constructor
//
KStreamServerSocket::KStreamServerSocket(PTRANSPORT_ADDRESS pTA, UINT nConns, UINT nBuffers, UINT Protocol,
                             const char* szProvider)
    : KTdiClient(pTA, Protocol, nBuffers, szProvider),
				m_pConnContext(NULL)
{
	if (!KTdiClient::IsCreated())
        return;                     // bad pTA

    // Create TDI connection context pool
    m_pConnContext = new KTdiConnObjStore(this, nConns);

}


//////////////////////////////////////////////////////////////////////
// Destructor
//
KStreamServerSocket::~KStreamServerSocket()
{

    // shut off all the event callbacks
	SetEvents(FALSE);

    // TODO: destroy active connections
    if (m_pConnContext)
        delete m_pConnContext;
}


//////////////////////////////////////////////////////////////////////
// Enable/Disable Events
//
// Parameters:
//     On      -   Enable (TRUE) / Disable (FALSE)
// Returns:
//     none
//
void KStreamServerSocket::SetEvents(BOOLEAN On)
{
    // Turn on/off all stream-related events (should we be more selective?)
	TdiSetEvent(m_Handle, TDI_EVENT_CONNECT, (On) ? ConnectEvent : NULL, this);
	TdiSetEvent(m_Handle, TDI_EVENT_DISCONNECT, (On) ? DisconnectEvent : NULL, this);
	TdiSetEvent(m_Handle, TDI_EVENT_RECEIVE, (On) ? RcvEvent : NULL, this);
	TdiSetEvent(m_Handle, TDI_EVENT_RECEIVE_EXPEDITED , (On) ? RcvExpeditedEvent : NULL, this);
}


//////////////////////////////////////////////////////////////////////
// Drop connection
//
// Parameters:
//     hSess       -   A valid KStreamServerSession handle
//     bAbort      -   TRUE causes RST, otherwise FIN
//     TimeoutMs   -   Timeout in msec.
// Returns:
//     TDI_STATUS
//
TDI_STATUS	KStreamServerSocket::disconnect(CONNECTION_CONTEXT hSess, BOOLEAN bAbort, UINT TimeoutMs)
{
	if ( !reinterpret_cast<KStreamServerSession*>(hSess)->IsValid() )
	{
		TDI_ASSERT(!"Valid hSess");

		On_disconnectComplete(hSess, TDI_INVALID_PARAMETER, 0);
#if TDI_WINDOWS_95
		return TDI_PENDING;
#endif

#if TDI_WINDOWS_NT
		return TDI_INVALID_PARAMETER;
#endif
	}

    // allocate context and save both session and socket ptrs:
	PTDI_REQCXT pTdiCxt = new (m_pContextStorage) TDI_REQCXT (this, NULL, NULL, hSess);
    if ( !pTdiCxt )
	{
		pTdiCxt = ::new TDI_REQCXT(this, (PVOID)1, NULL, hSess);
		if ( !pTdiCxt )
		{
			On_disconnectComplete(hSess, TDI_NO_RESOURCES, 0);

#if TDI_WINDOWS_95
			return TDI_PENDING;
#endif

#if TDI_WINDOWS_NT
			return TDI_NO_RESOURCES;
#endif

		}
	}

    // TDI connection point
    HANDLE hConn = *reinterpret_cast<KStreamServerSession*>(hSess);

	TDIREQ	Req(hConn, pTdiCxt, disconnect_complete);
    #ifdef TDI_WINDOWS_NT
    Req.m_pFO = m_pConnContext->GetFileObjByID(reinterpret_cast<KStreamServerSession*>(hSess)->GetID());
    #endif

	ushort  Flags = (bAbort) ? TDI_DISCONNECT_ABORT : TDI_DISCONNECT_WAIT;

	TDI_STATUS Status = TdiDisconnect(Req, &TimeoutMs, Flags, NULL, NULL);

#if TDI_WINDOWS_95
	if ( Status != TDI_PENDING )
	{
		On_disconnectComplete(hSess, Status, 0);
		Status = TDI_PENDING;
	}
#endif

#if 0
	if (Status != TDI_PENDING) {
        delete pTdiCxt;
    	if (Status != TDI_SUCCESS && Status != TDI_GRACEFUL_DISC)
        	TDI_TRACE("KStreamServerSocket::disconnect() failed with err %d\n", Status);
    }
#endif

	return Status;
}


//////////////////////////////////////////////////////////////////////
//  Send data over the connection
//
// Parameters:
//     hSess	-   A valid KStreamServerSession handle
//     buf		-   user's packet pointer
//     bytecnt	-   length of data area in the packet
//	   pCxt		-	opaque user context
//     Flags	-   optional TCP flags used for send.
// Returns:
//    TDI_STATUS
TDI_STATUS	KStreamServerSocket::send(CONNECTION_CONTEXT hSess,
                                        void* buf, UINT bytecnt, PVOID pCxt, USHORT Flags)
{
	TDI_STATUS Status;

	if ( !reinterpret_cast<KStreamServerSession*>(hSess)->IsValid() )
	{
		TDI_ASSERT(!"Valid hSess");
		On_sendComplete(hSess, pCxt, TDI_INVALID_PARAMETER, 0);
#if TDI_WINDOWS_95
		return TDI_PENDING;
#endif
#if TDI_WINDOWS_NT
		return TDI_INVALID_PARAMETER;
#endif
	}

    // TDI connection point
    HANDLE hConn = *reinterpret_cast<KStreamServerSession*>(hSess);

	// alloc buf descr & operation context
	PNDIS_BUFFER pNdisBuf = AllocBuf(buf, bytecnt);
	PTDI_REQCXT pTdiCxt = new (m_pContextStorage) TDI_REQCXT (this, pNdisBuf, pCxt, hSess);

	if (pNdisBuf && pTdiCxt)
	{
    	TDIREQ	Req(hConn, pTdiCxt, send_complete);

        #ifdef TDI_WINDOWS_NT
        Req.m_pFO = m_pConnContext->GetFileObjByID(static_cast<KStreamServerSession*>(hSess)->GetID());
        #endif

    	Status = TdiSend(Req, Flags, bytecnt, pNdisBuf);

#if TDI_WINDOWS_95
		if ( Status != TDI_PENDING )
		{
			On_sendComplete(hSess, pCxt, TDI_NO_RESOURCES, bytecnt);
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
    	TDI_TRACE("KStreamServerSocket::send(): No resources\n");
		if ( pTdiCxt )
		{
			TDI_ASSERT(pNdisBuf);
			delete pTdiCxt;
		}

		if ( pNdisBuf )
		{
			TDI_ASSERT(pTdiCxt);
        	FreeBuf(pNdisBuf);
		}

		On_sendComplete(hSess, pCxt, TDI_NO_RESOURCES, 0);
#if TDI_WINDOWS_95
		Status = TDI_PENDING;
#endif

#if TDI_WINDOWS_NT
		Status = TDI_NO_RESOURCES;
#endif
    }

	return Status;
}

//////////////////////////////////////////////////////////////////////
// Retrieve status data re given connection
//
// Parameters:
//     info    -   info structure to be filled in. Defined in MS' tdi.h.
// Returns:
//     TDI_STATUS
//
TDI_STATUS KStreamServerSocket::getinfo(CONNECTION_CONTEXT hSess, TDI_CONNECTION_INFO& info)
{
    // TDI_CONNECTION_INFO contains status/stats info on given TCP/IP connection

	TDI_STATUS Status = TDI_NO_RESOURCES;
	ULONG Size = sizeof(TDI_CONNECTION_INFO);
	PNDIS_BUFFER pNdisBuf = AllocBuf(&info, Size);

	if (pNdisBuf) {
    	SYNCTDIREQ	Req(static_cast<KStreamServerSession*>(hSess), this);
        #ifdef TDI_WINDOWS_NT
        Req.m_pFO = m_pConnContext->GetFileObjByID(static_cast<KStreamServerSession*>(hSess)->GetID());
        #endif
    	Status = TdiQueryInformation(Req, TDI_QUERY_CONNECTION_INFO,
        	pNdisBuf, (uint*)&Size, 0);
    	TDI_ASSERT(Status != TDI_PENDING); // would be bad...
    	FreeBuf(pNdisBuf);
    }

	return Status;
}


///////////////////////////////////////////////////////////////////////
// Solicit an explicit receive
//
// Parameters:
// 	hSess                - session handle
// 	buf                  - buffer to get the receive data into
// 	bytecnt              - the size of the buffer
// 	Flags                - optional TDI_RECEIVE_... flags
// Returns:
// 	Number of bytes actually received, 0 if an error occured.
// Comments:
//  The receive callbacks are suppressed until this request is completed.
// 	Unless the TDI_RECEIVE_PEEK flag is set, the underlying transport fills the given
//  buffer with received data until the client's buffer is full or the transport receives
//  an end-of-record indication from the remote node. If this flag is set, the client must
//  issue subsequent receive request(s) when this one is satisfied to obtain the full TSDU.
//	Usually, recv() is called as a part of a flow control policy of a driver. Whenever the driver
//  can't accept the entire indicated TSDU in its OnReceive() handler due to the lack of
//  buffer space, the driver triggers the flow control of the transport. The receive callbacks
//	get suppressed until the driver schedules a PASSIVE_LEVEL thread (workitem), which would
//	call recv() when the buffer space gets available.
//
TDI_STATUS KStreamServerSocket::recv(CONNECTION_CONTEXT hSess, void* buf, UINT bytecnt, ULONG Flags)
{
	TDI_STATUS Status;

	KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>(hSess);

	if ( !sess->IsValid() )
	{
		TDI_ASSERT(!"Valid hSess");
		On_recvComplete(hSess, buf, TDI_INVALID_PARAMETER, 0);
#if TDI_WINDOWS_95
		return TDI_PENDING;
#endif

#if TDI_WINDOWS_NT
		return TDI_INVALID_PARAMETER;
#endif
	}

    // TDI connection point
    HANDLE hConn = *sess;

	// alloc buf descr & operation context
	PNDIS_BUFFER pNdisBuf = AllocBuf(buf, bytecnt);
	PTDI_REQCXT pTdiCxt = new (m_pContextStorage) TDI_REQCXT (this, pNdisBuf, buf /* user cxt */, hSess);

	if (pNdisBuf && pTdiCxt)
	{
    	TDIREQ	Req(hConn, pTdiCxt, recv_complete);

        #ifdef TDI_WINDOWS_NT
        Req.m_pFO = m_pConnContext->GetFileObjByID(sess->GetID());
        #endif

    	Status = TdiReceive(Req, (ushort*) &Flags, (uint*) &bytecnt, pNdisBuf);

#if TDI_WINDOWS_95
		if ( Status != TDI_PENDING )
		{
			On_recvComplete(hSess, buf, Status, bytecnt);
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
    	TDI_TRACE("KStreamServerSocket::recv(): No resources\n");
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

		On_recvComplete(hSess, buf, TDI_NO_RESOURCES, 0);
#if TDI_WINDOWS_95
		Status = TDI_PENDING;
#endif

#if TDI_WINDOWS_NT
		Status = TDI_NO_RESOURCES;
#endif
    }

	return Status;

}

//////////////////////////////////////////////////////////////////////
// KStreamServerSocket::ConnectEvent()
//
//    Indication for incoming TCP connect request. Delegates to the derived class object
//    to get a response whether the connection should be granted.
// Parameters:
//     See TDI spec.
// Returns:
//     TDI_STATUS
// NOTE:
//     If a new session is to established, a 2nd TDI callback is scheduled. The session
//     OnCompleteDone() handler will be called after the transport is done with
//     establishing the connection.
//
TDI_STATUS	KStreamServerSocket::ConnectEvent(PVOID EventContext, uint AddressLength,
                     PTRANSPORT_ADDRESS Address,
                     uint/*UserDataLength*/, PVOID/*UserData*/,
                     uint OptionsLength, PVOID Options,
                     PVOID *AcceptingID, ConnectEventInfo *EventInfo)
{
	KStreamServerSocket* pClient = (KStreamServerSocket*) EventContext;
    TDI_ASSERT(pClient->IsValid());
    if (!pClient->IsValid())
        return TDI_NOT_ACCEPTED;

    // Call the derived class handler. The handler is supposed to return
    // the instantiated session id (hSess), which we'll then use as the TDI connection context

    CONNECTION_CONTEXT  hSess;

	if (pClient->OnEventConnect(&hSess, AddressLength, Address, OptionsLength, Options))
    {
        // Client wants the connection, let it be.
        // For now we assume that the hSess must represent a session object, else progr err

		TDI_ASSERT(reinterpret_cast<KStreamServerSession*>
			(pClient->m_pConnContext->Tdi2SessContext(hSess))->IsValid());

        // Form the response to TDI.
        // In Win9X: pass sess context value; in NT, set up and send "accept" IRP.
		//
		// NOTE1
		//
		// This implicitly relies on the different implementation of CConnObjStore in Win9X vs NT.
		//
		// In W9X, we can create connection objects in the context of the
		// ConnectEvent callback, so hSess can  be dynamically passed to TDI as our new
		// connection context.
		// In NT, the connection objects (and their contexts!) must be preallocated (at PASSIVE_LEVEL).
		// We use a preassigned numeric 'id' as the context, and, then dynamically associate it with
		// the new hSess in the client's OnEventConnect() handler. In this framework, KStreamServerSession<>
		// does the job. Thus, when NT's TDI calls our event handlers it passes the 'id', not hSess.
		// Then we use CConnObjStore::Tdi2SessContext() to map 'id' to 'hSess'.
		// See KStreamServerSession<>.
		//
        // NOTE2
		//
		// Passing user's data or options are not currently supported.
        //

        *AcceptingID = hSess;   // our connection context for the transport

#ifdef TDI_WINDOWS_95

    	EventInfo->cei_rtn = (CTEReqCmpltRtn) ConnectEvent_complete;
		EventInfo->cei_context = pClient->m_pConnContext->Tdi2SessContext(hSess); // 'session' ptr for the callback
    	EventInfo->cei_acceptinfo = NULL;       // NOT IMPL
    	EventInfo->cei_conninfo = NULL;         // NOT IMPL
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
		pContext->SessionContext = pClient->m_pConnContext->Tdi2SessContext(hSess);

        // Get the new connection's file object.
		// Note that in NT, the client's OnEventConnect() returns 'id' as hSess.

        PFILE_OBJECT pFO;	// conn end point
		pFO = pClient->m_pConnContext->GetFileObjByID((ULONG_PTR)hSess);
        TDI_ASSERT(pFO);

        TdiBuildAccept (irp,
            pClient->KTDInterface::m_pDevice,
            pFO,
            ConnectEvent_complete,
			pContext,
            *pClientAddress,
            NULL					// ReturnConnectionInfo: don't have to supply?
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
        *AcceptingID = NULL;
#ifdef TDI_WINDOWS_NT
        *EventInfo /*which is an PIRP. actually*/ = NULL;
#endif
    	return TDI_NOT_ACCEPTED;
    }
}


//////////////////////////////////////////////////////////////////////
// KStreamServerSocket::DisconnectEvent()
//
//     Disconnect event is signaled on the TCP connection. Delegate to the derived
//     class object.
// Parameters:
//     See TDI spec
// Returns:
//     TDI_STATUS
// Limitation:
//     Abortive disco ACK only. TODO: add semaphore/wait some day.
//
TDI_STATUS	KStreamServerSocket::DisconnectEvent(PVOID EventContext, PVOID ConnectionContext,
                	uint /*DisconnectDataLength*/, PVOID /*DisconnectData*/,
                	uint OptionsLength, PVOID Options, ulong Flags)
{
    // disconnect is signaled: notify the client

	KStreamServerSocket* pClient = (KStreamServerSocket*) EventContext;
    TDI_ASSERT(pClient->IsValid());
	if (!pClient->IsValid())
    	return TDI_SUCCESS;

	BOOLEAN bAbort = (Flags != TDI_DISCONNECT_RELEASE);

    // confirm disconnect according to [MSTCP, p.18]

    if (!bAbort) {
        // lookup tdi side context:
        CONNECTION_CONTEXT hSess = pClient->m_pConnContext->Tdi2SessContext(ConnectionContext);
        CONNECTION_CONTEXT hConn = *static_cast<KStreamServerSession*>(hSess);
        if (hConn) {
    	    SYNCTDIREQ	Req(hConn, pClient);
            #ifdef TDI_WINDOWS_NT
            // retireive file obj from id
            UINT sessid = static_cast<KStreamServerSession*>(hSess)->GetID();
            Req.m_pFO = pClient->m_pConnContext->GetFileObjByID(sessid);
            #endif
            UINT	Timeout = 0;
            Req.FinalStatus = pClient->TdiDisconnect(Req, &Timeout,
                TDI_DISCONNECT_ABORT, NULL, NULL);
        }
        else TDI_ASSERT(!"Retrieve hConn");
    }

    // inform the client when it's all gone.

	pClient->OnEventDisconnect(ConnectionContext, OptionsLength, Options, bAbort);

    // should we ever return anything but success?
	return TDI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// KStreamServerSocket::RcvEvent()
//     A data block received over the TCP connection. Delegate to the derived
//     class object.
// Parameters:
//     See TDI spec
// Returns:
//     TDI_STATUS
//
TDI_STATUS	KStreamServerSocket::RcvEvent(PVOID EventContext,
                    PVOID ConnectionContext,  ulong Flags,
                	uint Indicated, uint Available, uint *Taken, uchar *Data,
                	EventRcvBuffer **Buffer)
{
	UNREFERENCED_PARAMETER(Flags);

	KStreamServerSocket* pClient = (KStreamServerSocket*)EventContext;
	TDI_ASSERT(pClient->IsValid());
    if (!pClient->IsValid())
        return TDI_NOT_ACCEPTED;

	*Buffer = NULL;

	uchar* RcvBuffer=NULL;
	uint RcvSize=(uint)Flags; // piggyback

	// Call the client
	*Taken = pClient->OnReceive(CONNECTION_CONTEXT(ConnectionContext),
										Indicated, Data,
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
		pClient->OnReceiveComplete(CONNECTION_CONTEXT(ConnectionContext), TDI_NO_RESOURCES, 0, RcvBuffer);
		return TDI_SUCCESS;
	}

	// Cache the context data in the session object
	KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>
		(pClient->m_pConnContext->Tdi2SessContext(CONNECTION_CONTEXT(ConnectionContext)));

	sess->m_PartialRcv.Cache(pBuf, pClient, CONNECTION_CONTEXT(ConnectionContext));

#ifdef TDI_WINDOWS_95
	// simply point to the ERB and return "more processing"
	*Buffer = sess->m_PartialRcv;
#endif
#ifdef TDI_WINDOWS_NT
	// Build the TDI_RECEIVE irp for the remainder of TSDU
	PIRP irp = pClient->AllocIrp();
	if (irp == NULL)
	{
		TDI_ASSERT(!"irp");
		pClient->FreeBuf(pBuf);
		pClient->OnReceiveComplete(CONNECTION_CONTEXT(ConnectionContext), TDI_NO_RESOURCES, 0, RcvBuffer);
		return TDI_SUCCESS;
	}

	TdiBuildReceive (irp,
		pClient->m_pDevice,
		pClient->m_pConnContext->GetFileObjByID(sess->GetID()),
		(PIO_COMPLETION_ROUTINE) ReceiveEvent_complete,
		sess,	// context at ReceiveEvent_complete
		pBuf,
		0, /* TDI_RECEIVE_NORMAL | TDI_RECEIVE_EXPEDITED */
		RcvSize
		);

	// Advance IRP's stack location since we do NOT IoCallDriver here:
	IoSetNextIrpStackLocation(irp);
	*Buffer = irp;
#endif
	return TDI_MORE_PROCESSING;

}

//////////////////////////////////////////////////////////////////////
// KStreamServerSocket::RcvExpeditedEvent()
//     An expedited  data block received over the TCP connection. Delegate to the derived
//     class object.
// Parameters:
//     See TDI spec
// Returns:
//     TDI_STATUS
//
TDI_STATUS KStreamServerSocket::RcvExpeditedEvent(PVOID EventContext,
                    PVOID ConnectionContext,  ulong Flags,
                	uint Indicated, uint Available, uint *Taken, uchar *Data,
                	EventRcvBuffer **Buffer)
{
	// delegate to normal receive
	return RcvEvent(EventContext, ConnectionContext, Flags,
        	Indicated, Available, Taken, Data, Buffer);
}




//////////////////////////////////////////////////////////////////////
// KStreamServerSocket::disconnect_complete()
//
//     NORMALLY, our disconnect() request has completed, notify the client.
//     HOWEVER, when a client has issued more than 1 disconnect() (e.g. 2nd ABORT one)
//     we might get 2 or more callbacks, such as with TDI_CONNECTION_ABORTED status.
//     Also, TDI might return an "error" code 22 (TDI_GRACEFUL_DISC), which shoudn't
//     really be treated as error.
// Parameters:
//     pContext    -   object context (PTDI_REQCXT)
//     Status      -   TDI result code
//     ByteCount   -   user data count
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamServerSocket::disconnect_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	PTDI_REQCXT pTdiCxt = reinterpret_cast<TDI_REQCXT*>(pContext);
	TDI_ASSERT(pTdiCxt);

	KStreamServerSocket* pClient = reinterpret_cast<KStreamServerSocket*>(pTdiCxt->This);
	CONNECTION_CONTEXT hSess = (CONNECTION_CONTEXT)pTdiCxt->Session;
    if ( pClient->IsValid() )
	{
    	if ( Status != TDI_SUCCESS )
		{
        	TDI_TRACE("KStreamServerSocket::disconnect_complete() with status %d\n", Status);
		}

		if ( pTdiCxt->Buffer )
			::delete pTdiCxt;
		else
			delete pTdiCxt;

        pClient->On_disconnectComplete(hSess, Status, (uint)ByteCount);
    }
	else
	{
	    TDI_ASSERT(!"Good client");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}

//////////////////////////////////////////////////////////////////////
// KStreamServerSocket::ConnectEvent_complete()
//    Remote connect event has completed, finally (2nd TDI callback)
// Parameters:
//     pContext    -   connection context - session (see ConnectEvent())
//     Status      -   TDI result code
//     ByteCount   -   user data count (not used)
// Returns:
//     Unlike other completions the context used is pSess object
TDI_COMPLETE_CALLBACK (KStreamServerSocket::ConnectEvent_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	KStreamServerSession* pSess;

#ifdef TDI_WINDOWS_NT
	PCONNECT_EVENT_CONTEXT pConnectContext = (PCONNECT_EVENT_CONTEXT)pContext;
	pSess = (KStreamServerSession*) pConnectContext->SessionContext;
	delete pConnectContext->pConnectionInformation;
	delete pConnectContext;
#else
	pSess = (KStreamServerSession*) pContext;
#endif

	KStreamServerSocket* pClient = pSess->m_pSocket;
	TDI_ASSERT(pClient->IsValid());

	pClient->OnConnectDone(CONNECTION_CONTEXT(pSess), NULL, Status, (uint)ByteCount);

    TDI_COMPLETE_CALLBACK_END(pClient);
}


//////////////////////////////////////////////////////////////////////
// KStreamServerSocket::send_complete()
//
// Solicited send() request has completed, free the buffer descr, context and
// notify the client.
//
// Parameters:
//     pContext    -   object context (PTDI_REQCXT)
//     Status      -   TDI result code
//     ByteCount   -   user data count
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamServerSocket::send_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	PTDI_REQCXT pTdiCxt = reinterpret_cast<TDI_REQCXT*>(pContext);
	TDI_ASSERT(pTdiCxt);

	KStreamServerSocket* pClient = reinterpret_cast<KStreamServerSocket*> (pTdiCxt->This);
	PNDIS_BUFFER pBuf = (PNDIS_BUFFER)pTdiCxt->Buffer;
	PVOID UContext = pTdiCxt->User;
	CONNECTION_CONTEXT hSess = (CONNECTION_CONTEXT)pTdiCxt->Session;
    TDI_ASSERT(pClient->IsValid());

    if ( pClient->IsValid() )
	{
		delete pTdiCxt;
    	pClient->FreeBuf(pBuf);
        pClient->On_sendComplete(hSess, UContext, Status, (uint)ByteCount);
    }
	else
	{
	    TDI_ASSERT(!"Good client");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}


///////////////////////////////////////////////////////////////////////
// KStreamServerSocket::ReceiveEvent_complete(void* pContext, uint Status, uint ByteCount)
//
// A partially indicated TSDU receive has completed. Delegate to the client's
// derived object.
//
// Parameters:
//     pContext    -   ConnectionContext (!)
//     Status      -   TDI result code
//     ByteCount   -   byte counter
// Returns:
//     none
//
TDI_COMPLETE_CALLBACK (KStreamServerSocket::ReceiveEvent_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	KStreamServerSocket* pClient;
	KStreamServerSession* pSess = reinterpret_cast<KStreamServerSession*> (pContext);

    if (pSess->IsValid())
	{
		pClient = pSess->m_pSocket;
		// retrieve the buffer for indication
		KTdiPartialReceive* pRcvBuf = &pSess->m_PartialRcv;
		PNDIS_BUFFER pBuf = *pRcvBuf;

		uchar* buf;
		uint buflen; // >= ByteCount
		NdisQueryBuffer(pBuf, (PVOID*) &buf, &buflen);

		// free the descriptor used for the transfer
		pClient->FreeBuf(pBuf);

		// indicate to the client
		pClient->OnReceiveComplete(CONNECTION_CONTEXT(pSess), Status, (uint)ByteCount, buf);
	}
	else {
	    TDI_ASSERT(!"Good ReceiveEvent_complete");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}

///////////////////////////////////////////////////////////////////////
// KStreamServerSocket::recv_complete(void* pContext, uint Status, uint ByteCount)
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
TDI_COMPLETE_CALLBACK (KStreamServerSocket::recv_complete)
{
    TDI_COMPLETE_CALLBACK_BEGIN;

	PTDI_REQCXT pTdiCxt = reinterpret_cast<TDI_REQCXT*>(pContext);
	TDI_ASSERT(pTdiCxt);
	KStreamServerSocket* pClient = reinterpret_cast<KStreamServerSocket*> (pTdiCxt->This);
	PNDIS_BUFFER pBuf = (PNDIS_BUFFER) pTdiCxt->Buffer;
	PVOID UContext = pTdiCxt->User; // user's buffer
	CONNECTION_CONTEXT hSess = (CONNECTION_CONTEXT) pTdiCxt->Session;
    TDI_ASSERT(pClient->IsValid());
    if (pClient->IsValid()) {
		delete pTdiCxt;
    	pClient->FreeBuf(pBuf);
        pClient->On_recvComplete(hSess, UContext, Status, (uint)ByteCount);
    }
	else {
	    TDI_ASSERT(!"Good client");
		TDI_COMPLETE_CALLBACK_RETERR;
	}

    TDI_COMPLETE_CALLBACK_END(pClient);
}

// end of file
