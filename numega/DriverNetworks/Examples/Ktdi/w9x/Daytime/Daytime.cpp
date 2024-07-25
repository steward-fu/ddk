// DAYTIME.cpp - main module for VxD DAYTIME

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
#define DEVICE_MAIN
#include "daytime.h"
Declare_Virtual_Device(DAYTIME)
#undef DEVICE_MAIN


// Driver is loading
BOOL DaytimeDevice::OnSysDynamicDeviceInit()
{
	m_pSocket = NULL;

	// Initialize the TDIClient framework first
	if (!KTDInterface::Initialize())
	{
		ASSERT(0);
		return FALSE; // something wrong with TDI
	}

	// Create our client socket
	m_pSocket = new CDaytimeSocket;

	// If succeeded - enable network events
    if (m_pSocket && m_pSocket->IsCreated()) {
        m_pSocket->SetEvents(TRUE);
        dprintf("Daytime device: started at TCP port %u\n", m_pSocket->QueryPort());
    }
    else {
        dprintf("Daytime: Failed to start\n");
        return FALSE;
    }

    // Init 1st query

	m_pSocket->ScheduleQuery();

	return TRUE;
}

// Driver is unloading
BOOL DaytimeDevice::OnSysDynamicDeviceExit()
{
	delete m_pSocket;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// TCP client-side socket for "daytime"
CDaytimeSocket::CDaytimeSocket() : 
    KStreamSocket(), m_Timer(DAYTIME_QUERY_PERIOD) 
{
}

CDaytimeSocket::~CDaytimeSocket()
{
    m_Timer.Cancel();
}


// Events:
void CDaytimeSocket::OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort)
{
    // OK: server disconnects, according to the RFC. Schedule next query.
    ScheduleQuery(); 
}

uint CDaytimeSocket::OnReceive(uint Indicated, uchar *Data,
									uint Available, uchar **RcvBuffer, uint* RcvBufferLen)
{
	UNREFERENCED_PARAMETERS3(Available, RcvBuffer, RcvBufferLen); // no need in this simple proto

    // Data from the server. It must be the ascii GMT, so print it.
    // NOTE that the server's string is NOT null-terminated, so
    // we put 0 at the end.

    Data[Indicated-1]=0;
    dprintf("Daytime GMT (%u bytes): %s\n", Indicated, (char*)Data);
    return Indicated;
}

void CDaytimeSocket::On_connectComplete(PVOID, TDI_STATUS Status, uint)
{
    if (Status != TDI_SUCCESS) {
        dprintf("Daytime: Error %X connecting to the server\n", Status);
        ScheduleQuery(); // try later
    }
}

//////////////////////////////////////////////////////////////////////
// Procedure called periodically. 
// It initiates a TCP connection to server's port 13
//
void CDaytimeSocket::Query()
{
    TDI_ADDRESS_IP addr = {htons(IPPORT_DAYTIME), SERVER_US_NAVAL_OBSERVATORY};
    CTDI_CONNECTION_INFORMATION server(addr);

    TDI_STATUS status = connect(server, DAYTIME_QUERY_PERIOD/2);
    if (status != TDI_PENDING && status != TDI_SUCCESS)
       dprintf("Daytime: Failed to initiate connection due to err %X\n", status);
}

