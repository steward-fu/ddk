// tdiSsession.cpp     --  KStreamServerSession methods.
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

///////////////////////////////////////////////////////////////////////
// Constructor (call by the framework only)
KStreamServerSession::KStreamServerSession()
    : KObject<HANDLE, 'tdis'>(),
          m_pSocket(NULL),
			m_RefCount(1),
			m_RequestCount(1),
			 m_PartialRcv(KStreamServerSocket::ReceiveEvent_complete)
{
}

///////////////////////////////////////////////////////////////////////
// Destructor (call by the framework only)
KStreamServerSession::~KStreamServerSession()
{
}

//////////////////////////////////////////////////////////////////////
// Initialize. Used by the framework only.
//
// Called by the framwork once to cache socket ptr and TDI session handle.
// In NT, we also get the file obj pointer for the TDI connection object.
// This is needed to quickly retrieve parameters needed to issue TDI
// requests over the session.
//
void KStreamServerSession::Init(KStreamServerSocket* socket, HANDLE hConn, UINT id)
{
	// Allow one shot only: at this stage the session object should
	// be virgin, i.e. no socket neither connection handle were yet assigned

    TDI_ASSERT(!m_pSocket && !IsValid());
    TDI_ASSERT(id>0 && id<1024);		  // kludgy...

    m_pSocket=socket;
    KObject<HANDLE, 'tdis'>::Assign(hConn);
    m_ConnID = id;
}

///////////////////////////////////////////////////////////////////////
// Send data to the client
//
// Parameters:
// 	buf                  - buffer to send
// 	bytecnt              - length to send
// 	pCxt                 - context (optional)
// 	Flags                - flags (optional)
// Returns:
// 	TDI_STATUS
// Comments:
// 	Before send() is submitted to transport the session object is locked
//  to avoid premature session object destruction on surprise remote disconnect.
//	The framework unlocks the session on send complete.
TDI_STATUS KStreamServerSession::send(void* buf, UINT bytecnt, PVOID pCxt, USHORT Flags)
{
	Lock();
	TDI_STATUS Status = m_pSocket->send(CONNECTION_CONTEXT(this), buf, bytecnt, pCxt, Flags);
	if (Status != TDI_PENDING)
		Unlock();
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
//	Do NOT use this request for purposes othen than implementing
//	a flow control mechanism for the receive path. When the server runs out
//	of input buffers, it might trigger a control flow event in OnReceive() handler,
//	which supresses further receive indications and, in turn, causes the transport
//  protocol flow control to activate. When buffers become available, the server
//	issues recv(), usually at PASSIVE_LEVEL, which unlocks the transport driver
//  receive indications.
//	When the request completes, the overridable OnRecvComplete() handler is called.
TDI_STATUS KStreamServerSession::recv(void* buf, UINT bytecnt, ULONG Flags)
{
	Lock();
	TDI_STATUS Status = m_pSocket->recv(CONNECTION_CONTEXT(this), buf, bytecnt, Flags);
	if (Status != TDI_PENDING)
		Unlock();
	return Status;
}


///////////////////////////////////////////////////////////////////////
// Drop connection
//
// Parameters:
// 	bAbort               - Abortive vs Graceful
// Returns:
// 	TDI_STATUS
// Comments:
// 	Before disconnect() is submitted to transport the session object is locked
//  to avoid premature session object destruction on surprise remote disconnect.
//	The framework unlocks the session on disconnect complete.
TDI_STATUS KStreamServerSession::disconnect(BOOLEAN bAbort)
{
	Lock();
	TDI_STATUS Status = m_pSocket->disconnect(CONNECTION_CONTEXT(this), bAbort);
	if ( Status != TDI_PENDING )
		Unlock();

	return Status;
}


///////////////// Default handler implementations ///////////////////
// NOTE: Those are virtuals usually overridden by the derived class /
/////////////////////////////////////////////////////////////////////
#pragma warning (disable:4100)	// non-ref formals


///////////////////////////////////////////////////////////////////////
// Connection Established (handler)
//
// Parameters:
// 	Status               - Result of connection setup
// 	ByteCount            - Bytes Transferred (currently not used)
// Returns:
// 	none
// Comments:
// 	The framework calls OnConnectDone() as a result of completion of
//	the connect procedure requested by returning TRUE from OnConnect() handler.
//	Any Status value other than TDI_SUCCESS indicates a failure, perhaps
//	at the low level, to establish the connection.
//	Called at DISPATCH_LEVEL.
void KStreamServerSession::OnConnectDone(TDI_STATUS Status, uint ByteCount) {}

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
void KStreamServerSession::OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort)
{
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
//	Called at the DISPATCH_LEVEL.
void KStreamServerSession::OnSendComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount) {}

///////////////////////////////////////////////////////////////////////
// Recv() Completion (handler)
//
// Parameters:
//	pCxt				 - Request context (user buffer pointer in this case)
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
void KStreamServerSession::OnRecvComplete(PVOID pCxt, TDI_STATUS status, uint bytecnt) {}

///////////////////////////////////////////////////////////////////////
// Disconnect complete Indication (handler)
//
// Parameters:
// 	Status               - Result of operation
// 	ByteCount            - Bytes Transferred (currently not used)
// Returns:
// 	none
// Comments:
// 	The framework calls this handler when the associated disconnect() request has
//	completed. The user overrides this handler to perform custom processing
//	of disconnect completions.
//	Called at the DISPATCH_LEVEL.
void KStreamServerSession::OnDisconnectComplete(TDI_STATUS Status, uint ByteCount) {}

///////////////////////////////////////////////////////////////////////
// Receive Indication (handler)
//
// Parameters:
// 	Indicated            - Number of bytes being indicated
// 	Data                 - Pointer to the data buffer
// 	Available            - Number of bytes available (Available > Indicated)
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
uint KStreamServerSession::OnReceive(uint Indicated, uchar *Data, uint Available,
											  uchar **RcvBuffer, uint* RcvBufferLen)
{
	TDI_ASSERT(!"KStreamServerSession::OnReceive(): handler?\n");
	UNREFERENCED_PARAMETERS5(Indicated, Data, Available, RcvBuffer, RcvBufferLen);
	return 0;
}

///////////////////////////////////////////////////////////////////////
// Partial recieve completion (handler)
//
// Parameters:
// 	Indicated            - Number of bytes being indicated
// 	Data                 - Pointer to the data buffer
// Returns:
// 	none
// Comments:
// 	OnReceiveComplete() is called as a result of the request to retrieve partially indicated
//	data from OnReceive().
//  The client should implement this handler; the default one asserts when called.
void KStreamServerSession::OnReceiveComplete(TDI_STATUS Status, uint Indicated, uchar *Data)
{
	TDI_ASSERT(!"KStreamServerSession::OnReceiveComplete(): handler?\n");
	UNREFERENCED_PARAMETERS3(Status, Indicated, Data);
}

#pragma warning (default:4100)	// non-ref formals

// end of file
