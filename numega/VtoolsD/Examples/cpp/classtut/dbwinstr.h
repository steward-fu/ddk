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

// dbwinstr.h - include file for tutorial example class, Vdbwinstream
//
// Copyright (c) 1994, Compuware Corporation All rights reserved

#ifndef NotVxD

#include <vtoolscp.h>

class DBWSEvent;

// ***********************************************************************
// Vdbwinstream
//
class  Vdbwinstream : public Vdbostream, public VPMDPMIEntry, public VPipe 
{
public:
	Vdbwinstream();

protected:
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
	virtual VOID _output(const char* p);
	virtual VOID OnDataAvailable();

	BOOL m_bActive;
	WORD m_PostMessageSeg;
	WORD m_PostMessageOff;
	WORD m_PostMessageHwnd;

	friend DBWSEvent;
};

// ***********************************************************************
// DBWSEvent
//
class DBWSEvent : public VVMEvent
{
public:
	DBWSEvent(Vdbwinstream* pDbws);
	VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
};


#endif // NotVxD

#define DWINSTR_ID_STRING "Compuware VxD->DBWIN Class Version 1.0"
#define DBWS_PIPESIZE	0x1000		// size of VxD level pipe buffer
#define DBWS_REGISTER	0x0001		// function code to register Win app
#define DBWS_READ    	0x0002		// function code to request data
#define DBWS_EXIT    	0x0003		// function code to deregister app
#define DBWS_EVENT	0x0100		// Command code
#ifndef WM_COMMAND
#define WM_COMMAND      0x0111
#endif	
