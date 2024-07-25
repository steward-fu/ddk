//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// COMX.H - include file for port class of SERIAL example
// Copyright (c) 1994, Compuware Corporation

// To create a VCOMM port driver, you need to define a port driver class
// and at least one port class. This file declares the port class for
// the SERIAL example. Port classes are based on VCommPort (see vcomport.h).

// In this example, most all the virtual member functions are "implemented",
// although they only emit trace messages. Depending on the nature of your
// hardware, it may not be necessary to provide all functions for your
// port. However, many are necessary for basic functionality.

// Note that a useful implmentation of member function Setup is provided by 
// the base class, VCommPort.

class Comx : public VCommPort
{
public:
	Comx(PCHAR name);

protected:
	virtual COMMREQUEST Initialize(DEVNODE handle, DWORD baseIO, DWORD irq);
	virtual COMMREQUEST Open(VMHANDLE hVM, PDWORD pError);

	virtual COMMREQUEST Close(); 
	virtual COMMREQUEST SetCommState(P_DCB pDCB, DWORD ActionMask);
	virtual COMMREQUEST GetCommState(P_DCB pDCB);
	virtual COMMREQUEST GetProperties(_COMMPROP* pCommProp);
	virtual COMMREQUEST Setup(PCHAR RxQueue, DWORD cbRxQueue, PCHAR TxQueue, DWORD cbTxQueue);
	virtual COMMREQUEST ClearError(_COMSTAT* pComstat, PDWORD pError);
	virtual COMMREQUEST GetQueueStatus(_COMSTAT* pComstat);
	virtual COMMREQUEST Purge(DWORD qType);
	virtual COMMREQUEST Read(PCHAR buf, DWORD cbRequest, PDWORD pRxCount);
	virtual COMMREQUEST TransmitChar(CHAR ch);
	virtual COMMREQUEST Write(PCHAR buf, DWORD cbRequest, PDWORD pTxCount);
	virtual COMMREQUEST EnableNotification(PCOMMNOTIFYPROC pCallback, PVOID refData);
	virtual COMMREQUEST EscapeFunction(DWORD lFunc, DWORD InData, PVOID pOutData);
	virtual COMMREQUEST GetModemStatus(PDWORD pModemStatus);
	virtual COMMREQUEST GetEventMask(DWORD mask, PDWORD pEvents);
	virtual COMMREQUEST SetEventMask(DWORD mask, PDWORD pEvents);
	virtual COMMREQUEST SetModemStatusShadow(PBYTE pMSRShadow);
	virtual COMMREQUEST SetReadCallBack(DWORD RxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData);
	virtual COMMREQUEST SetWriteCallBack(DWORD TxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData);
#ifdef WIN40
	virtual COMMREQUEST GetCommConfig(PCOMMCONFIG pCC, PDWORD pSize);
	virtual COMMREQUEST SetCommConfig(PCOMMCONFIG pCC, DWORD dwSize);
	virtual COMMREQUEST GetError(PDWORD pError);
	virtual DWORD __cdecl DeviceIOCtl(DWORD IoControlCode, PVOID pInBuf, DWORD nInBufferSize, 
							PVOID pOutBuf,DWORD  nOutBufferSize,
							PIOCTLNOTIFICATIONFUNC pNotify, IOCtlBlock*  pIOCtlBlock);
#endif
};

