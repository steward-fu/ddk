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
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in 
// Compuware Corporation license agreement and as provided in 
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995), 
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995), 
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.  
// Compuware Corporation.
// 
// This product contains confidential information and trade secrets 
// of Compuware Corporation. Use, disclosure, or reproduction is 
// prohibited without the prior express written permission of Compuware 
// Corporation.
//
//=============================================================================
//
// vcomport.cpp - implementation of class VCommPort

#include <vtoolscp.h>

#ifdef __BORLANDC__
extern "C" BOOL __cdecl (* __cdecl createPortFunctionThunk(VCommPort*, VCommPort*, PBYTE, BOOL, VOID (VCommPort::*pFunc)()))();
#else
extern "C" BOOL (* __cdecl createPortFunctionThunk(VCommPort*, VCommPort*, PBYTE, BOOL, VOID (VCommPort::*pFunc)()))();
#endif

VCommPort::VCommPort(PCHAR szPortName)
{
	m_PortNumber=0;
	m_ownerVM = 0;
	m_PortFlags = 0;
	memset(&m_dcb, 0, sizeof(_DCB));
	m_dcb.DCBLength = sizeof(_DCB);

	m_name = _strdup(szPortName);
	m_chain = VCommPortDriver::m_PortList;
	VCommPortDriver::m_PortList = this;	

#ifdef WIN40
	m_ContentionHandler = _VCOMM_Get_Contention_Handler(szPortName);
	if (m_ContentionHandler)
		m_ResourceHandle = VCOMM_Map_Name_To_Resource(szPortName);
	else
		m_ResourceHandle = NULL;
	m_StealFlag = TRUE;
#endif

	InitPortData();
}

VOID VCommPort::InitPortData()
{
	m_data.PDLength = sizeof(PORTDATA); // size of struct
	m_data.PDVersion = 0;		// version
	m_data.PDNumFunctions = NPORTFUNCS;	// number of functions
	m_data.dwLastError = 0;		// what was the error
					// for the last operation
	m_data.dwClientEventMask = 0;	// event mask set by client 
	m_data.lpClientEventNotify = 0;	// address set by client for
				// event notification 
	m_data.lpClientReadNotify = 0;	// adress set by client for
					// read threshold notification 
	m_data.lpClientWriteNotify = 0;	// address set by client for
					// write threshold notification 
	m_data.dwClientRefData = 0;	// Client's reference data 
	m_data.dwWin31Req = 0;		// Used for WIN3.1 specific reasons 
	m_data.dwClientEvent = 0;	// Event to send to client 
	m_data.dwCallerVMId = 0;	// Used for supporting all VMs 

	m_data.dwDetectedEvents = 0;	// mask of detected and
					// enabled events 
    m_data.dwCommError = 0;		// non-zero if I/O error. 
	m_data.bMSRShadow = 0;		// the shadow of Modem Status
					// Register 
	m_data.wFlags = 0;		// flags for the port 
	m_data.LossByte = 0;		// For COMM ports 1-4 VCD flags this
					// byte telling port driver that it
					// has lost the port 

	m_data.QInAddr = 0;		// Address of the queue 
	m_data.QInSize = 0;		// Length of queue in bytes 
	m_data.QOutAddr = 0;		// Address of the queue 
	m_data.QOutSize = 0;		// Length of queue in bytes 
	m_data.QInCount = 0;		// # of bytes currently in queue 
	m_data.QInGet = 0;		// Offset into q to get bytes from 
	m_data.QInPut = 0;		// Offset into q to put bytes in 
	m_data.QOutCount = 0;		// Number of bytes currently in q 
	m_data.QOutGet = 0;		// Offset into q to get bytes from 
	m_data.QOutPut = 0;		// Offset into q to put bytes in 

	m_data.ValidPortData = 0;	// For checking validity etc. 

	m_data.lpLoadHandle = 0;	// load handle of the owner VxD 
	memset(&m_data.cmto, 0, sizeof(COMMTIMEOUTS));	// Commtimeouts struct 

	m_data.lpReadRequestQueue = 0;	// Pointer to pending Read requests 
	m_data.lpWriteRequestQueue = 0;	// Pointer to pending Write requests 

	m_data.dwLastReceiveTime = 0;	// Time of last reception of data 
		
	m_data.dwReserved1 = 0;		// Reserved 
	m_data.dwReserved2 = 0;		// Reserved  

}

VOID VCommPort::InitPortFunctions()
{
	VCommPort base;

	m_data.PDfunctions = &m_Functions;

	m_Functions.pPortSetCommState = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*0], FALSE, (VOID (VCommPort::*)())&VCommPort::SetCommState);
	m_Functions.pPortGetCommState = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*1], FALSE, (VOID (VCommPort::*)())&VCommPort::GetCommState);
	m_Functions.pPortSetup = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*2], TRUE, (VOID (VCommPort::*)())&VCommPort::Setup);
	m_Functions.pPortTransmitChar = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*3], FALSE, (VOID (VCommPort::*)())&VCommPort::TransmitChar);
	m_Functions.pPortClose = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*4], FALSE, (VOID (VCommPort::*)())&VCommPort::Close);
	m_Functions.pPortGetQueueStatus = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*5], FALSE, (VOID (VCommPort::*)())&VCommPort::GetQueueStatus);
	m_Functions.pPortClearError = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*6], FALSE, (VOID (VCommPort::*)())&VCommPort::ClearError);
	m_Functions.pPortSetModemStatusShadow = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*7], FALSE, (VOID (VCommPort::*)())&VCommPort::SetModemStatusShadow);
	m_Functions.pPortGetProperties = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*8], FALSE, (VOID (VCommPort::*)())&VCommPort::GetProperties);
	m_Functions.pPortEscapeFunction = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*9], FALSE, (VOID (VCommPort::*)())&VCommPort::EscapeFunction);
	m_Functions.pPortPurge = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*10], FALSE, (VOID (VCommPort::*)())&VCommPort::Purge);
	m_Functions.pPortSetEventMask = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*11], FALSE, (VOID (VCommPort::*)())&VCommPort::SetEventMask);
	m_Functions.pPortGetEventMask = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*12], FALSE, (VOID (VCommPort::*)())&VCommPort::GetEventMask);
	m_Functions.pPortWrite = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*13], FALSE, (VOID (VCommPort::*)())&VCommPort::Write);
	m_Functions.pPortRead = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*14], FALSE, (VOID (VCommPort::*)())&VCommPort::Read);
	m_Functions.pPortEnableNotification = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*15], FALSE, (VOID (VCommPort::*)())&VCommPort::EnableNotification);
	m_Functions.pPortSetReadCallBack = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*16], FALSE, (VOID (VCommPort::*)())&VCommPort::SetReadCallBack);
	m_Functions.pPortSetWriteCallBack = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*17], FALSE, (VOID (VCommPort::*)())&VCommPort::SetWriteCallBack);
	m_Functions.pPortGetModemStatus = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*18], FALSE, (VOID (VCommPort::*)())&VCommPort::GetModemStatus);
#ifdef WIN40	
	m_Functions.pPortGetCommConfig =  createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*19], FALSE, (VOID (VCommPort::*)())&VCommPort::GetCommConfig);
	m_Functions.pPortSetCommConfig =  createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*20], FALSE, (VOID (VCommPort::*)())&VCommPort::SetCommConfig);
	m_Functions.pPortGetError =  createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*21], FALSE, (VOID (VCommPort::*)())&VCommPort::GetError);
	m_Functions.pPortDeviceIOCtl = createPortFunctionThunk(this, &base, &m_thunks[PORTFUNCTHUNKSIZE*22], FALSE, (VOID (VCommPort::*)())&VCommPort::DeviceIOCtl);
#endif
}

COMMREQUEST VCommPort::Setup(PCHAR RxQueue, DWORD cbRxQueue, PCHAR TxQueue, DWORD cbTxQueue) 
{
	if ((m_PortFlags & PF_RXQINTERNAL) && (m_data.QInAddr != 0))
		delete (PBYTE)m_data.QInAddr;

	if (RxQueue == NULL)
	{
		if ((m_data.QInAddr=(DWORD) new BYTE[cbRxQueue]) == 0)
		{
			m_data.QInSize = 0;
			m_data.dwLastError = IE_MEMORY;
			return FALSE;
		}
		else
		{
			m_PortFlags |= PF_RXQINTERNAL;
			m_data.QInSize = cbRxQueue;
		}
	}
	else
	{
		m_data.QInAddr = (DWORD)RxQueue;	
		m_data.QInSize = cbRxQueue;
		m_PortFlags &= ~PF_RXQINTERNAL;
	}		


	if ((m_PortFlags & PF_TXQINTERNAL) && (m_data.QOutAddr != 0))
		delete (PBYTE)m_data.QOutAddr;

	if (TxQueue == NULL)
	{
		if (cbTxQueue == 0)
			m_data.QOutSize = 0;

		else if ((m_data.QOutAddr=(DWORD) new BYTE[cbTxQueue]) == 0)
		{
			m_data.QOutSize = 0;
			m_data.dwLastError = IE_MEMORY;
			return FALSE;
		}
		else
		{
			m_PortFlags |= PF_TXQINTERNAL;
			m_data.QOutSize = cbTxQueue;
			m_data.wFlags |= TxQueuesSet;
		}
	}
	else
	{
		m_data.QOutAddr = (DWORD)TxQueue;	
		m_data.QOutSize = cbTxQueue;
		m_PortFlags &= ~PF_TXQINTERNAL;
		m_data.wFlags |= TxQueuesSet;
	}		

	return TRUE;
}

COMMREQUEST VCommPort::Initialize(DEVNODE handle, DWORD baseIO, DWORD irq) 
{return FALSE;}
COMMREQUEST VCommPort::Open(VMHANDLE hVM, PDWORD pError) 
{return FALSE;}
COMMREQUEST VCommPort::Close() 
{return FALSE;}
COMMREQUEST VCommPort::GetCommState(P_DCB pDCB) 
{return FALSE;}
COMMREQUEST VCommPort::GetProperties(_COMMPROP* pCommProp) 
{return FALSE;}
COMMREQUEST VCommPort::SetCommState(P_DCB pDCB, DWORD ActionMask) 
{return FALSE;}
COMMREQUEST VCommPort::ClearError(_COMSTAT* pComstat, PDWORD pError) 
{return FALSE;}
COMMREQUEST VCommPort::GetQueueStatus(_COMSTAT* pComstat) 
{return FALSE;}
COMMREQUEST VCommPort::Purge(DWORD qType) 
{return FALSE;}
COMMREQUEST VCommPort::Read(PCHAR buf, DWORD cbRequest, PDWORD pRxCount) 
{return FALSE;}
COMMREQUEST VCommPort::TransmitChar(CHAR ch) 
{return FALSE;}
COMMREQUEST VCommPort::Write(PCHAR buf, DWORD cbRequest, PDWORD pTxCount) 
{return FALSE;}
COMMREQUEST VCommPort::EnableNotification(PCOMMNOTIFYPROC pCallback, PVOID refData) 
{return FALSE;}
COMMREQUEST VCommPort::EscapeFunction(DWORD lFunc, DWORD InData, PVOID pOutData) 
{return FALSE;}
COMMREQUEST VCommPort::GetModemStatus(PDWORD pModemStatus) 
{return FALSE;}
COMMREQUEST VCommPort::GetEventMask(DWORD mask, PDWORD pEvents) 
{return FALSE;}
COMMREQUEST VCommPort::SetEventMask(DWORD mask, PDWORD pEvents) 
{return FALSE;}
COMMREQUEST VCommPort::SetModemStatusShadow(PBYTE pMSRShadow) 
{return FALSE;}
COMMREQUEST VCommPort::SetReadCallBack(DWORD RxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData) 
{return FALSE;}
COMMREQUEST VCommPort::SetWriteCallBack(DWORD TxTrigger, PCOMMNOTIFYPROC pCallback, PVOID refData) 
{return FALSE;}
OBSOLETE VCommPort::SetReadCallback(DWORD RxTrigger, PCOMMNOTIFYPROC pCallback, DWORD refData) 
{}
OBSOLETE VCommPort::SetWriteCallback(DWORD TxTrigger, PCOMMNOTIFYPROC pCallback, DWORD refData) 
{}
#ifdef WIN40
COMMREQUEST VCommPort::GetCommConfig(PCOMMCONFIG pCC, PDWORD pSize)
{return FALSE;}
COMMREQUEST VCommPort::SetCommConfig(PCOMMCONFIG pCC, DWORD dwSize)
{return FALSE;}
COMMREQUEST VCommPort::GetError(PDWORD pError)
{return FALSE;}
COMMREQUEST VCommPort::CtnNotifyHandler(DWORD Notification)
{return TRUE;}
DWORD __cdecl VCommPort::DeviceIOCtl(DWORD IoControlCode, PVOID pInBuf, DWORD nInBufferSize, 
							PVOID pOutBuf,DWORD  nOutBufferSize,
							PIOCTLNOTIFICATIONFUNC pNotify, IOCtlBlock*  pIOCtlBlock)
{return 0;}
#endif
