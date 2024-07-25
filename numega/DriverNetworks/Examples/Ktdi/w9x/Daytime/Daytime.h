// DAYTIME.h - include file for VxD DAYTIME

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
#define USE_NDIS
#include <vtoolscp.h>

#define DEVICE_CLASS		DaytimeDevice
#define DAYTIME_DeviceID		UNDEFINED_DEVICE_ID
#define DAYTIME_Init_Order	UNDEFINED_INIT_ORDER
#define DAYTIME_Major		1
#define DAYTIME_Minor		0

//////////////////////////////////////////////////////////////////////
// Some constnts

#define DAYTIME_QUERY_PERIOD		10000                // update period
#define SERVER_US_NAVAL_OBSERVATORY	UIPADDR(192,5,41,41) // USNO server

#include    <tdiclient.h>   // KTDI


/////////////////////////////////////////////////////////////////////////
// TCP client-side socket for "daytime"
class CDaytimeSocket : public KStreamSocket {
public:
    CDaytimeSocket();
    ~CDaytimeSocket();

	// Initiates query
	void Query(); 

    // schedules connection to the server after specified interval
    void ScheduleQuery() { m_Timer.Set(this); }

protected:
// Overridables of the base class which we care about
    
    // Events:
	void OnDisconnect(uint OptionsLength, PVOID Options, BOOLEAN bAbort);
	uint OnReceive(uint Indicated, uchar *Data,
							uint Available, uchar **RcvBuffer, uint* RcvBufferLen);

    // Request Completions:
	void On_connectComplete(PVOID, TDI_STATUS, uint);

// Timer stuff. When the timer goes off the handler causes the new query

	class CTimer : public VGlobalTimeOut {
	public: 
		CTimer(DWORD msec) : VGlobalTimeOut(msec) {}
		void Set(CDaytimeSocket* s) { m_refData = s; VGlobalTimeOut::Set(); }
		virtual VOID handler(VMHANDLE, THREADHANDLE, PCLIENT_STRUCT,  DWORD)
		{ ((CDaytimeSocket*)m_refData)->Query(); }
	};

    CTimer     m_Timer;         

};


/////////////////////////////////////////////////////////////////////////
// TCP client-side socket for "daytime"
class DaytimeDevice : public VDevice
{
public:
	virtual BOOL OnSysDynamicDeviceInit();
	virtual BOOL OnSysDynamicDeviceExit();
protected:
    CDaytimeSocket*     m_pSocket;      // socket
};


