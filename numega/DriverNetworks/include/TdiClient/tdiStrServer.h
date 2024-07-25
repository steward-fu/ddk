// tdiStrServer.h	-	Defines KStreamServer class
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
// Stream Server Template.
//
// KStreamServer is a "listening" connection-oriented socket that acts as a
// factory of "session" objects. The session objects are the instances of
// class 'T', which is the parameter of the template. Class T is to be derived from
// another framework class, KStreamServerSession. Each instance of T represents
// a single connection; by default, this is a TCP/IP connection.
//
// KStreamServer<T> inherits from KStreamServerSocket and provides
// mapping between TDI connection contexts and user session objects
// derived from KStreamServerSession.
//
// Typically, a kernel mode server will have a single instance of KStreamServer<T>
// instantiated during driver initialization, in DriverEntry(). The driver writer
// designs his/her session class T, which is used as template parameter for KStreamServer.
//
// Whenever a new connect request arrives at the socket, KStreamServer instantiates
// a new instance of T and passes control into T::OnConnect() handler for this instance.
// The OnConnect() handler decides whether the connection should be granted or not.
// If the connection is granted KStreamServer starts monitoring network events for this
// session.	Whenever KStreamServer object gets a network event notification from the TDI,
// it delegates the event to the appropriate T::OnXxx() handler for processing. When
// the connection is dropped - either by local or remote request - KStreamServer destroys
// the session instance associated with that connection.
//
// NOTE that KStreamServer exposes no new public methods other than those inherited from
// KStreamServerSocket. Usually, the only method of interest for the driver writer is
// SetEvents(). All the network communication is done in the session class via methods
// exposed by KStreamServerSession.
//
// Please refer to the TCPEcho server for the example.
//
template<class T>
class KStreamServer : public KStreamServerSocket
{
	SAFE_DESTRUCTORS;
public:
    KStreamServer(PTRANSPORT_ADDRESS pTA, UINT nConns=5, UINT nBuffers=4, UINT Protocol=IPPROTO_TCP,
        const char* szProvider=NULL);
    ~KStreamServer();

	void disconnect_all();

	virtual void RemoveSession(KStreamServerSession* sess);

protected:
    // Implement base class overrides:

    // Async non-solicited events:
	BOOLEAN OnEventConnect(CONNECTION_CONTEXT* hConn, uint AddressLength, PTRANSPORT_ADDRESS pTA,
        uint OptionsLength, PVOID Options);
	void OnEventDisconnect(CONNECTION_CONTEXT hConn, uint OptionsLength, PVOID Options, BOOLEAN bAbort);

	uint OnReceive(CONNECTION_CONTEXT hConn, uint Indicated, uchar *Data,
								uint Available, uchar **RcvBuffer, uint* RcvBufferLen);
	void OnReceiveComplete(CONNECTION_CONTEXT hConn, TDI_STATUS Status, uint Indicated, uchar *Data);
	void OnConnectDone(CONNECTION_CONTEXT hConn, PVOID, TDI_STATUS Status, uint ByteCount);

    // Request Completions:
	void On_sendComplete(CONNECTION_CONTEXT, PVOID, TDI_STATUS, uint);
	void On_disconnectComplete(CONNECTION_CONTEXT hConn, TDI_STATUS, uint);
	void On_recvComplete(CONNECTION_CONTEXT, PVOID, TDI_STATUS, uint);

protected:
	KInterlockedList<KStreamServerSession>		m_SessionList;
	BOOLEAN										m_bDisconnectPending;
};


////////////////////////////////////////////////////////////////////////////////
/////////////////////////// IMPLEMENTATIONS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Constructor
//
// Parameters:
// 	pTA                  - Pointer to Transport Address.
//	nConns               - Maximimum number of simultaneous connections on this socket
// 	nBuffers             - Number of NDIS_BUFFER descriptors ("send-s" in-the-flight) per connection
// 	Protocol             - Protocol number according to ipnumbers.h. Default: TCP
// 	szProvider           - Device-provider name. Default: Microsoft TCP device.
// Returns:
// 	none
// Comments:
//	Instantiates a listening connection-oriented "server" socket.
//  The user specifies the server port number (usually, TCP port) by supplying a pointer
//	to a correct TRANSPORT_ADDRESS. CIPTRANSPORT_ADDRESS class can be conveniently
//	used in this case.
//	Parameter 'nConns' specifies the maximum number of TDI connection objects, which puts
//	a limit on max number of simultaneous session objects supported by this server. Current
//	implementation preallocates the TDI connection objects, so that after the maximum number
//	of connections has been reached, no more connections can be granted until one or
//	more active connections have been dropped.
//  The user might want to indicate the maximum number of send-s "in-the-flight"
//	(nBuffers) sent over a session object. The framework preallocates nBuffers*nConns
//	of NDIS buffer descriptors for the socket. If the number of
//	pending send-s exceeds the number of allocated descriptors the messages will be
//	dropped and an error status TDI_NO_RESOURCES will be returned on send-s until the
//	descriptors become available again. The driver writer should specify an appropriate
//	nConns/nBuffers values based on the driver's traffic load expectations.
//	The 'Protocol' parameter indicates the protocol number used for the socket; this
//	should be IPPROTO_TCP (default).
//	Finally, 'szProvider' specifies the device implementing the transport. By default,
//	the framework uses the TCP device of the standard Microsoft TCP/IP driver.
//	After the object has been constructed the user should call IsCreated() to
//	make sure the contsruction succeeded. Note that after successful construction
//	the events on the socket (the OnXxx handlers) are still disabled until the
//	user explicitely enables them with SetEvents(TRUE).
//	The constructor should be called at PASSIVE_LEVEL.
template<class T> KStreamServer<T>::KStreamServer
(PTRANSPORT_ADDRESS pTA, UINT nConns, UINT nBuffers, UINT Protocol, const char* szProvider) :
    KStreamServerSocket(pTA, nConns, nBuffers, Protocol, szProvider),
	m_bDisconnectPending(FALSE)
{
}

///////////////////////////////////////////////////////////////////////
// Destructor
//
// Parameters:
// Returns:
// Comments:
//	Disables network events and closes all the TDI objects associated
//	with the socket.
//	NOTE the server should make sure all the active sessions are closed
//	before calling the destructor.
//	The desstructor should be called at PASSIVE_LEVEL.
template<class T> KStreamServer<T>::~KStreamServer()
{
}

///////////////////////////////////////////////////////////////////////
// KStreamServer<T>::disconnect_all
//
// Parameters:
// Returns:
// Comments:
//	 disconnects all session objects
template<class T> void KStreamServer<T>::disconnect_all()
{
	m_SessionList.Lock();
	m_bDisconnectPending = TRUE;
	m_SessionList.Unlock();

	while ( KStreamServerSession* sess = m_SessionList.RemoveHead() )
	{
		sess->disconnect();
		sess->WaitForDisconnect();
		sess->Unlock();
	}

	// we need to make sure that we are not inside of KStreamServer<T>::RemoveSession
	m_SessionList.Lock();
	m_SessionList.Unlock();
}

///////////////////////////////////////////////////////////////////////
// KStreamServer<T>::RemoveSession
//
// Parameters:
// Returns:
// Comments:
//	 removes the session object from a collection
template<class T> void KStreamServer<T>::RemoveSession(KStreamServerSession* sess)
{
	m_SessionList.Lock();

	m_pConnContext->Free((CONNECTION_CONTEXT)sess);

	if ( !m_bDisconnectPending )
	{
		m_SessionList.RemoveNoLock(sess);
		sess->SetDisconnect();
		sess->Unlock();
	}
	else
		sess->SetDisconnect();

	m_SessionList.Unlock();
}

// Events
template<class T> BOOLEAN KStreamServer<T>::OnEventConnect(
	CONNECTION_CONTEXT* hSess, uint AddressLength,
	PTRANSPORT_ADDRESS pTA, uint OptionsLength, PVOID Options)
{
    // Instantiate new session obj derived from KStreamServerSession
    T* sess = new T ();   // ref count -> 1

	if ( sess )
	{
		// Create an association between it and TDI.
		// 'hConn' identifies the connection within the transport; we use
		// it to request services, such as send().
		//
		// 'sess' identifies us for the transport; the transport passes it to us
		// in the event callbacks, such as OnEventReceive().
		//
		// 'id' is KStreamServerSockets' connection "slot" identifier (index) in the
		// connection object container (m_pConnContext).
		// When TDI connection objects are preallocated,  this id
		// (rather than hSess) is used as the TDI connection context.
		// We then use CConnObjStore::Tdi2SessContext() to map it to hSess
		// in the callbacks. By default, NT uses the "preallocated", W9X "on-demand"
		// conn obj allocation mode.
		//
		UINT id;
		HANDLE hConn = m_pConnContext->Allocate((CONNECTION_CONTEXT)sess, &id);
		if ( hConn )
		{
			// Initialize the new session object and notify the session object
			sess->Init(this, hConn, id);

			if ( sess->OnConnect(AddressLength, pTA, OptionsLength, Options) )
			{
				// Pass the session context and grant the connection
				*hSess = (KStreamServerSocket::m_pConnContext->IsPreallocated()) ?
					CONNECTION_CONTEXT((ULONG_PTR)id) : CONNECTION_CONTEXT(sess);

				m_SessionList.Lock();
				BOOLEAN bDisconnectPending = m_bDisconnectPending;
				if ( !bDisconnectPending )
					m_SessionList.InsertTailNoLock(sess);
				m_SessionList.Unlock();

				if ( !bDisconnectPending )
					return TRUE;
			}

			m_pConnContext->Free((CONNECTION_CONTEXT)sess);
		}
		else
		{
			TDI_TRACE("KStreamServer<T>::OnEventConnect: too many connections\n");
		}

		sess->Unlock(); // delete sess;
	}

	return FALSE;
}

template<class T> void  KStreamServer<T>::OnEventDisconnect
(CONNECTION_CONTEXT hSess, uint OptionsLength, PVOID Options, BOOLEAN bAbort)
{
    KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>
                                    (m_pConnContext->Tdi2SessContext(hSess));
    if (sess->IsValid())
    {
        sess->OnDisconnect(OptionsLength, Options, bAbort);
		sess->DecrementRequestCount();
    }
    else TDI_ASSERT(!"Disconnect good CONNECTION_CONTEXT");
}

template<class T> uint KStreamServer<T>::OnReceive
	(CONNECTION_CONTEXT hSess, uint Indicated, uchar *Data, uint Available,
		uchar **RcvBuffer, uint* RcvBufferLen)
{
    KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>
                                    (m_pConnContext->Tdi2SessContext(hSess));
    if (sess->IsValid())
        return sess->OnReceive(Indicated, Data, Available, RcvBuffer, RcvBufferLen);
    else { TDI_ASSERT(!"CONNECTION_CONTEXT"); return 0; }
}

// Completions
template<class T> void KStreamServer<T>::OnReceiveComplete
	(CONNECTION_CONTEXT hSess, TDI_STATUS Status, uint Indicated, uchar *Data)
{
    KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>(hSess);
    if (sess->IsValid())
        sess->OnReceiveComplete(Status, Indicated, Data);
    else TDI_ASSERT(!"CONNECTION_CONTEXT");
}


template<class T> 	void  KStreamServer<T>::On_sendComplete
(CONNECTION_CONTEXT hSess, PVOID pCxt, TDI_STATUS status, uint bytecnt)
{
    KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>(hSess);
    if (sess->IsValid()) {
        sess->OnSendComplete(pCxt, status, bytecnt);
		sess->Unlock(); // send() bumped the ref count
	}
    else TDI_ASSERT(!"On_send_complete good CONNECTION_CONTEXT");
}

template<class T> 	void  KStreamServer<T>::On_recvComplete
(CONNECTION_CONTEXT hSess, PVOID pCxt, TDI_STATUS status, uint bytecnt)
{
    KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>(hSess);
    if (sess->IsValid()) {
        sess->OnRecvComplete(pCxt, status, bytecnt);
		sess->Unlock(); // recv() bumped the ref count
	}
    else TDI_ASSERT(!"On_send_complete good CONNECTION_CONTEXT");
}


template<class T> 	void  KStreamServer<T>::On_disconnectComplete
(CONNECTION_CONTEXT hSess, TDI_STATUS status, uint  bytecnt)
{
    KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>(hSess);
    if (sess->IsValid())
    {
        sess->OnDisconnectComplete(status, bytecnt);

		if ( status == TDI_SUCCESS )
			sess->DecrementRequestCount();

		sess->Unlock(); // disconnect() bumped the ref count
    }
    else TDI_ASSERT(!"On_disconnect_complete good CONNECTION_CONTEXT");
}

template<class T> 	void  KStreamServer<T>::OnConnectDone
(CONNECTION_CONTEXT hSess, PVOID, TDI_STATUS status, uint bytecnt)
{
	// Connection accept completion: don't confuse with OnConnectComplete().
	// This one is caused by the completion of our acception from OnConnect().
	// If the acception failed, the framework simulates OnDisconnect() anyway
	// followed by destruction of the session object
    KStreamServerSession* sess = reinterpret_cast<KStreamServerSession*>(hSess);
    if (sess->IsValid())
    {
        sess->OnConnectDone(status, bytecnt);
		if ( status != TDI_SUCCESS )
		{
			sess->OnDisconnect(0, 0, TRUE);
			sess->DecrementRequestCount();
		}
    }
    else TDI_ASSERT(!"OnConnectDone good CONNECTION_CONTEXT");
}
