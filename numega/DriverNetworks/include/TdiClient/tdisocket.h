// tdisocket.h
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
// Declares TDI-based "socket" classes, KDatagramSocket and KStreamSocket
//
#ifndef	_TDISOCKET_H_
#define _TDISOCKET_H_

#include	<tdiclient.h>
#include	"tdiPartialReceive.h"

//////////////// Naming Conventions for public methods ////////////////
//
// xyz()			-	BSD-like socket request methods, e.g. send()
// On_xyzComplete()	-	handlers called on xyz() completions, e.g. On_sendComplete()
// OnZyx()			-	handlers called on network events, e.g. OnReceive()
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Datagram callback-driven "socket"
//
// KDatagramSocket is the base class for a connectionless access to a
// network transport, such as UDP over the IP protocol.
//
// As a base class, KDatagramSocket exposes both service methods (requesting
// certain actions to be executed by the TDI transport) and event handlers
// (called on certain events indicated by the TDI transport).
//
// The driver writer derives his/her socket class from KDatagramSocket
// and implements event handlers, which are defined virtual in the base
// class.
//
// A KDatagramSocket-derived object can be used for both client and server
// side implementations. Datagrams are sent using sendto() method. Datagrams
// are received via OnReceive() event notification, which the driver writer
// implements. The driver writer also implements the request completion
// handler such as On_sendtoComplete().
//
// All the asynchronous requests (such as sendto()) come with an optional
// "context" parameter. The context paramter is completely opaque to the framework
// and is used by the driver writer to keep track of reusable resources
// (such as NDIS buffers) between the request invokation and apropriate event
// handler callback. All the event handlers are called at the DISPATCH_LEVEL.
//
// KDatagramSocket provides other service methods used by derived classes such as
// enabling/disabling event notifications (SetEvents()), controlling multicast
// group memberships, etc.
//
class KDatagramSocket : public KTdiClient {
	SAFE_DESTRUCTORS;
public:
// ctor/dtor
	KDatagramSocket(PTRANSPORT_ADDRESS pTA=NULL, UINT nBuffers=4, UINT Protocol=IPPROTO_UDP,
        const char* szProvider=NULL);
	virtual ~KDatagramSocket();

// Requests (services)

	TDI_STATUS sendto(PTDI_CONNECTION_INFORMATION pConn, void* pBuf, UINT Size, PVOID pCxt=NULL);
	TDI_STATUS recvfrom(PTDI_CONNECTION_INFORMATION pFilterInfo,
                            PTDI_CONNECTION_INFORMATION pFromInfo,
                                void* pBuf, UINT RcvSize, UINT* pBytesReceived, void* pCxt);

// controlling events
	void SetEvents(BOOLEAN OnOff);

// Multicast group support
	TDI_STATUS JoinMGroup(ULONG IpMGroup, ULONG IpIFace=0);
	TDI_STATUS LeaveMGroup(ULONG IpMGroup, ULONG IpIFace=0);

// Overridables: client's custom handlers

    // Request Completions
	virtual	void On_sendtoComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount);
	virtual	void On_recvfromComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount);

	// Partial TSDU (datagram) indication
	virtual uint OnReceive(uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options,
                	uint Indicated, uchar *Data,
					uint Available, uchar **RcvBuffer, uint* RcvBufferLen);

	// Completing partial TSDU (datagram) indication
	virtual void OnReceiveComplete(TDI_STATUS Status,
					uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options,
                	uint Indicated, uchar *Data);

protected:
	KTdiPartialReceive	m_PartialRcv;	// helper used to partial TSDU retrieval

    // Request Callbacks (completions):
    static TDI_COMPLETE_CALLBACK(sendto_complete);
    static TDI_COMPLETE_CALLBACK(recvfrom_complete);
    static TDI_COMPLETE_CALLBACK(RcvDGEvent_complete);

    // Native TDI Event Callbacks
	static TDI_STATUS TDI_CALLBACK_DECL RcvDGEvent(PVOID EventContext,
                	uint AddressLength,
                	PTRANSPORT_ADDRESS Address,
                	uint OptionsLength, PVOID Options,  uint Flags,
                	uint Indicated, uint Available,
                	uint *Taken, uchar *Data, EventRcvBuffer **Buffer);

	// Consider: congestion QoS
};

///////////////////////////////////////////////////////////////////////
// Connection-oriented callback-driven "socket"
//
// KStreamSocket is the base class for a connection-oriented access to a
// network transport, such as TCP over the IP protocol.
//
// As a base class, KStreamSocket exposes both service methods (requesting
// certain actions to be executed by the TDI transport) and event handlers
// (called on certain events indicated by the TDI transport).
//
// The driver writer derives his/her socket class from KStreamSocket
// and implements event handlers, which are defined virtual in the base
// class.
//
// A KStreamSocket-derived object is used primarily for the client
// side implementations. (The server side implementation should use
// KStreamServer<> class). The connection to the server is requested
// by asynchronous connect(). KStreamSocket supports only one connection
// a time. After the connection is established (which is signalled via
// On_connectComplete handler), data can be sent over the connection
// using send() requests. Send completions are signaled via On_sendComplete()
// handler.
//
// All the asynchronous requests (such as send()) come with an optional
// "context" parameter. The context paramter is completely opaque to the framework
// and is used by the driver writer to keep track of reusable resources
// (such as NDIS buffers) between the request invokation and apropriate event
// handler callback. All the event handlers are called at the DISPATCH_LEVEL.
//
// KStreamSocket provides other service methods used by derived classes such as
// enabling/disabling event notifications (SetEvents()), getting connection
// information (getinfo()), etc.
//
class KStreamSocket : public KTdiClient {
	SAFE_DESTRUCTORS;
public:
// ctor/dtor
	KStreamSocket(PTRANSPORT_ADDRESS pTA=NULL, UINT nBuffers=4, UINT Protocol=IPPROTO_TCP,
        const char* szProvider=NULL);
    virtual ~KStreamSocket();

    // copy constructor
    KStreamSocket(const KStreamSocket& s);

// Enable/Disable stream-related events

	void SetEvents(BOOLEAN OnOff);

// Requests (services)

    // establish (TCP) connection
	TDI_STATUS	connect(PTDI_CONNECTION_INFORMATION RequestAddr, UINT TimeoutMs=0);
	// send data over connection
	TDI_STATUS	send(void* pBuf, UINT Size, PVOID pCxt=NULL, USHORT Flags=0);
	// disconnect (TCP) connection
	TDI_STATUS	disconnect(BOOLEAN bAbort=TRUE, UINT TimeoutMs=0);
	// solicit receive (use for flow control only)
	TDI_STATUS recv(void* buf, UINT bytecnt, ULONG Flags=0);


// Statistics:
	TDI_STATUS	getinfo(TDI_CONNECTION_INFO& info);

// state helpers

	// Tests if the socket has been created successfuly
	BOOLEAN IsCreated() const { return m_hConnectionCxt != NULL; }
	// Tests if the socket is in idle state
	BOOLEAN IsIdle() const { return m_State == CONN_IDLE; }
	// Tests if the socket is in connected state
	BOOLEAN IsConnected() const { return m_State == CONN_ACTIVE; }

// Overridables: client's custom handlers

    // Event indications
	virtual BOOLEAN OnConnect(uint AddressLength, PTRANSPORT_ADDRESS pTA,
                	uint OptionsLength, PVOID Options);
	virtual void OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort);

	virtual uint OnReceive(uint Indicated, uchar *Data,
									uint Available, uchar **RcvBuffer, uint* RcvBufferLen);
	virtual void OnReceiveComplete(TDI_STATUS Status, uint Indicated, uchar *Data);

    // Request Completions:
	virtual	void On_sendComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount);
	virtual	void On_connectComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount);
	virtual	void On_disconnectComplete(PVOID pCxt, TDI_STATUS Status, uint ByteCount);
	virtual	void On_recvComplete(PVOID pCxt, TDI_STATUS status, uint bytecnt);

protected:
	typedef enum  {    	CONN_IDLE = 0,        // 0 no activity
                    	CONN_OFFERING,        // 1 connection is being offered
                    	CONN_REQUESTING,    // 2 connection is being requested
                    	CONN_ACCEPTING,        // 3 connection is being accepted
                    	CONN_ACTIVE,        // 4 connected
                    	CONN_DISCONNECTING,    // 5 connection is being disconnecting
    } Conn_State_t;
	Conn_State_t		m_State;             // a helper (not really needed functionally)
	CONNECTION_CONTEXT	m_hConnectionCxt;    // TDI handle for connection descriptor
	CTDI_CONNECTION_INFORMATION* m_pConnAddr;// actually connected remote address
    UINT                m_uTimeout;          // default timeout used for disconnects

	KTdiPartialReceive	m_PartialRcv;	// helper used to partial TSDU retrieval

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

///////////////////////////////////////////////////////////////////////
// A raw IP callback-driven "socket"
//
// KRawSocket is a special kind of KDatagramSocket that runs
// a "raw" IP protocol, such as ICMP.
//
// Different from the KDatagramSocket is the constructor signature,
// where the protocol number is the 1st and mandatory parameter.
// IP protocol numbers are define in ipnumbers.h, as IPPROTO_... constants.
//
// Another important run-time difference is that the OnReceive()
// handlers indicate the whole IP frame, not just the user's data portion
// as in the UDP.
//
// All the other programming considerations are similar to that of
// KDatagramSocket class.
//
class KRawSocket : public KDatagramSocket {
public:
	KRawSocket(UINT Protocol, PTRANSPORT_ADDRESS pTA=NULL, UINT nBuffers=4,
        const char* szProvider=NULL);

private:

	// Function object to generate the default transport name for ctor
	class DefaultDeviceName     {
			char	m_Name[sizeof(RAWIP_DEVICE_NAME)+16];
	public: DefaultDeviceName(UINT Protocol);
			operator char* () const { return  (char*) m_Name; }
	};
};

#endif // _TDISOCKET_H_
