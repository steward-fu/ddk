///////////////////////////////////////////////////////////////////
// tcpecho.cpp - main module for VxD tdiecho
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
//
// TDIECHO.VXD acts as TCP echo server.
// Accepts peer's connection and echoes its messages.
//
// The server starts up by loading the vxd.
//

#define DEVICE_MAIN
#include "tcpecho.h"
Declare_Virtual_Device(TCPECHO)
#undef DEVICE_MAIN


////////////////////////////////////////////////////////////////////
// Driver is Loading: create socket
BOOL TcpechoDevice::OnSysDynamicDeviceInit()
{
	// Initialize the TDIClient framework first
	if (!KTDInterface::Initialize())
	{
		return FALSE; // something wrong with TDI
	}

	// Create our echo server, port 7

	CIPTRANSPORT_ADDRESS echo_port(IPPORT_ECHO);
	m_pListener = new KStreamServer<TcpEchoSession> (echo_port);

    // If succeeded - enable network events
    if (m_pListener && m_pListener->IsCreated()) {
        m_pListener->SetEvents(TRUE);
        dprintf("TcpechoDevice: Listener started\n");
        return TRUE;
    }
    else {
        dprintf("TcpechoDevice: Failed to start (Tcpecho listener conflict?)\n");
        return FALSE;
    }
}

////////////////////////////////////////////////////////////////////
// Driver is Loading: destroy socket
BOOL TcpechoDevice::OnSysDynamicDeviceExit()
{
    // We could take care of destroying existing sessions gracefully.
    // For now, simply rely on the base class destructor.

    if (m_pListener) 
		delete m_pListener; 

    dprintf("TcpechoDevice: Listener stopped\n");
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// TcpEchoSession   -- Event handlers. 
BOOLEAN  TcpEchoSession::OnConnect(uint AddressLength, PTRANSPORT_ADDRESS pTA, 
                                    	uint OptionsLength, PVOID Options)
{
	// Connecting: print the IP address of the requestor and grant the connection

	char szIPaddr[20];

	inet_ntoa(PTDI_ADDRESS_IP(pTA->Address[0].Address)->in_addr, szIPaddr, sizeof(szIPaddr));

    dprintf("TcpechoDevice: Connecting client, IP addr = %s, session %8X\n", szIPaddr, this);

	UNREFERENCED_PARAMETERS4(AddressLength, pTA, OptionsLength, Options);
    return TRUE;
}

void TcpEchoSession::OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort)
{
    dprintf("TcpechoDevice: Disconnecting client, session %8X\n", this);
}

uint TcpEchoSession::OnReceive(uint Indicated, uchar *Data, uint Available,
									uchar **RcvBuffer, uint* RcvBufferLen) 
{ 
    // Received some data from the client peer. Allocate a buffer and echo the data.
	// Note the use of the echo buffer as a context for the callback.

    uchar* echo = new uchar [Indicated];
    if (echo) {
        memcpy(echo, Data, Indicated);
        TDI_STATUS status = send(echo, Indicated, echo);
        if (status == TDI_PENDING) 
            return Indicated; // normal case
        if (status != TDI_SUCCESS)
            dprintf("TcpechoDevice: Error %u sending echo\n", status);
        delete echo;
    }

	// Now, if the transport has more data available than indicated,
	// allocate another buffer to read the rest. When the transport
	// done with it - asynchronously - our OnReceiveComplete() handler
	// is called. Note that failure to submit a buffer supressed further
	// recieve indications - until and if a recv() is issued.

	if (Indicated < Available) {
		*RcvBuffer = new uchar [*RcvBufferLen = Available-Indicated];
	}

    return Indicated;
}

void TcpEchoSession::OnReceiveComplete(TDI_STATUS status, uint Indicated, uchar *Data)
{
	// Buffer for the partially indicated data allocated and submitted during 
	// OnReceive() processing is filled in by the transport. If everything is OK,
	// echo the buffer back to the remote client

    if (status == TDI_SUCCESS) 
		status = send(Data, Indicated, Data);
	else 
        dprintf("TcpechoDevice: Failed completing receive, err %X\n", status);

	if (status != TDI_PENDING)
		delete Data;
}

void TcpEchoSession::OnSendComplete(PVOID buf, TDI_STATUS status, uint bytecnt)
{ 
    // Our send request has completed. Free the buffer

    if (status != TDI_SUCCESS)
        dprintf("TcpechoDevice: Failed sending echo, err %X\n", status);
    delete ((uchar*)buf);
}


// end of file

