// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vv86api.h - include file for foreign V86 API class
//

#ifndef __vdeviceapi_h_
#define __vdeviceapi_h_

#include <vmachine.h>

#define VDEVAPITHUNKSIZE 0x14

#define DEVICE_PM_API	0x00000001
#define DEVICE_V86_API	0x00000002

class VDeviceAPI 
{
public:
		VDeviceAPI(DWORD id, DWORD flags);
		~VDeviceAPI() {unhook();}
		BOOL hook();
		BOOL unhook();
		VOID callPreviousHandler(VMHANDLE hVM, CLIENT_STRUCT* pRegs);
		virtual VOID handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs);

protected:
		DWORD m_ID;
		DWORD m_flags;
		VOID (VDeviceAPI::*m_prevV86)(VMHANDLE,CLIENT_STRUCT*);
		VOID (VDeviceAPI::*m_prevPM)(VMHANDLE,CLIENT_STRUCT*);
		BYTE m_thunk[VDEVAPITHUNKSIZE];
};
#endif
