// TCPECHO.h - include file for VxD TCPECHO

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

#define DEVICE_CLASS		TcpechoDevice
#define TCPECHO_DeviceID	UNDEFINED_DEVICE_ID
#define TCPECHO_Init_Order	UNDEFINED_INIT_ORDER
#define TCPECHO_Major		1
#define TCPECHO_Minor		0

#include	<tdiSclient.h> 

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
};

/////////////////////////////////////////////////////////////////////////
// Device class
class TcpechoDevice : public VDevice
{
protected:
    KStreamServer<TcpEchoSession>*     m_pListener;  // My Listening socket

public:
	virtual BOOL OnSysDynamicDeviceInit();
	virtual BOOL OnSysDynamicDeviceExit();
};


// end of file
