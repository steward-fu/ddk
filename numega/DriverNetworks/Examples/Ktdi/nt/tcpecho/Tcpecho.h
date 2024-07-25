// Tcpecho.h
//
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 2001 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

#ifndef __Tcpecho_h__
#define __Tcpecho_h__
   
#define EOL "\n"

#include    <tdiSclient.h>   // KTDI

/////////////////////////////////////////////////////////////////////////
// TCP Echo Session class. 
// Derived from KStreamServerSession and implements 
// TCP event handlers we interested in.
class TcpEchoSession : public KStreamServerSession {
public:

    // handlers - overrides from KStreamServerSession
    BOOLEAN  OnConnect(uint AddressLength, PTRANSPORT_ADDRESS pTA, 
                                    	uint OptionsLength, PVOID Options);
    void OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort);

	uint OnReceive(uint Indicated, uchar *Data, uint Available,
									uchar **RcvBuffer, uint* RcvBufferLen);
	void OnReceiveComplete(TDI_STATUS Status, uint Indicated, uchar *Data);

    void OnSendComplete(PVOID buf, TDI_STATUS status, uint bytecnt);

	LIST_ENTRY	m_ListEntry;
};

//////////////////////////////////////////////////////////////////////////////
// Driver Object
class Tcpecho : public KDriver
{
	SAFE_DESTRUCTORS
public:
	// Member functions
	virtual  VOID Unload(VOID);
	virtual  NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

	// Member data

protected:
    KStreamServer<TcpEchoSession>*		m_pListener;  // My Listening socket
	KInterlockedList<TcpEchoSession>	m_ActiveSessionList;// list of active sessions

	friend class TcpEchoSession;
};



#endif			// __Tcpecho_h__
