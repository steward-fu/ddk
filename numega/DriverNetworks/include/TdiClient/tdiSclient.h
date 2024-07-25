// tdiSclient.h     --  Classes for server-side implementation.
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

#ifndef _tdiSclient_h
#define _tdiSclient_h

#include <tdiclient.h>
#include "tdiconnobj.h"
#include "tdiPartialReceive.h"

////////////////////////////////////////////////////////////////
// forward declarations
class KStreamServerSession;

////////////////////////////////////////////////////////////////
// Server side listening socket
//
// KStreamServerSocket is a socket capable of supporting multiple simultaneous
// connections, i.e. it encapsulates a transport address object associated with
// multiple connection objects.
//
// KStreamServerSocket, however, is NOT used by the client directly, i.e. it is one of the
// framework's internal classes. KStreamServerSocket is the base
// for KStreamServer<> template class, which is used by the client to instantiate a
// listening connection server maintaining multiple sessions (see KStreamServerSession).
//
class KStreamServerSocket : public KTdiClient {
	SAFE_DESTRUCTORS;
public:
    // Constructor
    KStreamServerSocket(PTRANSPORT_ADDRESS pTA, UINT nConns=5, UINT nBuffers=4*5, UINT Protocol=IPPROTO_TCP,
                        const char* szProvider=NULL);
    // Destructor
    virtual ~KStreamServerSocket();

	// Send data for the session
	TDI_STATUS send(CONNECTION_CONTEXT hSess, void* buf, UINT bytecnt, PVOID pCxt=NULL, USHORT Flags=0);
	// Disconnect session
	TDI_STATUS disconnect(CONNECTION_CONTEXT hSess, BOOLEAN bAbort=TRUE, UINT TimeoutMs=0);
	// Get info for the session
	TDI_STATUS getinfo(CONNECTION_CONTEXT hSess, TDI_CONNECTION_INFO& info);
	// Solicit receive (use for flow control only)
	TDI_STATUS recv(CONNECTION_CONTEXT hSess, void* buf, UINT bytecnt, ULONG Flags=0);

	// Enable/disable network events
	void SetEvents(BOOLEAN OnOff);

	virtual void RemoveSession(KStreamServerSession* sess) {};

protected:
	friend class KStreamServerSession;

	// event handlers
	virtual BOOLEAN OnEventConnect(CONNECTION_CONTEXT* hConn, uint AddressLength,
		PTRANSPORT_ADDRESS pTA, uint OptionsLength, PVOID Options) = 0;

	virtual void OnEventDisconnect(CONNECTION_CONTEXT hConn, uint OptionsLength,
		PVOID Options, BOOLEAN bAbort) = 0;

	virtual uint OnReceive(CONNECTION_CONTEXT hConn,
		uint Indicated, uchar *Data, uint Available, uchar **RcvBuffer, uint* RcvBufferLen) = 0;

	virtual void OnReceiveComplete(CONNECTION_CONTEXT hConn, TDI_STATUS Status,
		uint Indicated, uchar *Data) = 0;

	virtual void OnConnectDone(CONNECTION_CONTEXT hConn, PVOID, TDI_STATUS Status, uint ByteCount) = 0;

	virtual	void On_sendComplete(CONNECTION_CONTEXT, PVOID, TDI_STATUS, uint) = 0;
	virtual	void On_disconnectComplete(CONNECTION_CONTEXT, TDI_STATUS, uint) = 0;
	virtual	void On_recvComplete(CONNECTION_CONTEXT, PVOID, TDI_STATUS, uint) = 0;

protected:
    KTdiConnObjStore*    m_pConnContext;  // vector of connection objects

    // Native TDI Event Callbacks
	static TDI_STATUS TDI_CALLBACK_DECL ConnectEvent(PVOID EventContext, uint AddressLength,
                	PTRANSPORT_ADDRESS Address, uint UserDataLength, PVOID UserData,
                        uint OptionsLength, PVOID Options,
                            PVOID *AcceptingID, ConnectEventInfo *EventInfo);

	static TDI_STATUS TDI_CALLBACK_DECL DisconnectEvent(PVOID EventContext, PVOID ConnectionContext,
                	uint DisconnectDataLength, PVOID DisconnectData, uint OptionsLength,
                        PVOID Options, ulong Flags);

	static TDI_STATUS TDI_CALLBACK_DECL RcvEvent(PVOID EventContext, PVOID ConnectionContext,
                    ulong Flags, uint Indicated, uint Available, uint *Taken, uchar *Data,
                        EventRcvBuffer **Buffer);

	static TDI_STATUS TDI_CALLBACK_DECL RcvExpeditedEvent(PVOID EventContext,
                        PVOID ConnectionContext,  ulong Flags,
                            uint Indicated, uint Available, uint *Taken, uchar *Data,
                                EventRcvBuffer **Buffer);


    // Native TDI Request completion callbacks
	static TDI_COMPLETE_CALLBACK (ConnectEvent_complete);
	static TDI_COMPLETE_CALLBACK (connect_complete);
	static TDI_COMPLETE_CALLBACK (disconnect_complete);
	static TDI_COMPLETE_CALLBACK (send_complete);
	static TDI_COMPLETE_CALLBACK (send_complete_ex);
    static TDI_COMPLETE_CALLBACK (ReceiveEvent_complete);
    static TDI_COMPLETE_CALLBACK (recv_complete);

#ifdef TDI_WINDOWS_NT
	protected:
	//Context structure for ConnectEvent
	typedef struct _CONNECT_EVENT_CONTEXT
	{
		CTDI_CONNECTION_INFORMATION*	pConnectionInformation;
		CONNECTION_CONTEXT				SessionContext;
	}CONNECT_EVENT_CONTEXT, *PCONNECT_EVENT_CONTEXT;
#endif

};

/////////////////////////////////////////////////////////////////
// Server Session
//
// KStreamServerSession is an abstract base class for client's
// session classes. A session respresents a single active connection
// at a connection-oriented ("stream") socket.
//
// The interface exposed by KStreamServerSession
// is very similar to the one of KStreamSocket. It has service methods (such as send()) and
// network event handlers (such as OnReceive()). The driver writer derives
// his/her session class from KStreamServerSession and implements
// network event handlers.
//
// The crucial difference from KStreamSocket is in that how the session objects
// get created and destroyed. While KStreamSocket-s are instantiated and deleted
// explicitely, the life time of KStreamServerSession-s are managed by a
// companion framework class, KStreamServer.
//
// A KStreamServer object "sits" between client's session objects and TDI subsystem.
// TDI network event notifications "pass through" KStreamServer, which dispatch them
// to an appropriate instance of the user session class. Similarly, session's send() and
// disconnect() requests "pass through" KStreamServer, which uses appropriate
// TDI objects to submit the requests to the transport.
//
// KStreamServerSession includes a simple reference count mechanism implemented
// via Lock()/Unlock() pair. When the framework instantiates a session object,
// its ref count goes to 1. Usually, the framework automatically destroys the
// session object when the associated network connection gets dropped. The user,
// however, might schedule a worker thread referring to this session while the
// connection was on. To avoid untimely destruction of the object, the user calls Lock()
// before scheduling the worker. When the thread completes it calls Unlock(), which
// performs self-destruction.
//
// The 'this' pointer of the object derived from KStreamServerSession
// is used as TDI connection context in the KStreamServer object.
//
// In NT, we also cache the file object needed to pass requests to the
// transport.
//
// Underlying system object: TDI connection handle; it is built into base (KObject).
//
class KStreamServerSession : public KObject<HANDLE, 'tdis'> {
	SAFE_DESTRUCTORS;
public:
    KStreamServerSession();
    virtual ~KStreamServerSession();

// Overridable handlers

	// Connection control
    virtual BOOLEAN  OnConnect(uint AddressLength, PTRANSPORT_ADDRESS pTA,
                                    	uint OptionsLength, PVOID Options) = 0;
	virtual void OnConnectDone(TDI_STATUS status, uint bytecnt);
    virtual void OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort);

	// Send/receive
    virtual void OnSendComplete(PVOID  pCxt, TDI_STATUS status, uint bytecnt);
    virtual void OnDisconnectComplete(TDI_STATUS status, uint bytecnt);
    virtual void OnRecvComplete(PVOID pCxt, TDI_STATUS status, uint bytecnt);

	virtual uint OnReceive(uint Indicated, uchar *Data, uint Available,
									uchar **RcvBuffer, uint* RcvBufferLen);
	virtual void OnReceiveComplete(TDI_STATUS Status, uint Indicated, uchar *Data);

// Services

	// Send Data.
    TDI_STATUS send(void* buf, UINT bytecnt, PVOID pCxt=NULL, USHORT Flags=0);

	// Drop Connection.
    TDI_STATUS disconnect(BOOLEAN bAbort=TRUE);

	// Solicit receive (use for flow control only)
	TDI_STATUS recv(void* buf, UINT bytecnt, ULONG Flags=0);

    // Get reference to the underlying socket
    inline operator KStreamServerSocket& () const { return *m_pSocket; }

	// Get session "ID" (assigned by framework)
    inline UINT GetID() const { return m_ConnID; }

	// Initialize. Used by the framework only.
    void Init(KStreamServerSocket* socket, HANDLE hConn, UINT id=0);

	// Bump ref count for the object (AddRef)
	inline void Lock()
	{
		++m_RefCount;
	}

	// Decr ref count (Release). When it reaches 0, the object gets destroyed
	inline void Unlock()
	{
		if ( --m_RefCount == ULONG(0) )
			delete this;
	}

	BOOLEAN IncrementRequestCount()
	{
		BOOLEAN bRes;

		m_Lock.Lock();
		bRes = (m_RequestCount != 0);
		if ( bRes )
			++m_RequestCount;
		m_Lock.Unlock();

		return bRes;
	}

	void DecrementRequestCount()
	{
		ULONG res = -1;

		m_Lock.Lock();
		if ( m_RequestCount > 0 )
			res = --m_RequestCount;
		m_Lock.Unlock();

		if ( res == 0 )
			m_pSocket->RemoveSession(this);
	}

	void WaitForDisconnect() { m_DisconnectEvent.Wait(); }
	void SetDisconnect() { m_DisconnectEvent.Set(); }

	friend KStreamServerSocket;

protected:
    KStreamServerSocket*     m_pSocket; // bound socket
    UINT                     m_ConnID;  // 1-based connection ID for faster lookup in the conn store

	KNdisCounter			 m_RefCount; // Ref Count

    // Helper for IRP-scheduled receive
	KTdiPartialReceive	m_PartialRcv;

	// request count
	ULONG				m_RequestCount;
	KNdisSpinLock		m_Lock;
	KNdisEvent			m_DisconnectEvent;

public:
	// List pointers for KList<>
	LIST_ENTRY			m_ListEntry;
};

#include <tdiStrServer.h>

#endif // _tdiSclient_h
