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

// dbwinstr.cpp - implementation of tutorial example class, Vdbwinstream
//
// Copyright (c) 1994, Compuware Corporation

#include "dbwinstr.h"

// **********************************************************************
// Constructor
//
Vdbwinstream::Vdbwinstream() : VPipe(DBWS_PIPESIZE), Vdbostream(), 
				VPMDPMIEntry(DWINSTR_ID_STRING, 0)
{
	m_bActive = FALSE;
}

// **********************************************************************
// Vendor Entry Point - override of VPMDPMIEntry::handler
//
BOOL Vdbwinstream::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	char* pBuf;
	DWORD cbCount;

	switch (_clientAX)
	{
	case DBWS_REGISTER:
		m_PostMessageHwnd = _clientBX;
		m_PostMessageSeg  = _clientCX;
		m_PostMessageOff  = _clientDX;
		m_bActive = TRUE;
		_clientAX = 0;
		break;

	case DBWS_READ:
		pBuf = (char*)Map_Flat(CLIENT_ES, CLIENT_DI);
		cbCount = _clientBX;
		_clientCX = read(pBuf, cbCount);
		_clientAX = 0;
		break;
		
	case DBWS_EXIT:
		m_bActive = FALSE;
		break;
	}

	setReturn();
	return TRUE;
}

// **********************************************************************
// Override of Vdbostream::_output
// 
VOID Vdbwinstream::_output(const char* s)
{
	if (m_bActive)
		write((char*)s, strlen(s)+1);
}

// **********************************************************************
// Override of VPipe::OnDataAvailable
//
VOID Vdbwinstream::OnDataAvailable()
{
	if (m_bActive)
	{
		DBWSEvent* p = new DBWSEvent(this);
		if (p)
			p->call();
	}
}

// **********************************************************************
// Constructor for class DBWSEvent
// 
DBWSEvent::DBWSEvent(Vdbwinstream* pDbws) : 
			VVMEvent(Get_Sys_VM_Handle(), (PVOID)pDbws)
{
}

// **********************************************************************
// Handler for DBWSEvent
//
VOID DBWSEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	CLIENT_STRUCT saveRegs;
	Vdbwinstream* pDbwin = (Vdbwinstream*)refData;

	Save_Client_State(&saveRegs);

	Begin_Nest_Exec();

	Simulate_Push(pDbwin->m_PostMessageHwnd);	// hwnd
	Simulate_Push(WM_COMMAND);			// message
	Simulate_Push(DBWS_EVENT);			// wParam
	Simulate_Push(0);				// lParam
	Simulate_Push(0);
	Simulate_Far_Call(pDbwin->m_PostMessageSeg, pDbwin->m_PostMessageOff);
	Resume_Exec();

	End_Nest_Exec();

	Restore_Client_State(&saveRegs);
}

