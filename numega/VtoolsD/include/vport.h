// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vport.h - include file for VIOPort class
//

#include <vmachine.h>

#ifndef __vioport_h_
#define __vioport_h_

#define IOPORTTHUNKSIZE 0x14

class VIOPort
{
public:
	VIOPort(DWORD port);
	~VIOPort();
	BOOL hook();
	BOOL unhook();
	VOID localEnable(VMHANDLE);
	VOID localDisable(VMHANDLE);
	VOID globalEnable();
	VOID globalDisable();
	virtual DWORD handler(VMHANDLE, DWORD port,
				CLIENT_STRUCT* pRegs, DWORD iotype, DWORD outdata);

protected:
	DWORD m_port;
	BYTE m_thunk[IOPORTTHUNKSIZE];
};

#endif
