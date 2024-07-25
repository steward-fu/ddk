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

// COMX.CPP - implementation of port class for VCOMM port driver example
// Copyright (c) 1994, Compuware Corporation

// This implementation of a port class does not interface to actual 
// hardware, and is intended only to show the basic structure of 
// a port class.

#include <vtoolscp.h>
#include "comx.h"

Comx::Comx(PCHAR szName) : VCommPort(szName) 
{
#ifdef WIN40
	if (m_ContentionHandler)
		m_StealFlag = TRUE;
#endif
}

COMMREQUEST Comx::Initialize(DEVNODE handle, DWORD baseIO, DWORD irq)
{
	dout << "Comx: initializing " << m_name << " Base I/O=" << baseIO
		<< " IRQ#=" << irq << endl;

	m_PortNumber = m_name[3]-'0';

	return TRUE;
}

COMMREQUEST Comx::Open(VMHANDLE hVM, PDWORD pError)
{
	dout << "Comx: open port for VM " << (DWORD)hVM << endl;
	return TRUE;
}

COMMREQUEST Comx::Close()
{
	dout << "Comx: closing port" << endl;
	return TRUE;
}

BOOL Trap(PCHAR szfunc)
{
	dout << "Comx: " << szfunc << endl;
	DebugBreak();
	return FALSE;
}


COMMREQUEST Comx::GetCommState(P_DCB pDCB) 
{
	return Trap("GetCommState");
}

COMMREQUEST Comx::GetProperties(_COMMPROP* pCommProp) 
{
	return Trap("GetProperties");
}

COMMREQUEST Comx::SetCommState(P_DCB pDCB, DWORD ActionMask) 
{
	return Trap("SetCommState");
}

COMMREQUEST Comx::Setup(PCHAR RxQueue, DWORD cbRxQueue, PCHAR TxQueue, DWORD cbTxQueue) 
{
	return Trap("Setup");
}

COMMREQUEST Comx::ClearError(_COMSTAT* pComstat, PDWORD pError) 
{
	return Trap("ClearError");
}

COMMREQUEST Comx::GetQueueStatus(_COMSTAT* pComstat) 
{
	return Trap("GetQueueStatus");
}

COMMREQUEST Comx::Purge(DWORD qType) 
{
	return Trap("Purge");
}

COMMREQUEST Comx::Read(PCHAR buf, DWORD cbRequest, PDWORD pRxCount) 
{
	return Trap("Read");
}

COMMREQUEST Comx::TransmitChar(CHAR ch) 
{
	return Trap("TransmitChar");
}

COMMREQUEST Comx::Write(PCHAR buf, DWORD cbRequest, PDWORD pTxCount) 
{
	return Trap("Write");
}

COMMREQUEST Comx::EnableNotification(PCOMMNOTIFYPROC pCallback, PVOID refData) 
{
	return Trap("EnableNotification");
}

COMMREQUEST Comx::EscapeFunction(DWORD lFunc, DWORD InData, PVOID pOutData) 
{
	return Trap("EscapeFunction");
}

COMMREQUEST Comx::GetModemStatus(PDWORD pModemStatus) 
{
	return Trap("GetModemStatus");
}

COMMREQUEST Comx::GetEventMask(DWORD mask, PDWORD pEvents) 
{
	return Trap("GetEventMask");
}

COMMREQUEST Comx::SetEventMask(DWORD mask, PDWORD pEvents) 
{
	return Trap("SetEventMask");
}

COMMREQUEST Comx::SetModemStatusShadow(PBYTE pMSRShadow) 
{
	return Trap("SetModemStatusShadow");
}

COMMREQUEST Comx::SetReadCallBack(DWORD RxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData) 
{
	return Trap("SetReadCallback");
}

COMMREQUEST Comx::SetWriteCallBack(DWORD TxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData) 
{
	return Trap("SetWriteCallback");
}

#ifdef WIN40
COMMREQUEST Comx::GetCommConfig(PCOMMCONFIG pCC, PDWORD pSize)
{
	return Trap("GetCommConfig");
}

COMMREQUEST Comx::SetCommConfig(PCOMMCONFIG pCC, DWORD dwSize)
{
	return Trap("SetCommConfig");
}

COMMREQUEST Comx::GetError(PDWORD pError)
{
	return Trap("GetError");
}

DWORD __cdecl Comx::DeviceIOCtl(DWORD IoControlCode, PVOID pInBuf, DWORD nInBufferSize, 
							PVOID pOutBuf,DWORD  nOutBufferSize,
							PIOCTLNOTIFICATIONFUNC pNotify, IOCtlBlock*  pIOCtlBlock)
{
	return Trap("DeviceIoctl"); // note this returns zero, which is OK
}
#endif

