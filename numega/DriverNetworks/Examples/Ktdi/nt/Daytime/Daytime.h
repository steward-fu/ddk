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
//////////////////////////////////////////////////////////////////////////
// A trivial KTDI driver implementing RFC867 ("daytime") client
//
// Daytime.h   
//

#ifndef __Daytime_h__
#define __Daytime_h__
   
#define EOL "\n"

#include    <tdiclient.h>   // KTDI


/////////////////////////////////////////////////////////////////////////
// TCP client-side socket for "daytime"
class CDaytimeSocket : public KStreamSocket {
public:
    CDaytimeSocket();
    ~CDaytimeSocket();

    // schedules connection to the server
    void ScheduleQuery();               

protected:
// Overridables of the base class which we care about
    
    // Events:
	void OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort);
	uint OnReceive(uint Indicated, uchar *Data,
							uint Available, uchar **RcvBuffer, uint* RcvBufferLen);

    // Request Completions:
	void On_connectComplete(PVOID, TDI_STATUS, uint);

// Timer stuff
    KTimedCallback     m_Timer;         
    MEMBER_DPC(CDaytimeSocket, QueryTimeProc); // timer procedure

};

/////////////////////////////////////////////////////////////////////////
// Driver
class Daytime : public KDriver
{
	SAFE_DESTRUCTORS
public:
	// Member functions
	virtual  VOID Unload(VOID);
	virtual  NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

protected:
    CDaytimeSocket*     m_pSocket;      // socket
};


//////////////////////////////////////////////////////////////////////
// Some constnts

#define DAYTIME_QUERY_PERIOD		10000                // update period

#define SERVER_US_NAVAL_OBSERVATORY	UIPADDR(192,5,41,41) // USNO server


#endif			// __Daytime_h__
